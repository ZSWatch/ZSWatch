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

#include "da7212_test_ui.h"

#include <zephyr/sys/__assert.h>

static lv_obj_t *root_page;
static lv_obj_t *status_label;
static lv_obj_t *melody_btn;
static lv_obj_t *melody_btn_label;
static lv_obj_t *voice_demo_btn;
static lv_obj_t *voice_demo_btn_label;
static lv_obj_t *stop_btn;
static lv_obj_t *stop_btn_label;

static da7212_test_ui_evt_cb_t melody_evt_cb;
static da7212_test_ui_evt_cb_t voice_demo_evt_cb;
static da7212_test_ui_evt_cb_t stop_evt_cb;

static void on_melody_btn_click(lv_event_t *e)
{
    LV_UNUSED(e);
    if (melody_evt_cb) {
        melody_evt_cb();
    }
}

static void on_voice_demo_btn_click(lv_event_t *e)

{
    LV_UNUSED(e);
    if (voice_demo_evt_cb) {
        voice_demo_evt_cb();
    }
}

static void on_stop_btn_click(lv_event_t *e)

{
    LV_UNUSED(e);
    if (stop_evt_cb) {
        stop_evt_cb();
    }
}

static void set_button_state(lv_obj_t *btn, lv_obj_t *label, const char *text,
                             lv_color_t color, bool enabled)

{
    lv_label_set_text(label, text);
    lv_obj_center(label);

    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_style_bg_color(btn, color, LV_STATE_DISABLED);

    if (enabled) {
        lv_obj_clear_state(btn, LV_STATE_DISABLED);
    } else {
        lv_obj_add_state(btn, LV_STATE_DISABLED);
    }
}

