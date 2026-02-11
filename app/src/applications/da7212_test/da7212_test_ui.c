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

#include "da7212_test_ui.h"

#include <zephyr/sys/__assert.h>

static lv_obj_t *root_page;
static lv_obj_t *status_label;
static lv_obj_t *play_btn;
static lv_obj_t *play_btn_label;
static bool is_playing;

static da7212_test_ui_evt_cb_t evt_cb;

static void on_play_btn_click(lv_event_t *e)
{
    LV_UNUSED(e);
    is_playing = !is_playing;
    if (evt_cb) {
        evt_cb(is_playing);
    }
}

void da7212_test_ui_show(lv_obj_t *root, da7212_test_ui_evt_cb_t play_cb)
{
    __ASSERT(root_page == NULL, "UI already shown");

    evt_cb = play_cb;
    is_playing = false;

    root_page = lv_obj_create(root);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(root_page, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(root_page, 0, 0);
    lv_obj_set_style_pad_all(root_page, 0, 0);
    lv_obj_set_flex_flow(root_page, LV_FLEX_FLOW_COLUMN);
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
    lv_obj_set_style_pad_top(status_label, 10, 0);
    lv_label_set_long_mode(status_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(status_label, 200);
    lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, 0);

    /* Play/Stop button */
    play_btn = lv_btn_create(root_page);
    lv_obj_set_size(play_btn, 120, 50);
    lv_obj_set_style_bg_color(play_btn, lv_color_hex(0x388E3C), 0);
    lv_obj_set_style_pad_top(play_btn, 0, 0);
    lv_obj_set_style_pad_bottom(play_btn, 0, 0);
    lv_obj_set_style_radius(play_btn, 25, 0);
    lv_obj_add_event_cb(play_btn, on_play_btn_click, LV_EVENT_CLICKED, NULL);

    play_btn_label = lv_label_create(play_btn);
    lv_label_set_text(play_btn_label, LV_SYMBOL_PLAY " Play");
    lv_obj_set_style_text_font(play_btn_label, &lv_font_montserrat_16, 0);
    lv_obj_center(play_btn_label);
}

void da7212_test_ui_remove(void)
{
    if (root_page != NULL) {
        lv_obj_del(root_page);
        root_page = NULL;
        status_label = NULL;
        play_btn = NULL;
        play_btn_label = NULL;
        evt_cb = NULL;
    }
}

void da7212_test_ui_set_status(const char *text)
{
    if (status_label != NULL) {
        lv_label_set_text(status_label, text);
    }
}

void da7212_test_ui_set_playing(bool playing)
{
    is_playing = playing;
    if (play_btn_label != NULL) {
        if (playing) {
            lv_label_set_text(play_btn_label, LV_SYMBOL_STOP " Stop");
            lv_obj_set_style_bg_color(play_btn, lv_color_hex(0xD32F2F), 0);
        } else {
            lv_label_set_text(play_btn_label, LV_SYMBOL_PLAY " Play");
            lv_obj_set_style_bg_color(play_btn, lv_color_hex(0x388E3C), 0);
        }
        lv_obj_center(play_btn_label);
    }
}
