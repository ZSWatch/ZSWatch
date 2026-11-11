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

#include <info/info_ui.h>
#include <lvgl.h>
#include <utils/zsw_ui_utils.h>
#include <zsw_coredump.h>
#include "assert.h"
#include <stdio.h>
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"

static void seconds_to_time_chunks(uint32_t time_seconds, int *days, int *hours, int *minutes, int *seconds);

#define DEBUG_COLOR         lv_color_hex(0x9B51C4)
#define INFO_LIST_WIDTH     172
#define COREDUMP_BTN_SIZE   40

static lv_obj_t *root_page = NULL;
static on_reset_ui_event_cb_t reset_callback;
static zsw_coredump_sumary_t cached_coredumps[ZSW_COREDUMP_MAX_STORED];
static int num_cached_coredumps;

// System page
static lv_obj_t *uptime_label;
static lv_obj_t *resets_label;
static lv_obj_t *runtime_label;
static lv_obj_t *wake_time_label;
static lv_obj_t *disp_pwr_off_label;
static lv_obj_t *inactive_time_label;

// Bluetooth page
static lv_obj_t *mac_addr_label;
static lv_obj_t *ble_remote_addr_label;
static lv_obj_t *ble_remote_info_label;
static lv_obj_t *ble_gatt_status_label;
static lv_obj_t *ble_security_status_label;

// Coredump page
static lv_obj_t *coredump_dropdown;
static lv_obj_t *coredump_info_label;
static lv_obj_t *download_btn;
static lv_obj_t *erase_btn;
static lv_obj_t *crash_btn;

static void reset_btn_pressed(lv_event_t *e)
{
    if (reset_callback) {
        reset_callback();
    }
}

static void create_page_info_ui(lv_obj_t *parent)
{
    lv_obj_t *screen = zsw_ui_screen_create(parent, DEBUG_COLOR);
    lv_obj_t *list = zsw_ui_info_list_create(screen, INFO_LIST_WIDTH);

    zsw_ui_title_create(screen, "SYSTEM", ZSW_COLOR_TEXT_DIM);

    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 48);
    lv_obj_set_style_pad_row(list, 4, LV_PART_MAIN);

    // Counted since the watch last lost power
    resets_label = zsw_ui_info_row_create(list, "Resets", false);
    uptime_label = zsw_ui_info_row_create(list, "Uptime", false);
    runtime_label = zsw_ui_info_row_create(list, "Total uptime", false);
    wake_time_label = zsw_ui_info_row_create(list, "Screen on", false);
    disp_pwr_off_label = zsw_ui_info_row_create(list, "Stationary", false);
    inactive_time_label = zsw_ui_info_row_create(list, "Inactive in", false);

    lv_obj_t *reset_btn = lv_button_create(screen);
    lv_obj_set_size(reset_btn, 96, 34);
    lv_obj_set_style_bg_color(reset_btn, ZSW_COLOR_SURFACE_HIGH, LV_PART_MAIN);
    lv_obj_align(reset_btn, LV_ALIGN_BOTTOM_MID, 0, -28);
    lv_obj_add_event_cb(reset_btn, reset_btn_pressed, LV_EVENT_CLICKED, NULL);

    lv_obj_t *reset_label = lv_label_create(reset_btn);
    lv_label_set_text(reset_label, "Reset");
    lv_obj_center(reset_label);
}

static lv_obj_t *create_ble_label(lv_obj_t *parent, const char *text, bool header)
{
    lv_obj_t *label = lv_label_create(parent);

    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, ZSW_FONT_CAPTION, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, header ? ZSW_COLOR_TEXT_DIM : ZSW_COLOR_TEXT, LV_PART_MAIN);
    if (header) {
        lv_obj_set_style_pad_top(label, 6, LV_PART_MAIN);
    }

    return label;
}

