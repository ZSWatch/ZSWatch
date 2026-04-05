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

#include <stdio.h>
#include <lvgl.h>
#include <zephyr/logging/log.h>

#include "ui/zsw_ui.h"
#include "applications/watchface/watchface_app.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(watchface_digital_v2, LOG_LEVEL_WRN);

/* ── Forward declarations ──────────────────────────────────────────────── */
static void watchface_ui_invalidate_cached(void);
static void on_tap(lv_event_t *e);
static void update_battery_display(int32_t percent, int32_t battery);
static void watchface_set_step(int32_t steps, int32_t distance, int32_t kcal);
static void watchface_set_battery_percent(int32_t percent, int32_t battery);
static void watchface_set_charging(bool is_charging);
static void watchface_set_num_notifcations(int32_t number);
static void watchface_set_ble_connected(bool connected);
static void watchface_set_weather_full(int8_t temperature, int weather_code, uint16_t humidity);
static void watchface_set_music(const char *track, const char *artist);

/* ── Root ──────────────────────────────────────────────────────────────── */
static lv_obj_t *root_page;

/* ── Steps outer ring ──────────────────────────────────────────────────── */
static lv_obj_t *ui_steps_ring;

/* ── Date row ──────────────────────────────────────────────────────────── */
static lv_obj_t *ui_day_label;
static lv_obj_t *ui_date_label;

/* ── Weather row ───────────────────────────────────────────────────────── */
static lv_obj_t *ui_weather_icon;
static lv_obj_t *ui_weather_temp_label;
static lv_obj_t *ui_weather_humidity_label;

/* ── Time ──────────────────────────────────────────────────────────────── */
static lv_obj_t *ui_hour_label;
static lv_obj_t *ui_colon_label;
static lv_obj_t *ui_min_label;

/* ── Stats row ─────────────────────────────────────────────────────────── */
static lv_obj_t *ui_steps_label;
static lv_obj_t *ui_notif_icon;
static lv_obj_t *ui_notif_count_label;
static lv_obj_t *ui_batt_icon;
static lv_obj_t *ui_batt_label;
static lv_obj_t *ui_ble_dot;

/* ── Music bar ─────────────────────────────────────────────────────────── */
static lv_obj_t *ui_music_bar;
static lv_obj_t *ui_music_label;

/* ── Image declarations ────────────────────────────────────────────────── */
ZSW_LV_IMG_DECLARE(ui_img_running_png);
ZSW_LV_IMG_DECLARE(ui_img_chat_png);
ZSW_LV_IMG_DECLARE(ui_img_charging_png);
ZSW_LV_IMG_DECLARE(ui_img_bluetooth_png);
ZSW_LV_IMG_DECLARE(drop_icon);
ZSW_LV_IMG_DECLARE(face_digital_v2_preview);

/* Battery fill-level icons (26×20, from pixel watchface) */
ZSW_LV_IMG_DECLARE(face_goog_20_61728_0);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_1);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_2);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_3);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_4);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_5);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_6);
ZSW_LV_IMG_DECLARE(light);

static const void *face_goog_battery[] = {
    ZSW_LV_IMG_USE(face_goog_20_61728_0),
    ZSW_LV_IMG_USE(face_goog_20_61728_1),
    ZSW_LV_IMG_USE(face_goog_20_61728_2),
    ZSW_LV_IMG_USE(face_goog_20_61728_3),
    ZSW_LV_IMG_USE(face_goog_20_61728_4),
    ZSW_LV_IMG_USE(face_goog_20_61728_5),
    ZSW_LV_IMG_USE(face_goog_20_61728_6),
};

LV_FONT_DECLARE(ui_font_aliean_47);
LV_FONT_DECLARE(ui_font_aliean_25);

