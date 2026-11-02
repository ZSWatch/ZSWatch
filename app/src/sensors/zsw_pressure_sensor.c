/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/sys/atomic.h>

#include "events/pressure_event.h"
#include "events/elevation_tracking_event.h"
#include "events/zsw_periodic_event.h"
#include "sensors/zsw_pressure_sensor.h"

LOG_MODULE_REGISTER(zsw_pressure_sensor, CONFIG_ZSW_SENSORS_LOG_LEVEL);

static void pressure_periodic_callback(const struct zbus_channel *chan);
static void tracking_request_callback(const struct zbus_channel *chan);
static void tracking_work_handler(struct k_work *work);

ZBUS_CHAN_DECLARE(pressure_data_chan);
ZBUS_CHAN_DECLARE(elevation_tracking_chan);
ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(zsw_pressure_sensor_periodic_lis, pressure_periodic_callback);
ZBUS_LISTENER_DEFINE(zsw_pressure_sensor_tracking_lis, tracking_request_callback);
ZBUS_CHAN_ADD_OBS(elevation_tracking_chan, zsw_pressure_sensor_tracking_lis, 1);
static const struct device *const bmp581 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bmp581));
static const struct {
    bosch_bmp581_configuration_t configuration;
    const struct zbus_channel *publication_chan;
} pressure_profiles[] = {
    {
        .configuration = {
            .mode = BOSCH_BMP581_MODE_NORMAL,
            .odr = BOSCH_BMP581_ODR_0_250_HZ,
            .osr_p = BOSCH_BMP581_OSR_4X,
            .osr_t = BOSCH_BMP581_OSR_64X,
            .iir_p = BOSCH_BMP581_IIR_63,
            .iir_t = BOSCH_BMP581_IIR_63,
            .fifo_enabled = false,
        },
        .publication_chan = &periodic_event_10s_chan,
    },
    {
        .configuration = {
            .mode = BOSCH_BMP581_MODE_NORMAL,
            .odr = BOSCH_BMP581_ODR_10_HZ,
            .osr_p = BOSCH_BMP581_OSR_4X,
            .osr_t = BOSCH_BMP581_OSR_1X,
            .iir_p = BOSCH_BMP581_IIR_3,
            .iir_t = BOSCH_BMP581_IIR_BYPASS,
            .fifo_enabled = false,
        },
        .publication_chan = &periodic_event_1s_chan,
    },
};
static K_MUTEX_DEFINE(pressure_lock);
static K_MUTEX_DEFINE(profile_update_lock);
static bool tracking_active;
static bool profile_programmed;
static uint32_t profile_generation;
static uint32_t published_sequence;
static atomic_val_t programmed_request;
static uint8_t missed_samples;
static const struct zbus_channel *sampling_chan;
static atomic_t tracking_requested;
static atomic_t request_revision;
static K_WORK_DELAYABLE_DEFINE(tracking_work, tracking_work_handler);

static void pressure_periodic_callback(const struct zbus_channel *chan)
{
    bosch_bmp581_sample_t sample;
    bool retry = false;
    bool publish = false;
    atomic_val_t revision = atomic_get(&request_revision);

    k_mutex_lock(&pressure_lock, K_FOREVER);
    if (!profile_programmed || chan != sampling_chan || revision != programmed_request ||
        tracking_active != (atomic_get(&tracking_requested) != 0)) {
        k_mutex_unlock(&pressure_lock);
        return;
    }

    int ret = zsw_bmp581_read_sample(bmp581, &sample);
    if (ret == 0 && sample.generation != profile_generation) {
        ret = -EIO;
    }
    if (ret == -EAGAIN || (ret == 0 && sample.sequence == published_sequence)) {
        if (++missed_samples >= 2) {
            ret = -ETIMEDOUT;
        }
    } else if (ret == 0) {
        missed_samples = 0;
        publish = revision == atomic_get(&request_revision);
    }

    if (ret != 0 && ret != -EAGAIN) {
        profile_programmed = false;
        retry = true;
        LOG_WRN("Pressure profile sampling failed: %d", ret);
    }
    if (publish) {
        published_sequence = sample.sequence;
    }
    k_mutex_unlock(&pressure_lock);

    if (retry) {
        k_work_reschedule(&tracking_work, K_SECONDS(1));
    }
    if (publish && revision == atomic_get(&request_revision)) {
        struct pressure_event evt = {
            .pressure = sample.pressure,
            .temperature = sample.temperature,
        };
        zbus_chan_pub(&pressure_data_chan, &evt, K_MSEC(250));
    }
}

