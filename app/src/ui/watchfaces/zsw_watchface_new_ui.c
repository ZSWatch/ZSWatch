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

#include <lvgl.h>

#include "ui/zsw_ui.h"
#include "applications/watchface/watchface_app.h"

#ifdef __ZEPHYR__
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(watchface_digital, LOG_LEVEL_WRN);
#endif

#include "748_2_dial/748_2_dial.h"

static void watchface_ui_invalidate_cached(void);
static void arc_event_pressed(lv_event_t *e);

static lv_obj_t *root_page = NULL;

// Remember last values as if no change then
// no reason to waste resourses and redraw
static int last_weekday = -1;
static int last_day = -1;
static int last_month = -1;
static int last_year = -1;

static watchface_app_evt_listener ui_evt_cb;

static void watchface_show(watchface_app_evt_listener evt_cb, zsw_settings_watchface_t *settings)
{
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
    root_page = lv_obj_create(lv_scr_act());

    init_face_748_2_dial();
}

static void watchface_remove(void)
{
    if (!root_page) {
        return;
    }
    lv_obj_del(root_page);
    root_page = NULL;
}

static void watchface_set_battery_percent(int32_t percent, int32_t value)
{
    if (!root_page) {
        return;
    }
}

static void watchface_set_hrm(int32_t value)
{
    if (!root_page) {
        return;
    }
}

static void watchface_set_step(int32_t value)
{
    if (!root_page) {
        return;
    }
}

static void watchface_set_time(int32_t hour, int32_t minute, int32_t second, uint32_t usec)
{
    if (!root_page) {
        return;
    }

    update_time_748_2_dial(second, minute, hour, true, true, last_day, last_month, last_year, last_weekday);
}

static void watchface_set_num_notifcations(int32_t value)
{
    if (!root_page) {
        return;
    }
}

static void watchface_set_ble_connected(bool connected)
{
    if (!root_page) {
        return;
    }
}

static void watchface_set_weather(int8_t temperature, int weather_code)
{
    if (!root_page) {
        return;
    }
}

static void watchface_set_shortdate(int day_of_week, int date)
{
    if (!root_page) {
        return;
    }
}

static void watchface_set_fulldate(int day, int month, int year, int weekday)
{
    if (!root_page) {
        return;
    }

    last_day = day;
    last_month = month;
    last_year = year;
    last_weekday = weekday;
}

static void watchface_set_watch_env_sensors(int temperature, int humidity, int pressure, float iaq, float co2)
{
    if (!root_page) {
        return;
    }
}

static void watchface_ui_invalidate_cached(void)
{
}

static void arc_event_pressed(lv_event_t *e)
{
}

static watchface_ui_api_t ui_api = {
    .show = watchface_show,
    .remove = watchface_remove,
    .set_battery_percent = watchface_set_battery_percent,
    .set_hrm = watchface_set_hrm,
    .set_step = watchface_set_step,
    .set_time = watchface_set_time,
    .set_ble_connected = watchface_set_ble_connected,
    .set_num_notifcations = watchface_set_num_notifcations,
    .set_weather = watchface_set_weather,
    .set_shortdate = watchface_set_shortdate,
    .set_fulldate = watchface_set_fulldate,
    .set_watch_env_sensors = watchface_set_watch_env_sensors,
    .ui_invalidate_cached = watchface_ui_invalidate_cached,
};

static int watchface_init(void)
{
    watchface_app_register_ui(&ui_api);

    return 0;
}

SYS_INIT(watchface_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
