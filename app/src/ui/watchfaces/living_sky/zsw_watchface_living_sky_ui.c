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
#include "living_sky_fx.h"
#include "ui/watchfaces/zsw_watchface_anim.h"

LOG_MODULE_REGISTER(watchface_living_sky, LOG_LEVEL_INF);

#define BURST_DURATION_MS   6000

// Palette shared with the Digital Minimal watchface.
#define COLOR_ACCENT        0xFF8600    // Minutes, separators
#define COLOR_DAY           0x8C8C8C
#define COLOR_DATE          0xBBBBBB
#define COLOR_TEMP          0xDDDDDD
#define COLOR_HUMIDITY      0x60AEF7
#define COLOR_STEPS         0xB97CF5
#define COLOR_SEPARATOR     0x666666

ZSW_LV_IMG_DECLARE(ui_img_running_png);
ZSW_LV_IMG_DECLARE(ui_img_chat_png);
ZSW_LV_IMG_DECLARE(drop_icon);
ZSW_LV_IMG_DECLARE(face_living_sky_preview);
LV_FONT_DECLARE(ui_font_aliean_47);

static void watchface_ui_invalidate_cached(void);
static void reset_cached_values(void);
static void frame_ready_cb(lv_event_t *e);

static lv_obj_t *root_page;
static lv_obj_t *ui_time[3];
static lv_obj_t *ui_time_shadow[3];
static lv_obj_t *ui_date;
static lv_obj_t *ui_weather_icon;
static lv_obj_t *ui_weather_temp;
static lv_obj_t *ui_humidity;
static lv_obj_t *ui_battery;
static lv_obj_t *ui_steps;
static lv_obj_t *ui_notif_icon;
static lv_obj_t *ui_notif_count;
static lv_obj_t *ui_notif_separator;
static lv_obj_t *ui_ble_dot;
static lv_timer_t *intro_timer;
static bool wake_pending;
static bool datetime_ready;
static bool face_active;

static int last_minute = -1;
static int last_hour = -1;
static int last_day = -1;
static int last_battery = -1;
static int last_steps = -1;
static int last_num_notifications = -1;
static int last_temperature = -1000;
static int last_humidity = -1;
static int last_weather_code = -1;

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
    // Do not interrupt the time intro with a full-screen weather redraw.
    if (face_active && !wake_pending && !intro_timer) {
        living_sky_fx_play(BURST_DURATION_MS);
    }
}