static void tracking_request_callback(const struct zbus_channel *chan)
{
    const struct elevation_tracking_event *event = zbus_chan_const_msg(chan);

    atomic_set(&tracking_requested, event->active);
    atomic_inc(&request_revision);
    k_work_reschedule(&tracking_work, K_NO_WAIT);
}

static int pressure_apply_profile_locked(bool requested, atomic_val_t revision)
{
    const struct zbus_channel *requested_chan = pressure_profiles[requested].publication_chan;
    const struct zbus_channel *previous_chan;
    uint32_t generation;

    k_mutex_lock(&pressure_lock, K_FOREVER);
    if (revision != atomic_get(&request_revision) || requested != (atomic_get(&tracking_requested) != 0)) {
        k_mutex_unlock(&pressure_lock);
        return -EAGAIN;
    }
    if (profile_programmed && requested == tracking_active && sampling_chan == requested_chan) {
        programmed_request = revision;
        k_mutex_unlock(&pressure_lock);
        return 0;
    }
    profile_programmed = false;
    previous_chan = sampling_chan;
    k_mutex_unlock(&pressure_lock);

    int ret = zsw_bmp581_apply_config(bmp581, &pressure_profiles[requested].configuration, &generation);
    if (ret != 0) {
        return ret;
    }

    if (previous_chan != requested_chan) {
        if (previous_chan != NULL) {
            ret = zsw_periodic_chan_rm_obs(previous_chan, &zsw_pressure_sensor_periodic_lis);
            if (ret < 0 && ret != -ENODATA) {
                return ret;
            }
            k_mutex_lock(&pressure_lock, K_FOREVER);
            sampling_chan = NULL;
            k_mutex_unlock(&pressure_lock);
        }

        ret = zsw_periodic_chan_add_obs(requested_chan, &zsw_pressure_sensor_periodic_lis);
        if (ret < 0 && ret != -EALREADY) {
            return ret;
        }
        k_mutex_lock(&pressure_lock, K_FOREVER);
        sampling_chan = requested_chan;
        k_mutex_unlock(&pressure_lock);
    }

    k_mutex_lock(&pressure_lock, K_FOREVER);
    if (revision != atomic_get(&request_revision) || requested != (atomic_get(&tracking_requested) != 0)) {
        ret = -EAGAIN;
    } else {
        profile_generation = generation;
        programmed_request = revision;
        tracking_active = requested;
        missed_samples = 0;
        profile_programmed = true;
        LOG_INF("Pressure profile programmed: %s, generation=%u, publication=%u s",
                requested ? "TRACKING" : "IDLE", generation, requested ? 1U : 10U);
        ret = 0;
    }
    k_mutex_unlock(&pressure_lock);
    return ret;
}

static int pressure_apply_profile(bool requested, atomic_val_t revision)
{
    k_mutex_lock(&profile_update_lock, K_FOREVER);
    int ret = pressure_apply_profile_locked(requested, revision);
    k_mutex_unlock(&profile_update_lock);
    return ret;
}

static void tracking_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    atomic_val_t revision = atomic_get(&request_revision);
    bool requested = atomic_get(&tracking_requested) != 0;
    int ret = pressure_apply_profile(requested, revision);

    if (ret == -EAGAIN || revision != atomic_get(&request_revision)) {
        k_work_reschedule(&tracking_work, K_NO_WAIT);
    } else if (ret != 0) {
        LOG_WRN("Pressure profile update failed: %d", ret);
        k_work_reschedule(&tracking_work, K_SECONDS(1));
    }
}

int zsw_pressure_sensor_init(void)
{
    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    atomic_val_t revision = atomic_get(&request_revision);
    int ret = pressure_apply_profile(atomic_get(&tracking_requested) != 0, revision);
    if (ret != 0) {
        k_work_reschedule(&tracking_work, ret == -EAGAIN ? K_NO_WAIT : K_SECONDS(1));
    }
    return ret == -EAGAIN ? 0 : ret;
}

int zsw_pressure_sensor_get_pressure(float *pressure)
{
    float temperature;
    if (pressure == NULL) {
        return -EINVAL;
    }
    return zsw_pressure_sensor_get_sample(pressure, &temperature);
}

int zsw_pressure_sensor_get_sample(float *pressure, float *temperature)
{
    bosch_bmp581_sample_t sample;

    if (pressure == NULL || temperature == NULL) {
        return -EINVAL;
    }

    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    int ret = zsw_bmp581_read_sample(bmp581, &sample);
    if (ret != 0) {
        return ret;
    }

    *pressure = sample.pressure;
    *temperature = sample.temperature;

    return 0;
}

int zsw_pressure_sensor_get_temperature(float *temperature)
{
    float pressure;
    if (temperature == NULL) {
        return -EINVAL;
    }
    return zsw_pressure_sensor_get_sample(&pressure, temperature);
}
