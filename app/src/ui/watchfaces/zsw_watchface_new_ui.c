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

static void watchface_ui_invalidate_cached(void);
static void arc_event_pressed(lv_event_t *e);

static lv_obj_t *root_page = NULL;
static lv_obj_t *ui_digital_watchface;
static lv_obj_t *ui_iaq_or_pressure_arc;
#ifdef CONFIG_EXTERNAL_USE_BOSCH_BSEC
static lv_obj_t *ui_co2_arc;
static lv_obj_t *ui_iaq_co2_text_image;
#else
static lv_obj_t *ui_pressure_image;
#endif
static lv_obj_t *ui_humidity_arc;
static lv_obj_t *ui_humidity_icon;
static lv_obj_t *ui_watch_temperature_label;
static lv_obj_t *ui_time;
static lv_obj_t *ui_min_label;
static lv_obj_t *ui_colon_label;
static lv_obj_t *ui_hour_label;
static lv_obj_t *ui_sec_label;
static lv_obj_t *ui_battery_arc;
static lv_obj_t *ui_battery_arc_icon;
static lv_obj_t *ui_battery_percent_label;
static lv_obj_t *ui_step_arc;
static lv_obj_t *ui_step_arc_icon;
static lv_obj_t *ui_step_arc_label;
static lv_obj_t *ui_top_panel;
static lv_obj_t *ui_day_label;
static lv_obj_t *ui_date_label;
static lv_obj_t *ui_notifications;
static lv_obj_t *ui_notification_icon;
static lv_obj_t *ui_notification_count_label;
static lv_obj_t *ui_bt_icon;
static lv_obj_t *ui_weather_temperature_label;
static lv_obj_t *ui_weather_icon;

LV_IMG_DECLARE(ui_img_iaq_co2_text);    // assets/air_quality.png
ZSW_LV_IMG_DECLARE(ui_img_pressure_png);    // assets/pressure.png
ZSW_LV_IMG_DECLARE(ui_img_temperatures_png);    // assets/temperatures.png
ZSW_LV_IMG_DECLARE(ui_img_charging_png);    // assets/charging.png
ZSW_LV_IMG_DECLARE(ui_img_running_png);    // assets/running.png
ZSW_LV_IMG_DECLARE(ui_img_chat_png);    // assets/chat.png
ZSW_LV_IMG_DECLARE(ui_img_bluetooth_png);    // assets/bluetooth.png

LV_FONT_DECLARE(ui_font_aliean_47);
LV_FONT_DECLARE(ui_font_aliean_25);

// Remember last values as if no change then
// no reason to waste resourses and redraw
static int last_hour = -1;
static int last_minute = -1;
static int last_second = -1;
static int last_num_not = -1;

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

static void watchface_set_date(int day_of_week, int date)
{
    if (!root_page) {
        return;
    }
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
    if (lv_event_get_target(e) == ui_battery_arc) {
        ui_evt_cb(WATCHFACE_APP_EVT_CLICK_BATT);
    } else if (lv_event_get_target(e) == ui_step_arc) {
        ui_evt_cb(WATCHFACE_APP_EVT_CLICK_STEP);
    }
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
    .set_date = watchface_set_date,
    .set_watch_env_sensors = watchface_set_watch_env_sensors,
    .ui_invalidate_cached = watchface_ui_invalidate_cached,
};

static int watchface_init(void)
{
    watchface_app_register_ui(&ui_api);

    return 0;
}

SYS_INIT(watchface_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
