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

#include <stdio.h>
#include <lvgl.h>
#include <zephyr/logging/log.h>

#include "ui/zsw_ui.h"
#include "applications/watchface/watchface_app.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(watchface_horizon, LOG_LEVEL_WRN);

/* ── Forward declarations ──────────────────────────────────────────────── */
static void watchface_ui_invalidate_cached(void);
static void on_tap(lv_event_t *e);
static void watchface_set_battery_percent(int32_t percent, int32_t voltage);
static void watchface_set_step(int32_t steps, int32_t distance, int32_t kcal);
static void watchface_set_weather(int8_t temperature, int weather_code);
static void watchface_set_ble_connected(bool connected);
static void watchface_set_num_notifcations(int32_t count);
static void watchface_set_music(const char *track, const char *artist);
static void watchface_set_watch_env_sensors(int pressure);

/* ── Root ──────────────────────────────────────────────────────────────── */
static lv_obj_t *root_page;

/* ── Time ──────────────────────────────────────────────────────────────── */
static lv_obj_t *ui_hour_label;
static lv_obj_t *ui_colon_label;
static lv_obj_t *ui_min_label;

/* ── Date row (below horizon) ──────────────────────────────────────────── */
static lv_obj_t *ui_day_label;
static lv_obj_t *ui_date_label;

/* ── Data section (below horizon) ──────────────────────────────────────── */
static lv_obj_t *ui_batt_val;
static lv_obj_t *ui_batt_lbl;
static lv_obj_t *ui_steps_val;
static lv_obj_t *ui_steps_lbl;
static lv_obj_t *ui_temp_val;
static lv_obj_t *ui_temp_lbl;

/* ── Status row ────────────────────────────────────────────────────────── */
static lv_obj_t *ui_ble_dot;
static lv_obj_t *ui_notif_badge;

/* ── Music bar ─────────────────────────────────────────────────────────── */
static lv_obj_t *ui_music_row;
static lv_obj_t *ui_music_label;

/* ── Pressure ──────────────────────────────────────────────────────────── */
static lv_obj_t *ui_pressure_label;

/* ── Image declarations ────────────────────────────────────────────────── */
ZSW_LV_IMG_DECLARE(face_digital_v2_preview);  /* reuse preview for now */

/* ── Custom Inter ExtraLight fonts ─────────────────────────────────────── */
LV_FONT_DECLARE(ui_font_inter_thin_60);

/* ── State cache ───────────────────────────────────────────────────────── */
static int  last_hour        = -1;
static int  last_minute      = -1;
static int  last_second      = -1;
static int  last_date        = -1;
static int  last_day_of_week = -1;
static bool colon_visible    = true;

static watchface_app_evt_listener ui_evt_cb;

/* ── Color constants (matching the HTML mockup) ────────────────────────── */
#define HZ_COLOR_SUNSET_ORANGE   lv_color_hex(0xFF8C28)
#define HZ_COLOR_SUNSET_YELLOW   lv_color_hex(0xFFC850)
#define HZ_COLOR_SKY_DARK        lv_color_hex(0x020412)
#define HZ_COLOR_SKY_MID         lv_color_hex(0x0C1A40)
#define HZ_COLOR_GROUND          lv_color_hex(0x060A14)
#define HZ_COLOR_WEEKDAY         lv_color_hex(0xFFA03C)
#define HZ_COLOR_DATE            lv_color_hex(0x606070)
#define HZ_COLOR_BATT_OK         lv_color_hex(0x00DC78)
#define HZ_COLOR_BATT_LOW        lv_color_hex(0xFF503C)
#define HZ_COLOR_STEPS           lv_color_hex(0x6496FF)
#define HZ_COLOR_TEMP            lv_color_hex(0xFFC850)
#define HZ_COLOR_DATA_LBL        lv_color_hex(0xFFFFFF)
#define HZ_COLOR_DATA_LBL_OPA    50
#define HZ_COLOR_BLE_ON          lv_color_hex(0x008CFF)
#define HZ_COLOR_BLE_OFF         lv_color_hex(0x222228)
#define HZ_COLOR_NOTIF_ON        lv_color_hex(0xFFAA1E)
#define HZ_COLOR_NOTIF_BG        lv_color_hex(0x332810)
#define HZ_COLOR_NOTIF_OFF       lv_color_hex(0x222228)
#define HZ_COLOR_MUSIC           lv_color_hex(0x50D282)
#define HZ_COLOR_PRESSURE        lv_color_hex(0xFFFFFF)
#define HZ_COLOR_PRESSURE_OPA    30
#define HZ_COLOR_COLON_DIM       lv_color_hex(0xFFA03C)

