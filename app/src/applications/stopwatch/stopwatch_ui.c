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

#include "stopwatch_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include "ui/zsw_ui.h"
#include <lvgl.h>
#include <assert.h>
#include <stdio.h>
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"

#define STOPWATCH_COLOR         ZSW_COLOR_ORANGE
/* The ring is the second hand: one turn per minute */
#define RING_PERIOD_MS          60000

static void on_start_pause_clicked(lv_event_t *e);
static void on_reset_clicked(lv_event_t *e);
static void on_lap_clicked(lv_event_t *e);
static void format_time(uint32_t ms, char *buf, size_t buf_size);

typedef struct {
    lv_obj_t *root_page;
    lv_obj_t *time_label;
    lv_obj_t *centis_label;
    lv_obj_t *start_pause_btn;
    lv_obj_t *reset_btn;
    lv_obj_t *lap_btn;
    lv_obj_t *lap_list;
    lv_obj_t *progress_arc;
    int lap_count;
    uint32_t shown_seconds;
} stopwatch_ui_t;

static stopwatch_ui_t ui = {0};

static stopwatch_event_cb_t start_cb = NULL;
static stopwatch_event_cb_t pause_cb = NULL;
static stopwatch_event_cb_t reset_cb = NULL;
static stopwatch_event_cb_t lap_cb = NULL;

static stopwatch_state_t current_state = STOPWATCH_STATE_STOPPED;

