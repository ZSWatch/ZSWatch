/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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

static void zbus_periodic_10s_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(pressure_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(zsw_pressure_sensor_perioidc_lis, zbus_periodic_10s_callback);
static const struct device *const bmp581 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bmp581));

static void zbus_periodic_10s_callback(const struct zbus_channel *chan)
{
    float pressure;
    float temperature;

    if (zsw_pressure_sensor_get_pressure(&pressure) || zsw_pressure_sensor_get_temperature(&temperature)) {
        return;
    }

    struct pressure_event evt = {
        .pressure = pressure,
        .temperature = temperature
    };
    zbus_chan_pub(&pressure_data_chan, &evt, K_MSEC(250));
}

int zsw_pressure_sensor_init(void)
{
    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    zsw_periodic_chan_add_obs(&periodic_event_10s_chan, &zsw_pressure_sensor_perioidc_lis);

    zsw_pressure_sensor_set_odr(BOSCH_BMP581_ODR_DEFAULT);

    return 0;
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

int zsw_pressure_sensor_get_temperature(float *temperature)
{
    struct sensor_value sensor_val;

    if (!device_is_ready(bmp581)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch(bmp581) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bmp581, SENSOR_CHAN_DIE_TEMP, &sensor_val) != 0) {
        return -ENODATA;
    }

    *temperature = sensor_value_to_float(&sensor_val);

    return 0;
}