/* ── State cache ───────────────────────────────────────────────────────── */
static int  last_hour        = -1;
static int  last_minute      = -1;
static int  last_second      = -1;
static int  last_date        = -1;
static int  last_day_of_week = -1;
static bool colon_visible    = true;
static bool use_relative_battery;
static bool cached_is_charging;
static int  cached_batt_percent;
static int  cached_batt_mv;

static watchface_app_evt_listener ui_evt_cb;

/* ── Helpers ───────────────────────────────────────────────────────────── */

/* Strip all padding/border/bg from a container so it is truly transparent. */
static void make_container(lv_obj_t *obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
}

/* Separator between stats items — use ASCII | (always in font). */
static void add_sep(lv_obj_t *parent)
{
    lv_obj_t *s = lv_label_create(parent);
    lv_label_set_text(s, "|");
    lv_obj_set_style_text_color(s, lv_color_hex(0x666666), LV_PART_MAIN);
    lv_obj_set_style_text_font(s, &lv_font_montserrat_12, LV_PART_MAIN);
}

/* Use icons at native 24×24 size — LVGL transform scale causes the flex
 * container to miscalculate content bounds, clipping the icons. */

static lv_obj_t *make_row(lv_obj_t *parent, int y_offset)
{
    lv_obj_t *row = lv_obj_create(parent);
    make_container(row);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(row, LV_ALIGN_CENTER, 0, y_offset);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row, 4, LV_PART_MAIN);
    return row;
}

/* ── show ──────────────────────────────────────────────────────────────── */

