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

#include "update_ui.h"
#include "ui/zsw_ui.h"
#include <assert.h>

#define BTN_SIZE        50
#define BTN_RADIUS      LV_RADIUS_CIRCLE
#define PROGRESS_ARC_W  4

LV_FONT_DECLARE(lv_font_montserrat_12);

static lv_obj_t *root_page = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *progress_arc = NULL;
static lv_obj_t *progress_pct_label = NULL;
static lv_obj_t *btn_ble_fota = NULL;
static lv_obj_t *btn_usb_fota = NULL;
static bool (*ble_toggle_callback)(void) = NULL;
static bool (*usb_toggle_callback)(void) = NULL;

static void btn_fota_usb_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (usb_toggle_callback) {
        usb_toggle_callback();
    }
}

static void btn_fota_ble_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (ble_toggle_callback) {
        ble_toggle_callback();
    }
}

static lv_obj_t *create_toggle_btn(lv_obj_t *parent, const char *symbol,
                                   lv_event_cb_t event_cb)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, BTN_SIZE, BTN_SIZE);
    lv_obj_set_style_bg_color(btn, zsw_color_gray(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, BTN_RADIUS, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, symbol);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(label);

    return btn;
}

void update_ui_show(lv_obj_t *root, bool (*ble_toggle_cb)(void), bool (*usb_toggle_cb)(void))
{
    ble_toggle_callback = ble_toggle_cb;
    usb_toggle_callback = usb_toggle_cb;

    assert(root_page == NULL);

    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(root_page, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_COVER, LV_PART_MAIN);

    // Arc progress bar
    progress_arc = lv_arc_create(root_page);
    lv_obj_set_size(progress_arc, LV_PCT(100), LV_PCT(100));
    lv_obj_center(progress_arc);
    lv_arc_set_range(progress_arc, 0, 100);
    lv_arc_set_value(progress_arc, 0);
    lv_arc_set_rotation(progress_arc, 270);
    lv_arc_set_bg_angles(progress_arc, 0, 360);
    lv_obj_set_style_arc_width(progress_arc, PROGRESS_ARC_W, LV_PART_MAIN);
    lv_obj_set_style_arc_color(progress_arc, zsw_color_gray(), LV_PART_MAIN);
    lv_obj_set_style_arc_width(progress_arc, PROGRESS_ARC_W, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(progress_arc, zsw_color_blue(), LV_PART_INDICATOR);
    lv_obj_remove_style(progress_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(progress_arc, LV_OBJ_FLAG_CLICKABLE);

    // Title
    lv_obj_t *title_label = lv_label_create(root_page);
    lv_label_set_text(title_label, LV_SYMBOL_DOWNLOAD "  Update");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_set_style_text_color(title_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 28);

    // Status
    status_label = lv_label_create(root_page);
    lv_obj_set_width(status_label, 180);
    lv_label_set_text(status_label, "Ready");
    lv_label_set_long_mode(status_label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(status_label, zsw_color_blue(), LV_PART_MAIN);
    lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 54);

    // Progress percentage label
    progress_pct_label = lv_label_create(root_page);
    lv_label_set_text(progress_pct_label, "0%");
    lv_obj_set_style_text_font(progress_pct_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(progress_pct_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(progress_pct_label, LV_ALIGN_CENTER, 0, -30);
    lv_obj_add_flag(progress_pct_label, LV_OBJ_FLAG_HIDDEN);

    // Help text
    lv_obj_t *help_label = lv_label_create(root_page);
    lv_label_set_text(help_label, "Enable BLE or USB, then\nstart update from phone/PC");
    lv_obj_set_width(help_label, 180);
    lv_obj_set_style_text_font(help_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(help_label, zsw_color_gray(), LV_PART_MAIN);
    lv_obj_set_style_text_align(help_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_long_mode(help_label, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_align(help_label, LV_ALIGN_CENTER, 0, -5);

    // Toggle buttons row
    lv_obj_t *btn_row = lv_obj_create(root_page);
    lv_obj_remove_style_all(btn_row);
    lv_obj_set_size(btn_row, 160, BTN_SIZE + 20);
    lv_obj_align(btn_row, LV_ALIGN_CENTER, 0, 40);
    lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // BLE toggle
    btn_ble_fota = create_toggle_btn(btn_row, LV_SYMBOL_BLUETOOTH, btn_fota_ble_cb);

    // USB toggle
    btn_usb_fota = create_toggle_btn(btn_row, LV_SYMBOL_USB, btn_fota_usb_cb);

    // Button labels underneath
    lv_obj_t *label_row = lv_obj_create(root_page);
    lv_obj_remove_style_all(label_row);
    lv_obj_set_size(label_row, 160, LV_SIZE_CONTENT);
    lv_obj_align(label_row, LV_ALIGN_CENTER, 0, 75);
    lv_obj_set_flex_flow(label_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(label_row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *ble_label = lv_label_create(label_row);
    lv_label_set_text(ble_label, "BLE");
    lv_obj_set_style_text_font(ble_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(ble_label, lv_color_white(), LV_PART_MAIN);

    lv_obj_t *usb_label = lv_label_create(label_row);
    lv_label_set_text(usb_label, "USB");
    lv_obj_set_style_text_font(usb_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(usb_label, lv_color_white(), LV_PART_MAIN);
}

void update_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void update_ui_set_status(const char *status)
{
    if (status_label) {
        lv_label_set_text(status_label, status);
    }
}

void update_ui_set_progress(int progress)
{
    if (progress_arc) {
        lv_arc_set_value(progress_arc, progress);
    }
    if (progress_pct_label) {
        lv_obj_clear_flag(progress_pct_label, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text_fmt(progress_pct_label, "%d%%", progress);
    }
}

void update_ui_update_ble_button_state(bool enabled)
{
    if (btn_ble_fota) {
        lv_obj_t *icon_label = lv_obj_get_child(btn_ble_fota, 0);
        if (enabled) {
            lv_obj_set_style_bg_color(btn_ble_fota, zsw_color_blue(), LV_PART_MAIN);
            lv_obj_set_style_text_color(icon_label, lv_color_white(), LV_PART_MAIN);
        } else {
            lv_obj_set_style_bg_color(btn_ble_fota, zsw_color_gray(), LV_PART_MAIN);
            lv_obj_set_style_text_color(icon_label, lv_color_white(), LV_PART_MAIN);
        }
    }
}

void update_ui_update_usb_button_state(bool enabled)
{
    if (btn_usb_fota) {
        lv_obj_t *icon_label = lv_obj_get_child(btn_usb_fota, 0);
        if (enabled) {
            lv_obj_set_style_bg_color(btn_usb_fota, zsw_color_blue(), LV_PART_MAIN);
            lv_obj_set_style_text_color(icon_label, lv_color_white(), LV_PART_MAIN);
        } else {
            lv_obj_set_style_bg_color(btn_usb_fota, zsw_color_gray(), LV_PART_MAIN);
            lv_obj_set_style_text_color(icon_label, lv_color_white(), LV_PART_MAIN);
        }
    }
}
