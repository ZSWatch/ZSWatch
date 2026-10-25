/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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
#include "living_sky_fx.h"

LOG_MODULE_REGISTER(watchface_living_sky, LOG_LEVEL_WRN);

#define BURST_DURATION_MS   (CONFIG_WATCHFACE_LIVING_SKY_ANIMATION_SECONDS * 1000)

// Palette shared with the Digital Minimal watchface.
#define COLOR_ACCENT        0xFF8600    // Minutes, separators
#define COLOR_DAY           0x8C8C8C
#define COLOR_DATE          0xBBBBBB
#define COLOR_TEMP          0xDDDDDD
#define COLOR_HUMIDITY      0x60AEF7
#define COLOR_STEPS         0xB97CF5
#define COLOR_SEPARATOR     0x666666
#define COLOR_BATTERY_TEXT  0x888888
#define COLOR_BATTERY_OK    0x4CAF50
#define COLOR_BATTERY_LOW   0xFF8600
#define COLOR_BATTERY_EMPTY 0xE53935

ZSW_LV_IMG_DECLARE(ui_img_running_png);
ZSW_LV_IMG_DECLARE(drop_icon);
ZSW_LV_IMG_DECLARE(face_living_sky_preview);
LV_FONT_DECLARE(ui_font_aliean_47);

static void watchface_ui_invalidate_cached(void);

static lv_obj_t *root_page;
static lv_obj_t *ui_time_shadow;
static lv_obj_t *ui_time;
static lv_obj_t *ui_date;
static lv_obj_t *ui_weather_icon;
static lv_obj_t *ui_weather_temp;
static lv_obj_t *ui_humidity;
static lv_obj_t *ui_battery;
static lv_obj_t *ui_steps;
static zsw_ui_notification_area_t *ui_notifications_area;

static int last_minute = -1;
static int last_hour = -1;
static int last_day = -1;
static int last_battery = -1;
static int last_steps = -1;
static int last_num_notifications = -1;

static lv_obj_t *create_label(lv_obj_t *parent, const lv_font_t *font, lv_color_t color, int32_t x, int32_t y)
{
    lv_obj_t *label = lv_label_create(parent);

    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
    lv_label_set_text(label, "");

    return label;
}

static lv_obj_t *create_row(lv_obj_t *parent, int32_t y)
{
    lv_obj_t *row = lv_obj_create(parent);

    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(row, LV_ALIGN_CENTER, 0, y);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row, 5, LV_PART_MAIN);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    return row;
}

static lv_obj_t *create_row_label(lv_obj_t *row, const lv_font_t *font, uint32_t color, const char *text)
{
    lv_obj_t *label = lv_label_create(row);

    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN);
    lv_label_set_text(label, text);

    return label;
}

static lv_obj_t *create_row_icon(lv_obj_t *row, const void *src, uint32_t color)
{
    lv_obj_t *icon = lv_image_create(row);

    lv_image_set_src(icon, src);
    lv_obj_set_style_image_recolor(icon, lv_color_hex(color), LV_PART_MAIN);
    lv_obj_set_style_image_recolor_opa(icon, LV_OPA_COVER, LV_PART_MAIN);

    return icon;
}

static void root_clicked_cb(lv_event_t *e)
{
    ARG_UNUSED(e);
    living_sky_fx_play(BURST_DURATION_MS);
}

