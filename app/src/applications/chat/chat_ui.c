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

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "chat_ui.h"
#include "ui/zsw_ui.h"

LOG_MODULE_REGISTER(chat_ui, CONFIG_ZSW_CHAT_LOG_LEVEL);

#define DISP_WIDTH   240
#define DISP_HEIGHT  240

LV_FONT_DECLARE(lv_font_montserrat_14_full)

static struct {
    lv_obj_t *root_obj;
    const chat_ui_callbacks_t *callbacks;

    /* State tracking */
    chat_ui_state_t current_state;

    /* Idle screen */
    lv_obj_t *idle_screen;
    lv_obj_t *mic_btn;
    lv_obj_t *idle_hint_label;

    /* Listening screen */
    lv_obj_t *listening_screen;
    lv_obj_t *level_bar;
    lv_obj_t *time_label;
    lv_obj_t *stop_btn;
    lv_obj_t *listening_label;
    lv_anim_t pulse_anim;

    /* Processing screen (Uploading / Thinking / Speaking) */
    lv_obj_t *processing_screen;
    lv_obj_t *status_label;
    lv_obj_t *transcript_label;
    lv_obj_t *cancel_btn;

    /* Speaking indicator */
    lv_obj_t *speaking_screen;
    lv_obj_t *speaking_arc;
    lv_obj_t *speaking_label;
    lv_obj_t *speaking_cancel_btn;

    /* Error screen */
    lv_obj_t *error_screen;
    lv_obj_t *error_label;
    lv_obj_t *error_detail_label;
    lv_obj_t *retry_btn;
} ui;

/* ---------- Callbacks ---------- */

static void mic_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (ui.callbacks && ui.callbacks->on_record_start) {
        ui.callbacks->on_record_start();
    }
}

static void stop_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (ui.callbacks && ui.callbacks->on_record_stop) {
        ui.callbacks->on_record_stop();
    }
}

static void cancel_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (ui.callbacks && ui.callbacks->on_cancel) {
        ui.callbacks->on_cancel();
    }
}

static void retry_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (ui.callbacks && ui.callbacks->on_retry) {
        ui.callbacks->on_retry();
    }
}

/* ---------- Pulse animation for listening indicator ---------- */
static void pulse_cb(void *obj, int32_t val)
{
    lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)val, 0);
}

/* ---------- Screen builders ---------- */