static void create_page_ble_ui(lv_obj_t *parent)
{
    lv_obj_t *screen = zsw_ui_screen_create(parent, DEBUG_COLOR);

    zsw_ui_title_create(screen, "BLUETOOTH", ZSW_COLOR_TEXT_DIM);

    lv_obj_t *column = lv_obj_create(screen);
    lv_obj_remove_style_all(column);
    lv_obj_set_size(column, 200, LV_SIZE_CONTENT);
    lv_obj_align(column, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_flex_flow(column, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(column, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(column, 2, LV_PART_MAIN);
    lv_obj_remove_flag(column, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    create_ble_label(column, "Watch address", true);
    mac_addr_label = create_ble_label(column, "-", false);
    create_ble_label(column, "Phone address", true);
    ble_remote_addr_label = create_ble_label(column, "-", false);
    create_ble_label(column, "Connection", true);
    ble_remote_info_label = create_ble_label(column, "-", false);
    ble_security_status_label = create_ble_label(column, "-", false);
    ble_gatt_status_label = create_ble_label(column, "-", false);
}

static void show_coredump(int index)
{
    if (index < num_cached_coredumps) {
        lv_label_set_text_fmt(coredump_info_label, "%s\n%s\nLine %d", cached_coredumps[index].datetime,
                              cached_coredumps[index].file, cached_coredumps[index].line);
    } else {
        lv_label_set_text(coredump_info_label, "No coredumps");
    }
}

static void on_coredump_button_pressed(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    if (btn == download_btn) {
        zsw_coredump_to_log();
    } else if (btn == erase_btn) {
        lv_dropdown_clear_options(coredump_dropdown); // TODO Handle when more than one
        num_cached_coredumps = 0;
        show_coredump(0);
        zsw_coredump_erase(0);
    } else if (btn == crash_btn) {
        __ASSERT(0, "User created ASSERT");
    }
}

static void coredump_dropdown_handler(lv_event_t *e)
{
    show_coredump(lv_dropdown_get_selected(lv_event_get_target(e)));
}

static lv_obj_t *create_coredump_button(lv_obj_t *parent, const char *symbol, lv_color_t color, int32_t x)
{
    lv_obj_t *btn = zsw_ui_round_button_create(parent, symbol, COREDUMP_BTN_SIZE, color);

    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, x, -30);
    lv_obj_add_event_cb(btn, on_coredump_button_pressed, LV_EVENT_CLICKED, NULL);

    return btn;
}

static void create_coredump_page_ui(lv_obj_t *parent)
{
    lv_obj_t *screen = zsw_ui_screen_create(parent, DEBUG_COLOR);

    zsw_ui_title_create(screen, "COREDUMP", ZSW_COLOR_TEXT_DIM);

    coredump_dropdown = lv_dropdown_create(screen);
    lv_dropdown_clear_options(coredump_dropdown);
    for (int i = 0; i < num_cached_coredumps; i++) {
        lv_dropdown_add_option(coredump_dropdown, cached_coredumps[i].datetime, i);
    }
    lv_obj_set_size(coredump_dropdown, 164, 32);
    lv_obj_align(coredump_dropdown, LV_ALIGN_TOP_MID, 0, 44);
    lv_obj_set_style_radius(coredump_dropdown, ZSW_RADIUS_PILL, LV_PART_MAIN);
    lv_obj_set_style_bg_color(coredump_dropdown, ZSW_COLOR_SURFACE_HIGH, LV_PART_MAIN);
    lv_obj_set_style_border_width(coredump_dropdown, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_ver(coredump_dropdown, 7, LV_PART_MAIN);
    lv_obj_set_style_pad_hor(coredump_dropdown, 14, LV_PART_MAIN);
    lv_obj_add_event_cb(coredump_dropdown, coredump_dropdown_handler, LV_EVENT_VALUE_CHANGED, NULL);

    // Where it crashed: time, file and line
    lv_obj_t *card = lv_obj_create(screen);
    lv_obj_set_size(card, 184, 66);
    lv_obj_align(card, LV_ALIGN_TOP_MID, 0, 82);
    lv_obj_set_style_pad_all(card, 8, LV_PART_MAIN);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    coredump_info_label = lv_label_create(card);
    lv_obj_set_width(coredump_info_label, LV_PCT(100));
    lv_label_set_long_mode(coredump_info_label, LV_LABEL_LONG_DOT);
    lv_obj_set_height(coredump_info_label, 3 * lv_font_get_line_height(ZSW_FONT_CAPTION));
    lv_obj_set_style_text_font(coredump_info_label, ZSW_FONT_CAPTION, LV_PART_MAIN);
    lv_obj_set_style_text_color(coredump_info_label, ZSW_COLOR_TEXT, LV_PART_MAIN);
    lv_obj_set_style_text_align(coredump_info_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_center(coredump_info_label);
    show_coredump(0);

    // Print to the log, erase, and crash on purpose to get a coredump
    download_btn = create_coredump_button(screen, LV_SYMBOL_DOWNLOAD, ZSW_COLOR_ACCENT, -48);
    erase_btn = create_coredump_button(screen, LV_SYMBOL_TRASH, ZSW_COLOR_ORANGE, 0);
    crash_btn = create_coredump_button(screen, LV_SYMBOL_WARNING, ZSW_COLOR_RED, 48);
}

void info_ui_show(lv_obj_t *root, on_reset_ui_event_cb_t reset_cb, zsw_coredump_sumary_t *coredumps, int num_coredumps)
{
    assert(root_page == NULL);

    reset_callback = reset_cb;
    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_remove_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *tv = lv_tileview_create(root_page);
    lv_obj_set_size(tv, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(tv, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    num_cached_coredumps = num_coredumps;
    memcpy(cached_coredumps, coredumps, num_coredumps * sizeof(zsw_coredump_sumary_t));

    create_coredump_page_ui(lv_tileview_add_tile(tv, 0, 0, LV_DIR_BOTTOM));
    create_page_info_ui(lv_tileview_add_tile(tv, 0, 1, LV_DIR_VER));
    create_page_ble_ui(lv_tileview_add_tile(tv, 0, 2, LV_DIR_TOP));

    // Tells that the pages are stacked vertically
    lv_obj_t *scroll_icon = lv_img_create(root_page);
    lv_img_set_src(scroll_icon, "S:scroll_icon.bin");
    lv_obj_align(scroll_icon, LV_ALIGN_RIGHT_MID, -6, 0);
}

void info_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void info_ui_set_uptime_sec(uint32_t uptime_seconds)
{
    int days;
    int hours;
    int minutes;
    int seconds;

    seconds_to_time_chunks(uptime_seconds, &days, &hours, &minutes, &seconds);
    lv_label_set_text_fmt(uptime_label, "%02d:%02d:%02d:%02d", days, hours, minutes, seconds);
}

void info_ui_set_resets(uint32_t resets)
{
    lv_label_set_text_fmt(resets_label, "%d", resets);
}

void info_ui_set_total_uptime_sec(uint32_t uptime_seconds)
{
    int days;
    int hours;
    int minutes;
    int seconds;

    seconds_to_time_chunks(uptime_seconds, &days, &hours, &minutes, &seconds);
    lv_label_set_text_fmt(runtime_label, "%02d:%02d:%02d:%02d", days, hours, minutes, seconds);
}

void info_ui_set_wakeup_time_sec(uint64_t total_wake_time, uint32_t percent_used)
{
    int days;
    int hours;
    int minutes;
    int seconds;

    seconds_to_time_chunks(total_wake_time, &days, &hours, &minutes, &seconds);
    lv_label_set_text_fmt(wake_time_label, "%02d:%02d:%02d:%02d (%d%%)", days, hours, minutes, seconds,
                          percent_used);
}

void info_ui_set_ref_off_time_sec(uint64_t total_off_time, uint32_t percent_off)
{
    int days;
    int hours;
    int minutes;
    int seconds;

    seconds_to_time_chunks(total_off_time, &days, &hours, &minutes, &seconds);
    lv_label_set_text_fmt(disp_pwr_off_label, "%02d:%02d:%02d:%02d (%d%%)", days, hours, minutes, seconds,
                          percent_off);
}

void info_ui_set_time_to_inactive_sec(uint32_t time_left_seconds)
{
    lv_label_set_text_fmt(inactive_time_label, "%d s", time_left_seconds);
}

void info_ui_set_mac_addr(char *mac_str)
{
    lv_label_set_text(mac_addr_label, mac_str);
}

void info_ui_set_gatt_status(bool cccd_enabled, uint16_t mtu)
{
    lv_label_set_text_fmt(ble_gatt_status_label, "Notifications %s, MTU %d", cccd_enabled ? "on" : "off", mtu);
}

void info_app_ui_set_conn_mac(char *mac)
{
    lv_label_set_text(ble_remote_addr_label, mac);
}

void info_app_ui_set_conn_params(uint16_t interval, uint16_t latency, uint16_t timeout)
{
    lv_label_set_text_fmt(ble_remote_info_label, "Interval %d ms, timeout %d ms", interval, timeout);
}

void info_app_ui_set_conn_security_info(int info, int err)
{
    char msg[100];

    if (!err) {
        snprintf(msg, sizeof(msg) - 1, "Security level: %d", info);
    } else {
        snprintf(msg, sizeof(msg) - 1, "Security failed: level %u err %d", info, err);
    }
    lv_label_set_text(ble_security_status_label, msg);
}

static void seconds_to_time_chunks(uint32_t time_seconds, int *days, int *hours, int *minutes, int *seconds)
{
    int n = time_seconds;
    *days = n / (24 * 3600);

    n = n % (24 * 3600);
    *hours = n / 3600;

    n %= 3600;
    *minutes = n / 60 ;

    n %= 60;
    *seconds = n;
}
