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
#include "events/pressure_publish_mode_event.h"
#include "events/zsw_periodic_event.h"
#include "sensors/zsw_pressure_sensor.h"

LOG_MODULE_REGISTER(zsw_pressure_sensor, CONFIG_ZSW_SENSORS_LOG_LEVEL);

static void pressure_periodic_callback(const struct zbus_channel *chan);
static void pressure_mode_request_callback(const struct zbus_channel *chan);
static void pressure_mode_work_handler(struct k_work *work);

ZBUS_CHAN_DECLARE(pressure_data_chan);
ZBUS_CHAN_DECLARE(pressure_publish_mode_chan);
ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(zsw_pressure_sensor_periodic_lis, pressure_periodic_callback);
ZBUS_LISTENER_DEFINE(zsw_pressure_sensor_mode_lis, pressure_mode_request_callback);
ZBUS_CHAN_ADD_OBS(pressure_publish_mode_chan, zsw_pressure_sensor_mode_lis, 1);
static const struct device *const bmp581 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bmp581));
static pressure_publish_mode_t active_publish_mode = PRESSURE_PUBLISH_MODE_SLOW;
static atomic_t requested_publish_mode = PRESSURE_PUBLISH_MODE_SLOW;
static K_WORK_DEFINE(pressure_mode_work, pressure_mode_work_handler);

static void publish_pressure_sample(void)
{
    float pressure;
    float temperature;

    if (zsw_pressure_sensor_get_sample(&pressure, &temperature) != 0) {
        return;
    }

    LOG_INF("Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);

    struct pressure_event evt = {
        .pressure = pressure,
        .temperature = temperature
    };
    zbus_chan_pub(&pressure_data_chan, &evt, K_MSEC(250));
}

static void pressure_periodic_callback(const struct zbus_channel *chan)
{
    ARG_UNUSED(chan);
    publish_pressure_sample();
}

static void pressure_mode_request_callback(const struct zbus_channel *chan)
{
    const struct pressure_publish_mode_event *event = zbus_chan_const_msg(chan);

    if (event->mode != PRESSURE_PUBLISH_MODE_SLOW && event->mode != PRESSURE_PUBLISH_MODE_FAST) {
        return;
    }

    atomic_set(&requested_publish_mode, event->mode);
    k_work_submit(&pressure_mode_work);
}

static void pressure_mode_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    pressure_publish_mode_t requested_mode = atomic_get(&requested_publish_mode);
    if (requested_mode == active_publish_mode) {
        return;
    }

    const struct zbus_channel *current_chan = active_publish_mode == PRESSURE_PUBLISH_MODE_FAST ?
                                               &periodic_event_1s_chan : &periodic_event_10s_chan;
    const struct zbus_channel *requested_chan = requested_mode == PRESSURE_PUBLISH_MODE_FAST ?
                                                 &periodic_event_1s_chan : &periodic_event_10s_chan;

    int ret = zsw_periodic_chan_rm_obs(current_chan, &zsw_pressure_sensor_periodic_lis);
    if (ret != 0) {
        LOG_ERR("Failed to remove pressure periodic observer: %d", ret);
        return;
    }

    ret = zsw_periodic_chan_add_obs(requested_chan, &zsw_pressure_sensor_periodic_lis);
    if (ret != 0) {
        LOG_ERR("Failed to add pressure periodic observer: %d", ret);
        int rollback_ret = zsw_periodic_chan_add_obs(current_chan, &zsw_pressure_sensor_periodic_lis);
        if (rollback_ret != 0) {
            LOG_ERR("Failed to restore pressure periodic observer: %d", rollback_ret);
        }
        return;
    }

    active_publish_mode = requested_mode;
    if (active_publish_mode == PRESSURE_PUBLISH_MODE_FAST) {
        publish_pressure_sample();
    }
}

int zsw_pressure_sensor_init(void)
{
    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    int ret = zsw_periodic_chan_add_obs(&periodic_event_10s_chan, &zsw_pressure_sensor_periodic_lis);
    if (ret != 0) {
        return ret;
    }

    return zsw_pressure_sensor_set_odr(BOSCH_BMP581_ODR_DEFAULT);

}

int zsw_pressure_sensor_set_odr(uint8_t odr)
{
    struct sensor_value value;

    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    value.val1 = odr;

    if (sensor_attr_set(bmp581, SENSOR_CHAN_ALL, SENSOR_ATTR_SAMPLING_FREQUENCY, &value) != 0) {
        return -EIO;
    }

    return 0;
}

int zsw_pressure_sensor_get_pressure(float *pressure)
{
    struct sensor_value sensor_val;

    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch(bmp581) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bmp581, SENSOR_CHAN_PRESS, &sensor_val) != 0) {
        return -ENODATA;
    }

    *pressure = sensor_value_to_float(&sensor_val);

    return 0;
}

int zsw_pressure_sensor_get_sample(float *pressure, float *temperature)
{
    struct sensor_value pressure_val;
    struct sensor_value temperature_val;

    if (pressure == NULL || temperature == NULL) {
        return -EINVAL;
    }

    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch(bmp581) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bmp581, SENSOR_CHAN_PRESS, &pressure_val) != 0 ||
        sensor_channel_get(bmp581, SENSOR_CHAN_AMBIENT_TEMP, &temperature_val) != 0) {
        return -ENODATA;
    }

    *pressure = sensor_value_to_float(&pressure_val);
    *temperature = sensor_value_to_float(&temperature_val);

    return 0;
}

int zsw_pressure_sensor_get_temperature(float *temperature)
{
    struct sensor_value sensor_val;

    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch(bmp581) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bmp581, SENSOR_CHAN_AMBIENT_TEMP, &sensor_val) != 0) {
        return -ENODATA;
    }

    *temperature = sensor_value_to_float(&sensor_val);

    return 0;
}