static void hide_all_screens(void)
{
    if (ui.idle_screen) {
        lv_obj_add_flag(ui.idle_screen, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui.listening_screen) {
        lv_obj_add_flag(ui.listening_screen, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui.processing_screen) {
        lv_obj_add_flag(ui.processing_screen, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui.speaking_screen) {
        lv_obj_add_flag(ui.speaking_screen, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui.error_screen) {
        lv_obj_add_flag(ui.error_screen, LV_OBJ_FLAG_HIDDEN);
    }
}

static void create_idle_screen(void)
{
    ui.idle_screen = lv_obj_create(ui.root_obj);
    lv_obj_set_size(ui.idle_screen, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_style_bg_opa(ui.idle_screen, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ui.idle_screen, 0, 0);
    lv_obj_set_style_pad_all(ui.idle_screen, 0, 0);
    lv_obj_clear_flag(ui.idle_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(ui.idle_screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui.idle_screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    /* Microphone button */
    ui.mic_btn = lv_btn_create(ui.idle_screen);
    lv_obj_set_size(ui.mic_btn, 80, 80);
    lv_obj_set_style_radius(ui.mic_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ui.mic_btn, zsw_color_blue(), 0);
    lv_obj_add_event_cb(ui.mic_btn, mic_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *mic_icon = lv_label_create(ui.mic_btn);
    lv_label_set_text(mic_icon, LV_SYMBOL_AUDIO);
    lv_obj_set_style_text_font(mic_icon, &lv_font_montserrat_28, 0);
    lv_obj_center(mic_icon);

    /* Hint label */
    ui.idle_hint_label = lv_label_create(ui.idle_screen);
    lv_label_set_text(ui.idle_hint_label, "Tap to ask");
    lv_obj_set_style_text_color(ui.idle_hint_label, zsw_color_gray(), 0);
    lv_obj_set_style_text_font(ui.idle_hint_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_pad_top(ui.idle_hint_label, 15, 0);
}

static void create_listening_screen(void)
{
    ui.listening_screen = lv_obj_create(ui.root_obj);
    lv_obj_set_size(ui.listening_screen, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_style_bg_opa(ui.listening_screen, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ui.listening_screen, 0, 0);
    lv_obj_set_style_pad_all(ui.listening_screen, 10, 0);
    lv_obj_clear_flag(ui.listening_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(ui.listening_screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui.listening_screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    /* "Listening..." label with pulse animation */
    ui.listening_label = lv_label_create(ui.listening_screen);
    lv_label_set_text(ui.listening_label, "Listening...");
    lv_obj_set_style_text_color(ui.listening_label, zsw_color_red(), 0);
    lv_obj_set_style_text_font(ui.listening_label, &lv_font_montserrat_18, 0);

    /* Level bar */
    ui.level_bar = lv_bar_create(ui.listening_screen);
    lv_obj_set_size(ui.level_bar, 140, 10);
    lv_bar_set_range(ui.level_bar, 0, 100);
    lv_bar_set_value(ui.level_bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(ui.level_bar, zsw_color_dark_gray(), 0);
    lv_obj_set_style_bg_color(ui.level_bar, zsw_color_red(), LV_PART_INDICATOR);
    lv_obj_set_style_pad_top(ui.level_bar, 10, 0);

    /* Time label */
    ui.time_label = lv_label_create(ui.listening_screen);
    lv_label_set_text(ui.time_label, "0:00");
    lv_obj_set_style_text_color(ui.time_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(ui.time_label, &lv_font_montserrat_28, 0);
    lv_obj_set_style_pad_top(ui.time_label, 10, 0);

    /* Stop button */
    ui.stop_btn = lv_btn_create(ui.listening_screen);
    lv_obj_set_size(ui.stop_btn, 60, 60);
    lv_obj_set_style_radius(ui.stop_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ui.stop_btn, zsw_color_red(), 0);
    lv_obj_add_event_cb(ui.stop_btn, stop_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_pad_top(ui.stop_btn, 10, 0);

    lv_obj_t *stop_icon = lv_label_create(ui.stop_btn);
    lv_label_set_text(stop_icon, LV_SYMBOL_STOP);
    lv_obj_set_style_text_font(stop_icon, &lv_font_montserrat_20, 0);
    lv_obj_center(stop_icon);

    /* Pulse animation on listening label */
    lv_anim_init(&ui.pulse_anim);
    lv_anim_set_var(&ui.pulse_anim, ui.listening_label);
    lv_anim_set_values(&ui.pulse_anim, LV_OPA_40, LV_OPA_COVER);
    lv_anim_set_duration(&ui.pulse_anim, 800);
    lv_anim_set_playback_duration(&ui.pulse_anim, 800);
    lv_anim_set_repeat_count(&ui.pulse_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&ui.pulse_anim, pulse_cb);

    lv_obj_add_flag(ui.listening_screen, LV_OBJ_FLAG_HIDDEN);
}

static void create_processing_screen(void)
{
    ui.processing_screen = lv_obj_create(ui.root_obj);
    lv_obj_set_size(ui.processing_screen, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_style_bg_opa(ui.processing_screen, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ui.processing_screen, 0, 0);
    lv_obj_set_style_pad_all(ui.processing_screen, 15, 0);
    lv_obj_clear_flag(ui.processing_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(ui.processing_screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui.processing_screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    /* Status label */
    ui.status_label = lv_label_create(ui.processing_screen);
    lv_label_set_text(ui.status_label, "Uploading...");
    lv_obj_set_style_text_color(ui.status_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(ui.status_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_pad_top(ui.status_label, 10, 0);

    /* Transcript preview */
    ui.transcript_label = lv_label_create(ui.processing_screen);
    lv_label_set_text(ui.transcript_label, "");
    lv_obj_set_style_text_color(ui.transcript_label, zsw_color_gray(), 0);
    lv_obj_set_style_text_font(ui.transcript_label, &lv_font_montserrat_14_full, 0);
    lv_label_set_long_mode(ui.transcript_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(ui.transcript_label, 180);
    lv_obj_set_style_text_align(ui.transcript_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_top(ui.transcript_label, 8, 0);

    /* Cancel button */
    ui.cancel_btn = lv_btn_create(ui.processing_screen);
    lv_obj_set_size(ui.cancel_btn, 90, 32);
    lv_obj_set_style_bg_color(ui.cancel_btn, zsw_color_dark_gray(), 0);
    lv_obj_add_event_cb(ui.cancel_btn, cancel_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_pad_top(ui.cancel_btn, 10, 0);

    lv_obj_t *cancel_label = lv_label_create(ui.cancel_btn);
    lv_label_set_text(cancel_label, "Cancel");
    lv_obj_set_style_text_font(cancel_label, &lv_font_montserrat_14, 0);
    lv_obj_center(cancel_label);

    lv_obj_add_flag(ui.processing_screen, LV_OBJ_FLAG_HIDDEN);
}

static void create_speaking_screen(void)
{
    ui.speaking_screen = lv_obj_create(ui.root_obj);
    lv_obj_set_size(ui.speaking_screen, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_style_bg_opa(ui.speaking_screen, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ui.speaking_screen, 0, 0);
    lv_obj_set_style_pad_all(ui.speaking_screen, 15, 0);
    lv_obj_clear_flag(ui.speaking_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(ui.speaking_screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui.speaking_screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    /* Animated arc as speaking indicator */
    ui.speaking_arc = lv_arc_create(ui.speaking_screen);
    lv_obj_set_size(ui.speaking_arc, 80, 80);
    lv_arc_set_rotation(ui.speaking_arc, 270);
    lv_arc_set_range(ui.speaking_arc, 0, 360);
    lv_arc_set_bg_angles(ui.speaking_arc, 0, 360);
    lv_arc_set_value(ui.speaking_arc, 270);
    lv_obj_set_style_arc_color(ui.speaking_arc, zsw_color_blue(), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui.speaking_arc, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui.speaking_arc, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui.speaking_arc, zsw_color_dark_gray(), LV_PART_MAIN);
    lv_obj_remove_flag(ui.speaking_arc, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *speaker_icon = lv_label_create(ui.speaking_arc);
    lv_label_set_text(speaker_icon, LV_SYMBOL_VOLUME_MAX);
    lv_obj_set_style_text_font(speaker_icon, &lv_font_montserrat_20, 0);
    lv_obj_center(speaker_icon);

    /* Speaking label */
    ui.speaking_label = lv_label_create(ui.speaking_screen);
    lv_label_set_text(ui.speaking_label, "Speaking...");
    lv_obj_set_style_text_color(ui.speaking_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(ui.speaking_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_pad_top(ui.speaking_label, 10, 0);

    /* Cancel button */
    ui.speaking_cancel_btn = lv_btn_create(ui.speaking_screen);
    lv_obj_set_size(ui.speaking_cancel_btn, 90, 32);
    lv_obj_set_style_bg_color(ui.speaking_cancel_btn, zsw_color_dark_gray(), 0);
    lv_obj_add_event_cb(ui.speaking_cancel_btn, cancel_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_pad_top(ui.speaking_cancel_btn, 15, 0);

    lv_obj_t *cancel_label = lv_label_create(ui.speaking_cancel_btn);
    lv_label_set_text(cancel_label, "Cancel");
    lv_obj_set_style_text_font(cancel_label, &lv_font_montserrat_14, 0);
    lv_obj_center(cancel_label);

    lv_obj_add_flag(ui.speaking_screen, LV_OBJ_FLAG_HIDDEN);
}

static void create_error_screen(void)
{
    ui.error_screen = lv_obj_create(ui.root_obj);
    lv_obj_set_size(ui.error_screen, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_style_bg_opa(ui.error_screen, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ui.error_screen, 0, 0);
    lv_obj_set_style_pad_all(ui.error_screen, 15, 0);
    lv_obj_clear_flag(ui.error_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(ui.error_screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui.error_screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    /* Error icon label */
    ui.error_label = lv_label_create(ui.error_screen);
    lv_label_set_text(ui.error_label, LV_SYMBOL_WARNING " Error");
    lv_obj_set_style_text_color(ui.error_label, zsw_color_red(), 0);
    lv_obj_set_style_text_font(ui.error_label, &lv_font_montserrat_18, 0);

    /* Error detail */
    ui.error_detail_label = lv_label_create(ui.error_screen);
    lv_label_set_text(ui.error_detail_label, "Something went wrong");
    lv_obj_set_style_text_color(ui.error_detail_label, zsw_color_gray(), 0);
    lv_obj_set_style_text_font(ui.error_detail_label, &lv_font_montserrat_12, 0);
    lv_label_set_long_mode(ui.error_detail_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(ui.error_detail_label, 180);
    lv_obj_set_style_text_align(ui.error_detail_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_top(ui.error_detail_label, 10, 0);

    /* Retry button */
    ui.retry_btn = lv_btn_create(ui.error_screen);
    lv_obj_set_size(ui.retry_btn, 90, 36);
    lv_obj_set_style_bg_color(ui.retry_btn, zsw_color_blue(), 0);
    lv_obj_add_event_cb(ui.retry_btn, retry_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_pad_top(ui.retry_btn, 15, 0);

    lv_obj_t *retry_label = lv_label_create(ui.retry_btn);
    lv_label_set_text(retry_label, "Retry");
    lv_obj_set_style_text_font(retry_label, &lv_font_montserrat_14, 0);
    lv_obj_center(retry_label);

    lv_obj_add_flag(ui.error_screen, LV_OBJ_FLAG_HIDDEN);
}

/* ---------- Public API ---------- */

void chat_ui_show(lv_obj_t *root, const chat_ui_callbacks_t *callbacks)
{
    memset(&ui, 0, sizeof(ui));
    ui.root_obj = root;
    ui.callbacks = callbacks;
    ui.current_state = CHAT_UI_STATE_IDLE;

    create_idle_screen();
    create_listening_screen();
    create_processing_screen();
    create_speaking_screen();
    create_error_screen();

    /* Show idle by default */
    lv_obj_clear_flag(ui.idle_screen, LV_OBJ_FLAG_HIDDEN);
}

void chat_ui_remove(void)
{
    lv_anim_delete(ui.listening_label, pulse_cb);

    if (ui.idle_screen) {
        lv_obj_delete(ui.idle_screen);
    }
    if (ui.listening_screen) {
        lv_obj_delete(ui.listening_screen);
    }
    if (ui.processing_screen) {
        lv_obj_delete(ui.processing_screen);
    }
    if (ui.speaking_screen) {
        lv_obj_delete(ui.speaking_screen);
    }
    if (ui.error_screen) {
        lv_obj_delete(ui.error_screen);
    }

    memset(&ui, 0, sizeof(ui));
}

void chat_ui_set_state(chat_ui_state_t state)
{
    ui.current_state = state;
    hide_all_screens();

    switch (state) {
    case CHAT_UI_STATE_IDLE:
        lv_obj_clear_flag(ui.idle_screen, LV_OBJ_FLAG_HIDDEN);
        lv_anim_delete(ui.listening_label, pulse_cb);
        break;

    case CHAT_UI_STATE_LISTENING:
        lv_obj_clear_flag(ui.listening_screen, LV_OBJ_FLAG_HIDDEN);
        lv_anim_start(&ui.pulse_anim);
        lv_label_set_text(ui.time_label, "0:00");
        lv_bar_set_value(ui.level_bar, 0, LV_ANIM_OFF);
        break;

    case CHAT_UI_STATE_UPLOADING:
        lv_obj_clear_flag(ui.processing_screen, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(ui.status_label, "Uploading...");
        break;

    case CHAT_UI_STATE_THINKING:
        lv_obj_clear_flag(ui.processing_screen, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(ui.status_label, "Thinking...");
        break;

    case CHAT_UI_STATE_SPEAKING:
        lv_obj_clear_flag(ui.speaking_screen, LV_OBJ_FLAG_HIDDEN);
        break;

    case CHAT_UI_STATE_ERROR:
        lv_obj_clear_flag(ui.error_screen, LV_OBJ_FLAG_HIDDEN);
        break;
    }
}

void chat_ui_set_transcript(const char *text)
{
    if (ui.transcript_label) {
        lv_label_set_text(ui.transcript_label, text);
    }
}

void chat_ui_set_error(const char *text)
{
    if (ui.error_detail_label) {
        lv_label_set_text(ui.error_detail_label, text);
    }
}

void chat_ui_update_level(int level)
{
    if (ui.level_bar) {
        lv_bar_set_value(ui.level_bar, level, LV_ANIM_ON);
    }
}

void chat_ui_update_recording_time(uint32_t elapsed_ms)
{
    if (ui.time_label) {
        uint32_t sec = elapsed_ms / 1000;
        uint32_t min = sec / 60;
        sec %= 60;
        char buf[16];
        snprintf(buf, sizeof(buf), "%u:%02u", min, sec);
        lv_label_set_text(ui.time_label, buf);
    }
}