void stopwatch_ui_show(lv_obj_t *root, stopwatch_event_cb_t start_callback, stopwatch_event_cb_t pause_callback,
                       stopwatch_event_cb_t reset_callback, stopwatch_event_cb_t lap_callback)
{
    assert(ui.root_page == NULL);

    start_cb = start_callback;
    pause_cb = pause_callback;
    reset_cb = reset_callback;
    lap_cb = lap_callback;

    ui.root_page = zsw_ui_screen_create(root, STOPWATCH_COLOR);
    ui.progress_arc = zsw_ui_ring_create(ui.root_page, STOPWATCH_COLOR);
    ui.shown_seconds = UINT32_MAX;

    // Minutes and seconds are large, the hundredths smaller in the app color next to them.
    // They are two labels so the large one is only redrawn once per second.
    ui.time_label = lv_label_create(ui.root_page);
    lv_label_set_text(ui.time_label, "00:00");
    lv_obj_set_style_text_font(ui.time_label, ZSW_FONT_NUMBER, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui.time_label, ZSW_COLOR_TEXT, LV_PART_MAIN);
    lv_obj_set_style_text_align(ui.time_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
    lv_obj_set_width(ui.time_label, 150); // Fixed width to prevent jumping
    lv_obj_align(ui.time_label, LV_ALIGN_TOP_LEFT, 12, 44);

    ui.centis_label = lv_label_create(ui.root_page);
    lv_label_set_text(ui.centis_label, ".00");
    lv_obj_set_style_text_font(ui.centis_label, ZSW_FONT_TITLE, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui.centis_label, STOPWATCH_COLOR, LV_PART_MAIN);
    lv_obj_set_width(ui.centis_label, 40);
    lv_obj_align(ui.centis_label, LV_ALIGN_TOP_LEFT, 164, 67);

    ui.lap_btn = zsw_ui_round_button_create(ui.root_page, LV_SYMBOL_LIST, 44, ZSW_COLOR_SURFACE_HIGH);
    lv_obj_align(ui.lap_btn, LV_ALIGN_TOP_MID, -58, 110);
    lv_obj_add_event_cb(ui.lap_btn, on_lap_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(ui.lap_btn, LV_OBJ_FLAG_HIDDEN); // Hidden unless running

    ui.start_pause_btn = zsw_ui_round_button_create(ui.root_page, LV_SYMBOL_PLAY, 56, STOPWATCH_COLOR);
    lv_obj_align(ui.start_pause_btn, LV_ALIGN_TOP_MID, 0, 104);
    lv_obj_add_event_cb(ui.start_pause_btn, on_start_pause_clicked, LV_EVENT_CLICKED, NULL);

    ui.reset_btn = zsw_ui_round_button_create(ui.root_page, LV_SYMBOL_REFRESH, 44, ZSW_COLOR_SURFACE_HIGH);
    lv_obj_align(ui.reset_btn, LV_ALIGN_TOP_MID, 58, 110);
    lv_obj_add_event_cb(ui.reset_btn, on_reset_clicked, LV_EVENT_CLICKED, NULL);

    // Lap times, the latest one is scrolled into view
    ui.lap_list = lv_obj_create(ui.root_page);
    lv_obj_remove_style_all(ui.lap_list);
    lv_obj_set_size(ui.lap_list, 130, 50);
    lv_obj_align(ui.lap_list, LV_ALIGN_TOP_MID, 0, 168);
    lv_obj_set_flex_flow(ui.lap_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(ui.lap_list, 3, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(ui.lap_list, LV_SCROLLBAR_MODE_OFF);

    zsw_ui_slide_in(ui.start_pause_btn, 0);
    zsw_ui_slide_in(ui.reset_btn, 40);

    lv_group_focus_obj(ui.start_pause_btn);
}

void stopwatch_ui_remove(void)
{
    if (ui.root_page) {
        lv_obj_del(ui.root_page);
        memset(&ui, 0, sizeof(ui));
    }
}

void stopwatch_ui_update_time(uint32_t elapsed_ms)
{
    uint32_t total_seconds = elapsed_ms / 1000;

    if (ui.time_label && total_seconds != ui.shown_seconds) {
        ui.shown_seconds = total_seconds;
        lv_label_set_text_fmt(ui.time_label, "%02u:%02u", total_seconds / 60, total_seconds % 60);
    }

    if (ui.centis_label) {
        lv_label_set_text_fmt(ui.centis_label, ".%02u", (elapsed_ms % 1000) / 10);
    }

    // Only the part of the ring passed since the last update is redrawn.
    if (ui.progress_arc) {
        zsw_ui_ring_set_value(ui.progress_arc, ((elapsed_ms % RING_PERIOD_MS) * ZSW_UI_RING_MAX) / RING_PERIOD_MS, false);
    }
}

void stopwatch_ui_update_state(stopwatch_state_t state)
{
    current_state = state;

    if (!ui.start_pause_btn) {
        return;
    }

    switch (state) {
        case STOPWATCH_STATE_STOPPED:
            zsw_ui_round_button_set_symbol(ui.start_pause_btn, LV_SYMBOL_PLAY);
            lv_obj_add_flag(ui.lap_btn, LV_OBJ_FLAG_HIDDEN);
            break;
        case STOPWATCH_STATE_RUNNING:
            zsw_ui_round_button_set_symbol(ui.start_pause_btn, LV_SYMBOL_PAUSE);
            if (lv_obj_has_flag(ui.lap_btn, LV_OBJ_FLAG_HIDDEN)) {
                lv_obj_clear_flag(ui.lap_btn, LV_OBJ_FLAG_HIDDEN);
                zsw_ui_slide_in(ui.lap_btn, 0);
            }
            break;
        case STOPWATCH_STATE_PAUSED:
            zsw_ui_round_button_set_symbol(ui.start_pause_btn, LV_SYMBOL_PLAY);
            lv_obj_clear_flag(ui.lap_btn, LV_OBJ_FLAG_HIDDEN);
            break;
    }
}

void stopwatch_ui_add_lap_time(uint32_t lap_time, uint32_t total_time)
{
    if (!ui.lap_list) {
        return;
    }

    char time_str[16];
    format_time(lap_time, time_str, sizeof(time_str));
    ui.lap_count++;

    // Lap number in the app color, time in white
    lv_obj_t *lap_item = lv_label_create(ui.lap_list);
    lv_label_set_recolor(lap_item, true);
    lv_label_set_text_fmt(lap_item, "#FF9500 LAP %d#  %s", ui.lap_count, time_str);
    lv_obj_set_style_text_color(lap_item, ZSW_COLOR_TEXT, LV_PART_MAIN);
    lv_obj_set_style_text_font(lap_item, ZSW_FONT_TEXT, LV_PART_MAIN);
    lv_obj_set_width(lap_item, LV_PCT(100));
    lv_obj_set_style_text_align(lap_item, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    // Scroll to bottom to show latest lap
    lv_obj_scroll_to_y(ui.lap_list, LV_COORD_MAX, LV_ANIM_ON);
}

static void on_start_pause_clicked(lv_event_t *e)
{
    switch (current_state) {
        case STOPWATCH_STATE_STOPPED:
        case STOPWATCH_STATE_PAUSED:
            if (start_cb) {
                start_cb();
            }
            break;
        case STOPWATCH_STATE_RUNNING:
            if (pause_cb) {
                pause_cb();
            }
            break;
    }
}

static void on_reset_clicked(lv_event_t *e)
{
    if (reset_cb) {
        reset_cb();
    }

    // Clear lap list and reset lap counter
    if (ui.lap_list) {
        lv_obj_clean(ui.lap_list);
    }
    ui.lap_count = 0;
}

static void on_lap_clicked(lv_event_t *e)
{
    if (current_state == STOPWATCH_STATE_RUNNING && lap_cb) {
        lap_cb();
    }
}

static void format_time(uint32_t ms, char *buf, size_t buf_size)
{
    uint32_t total_seconds = ms / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    uint32_t centiseconds = (ms % 1000) / 10;

    snprintf(buf, buf_size, "%02u:%02u.%02u", minutes, seconds, centiseconds);
}