/* ── Helpers ───────────────────────────────────────────────────────────── */

static void make_container(lv_obj_t *obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
}

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
    ui_evt_cb = evt_cb;

    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    root_page = lv_obj_create(parent);
    make_container(root_page);
    lv_obj_set_size(root_page, 240, 240);
    lv_obj_align(root_page, LV_ALIGN_CENTER, 0, 0);

    /* Set background image if available, otherwise use a solid dark color */
    if (global_watchface_bg_img) {
        lv_obj_set_style_bg_img_src(root_page, global_watchface_bg_img, LV_PART_MAIN);
    } else {
        lv_obj_set_style_bg_opa(root_page, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_bg_color(root_page, HZ_COLOR_SKY_DARK, LV_PART_MAIN);
    }

    watchface_ui_invalidate_cached();

    /*
     * Layout (vertical, center-aligned, horizon at y=0).
     * All sizes scaled from HTML mockup (480px → 240px = 0.5×):
     *   HTML 120px → 60px time, HTML 20px → 10px weekday,
     *   HTML 24px → 12px data-val, HTML 14px → 10px labels.
     *
     *   -42  Time (HH:MM) — Inter ExtraLight 60px
     *    -2  Horizon glow line
     *    +6  Date row (WEEKDAY · DD MMM YYYY)
     *   +26  Data row (Battery / Steps / Temp)
     *   +48  Status (BLE dot + notification badge)
     *   +60  Music bar
     *   +72  Pressure
     */

    /* ── Time row (above horizon)  y = -42 ── */
    lv_obj_t *time_row = make_row(root_page, -42);
    lv_obj_set_style_pad_column(time_row, 0, LV_PART_MAIN);

    ui_hour_label = lv_label_create(time_row);
    lv_label_set_text(ui_hour_label, "--");
    lv_obj_set_style_text_font(ui_hour_label, &ui_font_inter_thin_60, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_hour_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_hour_label, -1, LV_PART_MAIN);

    ui_colon_label = lv_label_create(time_row);
    lv_label_set_text(ui_colon_label, ":");
    lv_obj_set_style_text_font(ui_colon_label, &ui_font_inter_thin_60, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_colon_label, HZ_COLOR_COLON_DIM, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_colon_label, LV_OPA_20, LV_PART_MAIN);

    ui_min_label = lv_label_create(time_row);
    lv_label_set_text(ui_min_label, "--");
    lv_obj_set_style_text_font(ui_min_label, &ui_font_inter_thin_60, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_min_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_min_label, -1, LV_PART_MAIN);

    /* ── Horizon glow is baked into the background image ── */
    /* No LVGL widget needed — the gen_horizon_bg.py script renders
     * the full CSS linear-gradient + box-shadow into the .c image. */

    /* ── Date row (just below horizon)  y = +6 ── */
    lv_obj_t *date_row = make_row(root_page, 6);
    lv_obj_set_style_pad_column(date_row, 6, LV_PART_MAIN);

    ui_day_label = lv_label_create(date_row);
    lv_label_set_text(ui_day_label, "---");
    lv_obj_set_style_text_color(ui_day_label, HZ_COLOR_WEEKDAY, LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_day_label, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_day_label, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_day_label, 3, LV_PART_MAIN);

    /* Dot separator */
    lv_obj_t *dot = lv_obj_create(date_row);
    lv_obj_remove_style_all(dot);
    lv_obj_set_size(dot, 2, 2);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(dot, HZ_COLOR_WEEKDAY, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(dot, LV_OPA_20, LV_PART_MAIN);

    ui_date_label = lv_label_create(date_row);
    lv_label_set_text(ui_date_label, "-- --- ----");
    lv_obj_set_style_text_color(ui_date_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_date_label, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_date_label, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_date_label, 1, LV_PART_MAIN);

    /* ── Data section (below horizon) ── */

    /* Battery / Steps / Weather row  y = +26 */
    lv_obj_t *data_row = make_row(root_page, 26);
    lv_obj_set_style_pad_column(data_row, 12, LV_PART_MAIN);

    /* Battery item */
    lv_obj_t *batt_col = lv_obj_create(data_row);
    make_container(batt_col);
    lv_obj_set_size(batt_col, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(batt_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(batt_col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(batt_col, 1, LV_PART_MAIN);
    lv_obj_add_flag(batt_col, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(batt_col, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(batt_col, on_tap, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)WATCHFACE_APP_EVT_CLICK_BATT);

    ui_batt_val = lv_label_create(batt_col);
    lv_label_set_text(ui_batt_val, "--%");
    lv_obj_set_style_text_color(ui_batt_val, HZ_COLOR_BATT_OK, LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_batt_val, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_batt_val, LV_OPA_40, LV_PART_MAIN);

    ui_batt_lbl = lv_label_create(batt_col);
    lv_label_set_text(ui_batt_lbl, "BATTERY");
    lv_obj_set_style_text_color(ui_batt_lbl, HZ_COLOR_DATA_LBL, LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_batt_lbl, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_batt_lbl, HZ_COLOR_DATA_LBL_OPA, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_batt_lbl, 1, LV_PART_MAIN);

    /* Steps item */
    lv_obj_t *steps_col = lv_obj_create(data_row);
    make_container(steps_col);
    lv_obj_set_size(steps_col, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(steps_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(steps_col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(steps_col, 1, LV_PART_MAIN);
    lv_obj_add_flag(steps_col, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(steps_col, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(steps_col, on_tap, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)WATCHFACE_APP_EVT_CLICK_STEP);

    ui_steps_val = lv_label_create(steps_col);
    lv_label_set_text(ui_steps_val, "0");
    lv_obj_set_style_text_color(ui_steps_val, HZ_COLOR_STEPS, LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_steps_val, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_steps_val, LV_OPA_40, LV_PART_MAIN);

    ui_steps_lbl = lv_label_create(steps_col);
    lv_label_set_text(ui_steps_lbl, "STEPS");
    lv_obj_set_style_text_color(ui_steps_lbl, HZ_COLOR_DATA_LBL, LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_steps_lbl, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_steps_lbl, HZ_COLOR_DATA_LBL_OPA, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_steps_lbl, 1, LV_PART_MAIN);

    /* Weather/Temp item */
    lv_obj_t *temp_col = lv_obj_create(data_row);
    make_container(temp_col);
    lv_obj_set_size(temp_col, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(temp_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(temp_col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(temp_col, 1, LV_PART_MAIN);
    lv_obj_add_flag(temp_col, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(temp_col, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(temp_col, on_tap, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)WATCHFACE_APP_EVT_CLICK_WEATHER);

    ui_temp_val = lv_label_create(temp_col);
    lv_label_set_text(ui_temp_val, "--\xc2\xb0");
    lv_obj_set_style_text_color(ui_temp_val, HZ_COLOR_TEMP, LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_temp_val, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_temp_val, LV_OPA_30, LV_PART_MAIN);

    ui_temp_lbl = lv_label_create(temp_col);
    lv_label_set_text(ui_temp_lbl, "WEATHER");
    lv_obj_set_style_text_color(ui_temp_lbl, HZ_COLOR_DATA_LBL, LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_temp_lbl, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_temp_lbl, HZ_COLOR_DATA_LBL_OPA, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_temp_lbl, 1, LV_PART_MAIN);

    /* ── Status row (BLE + notif)  y = +48 ── */
    lv_obj_t *status_row = make_row(root_page, 48);
    lv_obj_set_style_pad_column(status_row, 8, LV_PART_MAIN);

    ui_ble_dot = lv_obj_create(status_row);
    lv_obj_remove_style_all(ui_ble_dot);
    lv_obj_set_size(ui_ble_dot, 5, 5);
    lv_obj_set_style_radius(ui_ble_dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_ble_dot, HZ_COLOR_BLE_OFF, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_ble_dot, LV_OPA_COVER, LV_PART_MAIN);

    ui_notif_badge = lv_label_create(status_row);
    lv_label_set_text(ui_notif_badge, "0");
    lv_obj_set_style_text_color(ui_notif_badge, HZ_COLOR_NOTIF_OFF, LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_notif_badge, &lv_font_montserrat_10, LV_PART_MAIN);

    /* ── Music row  y = +60 ── */
    ui_music_row = make_row(root_page, 60);
    lv_obj_add_flag(ui_music_row, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_music_row, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(ui_music_row, on_tap, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)WATCHFACE_APP_EVT_CLICK_MUSIC);

    lv_obj_t *note_icon = lv_label_create(ui_music_row);
    lv_label_set_text(note_icon, LV_SYMBOL_AUDIO);
    lv_obj_set_style_text_color(note_icon, HZ_COLOR_MUSIC, LV_PART_MAIN);
    lv_obj_set_style_text_opa(note_icon, LV_OPA_40, LV_PART_MAIN);
    lv_obj_set_style_text_font(note_icon, &lv_font_montserrat_10, LV_PART_MAIN);

    ui_music_label = lv_label_create(ui_music_row);
    lv_label_set_text(ui_music_label, "");
    lv_obj_set_width(ui_music_label, 140);
    lv_label_set_long_mode(ui_music_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_anim_duration(ui_music_label, 8000, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_music_label, HZ_COLOR_MUSIC, LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_music_label, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_music_label, LV_OPA_30, LV_PART_MAIN);

    /* ── Pressure  y = +72 ── */
    ui_pressure_label = lv_label_create(root_page);
    lv_label_set_text(ui_pressure_label, "");
    lv_obj_align(ui_pressure_label, LV_ALIGN_CENTER, 0, 72);
    lv_obj_set_style_text_color(ui_pressure_label, HZ_COLOR_PRESSURE, LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_pressure_label, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_pressure_label, HZ_COLOR_PRESSURE_OPA, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_pressure_label, 1, LV_PART_MAIN);
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

/* ── Callbacks ─────────────────────────────────────────────────────────── */

static void watchface_set_battery_percent(int32_t percent, int32_t battery)
{
    if (!root_page) {
        return;
    }
    lv_label_set_text_fmt(ui_batt_val, "%d%%", (int)percent);
    if (percent <= 15) {
        lv_obj_set_style_text_color(ui_batt_val, HZ_COLOR_BATT_LOW, LV_PART_MAIN);
    } else {
        lv_obj_set_style_text_color(ui_batt_val, HZ_COLOR_BATT_OK, LV_PART_MAIN);
    }
}

static void watchface_set_hrm(int32_t bpm, int32_t oxygen)
{
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
    lv_label_set_text_fmt(ui_steps_val, "%d", (int)steps);
}

static void watchface_set_ble_connected(bool connected)
{
    if (!root_page) {
        return;
    }
    if (connected) {
        lv_obj_set_style_bg_color(ui_ble_dot, HZ_COLOR_BLE_ON, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(ui_ble_dot, 6, LV_PART_MAIN);
        lv_obj_set_style_shadow_color(ui_ble_dot, HZ_COLOR_BLE_ON, LV_PART_MAIN);
        lv_obj_set_style_shadow_opa(ui_ble_dot, LV_OPA_40, LV_PART_MAIN);
    } else {
        lv_obj_set_style_bg_color(ui_ble_dot, HZ_COLOR_BLE_OFF, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(ui_ble_dot, 0, LV_PART_MAIN);
    }
}

static void watchface_set_num_notifcations(int32_t number)
{
    if (!root_page) {
        return;
    }
    if (number > 0) {
        lv_label_set_text_fmt(ui_notif_badge, "%d", (int)number);
        lv_obj_set_style_text_color(ui_notif_badge, HZ_COLOR_NOTIF_ON, LV_PART_MAIN);
    } else {
        lv_label_set_text(ui_notif_badge, "0");
        lv_obj_set_style_text_color(ui_notif_badge, HZ_COLOR_NOTIF_OFF, LV_PART_MAIN);
    }
}

static void watchface_set_weather(int8_t temperature, int weather_code)
{
    if (!root_page) {
        return;
    }
    (void)weather_code;
    lv_label_set_text_fmt(ui_temp_val, "%d\xc2\xb0", (int)temperature);
}

static void watchface_set_weather_full(int8_t temperature, int weather_code, uint16_t humidity)
{
    if (!root_page) {
        return;
    }
    watchface_set_weather(temperature, weather_code);
}

static void watchface_set_datetime(int day_of_week, int date, int day, int month, int year,
                                   int weekday, int32_t hour, int32_t minute, int32_t second,
                                   uint32_t usec, bool am, bool mode)
{
    static const char *const day_names[] = {
        "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"
    };
    static const char *const month_names[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    (void)day;
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
        int m = (month >= 0 && month < 12) ? month : 0;
        lv_label_set_text_fmt(ui_date_label, "%02d %s %d", date, month_names[m], year);
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
        colon_visible = !colon_visible;
        lv_obj_set_style_text_opa(ui_colon_label,
                                  colon_visible ? LV_OPA_60 : LV_OPA_10,
                                  LV_PART_MAIN);
    }
}

static void watchface_set_watch_env_sensors(int pressure)
{
    if (!root_page) {
        return;
    }
    if (pressure > 0) {
        lv_label_set_text_fmt(ui_pressure_label, "%d hPa", pressure);
    }
}

static void watchface_set_charging(bool is_charging)
{
    (void)is_charging;
}

static void watchface_set_music(const char *track, const char *artist)
{
    if (!root_page) {
        return;
    }
    if (!track || track[0] == '\0') {
        lv_obj_add_flag(ui_music_row, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    char buf[64];
    if (artist && artist[0] != '\0') {
        snprintf(buf, sizeof(buf), "%s - %s", track, artist);
    } else {
        snprintf(buf, sizeof(buf), "%s", track);
    }
    lv_label_set_text(ui_music_label, buf);
    lv_obj_clear_flag(ui_music_row, LV_OBJ_FLAG_HIDDEN);
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
    .show                  = watchface_show,
    .remove                = watchface_remove,
    .set_battery_percent   = watchface_set_battery_percent,
    .set_hrm               = watchface_set_hrm,
    .set_step              = watchface_set_step,
    .set_ble_connected     = watchface_set_ble_connected,
    .set_num_notifcations  = watchface_set_num_notifcations,
    .set_weather           = watchface_set_weather,
    .set_datetime          = watchface_set_datetime,
    .set_watch_env_sensors = watchface_set_watch_env_sensors,
    .set_charging          = watchface_set_charging,
    .ui_invalidate_cached  = watchface_ui_invalidate_cached,
    .set_watchface_bg      = watchface_set_bg,
    .get_preview_img       = watchface_get_preview_img,
    .set_weather_full      = watchface_set_weather_full,
    .set_music             = watchface_set_music,
    .name                  = "Horizon",
};

static int watchface_init(void)
{
    watchface_app_register_ui(&ui_api);
    return 0;
}

SYS_INIT(watchface_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