static void watchface_show(lv_obj_t *parent, watchface_app_evt_listener evt_cb,
                           zsw_settings_watchface_t *settings)
{
    use_relative_battery = true; /* layout requires percentage display */
    ui_evt_cb            = evt_cb;

    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    root_page = lv_obj_create(parent);
    make_container(root_page);
    lv_obj_set_size(root_page, 240, 240);
    lv_obj_align(root_page, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_img_src(root_page, global_watchface_bg_img, LV_PART_MAIN);

    watchface_ui_invalidate_cached();

    /* ── Steps ring (outermost, behind everything) ── */
    ui_steps_ring = lv_arc_create(root_page);
    lv_obj_set_size(ui_steps_ring, 242, 242);  /* slightly oversized so arc edges bleed off-screen */
    lv_obj_align(ui_steps_ring, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_rotation(ui_steps_ring, 270);       /* start at 12 o'clock */
    lv_arc_set_bg_angles(ui_steps_ring, 0, 360);   /* full circle track */
    lv_arc_set_range(ui_steps_ring, 0, 10000);
    lv_arc_set_value(ui_steps_ring, 0);
    lv_arc_set_mode(ui_steps_ring, LV_ARC_MODE_NORMAL);
    lv_obj_set_style_arc_color(ui_steps_ring, lv_color_hex(0x1a1a2a), LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_steps_ring, 5, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui_steps_ring, lv_color_hex(0x9D3BE0), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_steps_ring, 5, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(ui_steps_ring, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_steps_ring, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_clear_flag(ui_steps_ring, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_steps_ring, LV_OBJ_FLAG_EVENT_BUBBLE | LV_OBJ_FLAG_HIDDEN);

    /* ── Date row  y = -84 ── */
    lv_obj_t *date_row = make_row(root_page, -84);

    ui_day_label = lv_label_create(date_row);
    lv_label_set_text(ui_day_label, "---");
    lv_obj_set_style_text_color(ui_day_label, lv_color_hex(0x777777), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_day_label, &lv_font_montserrat_14, LV_PART_MAIN);

    lv_obj_t *dot = lv_label_create(date_row);
    lv_label_set_text(dot, "|");
    lv_obj_set_style_text_color(dot, lv_color_hex(0xFF8600), LV_PART_MAIN);
    lv_obj_set_style_text_font(dot, &lv_font_montserrat_14, LV_PART_MAIN);

    ui_date_label = lv_label_create(date_row);
    lv_label_set_text(ui_date_label, "-- ---");
    lv_obj_set_style_text_color(ui_date_label, lv_color_hex(0xbbbbbb), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_date_label, &lv_font_montserrat_14, LV_PART_MAIN);

    /* ── Weather row  y = -65 ── */
    lv_obj_t *weather_row = make_row(root_page, -65);
    lv_obj_add_flag(weather_row, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(weather_row, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(weather_row, on_tap, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)WATCHFACE_APP_EVT_CLICK_WEATHER);

    lv_color_t icon_color;
    const lv_img_dsc_t *w_icon = zsw_ui_utils_icon_from_weather_code(802, &icon_color);

    ui_weather_icon = lv_image_create(weather_row);
    lv_image_set_src(ui_weather_icon, w_icon);
    lv_obj_set_style_img_recolor(ui_weather_icon, icon_color, LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(ui_weather_icon, LV_OPA_COVER, LV_PART_MAIN);
    lv_image_set_scale(ui_weather_icon, 200);  /* 24→19 for weather row */

    ui_weather_temp_label = lv_label_create(weather_row);
    lv_label_set_text(ui_weather_temp_label, "--\xc2\xb0");   /* --° (UTF-8 degree) */
    lv_obj_set_style_text_color(ui_weather_temp_label, lv_color_hex(0xdddddd), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_weather_temp_label, &lv_font_montserrat_14, LV_PART_MAIN);

    lv_obj_t *humidity_icon = lv_image_create(weather_row);
    lv_image_set_src(humidity_icon, ZSW_LV_IMG_USE(drop_icon));
    lv_obj_set_style_img_recolor(humidity_icon, lv_color_hex(0x60AEF7), LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(humidity_icon, LV_OPA_COVER, LV_PART_MAIN);
    lv_image_set_scale(humidity_icon, 200);  /* 24→19 */
    lv_obj_set_style_pad_right(humidity_icon, -6, LV_PART_MAIN);

    ui_weather_humidity_label = lv_label_create(weather_row);
    lv_label_set_text(ui_weather_humidity_label, "--%");
    lv_obj_set_style_text_color(ui_weather_humidity_label, lv_color_hex(0x60AEF7), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_weather_humidity_label, &lv_font_montserrat_14, LV_PART_MAIN);

    /* ── Time row  y = -10  (HH:MM) ── */
    lv_obj_t *time_row = make_row(root_page, -10);
    lv_obj_set_style_pad_column(time_row, 0, LV_PART_MAIN);

    ui_hour_label = lv_label_create(time_row);
    lv_label_set_text(ui_hour_label, "--");
    lv_obj_set_style_text_font(ui_hour_label, &ui_font_aliean_47, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_hour_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    ui_colon_label = lv_label_create(time_row);
    lv_label_set_text(ui_colon_label, ":");
    lv_obj_set_style_text_font(ui_colon_label, &ui_font_aliean_47, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_colon_label, lv_color_hex(0xFF8600), LV_PART_MAIN);

    ui_min_label = lv_label_create(time_row);
    lv_label_set_text(ui_min_label, "--");
    lv_obj_set_style_text_font(ui_min_label, &ui_font_aliean_47, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_min_label, lv_color_hex(0xFF8600), LV_PART_MAIN);

    /* ── Stats row  y = +62  (steps | notif | battery | BLE) ── */
    lv_obj_t *stats_row = make_row(root_page, 62);
    lv_obj_set_style_pad_column(stats_row, 3, LV_PART_MAIN);

    /* Steps icon + label */
    lv_obj_t *steps_icon = lv_image_create(stats_row);
    lv_image_set_src(steps_icon, ZSW_LV_IMG_USE(ui_img_running_png));
    lv_obj_set_style_img_recolor(steps_icon, lv_color_hex(0xb97cf5), LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(steps_icon, LV_OPA_COVER, LV_PART_MAIN);
    /* Use native 24×24 size — no scaling in stats row */
    lv_obj_add_flag(steps_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(steps_icon, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(steps_icon, on_tap, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)WATCHFACE_APP_EVT_CLICK_STEP);

    ui_steps_label = lv_label_create(stats_row);
    lv_label_set_text(ui_steps_label, "0");
    lv_obj_set_style_text_color(ui_steps_label, lv_color_hex(0xb97cf5), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_steps_label, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_add_flag(ui_steps_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_steps_label, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(ui_steps_label, on_tap, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)WATCHFACE_APP_EVT_CLICK_STEP);

    add_sep(stats_row);

    /* Notification icon with count overlaid (hidden when 0) */
    ui_notif_icon = lv_image_create(stats_row);
    lv_image_set_src(ui_notif_icon, ZSW_LV_IMG_USE(ui_img_chat_png));
    lv_obj_set_style_img_recolor(ui_notif_icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(ui_notif_icon, LV_OPA_COVER, LV_PART_MAIN);

    ui_notif_count_label = lv_label_create(ui_notif_icon);
    lv_label_set_text(ui_notif_count_label, "");
    lv_obj_set_x(ui_notif_count_label, -3);
    lv_obj_set_y(ui_notif_count_label, -2);
    lv_obj_set_align(ui_notif_count_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_font(ui_notif_count_label, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_notif_count_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    add_sep(stats_row);

    /* Battery icon with dynamic fill level (26×20, tappable) */
    ui_batt_icon = lv_image_create(stats_row);
    lv_image_set_src(ui_batt_icon, face_goog_battery[6]);
    lv_obj_add_flag(ui_batt_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_batt_icon, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(ui_batt_icon, on_tap, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)WATCHFACE_APP_EVT_CLICK_BATT);

    ui_batt_label = lv_label_create(stats_row);
    lv_label_set_text(ui_batt_label, "");
    lv_obj_set_style_text_color(ui_batt_label, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_batt_label, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_add_flag(ui_batt_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_batt_label, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(ui_batt_label, on_tap, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)WATCHFACE_APP_EVT_CLICK_BATT);

    /* BLE dot — tiny colored circle */
    ui_ble_dot = lv_obj_create(stats_row);
    lv_obj_remove_style_all(ui_ble_dot);
    lv_obj_set_size(ui_ble_dot, 7, 7);
    lv_obj_set_style_radius(ui_ble_dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_ble_dot, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_ble_dot, LV_OPA_COVER, LV_PART_MAIN);

    /* ── Music bar  y = +85  (hidden until music plays) ── */
    ui_music_bar = lv_obj_create(root_page);
    make_container(ui_music_bar);
    lv_obj_set_size(ui_music_bar, 150, LV_SIZE_CONTENT);
    lv_obj_align(ui_music_bar, LV_ALIGN_CENTER, 0, 85);
    lv_obj_set_flex_flow(ui_music_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_music_bar, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(ui_music_bar, 4, LV_PART_MAIN);
    lv_obj_add_flag(ui_music_bar, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_music_bar, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(ui_music_bar, on_tap, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)WATCHFACE_APP_EVT_CLICK_MUSIC);

    ui_music_label = lv_label_create(ui_music_bar);
    lv_label_set_text(ui_music_label, "");
    lv_obj_set_width(ui_music_label, 120);
    lv_label_set_long_mode(ui_music_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_anim_duration(ui_music_label, 8000, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_music_label, lv_color_hex(0x4CAF50), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_music_label, &lv_font_montserrat_10, LV_PART_MAIN);

}

/* ── remove ────────────────────────────────────────────────────────────── */

static void watchface_remove(void)
{
    if (!root_page) {
        return;
    }
    lv_obj_del(root_page);
    root_page = NULL;
}

/* ── battery helper ────────────────────────────────────────────────────── */

static void update_battery_display(int32_t percent, int32_t battery)
{
    int idx;

    if (percent <= 10) {
        idx = 0;
    } else if (percent <= 20) {
        idx = 1;
    } else if (percent <= 40) {
        idx = 2;
    } else if (percent <= 60) {
        idx = 3;
    } else if (percent <= 80) {
        idx = 4;
    } else if (percent <= 90) {
        idx = 5;
    } else {
        idx = 6;
    }

    lv_image_set_src(ui_batt_icon, face_goog_battery[idx]);

    if (use_relative_battery) {
        lv_label_set_text_fmt(ui_batt_label, "%d%%", (int)percent);
    } else {
        lv_label_set_text_fmt(ui_batt_label, "%dmV", (int)battery);
    }
}

/* ── Callbacks ─────────────────────────────────────────────────────────── */

static void watchface_set_battery_percent(int32_t percent, int32_t battery)
{
    if (!root_page) {
        return;
    }
    cached_batt_percent = percent;
    cached_batt_mv      = battery;
    update_battery_display(percent, battery);
}

static void watchface_set_hrm(int32_t bpm, int32_t oxygen)
{
    /* HRM addon hardware not yet available — nothing to display */
    (void)bpm;
    (void)oxygen;
}

static void watchface_set_step(int32_t steps, int32_t distance, int32_t kcal)
{
    if (!root_page) {
        return;
    }
    (void)distance;
    (void)kcal;
    lv_arc_set_value(ui_steps_ring, steps);
    lv_label_set_text_fmt(ui_steps_label, "%d", (int)steps);

    if (steps >= 500) {
        lv_obj_clear_flag(ui_steps_ring, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui_steps_ring, LV_OBJ_FLAG_HIDDEN);
    }
}

static void watchface_set_ble_connected(bool connected)
{
    if (!root_page) {
        return;
    }
    lv_obj_set_style_bg_color(ui_ble_dot,
                              connected ? lv_color_hex(0x0082FC) : lv_color_hex(0x333333),
                              LV_PART_MAIN);
}

static void watchface_set_num_notifcations(int32_t number)
{
    if (!root_page) {
        return;
    }
    lv_label_set_text_fmt(ui_notif_count_label, "%d", (int)number);
    lv_obj_clear_flag(ui_notif_icon, LV_OBJ_FLAG_HIDDEN);
}

static void watchface_set_weather(int8_t temperature, int weather_code)
{
    if (!root_page) {
        return;
    }
    lv_color_t color;
    const lv_img_dsc_t *icon = zsw_ui_utils_icon_from_weather_code(weather_code, &color);

    lv_image_set_src(ui_weather_icon, icon);
    lv_obj_set_style_img_recolor(ui_weather_icon, color, LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(ui_weather_icon, LV_OPA_COVER, LV_PART_MAIN);
    lv_label_set_text_fmt(ui_weather_temp_label, "%d\xc2\xb0", (int)temperature);
}

static void watchface_set_weather_full(int8_t temperature, int weather_code, uint16_t humidity)
{
    if (!root_page) {
        return;
    }
    watchface_set_weather(temperature, weather_code);
    lv_label_set_text_fmt(ui_weather_humidity_label, "%d%%", (int)humidity);
}

static void watchface_set_datetime(int day_of_week, int date, int day, int month, int year,
                                   int weekday, int32_t hour, int32_t minute, int32_t second,
                                   uint32_t usec, bool am, bool mode)
{
    static const char *const day_names[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
    static const char *const month_names[] = {
        "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
        "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
    };

    (void)day;
    (void)year;
    (void)weekday;
    (void)usec;
    (void)am;
    (void)mode;

    if (!root_page) {
        return;
    }

    if (last_day_of_week != day_of_week) {
        lv_label_set_text(ui_day_label, day_names[day_of_week]);
        last_day_of_week = day_of_week;
    }
    if (last_date != date || last_date == -1) {
        /* month is 0-based from struct tm */
        int m = (month >= 0 && month < 12) ? month : 0;
        lv_label_set_text_fmt(ui_date_label, "%d %s", date, month_names[m]);
        last_date = date;
    }
    if (last_hour != hour) {
        lv_label_set_text_fmt(ui_hour_label, "%02d", (int)hour);
        last_hour = hour;
    }
    if (last_minute != minute) {
        lv_label_set_text_fmt(ui_min_label, "%02d", (int)minute);
        last_minute = minute;
    }
    if (last_second != second) {
        last_second = second;

        /* Blink colon at 1 Hz: alternate between white and orange */
        colon_visible = !colon_visible;
        lv_obj_set_style_text_color(ui_colon_label,
                                    colon_visible ? lv_color_hex(0xFFFFFF)
                                    : lv_color_hex(0xFF8600),
                                    LV_PART_MAIN);
    }
}

static void watchface_set_watch_env_sensors(int pressure)
{
    /* On-watch pressure/temperature not displayed in this watchface */
    (void)pressure;
}

static void watchface_set_charging(bool is_charging)
{
    if (!root_page) {
        return;
    }
    cached_is_charging = is_charging;
    update_battery_display(cached_batt_percent, cached_batt_mv);
}

static void watchface_set_music(const char *track, const char *artist)
{
    if (!root_page) {
        return;
    }
    if (!track || track[0] == '\0') {
        lv_obj_add_flag(ui_music_bar, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    char buf[64];
    if (artist && artist[0] != '\0') {
        snprintf(buf, sizeof(buf), "%s - %s", track, artist);
    } else {
        snprintf(buf, sizeof(buf), "%s", track);
    }
    lv_label_set_text(ui_music_label, buf);
    lv_obj_clear_flag(ui_music_bar, LV_OBJ_FLAG_HIDDEN);
}

static void watchface_ui_invalidate_cached(void)
{
    last_hour        = -1;
    last_minute      = -1;
    last_second      = -1;
    last_date        = -1;
    last_day_of_week = -1;
    colon_visible    = true;
}

static const void *watchface_get_preview_img(void)
{
    return ZSW_LV_IMG_USE(face_digital_v2_preview);
}

static void on_tap(lv_event_t *e)
{
    watchface_app_evt_open_app_t app = (watchface_app_evt_open_app_t)(uintptr_t)lv_event_get_user_data(e);

    ui_evt_cb((watchface_app_evt_t) {
        .type     = WATCHFACE_APP_EVENT_OPEN_APP,
        .data.app = app,
    });
}

static void watchface_set_bg(const void *bg_img)
{
    if (root_page && lv_obj_is_valid(root_page)) {
        lv_obj_set_style_bg_img_src(root_page, bg_img, LV_PART_MAIN);
    }
}

/* ── Registration ──────────────────────────────────────────────────────── */

static watchface_ui_api_t ui_api = {
    .show                 = watchface_show,
    .remove               = watchface_remove,
    .set_battery_percent  = watchface_set_battery_percent,
    .set_hrm              = watchface_set_hrm,
    .set_step             = watchface_set_step,
    .set_ble_connected    = watchface_set_ble_connected,
    .set_num_notifcations = watchface_set_num_notifcations,
    .set_weather          = watchface_set_weather,
    .set_datetime         = watchface_set_datetime,
    .set_watch_env_sensors = watchface_set_watch_env_sensors,
    .set_charging         = watchface_set_charging,
    .ui_invalidate_cached = watchface_ui_invalidate_cached,
    .set_watchface_bg     = watchface_set_bg,
    .get_preview_img      = watchface_get_preview_img,
    .set_weather_full     = watchface_set_weather_full,
    .set_music            = watchface_set_music,
    .name                 = "ZSWatch Digital v2",
};

static int watchface_init(void)
{
    watchface_app_register_ui(&ui_api);
    return 0;
}

SYS_INIT(watchface_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
