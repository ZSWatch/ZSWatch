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

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "voice_memo_ui.h"
#include "ui/zsw_ui.h"
#include "zsw_clock.h"

LOG_MODULE_REGISTER(voice_memo_ui, CONFIG_ZSW_VOICE_MEMO_LOG_LEVEL);

/* Screen dimensions */
#define DISP_WIDTH   240
#define DISP_HEIGHT  240

/* Max entries displayed */
#define MAX_LIST_ENTRIES  CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_MAX_FILES

/* UI state */
static lv_obj_t *root_obj;
static const voice_memo_ui_callbacks_t *callbacks;

/* List screen objects */
static lv_obj_t *list_screen;
static lv_obj_t *list_container;
static lv_obj_t *record_btn;
static lv_obj_t *header_label;
static lv_obj_t *empty_label;
static lv_obj_t *storage_label;

/* Recording screen objects */
static lv_obj_t *rec_screen;
static lv_obj_t *time_label;
static lv_obj_t *rec_indicator;
static lv_obj_t *level_bar;
static lv_obj_t *stop_btn;
static lv_obj_t *remaining_label;

/* Delete confirmation */
static lv_obj_t *delete_msgbox;
static char delete_filename[VOICE_MEMO_MAX_FILENAME];

/* Back-during-recording confirmation */
static lv_obj_t *back_confirm_msgbox;

/* Track which screen is active */
static bool recording_screen_active;

/* Stored entry filenames for delete callback */
static char entry_filenames[MAX_LIST_ENTRIES][VOICE_MEMO_MAX_FILENAME];

/* Pulsing animation for recording indicator */
static lv_anim_t rec_pulse_anim;

/* ---------- Relative time helper ---------- */
static void format_relative_time(uint32_t timestamp, char *buf, size_t buf_size)
{
    if (timestamp == 0) {
        snprintf(buf, buf_size, "No date");
        return;
    }

    zsw_timeval_t ztm;
    zsw_clock_get_time(&ztm);
    struct tm tm_now;
    zsw_timeval_to_tm(&ztm, &tm_now);
    uint32_t now = (uint32_t)mktime(&tm_now);

    if (now <= timestamp) {
        snprintf(buf, buf_size, "Just now");
        return;
    }

    uint32_t diff = now - timestamp;

    if (diff < 60) {
        snprintf(buf, buf_size, "Just now");
    } else if (diff < 3600) {
        uint32_t mins = diff / 60;
        snprintf(buf, buf_size, "%u min ago", mins);
    } else if (diff < 86400) {
        uint32_t hrs = diff / 3600;
        snprintf(buf, buf_size, "%u hr ago", hrs);
    } else if (diff < 172800) {
        snprintf(buf, buf_size, "Yesterday");
    } else {
        uint32_t days = diff / 86400;
        snprintf(buf, buf_size, "%u days ago", days);
    }
}

/* ---------- Pulsing animation callback ---------- */
static void rec_pulse_cb(void *obj, int32_t val)
{
    lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)val, 0);
}

static void record_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (callbacks && callbacks->on_start_recording) {
        callbacks->on_start_recording();
    }
}

static void stop_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (callbacks && callbacks->on_stop_recording) {
        callbacks->on_stop_recording();
    }
}

static void delete_yes_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (callbacks && callbacks->on_delete) {
        callbacks->on_delete(delete_filename);
    }
    if (delete_msgbox) {
        lv_msgbox_close(delete_msgbox);
        delete_msgbox = NULL;
    }
}

static void delete_cancel_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (delete_msgbox) {
        lv_msgbox_close(delete_msgbox);
        delete_msgbox = NULL;
    }
}

static void back_save_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (back_confirm_msgbox) {
        lv_msgbox_close(back_confirm_msgbox);
        back_confirm_msgbox = NULL;
    }
    if (callbacks && callbacks->on_back_during_recording) {
        callbacks->on_back_during_recording(true);
    }
}

static void back_discard_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (back_confirm_msgbox) {
        lv_msgbox_close(back_confirm_msgbox);
        back_confirm_msgbox = NULL;
    }
    if (callbacks && callbacks->on_back_during_recording) {
        callbacks->on_back_during_recording(false);
    }
}

