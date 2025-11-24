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
#include <zephyr/device.h>
#include <zephyr/pm/device.h>
#include "events/zsw_periodic_event.h"
#include "events/environment_event.h"
#include "sensors/zsw_environment_sensor.h"

#include "../../drivers/sensor/bme68x_iaq/bosch_bme68x_iaq.h"

LOG_MODULE_REGISTER(zsw_environment_sensor, CONFIG_ZSW_SENSORS_LOG_LEVEL);

static void zbus_periodic_slow_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(environment_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(zsw_environment_sensor_lis, zbus_periodic_slow_callback);

#if CONFIG_BME680
static const struct device *const bme688 = DEVICE_DT_GET(DT_NODELABEL(bme688));
#else
static const struct device *const bme688 = NULL;
#endif

static void zbus_periodic_slow_callback(const struct zbus_channel *chan)
{
    float temperature = 0.0f;
    float pressure = 0.0f;
    float humidity = 0.0f;
    float iaq = -1.0;

    if (zsw_environment_sensor_get(&temperature, &humidity, &pressure)) {
        return;
    }

    // NOTE: No error check here, because IAQ is optional.
    zsw_environment_sensor_get_iaq(&iaq);

    struct environment_event evt = {
        .temperature = temperature,
        .humidity = humidity,
        .pressure = pressure,
        .iaq = iaq
    };
    zbus_chan_pub(&environment_data_chan, &evt, K_MSEC(250));
}

int zsw_environment_sensor_init(void)
{
    if (!device_is_ready(bme688)) {
        LOG_ERR("No environment sensor found!");
        return -ENODEV;
    }

    zsw_periodic_chan_add_obs(&periodic_event_10s_chan, &zsw_environment_sensor_lis);

    return 0;
}

int zsw_environment_sensor_get(float *temperature, float *humidity, float *pressure)
{
    struct sensor_value sensor_val;

    if (!device_is_ready(bme688)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch(bme688) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bme688, SENSOR_CHAN_AMBIENT_TEMP, &sensor_val) != 0) {
        return -ENODATA;
    }
    *temperature = sensor_value_to_float(&sensor_val);

    if (sensor_channel_get(bme688, SENSOR_CHAN_HUMIDITY, &sensor_val) != 0) {
        return -ENODATA;
    }
    *humidity = sensor_value_to_float(&sensor_val);

    if (sensor_channel_get(bme688, SENSOR_CHAN_PRESS, &sensor_val) != 0) {
        return -ENODATA;
    }
    *pressure = sensor_value_to_float(&sensor_val);

    return 0;
}

int zsw_environment_sensor_get_iaq(float *iaq)
{
    struct sensor_value sensor_val;

    if (!device_is_ready(bme688)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch(bme688) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bme688, SENSOR_CHAN_IAQ, &sensor_val) != 0) {
        return -ENODATA;
    }

    *iaq = sensor_value_to_float(&sensor_val);

    return 0;
}

int zsw_environment_sensor_get_voc(float *voc)
{
    struct sensor_value sensor_val;

    if (!device_is_ready(bme688)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch(bme688) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bme688, SENSOR_CHAN_VOC, &sensor_val) != 0) {
        return -ENODATA;
    }

    *voc = sensor_value_to_float(&sensor_val);

    return 0;
}

int zsw_environment_sensor_get_co2(float *co2)
{
    struct sensor_value sensor_val;

    if (!device_is_ready(bme688)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch(bme688) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bme688, SENSOR_CHAN_CO2, &sensor_val) != 0) {
        return -ENODATA;
    }

    *co2 = sensor_value_to_float(&sensor_val);

    return 0;
}
