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

static struct {
    /* Common */
    lv_obj_t *root_obj;
    const voice_memo_ui_callbacks_t *callbacks;

    /* List screen */
    lv_obj_t *list_screen;
    lv_obj_t *list_container;
    lv_obj_t *record_btn;
    lv_obj_t *header_label;
    lv_obj_t *empty_label;
    lv_obj_t *storage_label;

    /* Recording screen */
    lv_obj_t *rec_screen;
    lv_obj_t *time_label;
    lv_obj_t *rec_indicator;
    lv_obj_t *level_bar;
    lv_obj_t *stop_btn;
    lv_obj_t *remaining_label;

    /* Delete confirmation */
    lv_obj_t *delete_msgbox;
    char delete_filename[VOICE_MEMO_MAX_FILENAME];

    /* Back-during-recording confirmation */
    lv_obj_t *back_confirm_msgbox;

    /* State tracking */
    bool recording_screen_active;

    /* Entry filenames for delete callback */
    char entry_filenames[ZSW_RECORDING_MAX_FILES][VOICE_MEMO_MAX_FILENAME];

    /* Pulsing animation for recording indicator */
    lv_anim_t rec_pulse_anim;
} ui;

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
    if (ui.callbacks && ui.callbacks->on_start_recording) {
        ui.callbacks->on_start_recording();
    }
}

static void stop_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (ui.callbacks && ui.callbacks->on_stop_recording) {
        ui.callbacks->on_stop_recording();
    }
}

static void delete_yes_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (ui.callbacks && ui.callbacks->on_delete) {
        ui.callbacks->on_delete(ui.delete_filename);
    }
    if (ui.delete_msgbox) {
        lv_msgbox_close(ui.delete_msgbox);
        ui.delete_msgbox = NULL;
    }
}

static void delete_cancel_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (ui.delete_msgbox) {
        lv_msgbox_close(ui.delete_msgbox);
        ui.delete_msgbox = NULL;
    }
}

static void back_save_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (ui.back_confirm_msgbox) {
        lv_msgbox_close(ui.back_confirm_msgbox);
        ui.back_confirm_msgbox = NULL;
    }
    if (ui.callbacks && ui.callbacks->on_back_during_recording) {
        ui.callbacks->on_back_during_recording(true);
    }
}

static void back_discard_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (ui.back_confirm_msgbox) {
        lv_msgbox_close(ui.back_confirm_msgbox);
        ui.back_confirm_msgbox = NULL;
    }
    if (ui.callbacks && ui.callbacks->on_back_during_recording) {
        ui.callbacks->on_back_during_recording(false);
    }
}