static void watchface_show(lv_obj_t *parent, watchface_app_evt_listener evt_cb, zsw_settings_watchface_t *settings)
{
    ARG_UNUSED(evt_cb);
    ARG_UNUSED(settings);

    lv_obj_remove_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    root_page = lv_obj_create(parent);
    watchface_ui_invalidate_cached();

    lv_obj_remove_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(root_page, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, 240, 240);
    lv_obj_align(root_page, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(root_page, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_image_src(root_page, watchface_app_get_bg(), LV_PART_MAIN);
    // Tap anywhere on the face replays the weather animation.
    lv_obj_add_event_cb(root_page, root_clicked_cb, LV_EVENT_CLICKED, NULL);

    // Sky tint and weather effects, between the background and the text.
    living_sky_fx_create(root_page);

    ui_notifications_area = zsw_ui_notification_area_add(root_page);
    lv_obj_set_pos(ui_notifications_area->ui_notifications_container, 0, -96);

    // A dark copy behind the time keeps it readable on bright frames (sun, snow, lightning).
    ui_time_shadow = create_label(root_page, &ui_font_aliean_47, lv_color_black(), 2, -16);
    lv_obj_set_style_text_opa(ui_time_shadow, LV_OPA_60, LV_PART_MAIN);
    // White hour, orange colon and minutes, set with inline colour markup.
    ui_time = create_label(root_page, &ui_font_aliean_47, lv_color_white(), 0, -18);
    lv_label_set_recolor(ui_time, true);

    // "SUN | 25 OCT": grey day, orange separator, light date.
    ui_date = create_label(root_page, &lv_font_montserrat_16, lv_color_hex(COLOR_DATE), 0, 18);
    lv_label_set_recolor(ui_date, true);

    lv_obj_t *weather_row = create_row(root_page, 47);
    ui_weather_icon = create_row_icon(weather_row, NULL, 0xFFFFFF);
    lv_obj_add_flag(ui_weather_icon, LV_OBJ_FLAG_HIDDEN);
    ui_weather_temp = create_row_label(weather_row, &lv_font_montserrat_16, COLOR_TEMP, "--°");
    create_row_icon(weather_row, ZSW_LV_IMG_USE(drop_icon), COLOR_HUMIDITY);
    ui_humidity = create_row_label(weather_row, &lv_font_montserrat_16, COLOR_HUMIDITY, "--%");

    lv_obj_t *stats_row = create_row(root_page, 80);
    create_row_icon(stats_row, ZSW_LV_IMG_USE(ui_img_running_png), COLOR_STEPS);
    ui_steps = create_row_label(stats_row, &lv_font_montserrat_14, COLOR_STEPS, "0");
    create_row_label(stats_row, &lv_font_montserrat_14, COLOR_SEPARATOR, "|");
    ui_battery = create_row_label(stats_row, &lv_font_montserrat_14, COLOR_BATTERY_TEXT, "");
    lv_label_set_recolor(ui_battery, true);

    living_sky_fx_play(BURST_DURATION_MS);
}

static void watchface_remove(void)
{
    if (!root_page) {
        return;
    }

    living_sky_fx_delete();
    lv_obj_delete(root_page);
    root_page = NULL;
}

static void watchface_set_display_active(bool active)
{
    if (!root_page) {
        return;
    }

    if (active) {
        living_sky_fx_play(BURST_DURATION_MS);
    } else {
        living_sky_fx_stop();
    }
}

static void watchface_set_battery_percent(int32_t percent, int32_t battery)
{
    ARG_UNUSED(battery);

    if (!root_page || percent == last_battery) {
        return;
    }

    last_battery = percent;
    const char *symbol = percent > 80 ? LV_SYMBOL_BATTERY_FULL : percent > 55 ? LV_SYMBOL_BATTERY_3 :
                         percent > 30 ? LV_SYMBOL_BATTERY_2 : percent > 10 ? LV_SYMBOL_BATTERY_1 : LV_SYMBOL_BATTERY_EMPTY;
    uint32_t color = percent > 20 ? COLOR_BATTERY_OK : percent > 10 ? COLOR_BATTERY_LOW : COLOR_BATTERY_EMPTY;
    lv_label_set_text_fmt(ui_battery, "#%06X %s# %d%%", (unsigned int)color, symbol, (int)percent);
}

static void watchface_set_hrm(int32_t bpm, int32_t oxygen)
{
    ARG_UNUSED(bpm);
    ARG_UNUSED(oxygen);
}

static void watchface_set_step(int32_t steps, int32_t distance, int32_t kcal)
{
    ARG_UNUSED(distance);
    ARG_UNUSED(kcal);

    if (!root_page || steps == last_steps) {
        return;
    }

    last_steps = steps;
    lv_label_set_text_fmt(ui_steps, "%d", steps);
}

static void watchface_set_num_notifcations(int32_t number)
{
    if (!root_page || number == last_num_notifications) {
        return;
    }

    last_num_notifications = number;
    zsw_ui_notification_area_num_notifications(ui_notifications_area, number);
}

static void watchface_set_ble_connected(bool connected)
{
    if (!root_page) {
        return;
    }

    zsw_ui_notification_area_ble_connected(ui_notifications_area, connected);
}

static void watchface_set_weather(const watchface_weather_t *weather)
{
    lv_color_t icon_color;
    const void *icon;

    bool changed = living_sky_fx_set_weather(weather->weather_code, weather->wind, weather->wind_direction);

    if (!root_page) {
        return;
    }

    lv_label_set_text_fmt(ui_weather_temp, "%d°", weather->temperature_c);
    lv_label_set_text_fmt(ui_humidity, "%d%%", weather->humidity);
    icon = zsw_ui_utils_icon_from_weather_code(weather->weather_code, &icon_color);
    lv_image_set_src(ui_weather_icon, icon);
    lv_obj_set_style_image_recolor_opa(ui_weather_icon, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_image_recolor(ui_weather_icon, icon_color, LV_PART_MAIN);
    lv_obj_remove_flag(ui_weather_icon, LV_OBJ_FLAG_HIDDEN);

    // Show off the new weather right away.
    if (changed) {
        living_sky_fx_play(BURST_DURATION_MS);
    }
}

static void watchface_set_datetime(int day_of_week, int date, int day, int month, int year, int weekday, int32_t hour,
                                   int32_t minute, int32_t second, uint32_t usec, bool am, bool mode)
{
    static const char *const days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
    static const char *const months[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

    ARG_UNUSED(day);
    ARG_UNUSED(year);
    ARG_UNUSED(weekday);
    ARG_UNUSED(second);
    ARG_UNUSED(usec);
    ARG_UNUSED(am);
    ARG_UNUSED(mode);

    if (!root_page) {
        return;
    }

    if (hour != last_hour || minute != last_minute) {
        last_hour = hour;
        last_minute = minute;
        lv_label_set_text_fmt(ui_time, "%02d#%06X :%02d#", (int)hour, (unsigned int)COLOR_ACCENT, (int)minute);
        lv_label_set_text_fmt(ui_time_shadow, "%02d:%02d", (int)hour, (int)minute);
        living_sky_fx_set_time(hour, minute);
    }

    if (date != last_day) {
        last_day = date;
        lv_label_set_text_fmt(ui_date, "#%06X %s# #%06X |# %d %s", (unsigned int)COLOR_DAY, days[day_of_week % 7],
                              (unsigned int)COLOR_ACCENT, date, months[month % 12]);
    }
}

static void watchface_set_watch_env_sensors(int pressure)
{
    ARG_UNUSED(pressure);
}

static void watchface_ui_invalidate_cached(void)
{
    last_minute = -1;
    last_hour = -1;
    last_day = -1;
    last_battery = -1;
    last_steps = -1;
    last_num_notifications = -1;
}

static const void *watchface_get_preview_img(void)
{
    return ZSW_LV_IMG_USE(face_living_sky_preview);
}

static void watchface_set_bg(const void *bg_img)
{
    if (root_page && lv_obj_is_valid(root_page)) {
        lv_obj_set_style_bg_image_src(root_page, bg_img, LV_PART_MAIN);
    }
}

static watchface_ui_api_t ui_api = {
    .show = watchface_show,
    .remove = watchface_remove,
    .set_battery_percent = watchface_set_battery_percent,
    .set_hrm = watchface_set_hrm,
    .set_step = watchface_set_step,
    .set_ble_connected = watchface_set_ble_connected,
    .set_num_notifcations = watchface_set_num_notifcations,
    .set_weather_detailed = watchface_set_weather,
    .set_display_active = watchface_set_display_active,
    .set_datetime = watchface_set_datetime,
    .set_watch_env_sensors = watchface_set_watch_env_sensors,
    .ui_invalidate_cached = watchface_ui_invalidate_cached,
    .set_watchface_bg = watchface_set_bg,
    .get_preview_img = watchface_get_preview_img,
    .name = "Living Sky",
};

static int watchface_init(void)
{
    watchface_app_register_ui(&ui_api);

    return 0;
}

SYS_INIT(watchface_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
