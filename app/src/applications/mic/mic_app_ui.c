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

#include "mic_app_ui.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(mic_app_ui, LOG_LEVEL_DBG);

#define DECAY_UPDATE_INTERVAL     80
#define PEAK_DECAY_PIXELS_PER_TICK     2.0f
#define UI_BAR_MARGIN     6
#define UI_BAR_ROUND_RADIUS 3
#define UI_GAIN_MIN       0.1f
#define UI_GAIN_MAX       10.0f
#define UI_GAIN_DEFAULT   2.5f

static lv_obj_t *app_root_container = NULL;
static lv_obj_t *spectrum_obj = NULL;
static lv_obj_t *button_label = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *gain_label = NULL;
static lv_timer_t *spectrum_timer = NULL;

static float current_bar_vals[NUM_SPECTRUM_BARS];
static float decay_peak_marker_vals[NUM_SPECTRUM_BARS];
static bool is_recording = false;
static float current_gain = UI_GAIN_DEFAULT;

static watch_ui_toggle_cb_t toggle_callback = NULL;
static watch_ui_gain_change_cb_t gain_change_callback = NULL;
static watch_ui_rtt_output_cb_t rtt_output_callback = NULL;

static void spectrum_draw_event(lv_event_t *e);
static void spectrum_decay_tick_cb(lv_timer_t *t);
static void toggle_button_event_cb(lv_event_t *e);
static void gain_plus_event_cb(lv_event_t *e);
static void gain_minus_event_cb(lv_event_t *e);
static void rtt_checkbox_event_cb(lv_event_t *e);
static inline float norm_to_px(float v, float max_px);

