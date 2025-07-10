/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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

#include "managers/zsw_power_manager.h"
#include "ui/popup/zsw_popup_window.h"

static void on_popup_button_pressed(lv_event_t *e);
static void on_popup_close_button_pressed(lv_event_t *e);
static void close_popup_timer(lv_timer_t *timer);

static lv_obj_t *mbox;
static lv_obj_t *yes_btn;
static lv_obj_t *no_btn;
static on_close_popup_cb_t on_close_cb;
static lv_timer_t *auto_close_timer;

void zsw_popup_show(char *title, char *body, on_close_popup_cb_t close_cb, uint32_t close_after_seconds,
                    bool display_yes_no)
{
    if (mbox) {
        // TODO handle queue of popups
        return;
    }
    zsw_power_manager_reset_idle_timout();
    on_close_cb = close_cb;
    lv_obj_t *close_btn = NULL;

    mbox = lv_msgbox_create(lv_layer_top());
    lv_msgbox_add_title(mbox, title);
    lv_msgbox_add_text(mbox, body);
    if (display_yes_no) {
        yes_btn = lv_msgbox_add_footer_button(mbox, "Yes");
        no_btn = lv_msgbox_add_footer_button(mbox, "No");
        lv_obj_add_event_cb(yes_btn, on_popup_button_pressed, LV_EVENT_CLICKED, NULL);
        lv_obj_add_event_cb(no_btn, on_popup_button_pressed, LV_EVENT_CLICKED, NULL);
    } else {
        close_btn = lv_msgbox_add_header_button(mbox, LV_SYMBOL_CLOSE);
        lv_obj_add_event_cb(close_btn, on_popup_close_button_pressed, LV_EVENT_CLICKED, NULL);
    }

    lv_obj_set_scrollbar_mode(lv_layer_top(), LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(mbox, LV_SCROLLBAR_MODE_OFF);
    lv_obj_center(mbox);
    if (close_btn) {
        lv_group_focus_obj(close_btn);
    }
    lv_obj_set_size(mbox, 180, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(mbox, 5, 0);
    lv_obj_clear_flag(mbox, LV_OBJ_FLAG_SCROLLABLE);

    static lv_style_t style_indic_not_bg;
    lv_style_init(&style_indic_not_bg);
    lv_style_set_bg_color(&style_indic_not_bg, lv_color_hex(0x2C3333));
    lv_obj_add_style(mbox, &style_indic_not_bg, 0);

    static lv_style_t color_style;
    lv_style_init(&color_style);
    lv_style_set_text_color(&color_style, lv_color_hex(0xCBE4DE));
    lv_style_set_bg_color(&color_style, lv_color_hex(0x2C3333));
    if (close_btn) {
        lv_obj_add_style(close_btn, &color_style, 0);
    }

    auto_close_timer = lv_timer_create(close_popup_timer, close_after_seconds * 1000,  NULL);
    lv_timer_set_repeat_count(auto_close_timer, 1);
}

void zsw_popup_remove(void)
{
    if (mbox) {
        lv_timer_del(auto_close_timer);
        lv_msgbox_close(mbox);
        mbox = NULL;
    }
}

static void on_popup_button_pressed(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target_obj(e);
    bool is_yes_btn = (target == yes_btn);

    zsw_popup_remove();
    if (is_yes_btn) {
        if (on_close_cb) {
            on_close_cb(true);
        }
    } else {
        if (on_close_cb) {
            on_close_cb(false);
        }
    }
}

static void on_popup_close_button_pressed(lv_event_t *e)
{
    zsw_popup_remove();
    if (on_close_cb) {
        on_close_cb(false);
    }
}

static void close_popup_timer(lv_timer_t *timer)
{
    zsw_popup_remove();
    if (on_close_cb) {
        on_close_cb(false);
    }
}
