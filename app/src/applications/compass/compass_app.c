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
#include <zephyr/logging/log.h>

#include "compass_ui.h"
#include "ui/popup/zsw_popup_window.h"
#include "zsw_magnetometer.h"
#include "managers/zsw_app_manager.h"
#include "sensor_fusion/zsw_sensor_fusion.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(compass_app, LOG_LEVEL_DBG);

// Functions needed for all applications
static void compass_app_start(lv_obj_t *root, lv_group_t *group);
static void compass_app_stop(void);

// Functions related to app functionality
static void timer_callback(lv_timer_t *timer);
static void on_start_calibration(void);

ZSW_LV_IMG_DECLARE(move);

static application_t app = {
    .name = "Compass",
    .icon = ZSW_LV_IMG_USE(move),
    .start_func = compass_app_start,
    .stop_func = compass_app_stop,
    .category = ZSW_APP_CATEGORY_SENSORS,
};

static lv_timer_t *refresh_timer;
static bool is_calibrating;
static uint32_t cal_start_ms;

static void compass_app_start(lv_obj_t *root, lv_group_t *group)
{
    compass_ui_show(root, on_start_calibration);
    refresh_timer = lv_timer_create(timer_callback, CONFIG_APPLICATIONS_CONFIGURATION_COMPASS_REFRESH_INTERVAL_MS,  NULL);
    zsw_sensor_fusion_init();
}

static void compass_app_stop(void)
{
    if (refresh_timer) {
        lv_timer_del(refresh_timer);
    }
    compass_ui_remove();
    zsw_magnetometer_stop_calibration();
    zsw_sensor_fusion_deinit();
    if (is_calibrating) {
        zsw_popup_remove();
    }
}

static void on_start_calibration(void)
{
    zsw_magnetometer_start_calibration();
    is_calibrating = true;
    cal_start_ms = lv_tick_get();
    zsw_popup_show("Calibration",
                   "Rotate the watch 360 degrees\naround each x,y,z.\n a few times.", NULL,
                   CONFIG_APPLICATIONS_CONFIGURATION_COMPASS_CALIBRATION_TIME_S, false);
}

static void timer_callback(lv_timer_t *timer)
{
    float heading;
    if (is_calibrating &&
        (lv_tick_elaps(cal_start_ms) >= (CONFIG_APPLICATIONS_CONFIGURATION_COMPASS_CALIBRATION_TIME_S * 1000UL))) {
        zsw_magnetometer_stop_calibration();
        is_calibrating = false;
        zsw_popup_remove();
    }
    if (!is_calibrating) {
        zsw_sensor_fusion_get_heading(&heading);
        compass_ui_set_heading(heading);
    }
}

static int compass_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(compass_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