lv_obj_t *mic_app_ui_create(lv_obj_t *parent, watch_ui_toggle_cb_t toggle_cb,
                            watch_ui_gain_change_cb_t gain_change_cb,
                            watch_ui_rtt_output_cb_t rtt_output_cb, float initial_gain)
{
    if (!parent) {
        LOG_ERR("Parent object is NULL");
        return NULL;
    }

    toggle_callback = toggle_cb;
    gain_change_callback = gain_change_cb;
    rtt_output_callback = rtt_output_cb;
    current_gain = initial_gain;

    app_root_container = lv_obj_create(parent);
    lv_obj_set_scrollbar_mode(app_root_container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(app_root_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(app_root_container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(app_root_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(app_root_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(app_root_container, 0, 0);

    gain_label = lv_label_create(app_root_container);
    lv_label_set_text_fmt(gain_label, "Gain: %.1f", initial_gain);
    lv_obj_set_style_text_font(gain_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(gain_label, lv_color_white(), 0);
    lv_obj_align(gain_label, LV_ALIGN_TOP_MID, 0, 10);

    spectrum_obj = lv_obj_create(app_root_container);
    lv_obj_remove_flag(spectrum_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(spectrum_obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(spectrum_obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(spectrum_obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(spectrum_obj, 0, 0);
    lv_obj_add_event_cb(spectrum_obj, spectrum_draw_event, LV_EVENT_DRAW_MAIN, NULL);

    status_label = lv_label_create(app_root_container);
    lv_label_set_text(status_label, "Ready");
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(status_label, lv_color_white(), 0);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -50);

    lv_obj_t *toggle_button = lv_btn_create(app_root_container);
    lv_obj_set_size(toggle_button, 80, 30);
    lv_obj_align(toggle_button, LV_ALIGN_BOTTOM_MID, 0, -17);
    lv_obj_add_event_cb(toggle_button, toggle_button_event_cb, LV_EVENT_CLICKED, NULL);

    button_label = lv_label_create(toggle_button);
    lv_label_set_text(button_label, "Start");
    lv_obj_set_style_text_font(button_label, &lv_font_montserrat_12, 0);
    lv_obj_center(button_label);

    lv_obj_t *gain_minus_btn = lv_btn_create(app_root_container);
    lv_obj_set_size(gain_minus_btn, 30, 25);
    lv_obj_align_to(gain_minus_btn, toggle_button, LV_ALIGN_OUT_LEFT_MID, -25, -25);
    lv_obj_add_event_cb(gain_minus_btn, gain_minus_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *minus_label = lv_label_create(gain_minus_btn);
    lv_label_set_text(minus_label, LV_SYMBOL_MINUS);
    lv_obj_set_style_text_font(minus_label, &lv_font_montserrat_18, 0);
    lv_obj_center(minus_label);

    lv_obj_t *gain_plus_btn = lv_btn_create(app_root_container);
    lv_obj_set_size(gain_plus_btn, 30, 25);
    lv_obj_align_to(gain_plus_btn, toggle_button, LV_ALIGN_OUT_RIGHT_MID, 25, -25);
    lv_obj_add_event_cb(gain_plus_btn, gain_plus_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *plus_label = lv_label_create(gain_plus_btn);
    lv_label_set_text(plus_label, LV_SYMBOL_PLUS);
    lv_obj_set_style_text_font(plus_label, &lv_font_montserrat_18, 0);
    lv_obj_center(plus_label);

    // RTT output checkbox
    lv_obj_t *rtt_checkbox = lv_checkbox_create(app_root_container);
    lv_checkbox_set_text(rtt_checkbox, "RTT Output");
    lv_obj_set_style_text_font(rtt_checkbox, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(rtt_checkbox, lv_color_white(), 0);
    lv_obj_align_to(rtt_checkbox, gain_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_add_event_cb(rtt_checkbox, rtt_checkbox_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    memset(current_bar_vals, 0, sizeof(current_bar_vals));
    memset(decay_peak_marker_vals, 0, sizeof(decay_peak_marker_vals));

    // Timer to update the spectrum decay drawing
    spectrum_timer = lv_timer_create(spectrum_decay_tick_cb, DECAY_UPDATE_INTERVAL, NULL);

    return app_root_container;
}

void mic_app_ui_remove(void)
{
    if (spectrum_timer) {
        lv_timer_del(spectrum_timer);
        spectrum_timer = NULL;
    }

    if (app_root_container) {
        lv_obj_del(app_root_container);
        app_root_container = NULL;
        spectrum_obj = NULL;
        button_label = NULL;
        status_label = NULL;
        gain_label = NULL;
    }

    toggle_callback = NULL;
    gain_change_callback = NULL;
    rtt_output_callback = NULL;
}

void mic_app_ui_update_spectrum(const uint8_t *magnitudes, size_t count)
{
    if (!spectrum_obj || !magnitudes || count != NUM_SPECTRUM_BARS) {
        return;
    }

    const lv_coord_t h = lv_obj_get_height(spectrum_obj);
    const float band_h = 0.85f * (float)h;
    const float max_bar_px = band_h;

    for (uint32_t i = 0; i < NUM_SPECTRUM_BARS; i++) {
        float normalized = (float)magnitudes[i] / 255.0f;
        if (normalized > 1.0f) {
            normalized = 1.0f;    // Clamp to max
        }
        float px = norm_to_px(normalized, max_bar_px);

        current_bar_vals[i] = px;

        // Update peak markers if the new bar is higher
        if (current_bar_vals[i] > decay_peak_marker_vals[i]) {
            decay_peak_marker_vals[i] = current_bar_vals[i];
        }
    }

    lv_obj_invalidate(spectrum_obj);
}

void mic_app_ui_set_status(const char *status)
{
    if (status_label && status) {
        lv_label_set_text(status_label, status);
    }
}

void mic_app_ui_set_recording(bool recording)
{
    is_recording = recording;
    if (button_label) {
        lv_label_set_text(button_label, recording ? "Stop" : "Start");
    }

    // Clear spectrum when starting or stopping recording
    for (uint32_t i = 0; i < NUM_SPECTRUM_BARS; i++) {
        current_bar_vals[i] = 0;
        decay_peak_marker_vals[i] = 0;
    }
    if (spectrum_obj) {
        lv_obj_invalidate(spectrum_obj);
    }
}

static inline float norm_to_px(float v, float max_px)
{
    if (v < 0) {
        v = 0;
    }
    if (v > 1) {
        v = 1;
    }
    return v * max_px;
}

static void spectrum_decay_tick_cb(lv_timer_t *t)
{
    LV_UNUSED(t);

    if (!spectrum_obj || !is_recording) {
        return;
    }

    for (uint32_t i = 0; i < NUM_SPECTRUM_BARS; i++) {
        if (decay_peak_marker_vals[i] > current_bar_vals[i]) {
            decay_peak_marker_vals[i] -= PEAK_DECAY_PIXELS_PER_TICK;
            if (decay_peak_marker_vals[i] < current_bar_vals[i]) {
                decay_peak_marker_vals[i] = current_bar_vals[i];
            }
            if (decay_peak_marker_vals[i] < 0) {
                decay_peak_marker_vals[i] = 0;
            }
        }
    }

    lv_obj_invalidate(spectrum_obj);
}

static void spectrum_draw_event(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_layer_t *layer = lv_event_get_layer(e);

    if (!obj || !layer) {
        return;
    }

    const lv_coord_t w = lv_obj_get_width(obj);
    const lv_coord_t h = lv_obj_get_height(obj);

    const float left = UI_BAR_MARGIN;
    const float right = (float)w - UI_BAR_MARGIN;
    const float span = right - left;
    const float gap = 0.3f;
    const float bar_w = span / (float)NUM_SPECTRUM_BARS;
    const float inner_w = bar_w * (1.0f - gap);

    const float cy = h * 0.50f;

    for (uint32_t i = 0; i < NUM_SPECTRUM_BARS; i++) {
        float x0 = left + i * bar_w + (bar_w - inner_w) * 0.5f;
        float cur = current_bar_vals[i];
        float peak = decay_peak_marker_vals[i];

        // Draw main spectrum bar
        float y0 = cy - cur * 0.5f;
        float y1 = cy + cur * 0.5f;
        lv_area_t bar_area = {
            .x1 = (lv_coord_t)roundf(x0),
            .y1 = (lv_coord_t)roundf(y0),
            .x2 = (lv_coord_t)roundf(x0 + inner_w),
            .y2 = (lv_coord_t)roundf(y1)
        };

        lv_draw_rect_dsc_t bar_dsc;
        lv_draw_rect_dsc_init(&bar_dsc);
        bar_dsc.radius = UI_BAR_ROUND_RADIUS;

        // HSV rainbow coloring
        lv_color_t bar_color = lv_color_hsv_to_rgb((i * 360) / NUM_SPECTRUM_BARS, 80, 90);
        bar_dsc.bg_color = bar_color;
        bar_dsc.bg_opa = LV_OPA_COVER;
        bar_dsc.border_opa = LV_OPA_TRANSP;
        lv_draw_rect(layer, &bar_dsc, &bar_area);

        // Draw peak marker if it's significant
        if (peak > 2.0f) {
            float py0 = cy - peak * 0.5f - 2.0f;
            float py1 = py0 + 2.0f;
            lv_area_t peak_area = {
                .x1 = bar_area.x1,
                .y1 = (lv_coord_t)roundf(py0),
                .x2 = bar_area.x2,
                .y2 = (lv_coord_t)roundf(py1)
            };

            lv_draw_rect_dsc_t peak_dsc;
            lv_draw_rect_dsc_init(&peak_dsc);
            peak_dsc.radius = 1;
            peak_dsc.bg_color = lv_color_white();
            peak_dsc.bg_opa = LV_OPA_COVER;
            peak_dsc.border_opa = LV_OPA_TRANSP;
            lv_draw_rect(layer, &peak_dsc, &peak_area);
        }
    }
}

static void toggle_button_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    if (toggle_callback) {
        toggle_callback();
    }
}

static void gain_plus_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    if (!gain_label) {
        return;
    }

    current_gain += 0.1f;
    if (current_gain > UI_GAIN_MAX) {
        current_gain = UI_GAIN_MAX;
    }

    lv_label_set_text_fmt(gain_label, "Gain: %.1f", current_gain);

    if (gain_change_callback) {
        gain_change_callback(current_gain);
    }
}

static void gain_minus_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    if (!gain_label) {
        return;
    }

    current_gain -= 0.1f;
    if (current_gain < UI_GAIN_MIN) {
        current_gain = UI_GAIN_MIN;
    }

    lv_label_set_text_fmt(gain_label, "Gain: %.1f", current_gain);

    if (gain_change_callback) {
        gain_change_callback(current_gain);
    }
}

static void rtt_checkbox_event_cb(lv_event_t *e)
{
    lv_obj_t *checkbox = lv_event_get_target(e);
    bool checked = lv_obj_get_state(checkbox) & LV_STATE_CHECKED;
    if (rtt_output_callback) {
        rtt_output_callback(checked);
    }
}
