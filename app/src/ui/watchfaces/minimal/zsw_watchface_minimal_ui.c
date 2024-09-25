
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

#include <zephyr/logging/log.h>

#include "ui/utils/zsw_ui_utils.h"
#include "applications/watchface/watchface_app.h"
#include "ui/watchfaces/zsw_ui_notification_area.h"

LOG_MODULE_REGISTER(watchface_minimal, LOG_LEVEL_WRN);

static void watchface_ui_invalidate_cached(void);

LV_IMG_DECLARE(hour_minimal);
LV_IMG_DECLARE(minute_minimal);
LV_IMG_DECLARE(second_minimal);
ZSW_LV_IMG_DECLARE(face_minimal_preview);

static zsw_ui_notification_area_t *zsw_ui_notifications_area;

static lv_obj_t *root_page;
static lv_obj_t *ui_minimal_watchface;
static lv_obj_t *ui_hour_img;
static lv_obj_t *ui_min_img;
static lv_obj_t *ui_day_data_label;
static lv_obj_t *ui_second_img;

// Remember last values as if no change then
// no reason to waste resourses and redraw
static int last_hour = -1;
static int last_minute = -1;
static int last_second = -1;
static int last_num_not = -1;

static void watchface_show(lv_obj_t *parent, watchface_app_evt_listener evt_cb, zsw_settings_watchface_t *settings)
{
    ARG_UNUSED(evt_cb);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    root_page = lv_obj_create(parent);
    watchface_ui_invalidate_cached();

    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, 240, 240);
    lv_obj_align(root_page, LV_ALIGN_CENTER, 0, 0);

    ui_minimal_watchface = root_page;
    lv_obj_clear_flag(ui_minimal_watchface, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_img_src(ui_minimal_watchface, global_watchface_bg_img, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_hour_img = lv_img_create(ui_minimal_watchface);
    lv_img_set_src(ui_hour_img, &hour_minimal);
    lv_obj_set_width(ui_hour_img, LV_SIZE_CONTENT);   /// 25
    lv_obj_set_height(ui_hour_img, LV_SIZE_CONTENT);    /// 89
    lv_obj_set_x(ui_hour_img, 0);
    lv_obj_set_y(ui_hour_img, -32);
    lv_obj_set_align(ui_hour_img, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_hour_img, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_pivot(ui_hour_img, 18, 82);
    lv_obj_set_style_img_recolor(ui_hour_img, lv_color_hex(0x0EA7FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_hour_img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_min_img = lv_img_create(ui_minimal_watchface);
    lv_img_set_src(ui_min_img, &minute_minimal);
    lv_obj_set_width(ui_min_img, LV_SIZE_CONTENT);   /// 12
    lv_obj_set_height(ui_min_img, LV_SIZE_CONTENT);    /// 104
    lv_obj_set_x(ui_min_img, 0);
    lv_obj_set_y(ui_min_img, -46);
    lv_obj_set_align(ui_min_img, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_min_img, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_pivot(ui_min_img, 11, 104);
    lv_obj_set_style_img_recolor(ui_min_img, lv_color_hex(0xF0FFD5), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_min_img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_day_data_label = lv_label_create(ui_minimal_watchface);
    lv_obj_set_width(ui_day_data_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_day_data_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_day_data_label, -10);
    lv_obj_set_y(ui_day_data_label, 0);
    lv_obj_set_align(ui_day_data_label, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_day_data_label, "WED 10");
    lv_obj_set_style_text_color(ui_day_data_label, lv_color_hex(0xCF9C60), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_day_data_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_second_img = lv_img_create(ui_minimal_watchface);
    lv_img_set_src(ui_second_img, &second_minimal);
    lv_obj_set_width(ui_second_img, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_second_img, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_second_img, -1);
    lv_obj_set_y(ui_second_img, -47);
    lv_obj_set_align(ui_second_img, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_second_img, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_second_img, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_pivot(ui_second_img, 4, 108);
    lv_obj_set_style_img_recolor(ui_second_img, lv_color_hex(0xFF4242), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_second_img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

#if defined(CONFIG_LV_Z_USE_FILESYSTEM)
    if (settings->animations_on) {
        lv_obj_t *img = lv_gif_create(ui_minimal_watchface);
        lv_gif_set_src(img, "/lvgl_lfs/snoopy_alt.gif");
        lv_obj_set_align(img, LV_ALIGN_CENTER);
        lv_obj_set_width(img, LV_SIZE_CONTENT);
        lv_obj_set_height(img, LV_SIZE_CONTENT);
        lv_obj_set_x(img, -10);
        lv_obj_set_y(img, 90);
    }
#endif

    zsw_ui_notifications_area = zsw_ui_notification_area_add(ui_minimal_watchface);
    lv_obj_set_pos(zsw_ui_notifications_area->ui_notifications_container, 0, 90);
}

static void watchface_remove(void)
{
    if (!root_page) {
        return;
    }
    lv_obj_del(root_page);
    root_page = NULL;
}

static void watchface_set_battery_percent(int32_t percent, int32_t battery)
{
}

static void watchface_set_hrm(int32_t bpm, int32_t oxygen)
{
}

static void watchface_set_step(int32_t steps, int32_t distance, int32_t kcal)
{
}

static void watchface_set_num_notifcations(int32_t number)
{
    if (!root_page) {
        return;
    }

    zsw_ui_notification_area_num_notifications(zsw_ui_notifications_area, number);
}

static void watchface_set_ble_connected(bool connected)
{
    if (!root_page) {
        return;
    }

    zsw_ui_notification_area_ble_connected(zsw_ui_notifications_area, connected);
}

static void watchface_set_weather(int8_t temperature, int weather_code)
{
}

static void watchface_set_datetime(int day_of_week, int date, int day, int month, int year, int weekday, int32_t hour,
                                   int32_t minute, int32_t second, uint32_t usec, bool am, bool mode)
{
    int hour_minute_offset;

    if (!root_page) {
        return;
    }
    char *days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

    lv_label_set_text_fmt(ui_day_data_label, "%s %d", days[day_of_week], date);

    hour = hour % 12;
    // Move hour hand with greater resolution than 12.
    hour_minute_offset = (int)((minute / 60.0) * (3600 / 12.0));
    last_hour = hour_minute_offset + hour * (3600 / 12);
    last_minute = minute * (3600 / 60);
    last_second = second * (3600 / 60);
    lv_img_set_angle(ui_hour_img, last_hour);
    lv_img_set_angle(ui_min_img, last_minute);

    last_second += lv_map(usec, 0, 999999, 0, 3600 / 60);
    lv_img_set_angle(ui_second_img, last_second);
}

static void watchface_set_watch_env_sensors(int temperature, int humidity, int pressure, float iaq, float co2)
{
}

static void watchface_ui_invalidate_cached(void)
{
    last_hour = -1;
    last_minute = -1;
    last_num_not = -1;
    last_second = -1;
}

static const void *watchface_get_preview_img(void)
{
    return ZSW_LV_IMG_USE(face_minimal_preview);
}

static watchface_ui_api_t ui_api = {
    .show = watchface_show,
    .remove = watchface_remove,
    .set_battery_percent = watchface_set_battery_percent,
    .set_hrm = watchface_set_hrm,
    .set_step = watchface_set_step,
    .set_ble_connected = watchface_set_ble_connected,
    .set_num_notifcations = watchface_set_num_notifcations,
    .set_weather = watchface_set_weather,
    .set_datetime = watchface_set_datetime,
    .set_watch_env_sensors = watchface_set_watch_env_sensors,
    .ui_invalidate_cached = watchface_ui_invalidate_cached,
    .get_preview_img = watchface_get_preview_img,
    .name = "Analog Minimal",
};

static int watchface_init(void)
{
    watchface_app_register_ui(&ui_api);

    return 0;
}

SYS_INIT(watchface_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