static void watchface_show(lv_obj_t *parent, watchface_app_evt_listener evt_cb, zsw_settings_watchface_t *settings)
{
    ARG_UNUSED(evt_cb);
    ARG_UNUSED(settings);

    lv_obj_remove_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    root_page = lv_obj_create(parent);
    reset_cached_values();
    face_active = true;

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

    // Separate hour, colon and minute labels, like Digital Minimal. Their
    // shadows follow the same small invalidated areas instead of moving HH:MM
    // as one wide label while the whole weather layer is also being redrawn.
    lv_obj_t *shadow_row = create_row(root_page, -16);
    lv_obj_align(shadow_row, LV_ALIGN_CENTER, 2, -16);
    lv_obj_set_style_pad_column(shadow_row, 0, 0);
    lv_obj_t *time_row = create_row(root_page, -18);
    lv_obj_set_style_pad_column(time_row, 0, 0);
    for (int i = 0; i < ARRAY_SIZE(ui_time); i++) {
        const char *text = i == 1 ? ":" : "--";
        ui_time_shadow[i] = create_row_label(shadow_row, &ui_font_aliean_47, 0, text);
        lv_obj_set_style_text_opa(ui_time_shadow[i], LV_OPA_60, 0);
        ui_time[i] = create_row_label(time_row, &ui_font_aliean_47,
                                      i == 0 ? 0xFFFFFF : COLOR_ACCENT, text);
    }

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
    lv_obj_set_style_pad_column(stats_row, 3, 0);
    create_row_icon(stats_row, ZSW_LV_IMG_USE(ui_img_running_png), COLOR_STEPS);
    ui_steps = create_row_label(stats_row, &lv_font_montserrat_10, COLOR_STEPS, "0");
    create_row_label(stats_row, &lv_font_montserrat_14, COLOR_SEPARATOR, "|");
    ui_notif_icon = create_row_icon(stats_row, ZSW_LV_IMG_USE(ui_img_chat_png), 0xFFFFFF);
    ui_notif_count = create_label(ui_notif_icon, &lv_font_montserrat_10, lv_color_white(), -3, -2);
    ui_notif_separator = create_row_label(stats_row, &lv_font_montserrat_14, COLOR_SEPARATOR, "|");
    lv_obj_add_flag(ui_notif_icon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_notif_separator, LV_OBJ_FLAG_HIDDEN);

    ui_battery = lv_image_create(stats_row);
    lv_image_set_src(ui_battery, zsw_ui_utils_battery_icon(100));

    ui_ble_dot = lv_obj_create(stats_row);
    lv_obj_remove_style_all(ui_ble_dot);
    lv_obj_set_size(ui_ble_dot, 7, 7);
    lv_obj_set_style_radius(ui_ble_dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ui_ble_dot, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(ui_ble_dot, LV_OPA_COVER, 0);

    lv_display_add_event_cb(lv_obj_get_display(root_page), frame_ready_cb, LV_EVENT_REFR_READY, root_page);
}

static void watchface_remove(void)
{
    if (!root_page) {
        return;
    }

    wake_pending = false;
    face_active = false;
    if (intro_timer) {
        lv_timer_delete(intro_timer);
        intro_timer = NULL;
    }
    lv_display_remove_event_cb_with_user_data(lv_obj_get_display(root_page), frame_ready_cb, root_page);
    living_sky_fx_delete();
    lv_obj_delete(root_page);
    root_page = NULL;
}

// Keep each digit group and its shadow together without an opacity layer.
static void time_intro_exec(void *label, int32_t progress)
{
    for (int i = 0; i < ARRAY_SIZE(ui_time); i++) {
        if (label != ui_time[i]) {
            continue;
        }
        int32_t offset = 24 - (24 * progress) / LV_OPA_COVER;
        lv_obj_set_style_translate_y(ui_time[i], offset, 0);
        lv_obj_set_style_translate_y(ui_time_shadow[i], offset, 0);
        lv_obj_set_style_text_opa(ui_time[i], progress, 0);
        lv_obj_set_style_text_opa(ui_time_shadow[i], (LV_OPA_60 * progress) / LV_OPA_COVER, 0);
    }
}

static void intro_finished_cb(lv_timer_t *timer)
{
    intro_timer = NULL;
    lv_timer_delete(timer);
    if (root_page && face_active) {
        LOG_INF("Wake intro done");
        living_sky_fx_play(BURST_DURATION_MS);
    }
}

static void frame_ready_cb(lv_event_t *e)
{
    ARG_UNUSED(e);
    if (!wake_pending || !datetime_ready || !face_active) {
        return;
    }
    // The first full-screen redraw is finished. Start the intro's clock here,
    // so panel resume and initial rendering cannot consume its first frames.
    wake_pending = false;
    LOG_INF("Wake intro start");
    for (int i = 0; i < ARRAY_SIZE(ui_time); i++) {
        lv_anim_t anim;
        lv_anim_init(&anim);
        lv_anim_set_var(&anim, ui_time[i]);
        lv_anim_set_exec_cb(&anim, time_intro_exec);
        lv_anim_set_values(&anim, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_anim_set_duration(&anim, ZSW_WATCHFACE_ANIM_RISE_MS);
        lv_anim_set_delay(&anim, i * 60);
        lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
        lv_anim_start(&anim);
    }
    zsw_watchface_anim_rise(ui_date, 200);
    // Give the last date frame a refresh before starting full-screen effects.
    intro_timer = lv_timer_create(intro_finished_cb, ZSW_WATCHFACE_ANIM_RISE_MS + 200 + 80, NULL);
}

static void watchface_wake(void)
{
    if (!root_page || wake_pending || intro_timer) {
        return;
    }
    face_active = true;
    wake_pending = true;
    datetime_ready = false;
    living_sky_fx_stop();
    for (int i = 0; i < ARRAY_SIZE(ui_time); i++) {
        lv_anim_delete(ui_time[i], time_intro_exec);
        time_intro_exec(ui_time[i], LV_OPA_TRANSP);
    }
    zsw_watchface_anim_reset_label(ui_date);
}

static void watchface_set_display_active(bool active)
{
    if (!root_page) {
        return;
    }
    face_active = active;
    if (active) {
        watchface_wake();
    } else {
        living_sky_fx_stop();
        wake_pending = false;
        if (intro_timer) {
            lv_timer_delete(intro_timer);
            intro_timer = NULL;
        }
        for (int i = 0; i < ARRAY_SIZE(ui_time); i++) {
            lv_anim_delete(ui_time[i], time_intro_exec);
            time_intro_exec(ui_time[i], LV_OPA_COVER);
        }
        lv_anim_delete(ui_date, NULL);
        lv_obj_set_style_translate_y(ui_date, 0, 0);
        lv_obj_set_style_text_opa(ui_date, LV_OPA_COVER, 0);
    }
}

static void watchface_set_battery_percent(int32_t percent, int32_t battery)
{
    ARG_UNUSED(battery);

    if (!root_page || percent == last_battery) {
        return;
    }

    last_battery = percent;
    lv_image_set_src(ui_battery, zsw_ui_utils_battery_icon(percent));
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
    if (number > 0) {
        lv_label_set_text_fmt(ui_notif_count, "%d", (int)number);
        lv_obj_remove_flag(ui_notif_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(ui_notif_separator, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui_notif_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_notif_separator, LV_OBJ_FLAG_HIDDEN);
    }
}

static void watchface_set_ble_connected(bool connected)
{
    if (!root_page) {
        return;
    }

    lv_obj_set_style_bg_color(ui_ble_dot, lv_color_hex(connected ? 0x0082FC : 0x333333), 0);
}

static void watchface_set_weather(const watchface_weather_t *weather)
{
    lv_color_t icon_color;
    const void *icon;

    bool changed = living_sky_fx_set_weather(weather->weather_code, weather->wind, weather->wind_direction);

    if (!root_page) {
        return;
    }

    if (weather->temperature_c != last_temperature) {
        last_temperature = weather->temperature_c;
        lv_label_set_text_fmt(ui_weather_temp, "%d°", weather->temperature_c);
    }
    if (weather->humidity != last_humidity) {
        last_humidity = weather->humidity;
        lv_label_set_text_fmt(ui_humidity, "%d%%", weather->humidity);
    }
    if (weather->weather_code != last_weather_code) {
        last_weather_code = weather->weather_code;
        icon = zsw_ui_utils_icon_from_weather_code(weather->weather_code, &icon_color);
        lv_image_set_src(ui_weather_icon, icon);
        lv_obj_set_style_image_recolor_opa(ui_weather_icon, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_image_recolor(ui_weather_icon, icon_color, LV_PART_MAIN);
        lv_obj_remove_flag(ui_weather_icon, LV_OBJ_FLAG_HIDDEN);
    }

    // Show off the new weather right away.
    if (changed && face_active && !wake_pending && !intro_timer && !living_sky_fx_is_playing()) {
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
        lv_label_set_text_fmt(ui_time[0], "%02d", (int)hour);
        lv_label_set_text_fmt(ui_time_shadow[0], "%02d", (int)hour);
        lv_label_set_text_fmt(ui_time[2], "%02d", (int)minute);
        lv_label_set_text_fmt(ui_time_shadow[2], "%02d", (int)minute);
        living_sky_fx_set_time(hour, minute);
    }

    if (date != last_day) {
        last_day = date;
        lv_label_set_text_fmt(ui_date, "#%06X %s# #%06X |# %d %s", (unsigned int)COLOR_DAY, days[day_of_week % 7],
                              (unsigned int)COLOR_ACCENT, date, months[month % 12]);
    }

    datetime_ready = true;
}

static void watchface_set_watch_env_sensors(int pressure)
{
    ARG_UNUSED(pressure);
}

static void reset_cached_values(void)
{
    datetime_ready = false;
    last_minute = -1;
    last_hour = -1;
    last_day = -1;
    last_battery = -1;
    last_steps = -1;
    last_num_notifications = -1;
    last_temperature = -1000;
    last_humidity = -1;
    last_weather_code = -1;
}

static void watchface_ui_invalidate_cached(void)
{
    // refresh_ui rereads the values; retain caches so unchanged labels do not
    // trigger extra redraws on wake. The intro waits for a fresh clock update.
    datetime_ready = false;
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