static void entry_delete_btn_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(btn);

    if (idx >= 0 && idx < ZSW_RECORDING_MAX_FILES) {
        strncpy(ui.delete_filename, ui.entry_filenames[idx], sizeof(ui.delete_filename) - 1);
        ui.delete_filename[sizeof(ui.delete_filename) - 1] = '\0';

        ui.delete_msgbox = lv_msgbox_create(ui.root_obj);
        lv_msgbox_add_title(ui.delete_msgbox, "Delete?");

        char msg[64];
        snprintf(msg, sizeof(msg), "Delete %s?", ui.delete_filename);
        lv_msgbox_add_text(ui.delete_msgbox, msg);

        lv_obj_t *btn_del = lv_msgbox_add_footer_button(ui.delete_msgbox, "Delete");
        lv_obj_add_event_cb(btn_del, delete_yes_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t *btn_cancel = lv_msgbox_add_footer_button(ui.delete_msgbox, "Cancel");
        lv_obj_add_event_cb(btn_cancel, delete_cancel_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_set_size(ui.delete_msgbox, 180, LV_SIZE_CONTENT);
        lv_obj_set_style_radius(ui.delete_msgbox, 5, 0);
        lv_obj_set_style_bg_color(ui.delete_msgbox, lv_color_hex(0x2C3333), 0);
        lv_obj_clear_flag(ui.delete_msgbox, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_scrollbar_mode(ui.delete_msgbox, LV_SCROLLBAR_MODE_OFF);
        lv_obj_center(ui.delete_msgbox);
    }
}

static void create_list_screen(void)
{
    ui.list_screen = lv_obj_create(ui.root_obj);
    lv_obj_set_size(ui.list_screen, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_style_bg_opa(ui.list_screen, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ui.list_screen, 0, 0);
    lv_obj_set_style_pad_all(ui.list_screen, 0, 0);
    lv_obj_center(ui.list_screen);

    /* Header */
    ui.header_label = lv_label_create(ui.list_screen);
    lv_label_set_text(ui.header_label, "Voice Memos");
    lv_obj_set_style_text_font(ui.header_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(ui.header_label, lv_color_white(), 0);
    lv_obj_align(ui.header_label, LV_ALIGN_TOP_MID, 0, 12);

    /* Storage label */
    ui.storage_label = lv_label_create(ui.list_screen);
    lv_label_set_text(ui.storage_label, "");
    lv_obj_set_style_text_font(ui.storage_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(ui.storage_label, zsw_color_gray(), 0);
    lv_obj_align(ui.storage_label, LV_ALIGN_TOP_MID, 0, 32);

    /* Scrollable list container */
    ui.list_container = lv_obj_create(ui.list_screen);
    lv_obj_set_size(ui.list_container, DISP_WIDTH - 20, DISP_HEIGHT - 100);
    lv_obj_set_style_bg_opa(ui.list_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ui.list_container, 0, 0);
    lv_obj_set_style_pad_all(ui.list_container, 4, 0);
    lv_obj_set_flex_flow(ui.list_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui.list_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(ui.list_container, LV_ALIGN_TOP_MID, 0, 46);
    lv_obj_set_scroll_dir(ui.list_container, LV_DIR_VER);

    /* Empty state label */
    ui.empty_label = lv_label_create(ui.list_container);
    lv_label_set_text(ui.empty_label, LV_SYMBOL_AUDIO "\nNo recordings\nTap to record");
    lv_obj_set_style_text_align(ui.empty_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(ui.empty_label, zsw_color_gray(), 0);
    lv_obj_set_style_text_font(ui.empty_label, &lv_font_montserrat_14, 0);
    lv_obj_center(ui.empty_label);

    /* Record button (red circle at bottom) */
    ui.record_btn = lv_btn_create(ui.list_screen);
    lv_obj_set_size(ui.record_btn, 50, 50);
    lv_obj_set_style_radius(ui.record_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ui.record_btn, zsw_color_red(), 0);
    lv_obj_set_style_bg_opa(ui.record_btn, LV_OPA_COVER, 0);
    lv_obj_align(ui.record_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(ui.record_btn, record_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(ui.record_btn);
    lv_label_set_text(btn_label, LV_SYMBOL_AUDIO);
    lv_obj_set_style_text_color(btn_label, lv_color_white(), 0);
    lv_obj_center(btn_label);
}

static void create_recording_screen(void)
{
    ui.rec_screen = lv_obj_create(ui.root_obj);
    lv_obj_set_size(ui.rec_screen, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_style_bg_opa(ui.rec_screen, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ui.rec_screen, 0, 0);
    lv_obj_set_style_pad_all(ui.rec_screen, 0, 0);
    lv_obj_center(ui.rec_screen);

    /* Recording indicator (red circle with pulsing animation) */
    ui.rec_indicator = lv_obj_create(ui.rec_screen);
    lv_obj_set_size(ui.rec_indicator, 20, 20);
    lv_obj_set_style_radius(ui.rec_indicator, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ui.rec_indicator, zsw_color_red(), 0);
    lv_obj_set_style_bg_opa(ui.rec_indicator, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(ui.rec_indicator, 0, 0);
    lv_obj_align(ui.rec_indicator, LV_ALIGN_TOP_MID, 0, 40);

    /* Setup pulsing animation */
    lv_anim_init(&ui.rec_pulse_anim);
    lv_anim_set_var(&ui.rec_pulse_anim, ui.rec_indicator);
    lv_anim_set_exec_cb(&ui.rec_pulse_anim, rec_pulse_cb);
    lv_anim_set_values(&ui.rec_pulse_anim, LV_OPA_40, LV_OPA_COVER);
    lv_anim_set_duration(&ui.rec_pulse_anim, 800);
    lv_anim_set_playback_duration(&ui.rec_pulse_anim, 800);
    lv_anim_set_repeat_count(&ui.rec_pulse_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&ui.rec_pulse_anim);

    /* Timer label */
    ui.time_label = lv_label_create(ui.rec_screen);
    lv_label_set_text(ui.time_label, "00:00");
    lv_obj_set_style_text_font(ui.time_label, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(ui.time_label, lv_color_white(), 0);
    lv_obj_align(ui.time_label, LV_ALIGN_TOP_MID, 0, 65);

    /* Audio level bar */
    ui.level_bar = lv_bar_create(ui.rec_screen);
    lv_obj_set_size(ui.level_bar, 140, 8);
    lv_bar_set_range(ui.level_bar, 0, 100);
    lv_bar_set_value(ui.level_bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(ui.level_bar, zsw_color_dark_gray(), 0);
    lv_obj_set_style_bg_color(ui.level_bar, zsw_color_blue(), LV_PART_INDICATOR);
    lv_obj_align(ui.level_bar, LV_ALIGN_CENTER, 0, -10);

    /* Remaining time label */
    ui.remaining_label = lv_label_create(ui.rec_screen);
    lv_label_set_text(ui.remaining_label, "");
    lv_obj_set_style_text_font(ui.remaining_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(ui.remaining_label, zsw_color_gray(), 0);
    lv_obj_align(ui.remaining_label, LV_ALIGN_CENTER, 0, 15);

    /* Stop button */
    ui.stop_btn = lv_btn_create(ui.rec_screen);
    lv_obj_set_size(ui.stop_btn, 60, 60);
    lv_obj_set_style_radius(ui.stop_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ui.stop_btn, zsw_color_red(), 0);
    lv_obj_set_style_bg_opa(ui.stop_btn, LV_OPA_COVER, 0);
    lv_obj_align(ui.stop_btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(ui.stop_btn, stop_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *stop_icon = lv_label_create(ui.stop_btn);
    lv_label_set_text(stop_icon, LV_SYMBOL_STOP);
    lv_obj_set_style_text_color(stop_icon, lv_color_white(), 0);
    lv_obj_set_style_text_font(stop_icon, &lv_font_montserrat_20, 0);
    lv_obj_center(stop_icon);

    /* Start hidden */
    lv_obj_add_flag(ui.rec_screen, LV_OBJ_FLAG_HIDDEN);
}

void voice_memo_ui_show(lv_obj_t *root, const voice_memo_ui_callbacks_t *cbs)
{
    ui.root_obj = root;
    ui.callbacks = cbs;
    ui.recording_screen_active = false;

    create_list_screen();
    create_recording_screen();
}

void voice_memo_ui_remove(void)
{
    /* Stop pulsing animation */
    lv_anim_delete(ui.rec_indicator, rec_pulse_cb);

    if (ui.back_confirm_msgbox) {
        lv_msgbox_close(ui.back_confirm_msgbox);
        ui.back_confirm_msgbox = NULL;
    }
    if (ui.delete_msgbox) {
        lv_msgbox_close(ui.delete_msgbox);
        ui.delete_msgbox = NULL;
    }
    if (ui.list_screen) {
        lv_obj_del(ui.list_screen);
        ui.list_screen = NULL;
    }
    if (ui.rec_screen) {
        lv_obj_del(ui.rec_screen);
        ui.rec_screen = NULL;
    }
    ui.list_container = NULL;
    ui.record_btn = NULL;
    ui.header_label = NULL;
    ui.empty_label = NULL;
    ui.storage_label = NULL;
    ui.time_label = NULL;
    ui.rec_indicator = NULL;
    ui.level_bar = NULL;
    ui.stop_btn = NULL;
    ui.remaining_label = NULL;
    ui.root_obj = NULL;
}

void voice_memo_ui_show_recording(void)
{
    if (!ui.rec_screen || !ui.list_screen) {
        return;
    }
    lv_obj_add_flag(ui.list_screen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(ui.rec_screen, LV_OBJ_FLAG_HIDDEN);
    ui.recording_screen_active = true;

    /* Reset display */
    lv_label_set_text(ui.time_label, "00:00");
    lv_bar_set_value(ui.level_bar, 0, LV_ANIM_OFF);
    if (ui.remaining_label) {
        lv_label_set_text(ui.remaining_label, "");
    }
}

void voice_memo_ui_show_list(void)
{
    if (!ui.rec_screen || !ui.list_screen) {
        return;
    }
    lv_obj_add_flag(ui.rec_screen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(ui.list_screen, LV_OBJ_FLAG_HIDDEN);
    ui.recording_screen_active = false;
}

void voice_memo_ui_update_time(uint32_t elapsed_ms, uint32_t remaining_s)
{
    if (!ui.time_label) {
        return;
    }
    uint32_t secs = elapsed_ms / 1000;
    uint32_t mins = secs / 60;
    secs %= 60;

    lv_label_set_text_fmt(ui.time_label, "%02u:%02u", mins, secs);

    if (ui.remaining_label && remaining_s > 0) {
        lv_label_set_text_fmt(ui.remaining_label, "%u:%02u left",
                              remaining_s / 60, remaining_s % 60);
    }
}

void voice_memo_ui_update_level(uint8_t level)
{
    if (!ui.level_bar) {
        return;
    }
    lv_bar_set_value(ui.level_bar, level, LV_ANIM_ON);
}

void voice_memo_ui_update_list(const zsw_recording_entry_t *entries, int count,
                               uint32_t free_space_kb, uint32_t kb_per_sec)
{
    if (!ui.list_container) {
        return;
    }

    /* Clear existing entries (except empty_label) */
    uint32_t child_count = lv_obj_get_child_count(ui.list_container);
    for (int i = (int)child_count - 1; i >= 0; i--) {
        lv_obj_t *child = lv_obj_get_child(ui.list_container, i);
        if (child != ui.empty_label) {
            lv_obj_del(child);
        }
    }

    if (ui.storage_label && kb_per_sec > 0) {
        uint32_t mins_left = free_space_kb / kb_per_sec / 60;
        lv_label_set_text_fmt(ui.storage_label, "%u min left", mins_left);
    }

    if (count == 0) {
        if (ui.empty_label) {
            lv_obj_remove_flag(ui.empty_label, LV_OBJ_FLAG_HIDDEN);
        }
        return;
    }

    if (ui.empty_label) {
        lv_obj_add_flag(ui.empty_label, LV_OBJ_FLAG_HIDDEN);
    }

    /* Add entries (newest first) */
    for (int i = count - 1; i >= 0; i--) {
        /* Store filename for delete callback */
        int display_idx = count - 1 - i;
        if (display_idx < ZSW_RECORDING_MAX_FILES) {
            strncpy(ui.entry_filenames[display_idx], entries[i].filename,
                    VOICE_MEMO_MAX_FILENAME - 1);
            ui.entry_filenames[display_idx][VOICE_MEMO_MAX_FILENAME - 1] = '\0';
        }

        lv_obj_t *entry = lv_obj_create(ui.list_container);
        lv_obj_set_size(entry, DISP_WIDTH - 40, 44);
        lv_obj_set_style_bg_color(entry, zsw_color_dark_gray(), 0);
        lv_obj_set_style_bg_opa(entry, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(entry, 8, 0);
        lv_obj_set_style_pad_all(entry, 6, 0);
        lv_obj_set_style_border_width(entry, 0, 0);

        /* Delete button on right side */
        lv_obj_t *del_btn = lv_button_create(entry);
        lv_obj_set_size(del_btn, 28, 28);
        lv_obj_align(del_btn, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_set_style_bg_color(del_btn, zsw_color_red(), 0);
        lv_obj_set_style_bg_opa(del_btn, LV_OPA_60, 0);
        lv_obj_set_style_radius(del_btn, 6, 0);
        lv_obj_set_style_pad_all(del_btn, 0, 0);
        lv_obj_set_user_data(del_btn, (void *)(intptr_t)display_idx);
        lv_obj_add_event_cb(del_btn, entry_delete_btn_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_t *del_icon = lv_label_create(del_btn);
        lv_label_set_text(del_icon, LV_SYMBOL_TRASH);
        lv_obj_set_style_text_font(del_icon, &lv_font_montserrat_12, 0);
        lv_obj_center(del_icon);

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
    if (!ui.root_obj || ui.back_confirm_msgbox) {
        return;
    }

    ui.back_confirm_msgbox = lv_msgbox_create(ui.root_obj);
    lv_msgbox_add_title(ui.back_confirm_msgbox, "Recording");
    lv_msgbox_add_text(ui.back_confirm_msgbox, "Save or discard?");

    lv_obj_t *btn_save = lv_msgbox_add_footer_button(ui.back_confirm_msgbox, "Save");
    lv_obj_add_event_cb(btn_save, back_save_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_discard = lv_msgbox_add_footer_button(ui.back_confirm_msgbox, "Discard");
    lv_obj_add_event_cb(btn_discard, back_discard_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_set_size(ui.back_confirm_msgbox, 180, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(ui.back_confirm_msgbox, 5, 0);
    lv_obj_set_style_bg_color(ui.back_confirm_msgbox, lv_color_hex(0x2C3333), 0);
    lv_obj_clear_flag(ui.back_confirm_msgbox, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(ui.back_confirm_msgbox, LV_SCROLLBAR_MODE_OFF);
    lv_obj_center(ui.back_confirm_msgbox);
}