void da7212_test_ui_show(lv_obj_t *root,
                         da7212_test_ui_evt_cb_t melody_cb,
                         da7212_test_ui_evt_cb_t voice_demo_cb,
                         da7212_test_ui_evt_cb_t stop_cb)
{
    __ASSERT(root_page == NULL, "UI already shown");

    melody_evt_cb = melody_cb;
    voice_demo_evt_cb = voice_demo_cb;
    stop_evt_cb = stop_cb;

    root_page = lv_obj_create(root);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(root_page, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(root_page, 0, 0);
    lv_obj_set_style_pad_all(root_page, 0, 0);
    lv_obj_set_flex_flow(root_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(root_page, 8, 0);
    lv_obj_set_flex_align(root_page, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    /* Title */
    lv_obj_t *title = lv_label_create(root_page);
    lv_label_set_text(title, "DA7212 Test");
    lv_obj_set_style_text_color(title, lv_color_hex(0x4FC3F7), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);

    /* Status label */
    status_label = lv_label_create(root_page);
    lv_label_set_text(status_label, "Ready");
    lv_obj_set_style_text_color(status_label, lv_color_hex(0xBDBDBD), 0);
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
    lv_label_set_long_mode(status_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(status_label, 200);
    lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, 0);

    /* Hint label */
    lv_obj_t *hint = lv_label_create(root_page);
    lv_label_set_text(hint, "Voice demo records a short clip to flash and then replays it through the speaker.");
    lv_obj_set_width(hint, 200);
    lv_label_set_long_mode(hint, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(hint, lv_color_hex(0x90A4AE), 0);
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_12, 0);

    voice_demo_btn = lv_btn_create(root_page);
    lv_obj_set_size(voice_demo_btn, 150, 40);
    lv_obj_set_style_pad_top(voice_demo_btn, 0, 0);
    lv_obj_set_style_pad_bottom(voice_demo_btn, 0, 0);
    lv_obj_set_style_radius(voice_demo_btn, 20, 0);
    lv_obj_add_event_cb(voice_demo_btn, on_voice_demo_btn_click, LV_EVENT_CLICKED, NULL);

    voice_demo_btn_label = lv_label_create(voice_demo_btn);
    lv_obj_set_style_text_font(voice_demo_btn_label, &lv_font_montserrat_14, 0);
    lv_obj_center(voice_demo_btn_label);

    melody_btn = lv_btn_create(root_page);
    lv_obj_set_size(melody_btn, 150, 40);
    lv_obj_set_style_pad_top(melody_btn, 0, 0);
    lv_obj_set_style_pad_bottom(melody_btn, 0, 0);
    lv_obj_set_style_radius(melody_btn, 20, 0);
    lv_obj_add_event_cb(melody_btn, on_melody_btn_click, LV_EVENT_CLICKED, NULL);

    melody_btn_label = lv_label_create(melody_btn);
    lv_obj_set_style_text_font(melody_btn_label, &lv_font_montserrat_14, 0);
    lv_obj_center(melody_btn_label);

    stop_btn = lv_btn_create(root_page);
    lv_obj_set_size(stop_btn, 150, 40);
    lv_obj_set_style_pad_top(stop_btn, 0, 0);
    lv_obj_set_style_pad_bottom(stop_btn, 0, 0);
    lv_obj_set_style_radius(stop_btn, 20, 0);
    lv_obj_add_event_cb(stop_btn, on_stop_btn_click, LV_EVENT_CLICKED, NULL);

    stop_btn_label = lv_label_create(stop_btn);
    lv_obj_set_style_text_font(stop_btn_label, &lv_font_montserrat_14, 0);
    lv_obj_center(stop_btn_label);

    da7212_test_ui_set_state(DA7212_TEST_UI_STATE_IDLE);
}

void da7212_test_ui_remove(void)
{
    if (root_page != NULL) {
        lv_obj_del(root_page);
        root_page = NULL;
        status_label = NULL;
        melody_btn = NULL;
        melody_btn_label = NULL;
        voice_demo_btn = NULL;
        voice_demo_btn_label = NULL;
        stop_btn = NULL;
        stop_btn_label = NULL;
        melody_evt_cb = NULL;
        voice_demo_evt_cb = NULL;
        stop_evt_cb = NULL;
    }
}

void da7212_test_ui_set_status(const char *text)
{
    if (status_label != NULL) {
        lv_label_set_text(status_label, text);
    }
}

void da7212_test_ui_set_state(da7212_test_ui_state_t state)
{
    if (root_page == NULL) {
        return;
    }

    switch (state) {
    case DA7212_TEST_UI_STATE_IDLE:
        set_button_state(voice_demo_btn, voice_demo_btn_label, "Voice Demo",
                         lv_color_hex(0x1976D2), true);
        set_button_state(melody_btn, melody_btn_label, "Play Melody",
                         lv_color_hex(0x388E3C), true);
        set_button_state(stop_btn, stop_btn_label, "Stop",
                         lv_color_hex(0x616161), false);
        break;
    case DA7212_TEST_UI_STATE_PLAYING_MELODY:
        set_button_state(voice_demo_btn, voice_demo_btn_label, "Voice Demo",
                         lv_color_hex(0x546E7A), false);
        set_button_state(melody_btn, melody_btn_label, "Playing Melody",
                         lv_color_hex(0x2E7D32), false);
        set_button_state(stop_btn, stop_btn_label, "Stop",
                         lv_color_hex(0xD32F2F), true);
        break;
    case DA7212_TEST_UI_STATE_RECORDING_VOICE:
        set_button_state(voice_demo_btn, voice_demo_btn_label, "Recording Clip",
                         lv_color_hex(0x1565C0), false);
        set_button_state(melody_btn, melody_btn_label, "Play Melody",
                         lv_color_hex(0x546E7A), false);
        set_button_state(stop_btn, stop_btn_label, "Stop",
                         lv_color_hex(0xD32F2F), true);
        break;
    case DA7212_TEST_UI_STATE_PLAYING_VOICE:
        set_button_state(voice_demo_btn, voice_demo_btn_label, "Playing Clip",
                         lv_color_hex(0x1565C0), false);
        set_button_state(melody_btn, melody_btn_label, "Play Melody",
                         lv_color_hex(0x546E7A), false);
        set_button_state(stop_btn, stop_btn_label, "Stop",
                         lv_color_hex(0xD32F2F), true);
        break;
    }
}
