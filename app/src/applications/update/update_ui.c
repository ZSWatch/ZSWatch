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
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"

#define UPDATE_COLOR    lv_color_hex(0x14B8A6)
#define USB_BTN_SIZE    48
#define RING_WIDTH      4

static lv_obj_t *root_page = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *progress_ring = NULL;
static lv_obj_t *progress_pct_label = NULL;
static lv_obj_t *help_label = NULL;
static lv_obj_t *btn_usb_fota = NULL;
static bool (*usb_toggle_callback)(void) = NULL;

static void btn_fota_usb_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (usb_toggle_callback) {
        usb_toggle_callback();
    }
}

void update_ui_show(lv_obj_t *root, bool (*usb_toggle_cb)(void))
{
    usb_toggle_callback = usb_toggle_cb;

    assert(root_page == NULL);

    root_page = zsw_ui_screen_create(root, UPDATE_COLOR);

    // The bezel fills up as the image is received
    progress_ring = zsw_ui_ring_create(root_page, UPDATE_COLOR);
    lv_obj_set_style_arc_width(progress_ring, RING_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_width(progress_ring, RING_WIDTH, LV_PART_INDICATOR);

    zsw_ui_title_create(root_page, "UPDATE", ZSW_COLOR_TEXT_DIM);

    status_label = lv_label_create(root_page);
    lv_obj_set_width(status_label, 150);
    lv_label_set_text(status_label, "Ready");
    lv_label_set_long_mode(status_label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(status_label, ZSW_FONT_TEXT, LV_PART_MAIN);
    lv_obj_set_style_text_color(status_label, UPDATE_COLOR, LV_PART_MAIN);
    lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 44);

    // Shown instead of the help text once an update is running
    progress_pct_label = lv_label_create(root_page);
    lv_label_set_text(progress_pct_label, "0%");
    lv_obj_set_style_text_font(progress_pct_label, ZSW_FONT_NUMBER, LV_PART_MAIN);
    lv_obj_set_style_text_color(progress_pct_label, ZSW_COLOR_TEXT, LV_PART_MAIN);
    lv_obj_align(progress_pct_label, LV_ALIGN_CENTER, 0, -14);
    lv_obj_add_flag(progress_pct_label, LV_OBJ_FLAG_HIDDEN);

    help_label = lv_label_create(root_page);
    lv_label_set_text(help_label, "Bluetooth is always ready.\nFor USB, tap below.");
    lv_obj_set_width(help_label, 190);
    lv_obj_set_style_text_font(help_label, ZSW_FONT_CAPTION, LV_PART_MAIN);
    lv_obj_set_style_text_color(help_label, ZSW_COLOR_TEXT_DIM, LV_PART_MAIN);
    lv_obj_set_style_text_align(help_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(help_label, LV_ALIGN_CENTER, 0, -18);

    btn_usb_fota = zsw_ui_round_button_create(root_page, LV_SYMBOL_USB, USB_BTN_SIZE, ZSW_COLOR_SURFACE_HIGH);
    lv_obj_align(btn_usb_fota, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_add_event_cb(btn_usb_fota, btn_fota_usb_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *usb_label = lv_label_create(root_page);
    lv_label_set_text(usb_label, "USB");
    lv_obj_set_style_text_font(usb_label, ZSW_FONT_CAPTION, LV_PART_MAIN);
    lv_obj_set_style_text_color(usb_label, ZSW_COLOR_TEXT_DIM, LV_PART_MAIN);
    lv_obj_align(usb_label, LV_ALIGN_BOTTOM_MID, 0, -30);
}

void update_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
    status_label = NULL;
    progress_ring = NULL;
    progress_pct_label = NULL;
    btn_usb_fota = NULL;
}

void update_ui_set_status(const char *status)
{
    if (status_label) {
        lv_label_set_text(status_label, status);
    }
}

void update_ui_set_progress(int progress)
{
    if (progress_ring) {
        zsw_ui_ring_set_value(progress_ring, progress * ZSW_UI_RING_MAX / 100, false);
        lv_obj_add_flag(help_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(progress_pct_label, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text_fmt(progress_pct_label, "%d%%", progress);
    }
}

void update_ui_update_usb_button_state(bool enabled)
{
    if (btn_usb_fota) {
        lv_obj_set_style_bg_color(btn_usb_fota, enabled ? UPDATE_COLOR : ZSW_COLOR_SURFACE_HIGH, LV_PART_MAIN);
    }
}
