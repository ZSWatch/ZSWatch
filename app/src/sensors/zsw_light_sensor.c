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

#include "events/zsw_periodic_event.h"
#include "events/light_event.h"
#include "sensors/zsw_light_sensor.h"

LOG_MODULE_REGISTER(zsw_light_sensor, CONFIG_ZSW_SENSORS_LOG_LEVEL);

static void zbus_periodic_slow_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(light_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(zsw_light_sensor_lis, zbus_periodic_slow_callback);
static const struct device *const apds9306 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(apds9306));

static void zbus_periodic_slow_callback(const struct zbus_channel *chan)
{
    float light = 0.0;

    if (zsw_light_sensor_get_light(&light)) {
        return;
    }

    struct light_event evt = {
        .light = light,
    };
    zbus_chan_pub(&light_data_chan, &evt, K_MSEC(250));
}

int zsw_light_sensor_init(void)
{
    if (!device_is_ready(apds9306)) {
        LOG_ERR("No light sensor found!");

        return -ENODEV;
    }

    zsw_periodic_chan_add_obs(&periodic_event_10s_chan, &zsw_light_sensor_lis);

    return 0;
}

int zsw_light_sensor_get_light(float *light)
{
    struct sensor_value sensor_val;

    if (!device_is_ready(apds9306)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch(apds9306) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(apds9306, SENSOR_CHAN_LIGHT, &sensor_val) != 0) {
        return -ENODATA;
    }

    *light = sensor_value_to_float(&sensor_val);

    return 0;
}
