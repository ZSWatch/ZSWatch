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

#include "events/pressure_event.h"
#include "events/zsw_periodic_event.h"
#include "sensors/zsw_pressure_sensor.h"

LOG_MODULE_REGISTER(zsw_pressure_sensor, CONFIG_ZSW_SENSORS_LOG_LEVEL);

typedef enum {
    PROFILE_10S,
    PROFILE_1S,
    PROFILE_MAX,
} profile_id_t;

typedef struct {
    const char *name;
    uint8_t publication_interval_s;
    bosch_bmp581_configuration_t configuration;
    const struct zbus_channel *publication_chan;
} pressure_profile_t;

static void pressure_periodic_callback(const struct zbus_channel *chan);
static void profile_work_handler(struct k_work *work);

ZBUS_CHAN_DECLARE(pressure_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(zsw_pressure_sensor_periodic_lis, pressure_periodic_callback);

static const struct device *const bmp581 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bmp581));
static const pressure_profile_t pressure_profiles[PROFILE_MAX] = {
    [PROFILE_10S] = {
        .name = "IDLE",
        .publication_interval_s = 10,
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
    [PROFILE_1S] = {
        .name = "FAST",
        .publication_interval_s = 1,
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

/* Serializes request ownership and profile transitions. Acquire before pressure_lock. */
static K_MUTEX_DEFINE(profile_update_lock);
static sys_slist_t fast_requests = SYS_SLIST_STATIC_INIT(&fast_requests);

/* Desired/applied profile and sample state, guarded by pressure_lock. */
static K_MUTEX_DEFINE(pressure_lock);
static profile_id_t requested_profile = PROFILE_10S;
static profile_id_t active_profile = PROFILE_10S;
static bool profile_programmed;
static const struct zbus_channel *sampling_chan;
/* Reject samples from an old configuration or an already published conversion. */
static uint32_t profile_generation;
static uint32_t published_sequence;
static uint8_t missed_samples;

static K_WORK_DELAYABLE_DEFINE(profile_work, profile_work_handler);

static profile_id_t pressure_requested_profile_locked(void)
{
    return sys_slist_is_empty(&fast_requests) ? PROFILE_10S : PROFILE_1S;
}

/* Caller holds profile_update_lock; release pressure_lock before changing observers. */
static int pressure_apply_profile_locked(profile_id_t profile_id)
{
    if ((unsigned int)profile_id >= PROFILE_MAX || pressure_profiles[profile_id].publication_chan == NULL) {
        return -EINVAL;
    }

    const pressure_profile_t *profile = &pressure_profiles[profile_id];
    const struct zbus_channel *requested_chan = profile->publication_chan;
    const struct zbus_channel *previous_chan;
    uint32_t generation;

    k_mutex_lock(&pressure_lock, K_FOREVER);
    if (profile_programmed && profile_id == active_profile && sampling_chan == requested_chan) {
        k_mutex_unlock(&pressure_lock);
        return 0;
    }
    profile_programmed = false;
    previous_chan = sampling_chan;
    k_mutex_unlock(&pressure_lock);

    int ret = zsw_bmp581_apply_config(bmp581, &profile->configuration, &generation);
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
    profile_generation = generation;
    active_profile = profile_id;
    missed_samples = 0;
    profile_programmed = true;
    LOG_INF("Pressure profile programmed: %s, generation=%u, publication=%u s",
            profile->name, generation, profile->publication_interval_s);
    k_mutex_unlock(&pressure_lock);
    return 0;
}

static void profile_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    k_mutex_lock(&profile_update_lock, K_FOREVER);
    int ret = pressure_apply_profile_locked(pressure_requested_profile_locked());
    if (ret != 0) {
        LOG_WRN("Pressure profile update failed: %d", ret);
        k_work_reschedule(&profile_work, K_SECONDS(1));
    }
    k_mutex_unlock(&profile_update_lock);
}

static void pressure_periodic_callback(const struct zbus_channel *chan)
{
    bosch_bmp581_sample_t sample;
    bool retry = false;
    bool publish = false;

    k_mutex_lock(&pressure_lock, K_FOREVER);
    if (!profile_programmed || chan != sampling_chan || active_profile != requested_profile) {
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
        publish = true;
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
        k_work_reschedule(&profile_work, K_SECONDS(1));
    }
    if (publish) {
        k_mutex_lock(&pressure_lock, K_FOREVER);
        publish = profile_programmed && active_profile == requested_profile && sample.generation == profile_generation;
        k_mutex_unlock(&pressure_lock);
    }
    if (publish) {
        struct pressure_event evt = {
            .pressure = sample.pressure,
            .temperature = sample.temperature,
        };
        zbus_chan_pub(&pressure_data_chan, &evt, K_MSEC(250));
    }
}

int zsw_pressure_sensor_init(void)
{
    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    k_mutex_lock(&profile_update_lock, K_FOREVER);
    int ret = pressure_apply_profile_locked(pressure_requested_profile_locked());
    if (ret != 0) {
        k_work_reschedule(&profile_work, K_SECONDS(1));
    }
    k_mutex_unlock(&profile_update_lock);
    return ret;
}

int zsw_pressure_sensor_request_fast(struct zsw_pressure_fast_request *req, bool enable)
{
    if (req == NULL) {
        return -EINVAL;
    }
    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    k_mutex_lock(&profile_update_lock, K_FOREVER);
    if (enable != req->active) {
        if (enable) {
            sys_slist_append(&fast_requests, &req->node);
        } else {
            sys_slist_find_and_remove(&fast_requests, &req->node);
        }
        req->active = enable;
    }
    k_mutex_lock(&pressure_lock, K_FOREVER);
    requested_profile = pressure_requested_profile_locked();
    k_mutex_unlock(&pressure_lock);
    k_work_reschedule(&profile_work, K_NO_WAIT);
    k_mutex_unlock(&profile_update_lock);
    return 0;
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

int zsw_pressure_sensor_get_pressure(float *pressure)
{
    float temperature;
    if (pressure == NULL) {
        return -EINVAL;
    }
    return zsw_pressure_sensor_get_sample(pressure, &temperature);
}

int zsw_pressure_sensor_get_temperature(float *temperature)
{
    float pressure;
    if (temperature == NULL) {
        return -EINVAL;
    }
    return zsw_pressure_sensor_get_sample(&pressure, temperature);
}