static void entry_swipe_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(obj);
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

    if (dir != LV_DIR_LEFT) {
        return;
    }

    if (idx >= 0 && idx < MAX_LIST_ENTRIES) {
        strncpy(delete_filename, entry_filenames[idx], sizeof(delete_filename) - 1);
        delete_filename[sizeof(delete_filename) - 1] = '\0';

        delete_msgbox = lv_msgbox_create(root_obj);
        lv_msgbox_add_title(delete_msgbox, "Delete?");

        char msg[64];
        snprintf(msg, sizeof(msg), "Delete %s?", delete_filename);
        lv_msgbox_add_text(delete_msgbox, msg);
        lv_msgbox_add_close_button(delete_msgbox);

        lv_obj_t *btn_del = lv_msgbox_add_footer_button(delete_msgbox, "Delete");
        lv_obj_add_event_cb(btn_del, delete_yes_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t *btn_cancel = lv_msgbox_add_footer_button(delete_msgbox, "Cancel");
        lv_obj_add_event_cb(btn_cancel, delete_cancel_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_center(delete_msgbox);
    }
}

static void entry_long_press_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(obj);

    if (idx >= 0 && idx < MAX_LIST_ENTRIES) {
        strncpy(delete_filename, entry_filenames[idx], sizeof(delete_filename) - 1);
        delete_filename[sizeof(delete_filename) - 1] = '\0';

        delete_msgbox = lv_msgbox_create(root_obj);
        lv_msgbox_add_title(delete_msgbox, "Delete?");

        char msg[64];
        snprintf(msg, sizeof(msg), "Delete %s?", delete_filename);
        lv_msgbox_add_text(delete_msgbox, msg);
        lv_msgbox_add_close_button(delete_msgbox);

        lv_obj_t *btn_del = lv_msgbox_add_footer_button(delete_msgbox, "Delete");
        lv_obj_add_event_cb(btn_del, delete_yes_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t *btn_cancel = lv_msgbox_add_footer_button(delete_msgbox, "Cancel");
        lv_obj_add_event_cb(btn_cancel, delete_cancel_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_center(delete_msgbox);
    }
}

static void create_list_screen(void)
{
    list_screen = lv_obj_create(root_obj);
    lv_obj_set_size(list_screen, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_style_bg_opa(list_screen, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list_screen, 0, 0);
    lv_obj_set_style_pad_all(list_screen, 0, 0);
    lv_obj_center(list_screen);

    /* Header */
    header_label = lv_label_create(list_screen);
    lv_label_set_text(header_label, "Voice Memos");
    lv_obj_set_style_text_font(header_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(header_label, lv_color_white(), 0);
    lv_obj_align(header_label, LV_ALIGN_TOP_MID, 0, 12);

    /* Storage label */
    storage_label = lv_label_create(list_screen);
    lv_label_set_text(storage_label, "");
    lv_obj_set_style_text_font(storage_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(storage_label, zsw_color_gray(), 0);
    lv_obj_align(storage_label, LV_ALIGN_TOP_MID, 0, 32);

    /* Scrollable list container */
    list_container = lv_obj_create(list_screen);
    lv_obj_set_size(list_container, DISP_WIDTH - 20, DISP_HEIGHT - 100);
    lv_obj_set_style_bg_opa(list_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list_container, 0, 0);
    lv_obj_set_style_pad_all(list_container, 4, 0);
    lv_obj_set_flex_flow(list_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(list_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(list_container, LV_ALIGN_TOP_MID, 0, 46);
    lv_obj_set_scroll_dir(list_container, LV_DIR_VER);

    /* Empty state label */
    empty_label = lv_label_create(list_container);
    lv_label_set_text(empty_label, LV_SYMBOL_AUDIO "\nNo recordings\nTap to record");
    lv_obj_set_style_text_align(empty_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(empty_label, zsw_color_gray(), 0);
    lv_obj_set_style_text_font(empty_label, &lv_font_montserrat_14, 0);
    lv_obj_center(empty_label);

    /* Record button (red circle at bottom) */
    record_btn = lv_btn_create(list_screen);
    lv_obj_set_size(record_btn, 50, 50);
    lv_obj_set_style_radius(record_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(record_btn, zsw_color_red(), 0);
    lv_obj_set_style_bg_opa(record_btn, LV_OPA_COVER, 0);
    lv_obj_align(record_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(record_btn, record_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(record_btn);
    lv_label_set_text(btn_label, LV_SYMBOL_AUDIO);
    lv_obj_set_style_text_color(btn_label, lv_color_white(), 0);
    lv_obj_center(btn_label);
}

static void create_recording_screen(void)
{
    rec_screen = lv_obj_create(root_obj);
    lv_obj_set_size(rec_screen, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_style_bg_opa(rec_screen, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(rec_screen, 0, 0);
    lv_obj_set_style_pad_all(rec_screen, 0, 0);
    lv_obj_center(rec_screen);

    /* Recording indicator (red circle with pulsing animation) */
    rec_indicator = lv_obj_create(rec_screen);
    lv_obj_set_size(rec_indicator, 20, 20);
    lv_obj_set_style_radius(rec_indicator, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(rec_indicator, zsw_color_red(), 0);
    lv_obj_set_style_bg_opa(rec_indicator, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(rec_indicator, 0, 0);
    lv_obj_align(rec_indicator, LV_ALIGN_TOP_MID, 0, 40);

    /* Setup pulsing animation */
    lv_anim_init(&rec_pulse_anim);
    lv_anim_set_var(&rec_pulse_anim, rec_indicator);
    lv_anim_set_exec_cb(&rec_pulse_anim, rec_pulse_cb);
    lv_anim_set_values(&rec_pulse_anim, LV_OPA_40, LV_OPA_COVER);
    lv_anim_set_duration(&rec_pulse_anim, 800);
    lv_anim_set_playback_duration(&rec_pulse_anim, 800);
    lv_anim_set_repeat_count(&rec_pulse_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&rec_pulse_anim);

    /* Timer label */
    time_label = lv_label_create(rec_screen);
    lv_label_set_text(time_label, "00:00");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_align(time_label, LV_ALIGN_TOP_MID, 0, 65);

    /* Audio level bar */
    level_bar = lv_bar_create(rec_screen);
    lv_obj_set_size(level_bar, 140, 8);
    lv_bar_set_range(level_bar, 0, 100);
    lv_bar_set_value(level_bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(level_bar, zsw_color_dark_gray(), 0);
    lv_obj_set_style_bg_color(level_bar, zsw_color_blue(), LV_PART_INDICATOR);
    lv_obj_align(level_bar, LV_ALIGN_CENTER, 0, -10);

    /* Remaining time label */
    remaining_label = lv_label_create(rec_screen);
    lv_label_set_text(remaining_label, "");
    lv_obj_set_style_text_font(remaining_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(remaining_label, zsw_color_gray(), 0);
    lv_obj_align(remaining_label, LV_ALIGN_CENTER, 0, 15);

    /* Stop button */
    stop_btn = lv_btn_create(rec_screen);
    lv_obj_set_size(stop_btn, 60, 60);
    lv_obj_set_style_radius(stop_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(stop_btn, zsw_color_red(), 0);
    lv_obj_set_style_bg_opa(stop_btn, LV_OPA_COVER, 0);
    lv_obj_align(stop_btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(stop_btn, stop_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *stop_icon = lv_label_create(stop_btn);
    lv_label_set_text(stop_icon, LV_SYMBOL_STOP);
    lv_obj_set_style_text_color(stop_icon, lv_color_white(), 0);
    lv_obj_set_style_text_font(stop_icon, &lv_font_montserrat_20, 0);
    lv_obj_center(stop_icon);

    /* Start hidden */
    lv_obj_add_flag(rec_screen, LV_OBJ_FLAG_HIDDEN);
}

void voice_memo_ui_show(lv_obj_t *root, const voice_memo_ui_callbacks_t *cbs)
{
    root_obj = root;
    callbacks = cbs;
    recording_screen_active = false;

    create_list_screen();
    create_recording_screen();
}

void voice_memo_ui_remove(void)
{
    /* Stop pulsing animation */
    lv_anim_delete(rec_indicator, rec_pulse_cb);

    if (back_confirm_msgbox) {
        lv_msgbox_close(back_confirm_msgbox);
        back_confirm_msgbox = NULL;
    }
    if (delete_msgbox) {
        lv_msgbox_close(delete_msgbox);
        delete_msgbox = NULL;
    }
    if (list_screen) {
        lv_obj_del(list_screen);
        list_screen = NULL;
    }
    if (rec_screen) {
        lv_obj_del(rec_screen);
        rec_screen = NULL;
    }
    list_container = NULL;
    record_btn = NULL;
    header_label = NULL;
    empty_label = NULL;
    storage_label = NULL;
    time_label = NULL;
    rec_indicator = NULL;
    level_bar = NULL;
    stop_btn = NULL;
    remaining_label = NULL;
    root_obj = NULL;
}

void voice_memo_ui_show_recording(void)
{
    if (!rec_screen || !list_screen) {
        return;
    }
    lv_obj_add_flag(list_screen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(rec_screen, LV_OBJ_FLAG_HIDDEN);
    recording_screen_active = true;

    /* Reset display */
    lv_label_set_text(time_label, "00:00");
    lv_bar_set_value(level_bar, 0, LV_ANIM_OFF);
}

void voice_memo_ui_show_list(void)
{
    if (!rec_screen || !list_screen) {
        return;
    }
    lv_obj_add_flag(rec_screen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(list_screen, LV_OBJ_FLAG_HIDDEN);
    recording_screen_active = false;
}

void voice_memo_ui_update_time(uint32_t elapsed_ms)
{
    if (!time_label) {
        return;
    }
    uint32_t secs = elapsed_ms / 1000;
    uint32_t mins = secs / 60;
    secs %= 60;

    lv_label_set_text_fmt(time_label, "%02u:%02u", mins, secs);

    /* Update remaining time */
    if (remaining_label) {
        uint32_t max_s = CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_MAX_DURATION_S;
        uint32_t elapsed_s = elapsed_ms / 1000;
        if (elapsed_s < max_s) {
            uint32_t rem = max_s - elapsed_s;
            lv_label_set_text_fmt(remaining_label, "%u:%02u left", rem / 60, rem % 60);
        }
    }
}

void voice_memo_ui_update_level(uint8_t level)
{
    if (!level_bar) {
        return;
    }
    lv_bar_set_value(level_bar, level, LV_ANIM_ON);
}

void voice_memo_ui_update_list(const zsw_recording_entry_t *entries, int count,
                               uint32_t free_space_kb)
{
    if (!list_container) {
        return;
    }

    /* Clear existing entries (except empty_label) */
    uint32_t child_count = lv_obj_get_child_count(list_container);
    for (int i = (int)child_count - 1; i >= 0; i--) {
        lv_obj_t *child = lv_obj_get_child(list_container, i);
        if (child != empty_label) {
            lv_obj_del(child);
        }
    }

    /* Update storage indicator */
    if (storage_label) {
        uint32_t mins_left = free_space_kb / 4;  /* ~4 KB/sec at 32kbps */
        lv_label_set_text_fmt(storage_label, "%u min left", mins_left);
    }

    if (count == 0) {
        if (empty_label) {
            lv_obj_remove_flag(empty_label, LV_OBJ_FLAG_HIDDEN);
        }
        return;
    }

    if (empty_label) {
        lv_obj_add_flag(empty_label, LV_OBJ_FLAG_HIDDEN);
    }

    /* Add entries (newest first) */
    for (int i = count - 1; i >= 0; i--) {
        /* Store filename for delete callback */
        int display_idx = count - 1 - i;
        if (display_idx < MAX_LIST_ENTRIES) {
            strncpy(entry_filenames[display_idx], entries[i].filename,
                    VOICE_MEMO_MAX_FILENAME - 1);
            entry_filenames[display_idx][VOICE_MEMO_MAX_FILENAME - 1] = '\0';
        }

        lv_obj_t *entry = lv_obj_create(list_container);
        lv_obj_set_size(entry, DISP_WIDTH - 40, 44);
        lv_obj_set_style_bg_color(entry, zsw_color_dark_gray(), 0);
        lv_obj_set_style_bg_opa(entry, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(entry, 8, 0);
        lv_obj_set_style_pad_all(entry, 6, 0);
        lv_obj_set_style_border_width(entry, 0, 0);
        lv_obj_set_user_data(entry, (void *)(intptr_t)display_idx);
        lv_obj_add_event_cb(entry, entry_long_press_cb, LV_EVENT_LONG_PRESSED, NULL);
        lv_obj_add_event_cb(entry, entry_swipe_cb, LV_EVENT_GESTURE, NULL);

        /* Relative time or filename */
        lv_obj_t *name = lv_label_create(entry);
        char time_str[32];
        format_relative_time(entries[i].timestamp, time_str, sizeof(time_str));
        lv_label_set_text(name, time_str);
        lv_obj_set_style_text_font(name, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(name, lv_color_white(), 0);
        lv_obj_align(name, LV_ALIGN_TOP_LEFT, 0, 0);

        /* Duration and size */
        lv_obj_t *info = lv_label_create(entry);
        uint32_t dur_s = (entries[i].duration_ms + 999) / 1000;
        lv_label_set_text_fmt(info, "%u:%02u  %u KB",
                              dur_s / 60, dur_s % 60,
                              entries[i].size_bytes / 1024);
        lv_obj_set_style_text_font(info, &lv_font_montserrat_10, 0);
        lv_obj_set_style_text_color(info, zsw_color_gray(), 0);
        lv_obj_align(info, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    }
}

void voice_memo_ui_show_back_confirm(void)
{
    if (!root_obj || back_confirm_msgbox) {
        return;
    }

    back_confirm_msgbox = lv_msgbox_create(root_obj);
    lv_msgbox_add_title(back_confirm_msgbox, "Recording");
    lv_msgbox_add_text(back_confirm_msgbox, "Save or discard recording?");
    lv_msgbox_add_close_button(back_confirm_msgbox);

    lv_obj_t *btn_save = lv_msgbox_add_footer_button(back_confirm_msgbox, "Save");
    lv_obj_add_event_cb(btn_save, back_save_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_discard = lv_msgbox_add_footer_button(back_confirm_msgbox, "Discard");
    lv_obj_add_event_cb(btn_discard, back_discard_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_center(back_confirm_msgbox);
}


