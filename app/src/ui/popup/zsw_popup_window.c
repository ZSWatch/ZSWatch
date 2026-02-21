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

#include "managers/zsw_power_manager.h"
#include "ui/popup/zsw_popup_window.h"
#include "ui/zsw_ui.h"

static void on_popup_button_pressed(lv_event_t *e);
static void on_popup_close_button_pressed(lv_event_t *e);
static void close_popup_timer(lv_timer_t *timer);
static void icon_popup_close_cb(lv_event_t *e);
static void close_icon_popup_timer(lv_timer_t *timer);
static void icon_popup_remove_internal(void);

static lv_obj_t *mbox;
static lv_obj_t *yes_btn;
static lv_obj_t *no_btn;
static on_close_popup_cb_t on_close_cb;
static lv_timer_t *auto_close_timer;

static lv_obj_t *icon_popup;
static on_close_popup_cb_t icon_popup_close_cb_fn;
static lv_timer_t *icon_popup_auto_close_timer;

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
    icon_popup_remove_internal();
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

/* --------------------------------------------------------------------------
 * Icon Popup
 * -------------------------------------------------------------------------- */

#define ICON_POPUP_WIDTH       150
#define ICON_POPUP_ICON_SIZE   64
#define ICON_POPUP_PADDING     12
#define ICON_POPUP_RADIUS      16

static void icon_popup_remove_internal(void)
{
    if (icon_popup) {
        if (icon_popup_auto_close_timer) {
            lv_timer_del(icon_popup_auto_close_timer);
            icon_popup_auto_close_timer = NULL;
        }
        lv_obj_delete(icon_popup);
        icon_popup = NULL;
    }
}

static void icon_popup_close_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    on_close_popup_cb_t cb = icon_popup_close_cb_fn;

    icon_popup_remove_internal();
    if (cb) {
        cb(false);
    }
}

static void close_icon_popup_timer(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    on_close_popup_cb_t cb = icon_popup_close_cb_fn;

    icon_popup_remove_internal();
    if (cb) {
        cb(false);
    }
}

void zsw_popup_show_with_icon(const char *title, const char *body, const void *icon,
                              on_close_popup_cb_t close_cb, uint32_t close_after_seconds)
{
    if (icon_popup) {
        icon_popup_remove_internal();
    }

    zsw_power_manager_reset_idle_timout();
    icon_popup_close_cb_fn = close_cb;

    /* Semi-transparent backdrop so the popup stands out */
    icon_popup = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(icon_popup);
    lv_obj_set_size(icon_popup, ICON_POPUP_WIDTH, LV_SIZE_CONTENT);
    lv_obj_center(icon_popup);
    lv_obj_clear_flag(icon_popup, LV_OBJ_FLAG_SCROLLABLE);

    /* Card background */
    lv_obj_set_style_bg_opa(icon_popup, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(icon_popup, lv_color_hex(0x1E2530), 0);
    lv_obj_set_style_radius(icon_popup, ICON_POPUP_RADIUS, 0);
    lv_obj_set_style_border_width(icon_popup, 0, 0);
    lv_obj_set_style_shadow_width(icon_popup, 12, 0);
    lv_obj_set_style_shadow_color(icon_popup, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(icon_popup, LV_OPA_60, 0);
    lv_obj_set_style_pad_all(icon_popup, ICON_POPUP_PADDING, 0);
    lv_obj_set_style_pad_top(icon_popup, ICON_POPUP_PADDING + 4, 0);
    lv_obj_set_style_pad_bottom(icon_popup, ICON_POPUP_PADDING + 4, 0);

    lv_obj_set_flex_flow(icon_popup, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(icon_popup, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(icon_popup, 6, 0);

    /* App icon */
    if (icon != NULL) {
        lv_obj_t *img = lv_image_create(icon_popup);
        lv_image_set_src(img, icon);
        lv_obj_set_size(img, ICON_POPUP_ICON_SIZE, ICON_POPUP_ICON_SIZE);
        lv_image_set_inner_align(img, LV_IMAGE_ALIGN_CENTER);
        lv_obj_set_style_radius(img, 12, 0);
        lv_obj_set_style_clip_corner(img, true, 0);
    }

    /* Title label */
    lv_obj_t *lbl_title = lv_label_create(icon_popup);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(lbl_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(lbl_title, ICON_POPUP_WIDTH - 2 * ICON_POPUP_PADDING);
    lv_label_set_long_mode(lbl_title, LV_LABEL_LONG_WRAP);

    /* Body label */
    lv_obj_t *lbl_body = lv_label_create(icon_popup);
    lv_label_set_text(lbl_body, body);
    lv_obj_set_style_text_color(lbl_body, lv_color_hex(0x9EA8B8), 0);
    lv_obj_set_style_text_font(lbl_body, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(lbl_body, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(lbl_body, ICON_POPUP_WIDTH - 2 * ICON_POPUP_PADDING);
    lv_label_set_long_mode(lbl_body, LV_LABEL_LONG_WRAP);

    /* Tap anywhere on the popup to dismiss */
    lv_obj_add_flag(icon_popup, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(icon_popup, icon_popup_close_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_set_scrollbar_mode(lv_layer_top(), LV_SCROLLBAR_MODE_OFF);

    /* Auto-close timer */
    icon_popup_auto_close_timer = lv_timer_create(close_icon_popup_timer,
                                                  close_after_seconds * 1000, NULL);
    lv_timer_set_repeat_count(icon_popup_auto_close_timer, 1);
}
