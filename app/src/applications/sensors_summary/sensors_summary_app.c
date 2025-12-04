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

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <math.h>

#include "sensors_summary_ui.h"
#include "sensors/zsw_pressure_sensor.h"
#include "sensors/zsw_light_sensor.h"
#include "sensors/zsw_environment_sensor.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

static void sensors_summary_app_start(lv_obj_t *root, lv_group_t *group);
static void sensors_summary_app_stop(void);
static void on_close_sensors_summary(void);
static void on_ref_set(void);

static void timer_callback(lv_timer_t *timer);

ZSW_LV_IMG_DECLARE(move);

static application_t app = {
    .name = "Sensor",
    .icon = ZSW_LV_IMG_USE(move),
    .start_func = sensors_summary_app_start,
    .stop_func = sensors_summary_app_stop,
    .category = ZSW_APP_CATEGORY_SENSORS,
};

static lv_timer_t *refresh_timer;
static float relative_pressure;

static void sensors_summary_app_start(lv_obj_t *root, lv_group_t *group)
{
    sensors_summary_ui_show(root, on_close_sensors_summary, on_ref_set);

    // Set inital relative pressure.
    on_ref_set();

    // Increase ODR since we want high accuracy here.
    zsw_pressure_sensor_set_odr(BOSCH_BMP581_ODR_160_HZ);
    refresh_timer = lv_timer_create(timer_callback, CONFIG_APPLICATIONS_CONFIGURATION_SENSORS_SUMMARY_REFRESH_INTERVAL_MS,
                                    NULL);
}

static void sensors_summary_app_stop(void)
{
    // Cleanup after ourselves
    // TODO need mechanism so that multiple user can request ODR without
    // breaking for another when changed.
    zsw_pressure_sensor_set_odr(BOSCH_BMP581_ODR_DEFAULT);
    lv_timer_del(refresh_timer);
    sensors_summary_ui_remove();
}

static double get_relative_height_m(double relative_pressure, double new_pressure, double temperature)
{
    return ((powf((relative_pressure / new_pressure), 1.f / 5.257f) - 1.f) * (temperature + 273.15f)) / 0.0065f;
}

static void timer_callback(lv_timer_t *timer)
{
    float temperature = 0.0;
    float pressure = 0.0;
    float humidity = 0.0;
    float light = -1.0;
    float iaq = -1.0;
    float co2 = -1.0;

    zsw_environment_sensor_get(&temperature, &humidity, &pressure);
    zsw_environment_sensor_get_iaq(&iaq);
    zsw_environment_sensor_get_co2(&co2);
    zsw_pressure_sensor_get_pressure(&pressure);
    zsw_light_sensor_get_light(&light);

    sensors_summary_ui_set_pressure(pressure);
    sensors_summary_ui_set_temp(temperature);
    sensors_summary_ui_set_humidity(humidity);
    sensors_summary_ui_set_iaq(iaq);
    sensors_summary_ui_set_co2(co2);
    sensors_summary_ui_set_light(light);
    sensors_summary_ui_set_rel_height(get_relative_height_m(relative_pressure, pressure, temperature));
}

static void on_close_sensors_summary(void)
{
    zsw_app_manager_app_close_request(&app);
}

static void on_ref_set(void)
{
    zsw_pressure_sensor_get_pressure(&relative_pressure);
}

static int sensors_summary_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(sensors_summary_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
