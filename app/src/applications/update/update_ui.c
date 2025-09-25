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
#include "filesystem/zsw_filesystem.h"
#include <assert.h>

static lv_obj_t *root_page = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *progress_bar = NULL;
static lv_obj_t *btn_ble_fota = NULL;
static lv_obj_t *btn_usb_fota = NULL;
static bool (*ble_toggle_callback)(void) = NULL;
static bool (*usb_toggle_callback)(void) = NULL;

static void btn_flash_erase_cb(lv_event_t *e)
{
    // Perform the flash erase operation
    update_ui_set_status("Status: Erasing flash... Don't do anything until it's done.");
    int rc = zsw_filesytem_erase();
    if (rc == 0) {
        update_ui_set_status("Status: Flash erase successful");
    } else {
        update_ui_set_status("Status: Flash erase failed");
    }
}

static void btn_fota_usb_cb(lv_event_t *e)
{
    if (usb_toggle_callback) {
        usb_toggle_callback();
    }
}

static void btn_fota_ble_cb(lv_event_t *e)
{
    if (ble_toggle_callback) {
        ble_toggle_callback();
    }
}

void update_ui_show(lv_obj_t *root, bool (*ble_toggle_cb)(void), bool (*usb_toggle_cb)(void))
{
    ble_toggle_callback = ble_toggle_cb;
    usb_toggle_callback = usb_toggle_cb;

    assert(root_page == NULL);

    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_width(root_page, lv_pct(100));
    lv_obj_set_height(root_page, lv_pct(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Create a label for status text
    status_label = lv_label_create(root_page);
    lv_obj_set_width(status_label, LV_PCT(70));
    lv_label_set_text(status_label, "Status: Idle - Enable BLE or USB FOTA");
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_label_set_long_mode(status_label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);

    // Create a progress bar for firmware update progress
    progress_bar = lv_bar_create(root_page);
    lv_obj_set_size(progress_bar, 200, 15);
    lv_obj_align(progress_bar, LV_ALIGN_TOP_MID, 0, 45);
    lv_bar_set_range(progress_bar, 0, 100);
    lv_bar_set_value(progress_bar, 0, LV_ANIM_OFF);

    // Create BLE FOTA toggle button
    btn_ble_fota = lv_btn_create(root_page);
    lv_obj_set_size(btn_ble_fota, 90, 40);
    lv_obj_align(btn_ble_fota, LV_ALIGN_TOP_LEFT, 15, 75);
    lv_obj_add_event_cb(btn_ble_fota, btn_fota_ble_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *btn_ble_label = lv_label_create(btn_ble_fota);
    lv_label_set_text(btn_ble_label, "BLE: OFF");
    lv_obj_center(btn_ble_label);
    lv_obj_set_style_bg_color(btn_ble_fota, lv_color_hex(0xAA0000), LV_PART_MAIN);

    // Create USB FOTA toggle button (with spacing from BLE button)
    btn_usb_fota = lv_btn_create(root_page);
    lv_obj_set_size(btn_usb_fota, 90, 40);
    lv_obj_align(btn_usb_fota, LV_ALIGN_TOP_RIGHT, -15, 75);
    lv_obj_add_event_cb(btn_usb_fota, btn_fota_usb_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *btn_usb_label = lv_label_create(btn_usb_fota);
    lv_label_set_text(btn_usb_label, "USB: OFF");
    lv_obj_center(btn_usb_label);
    lv_obj_set_style_bg_color(btn_usb_fota, lv_color_hex(0xAA0000), LV_PART_MAIN);

    // Create a button for erasing flash
    lv_obj_t *btn_flash_erase = lv_btn_create(root_page);
    lv_obj_set_size(btn_flash_erase, 150, 40);
    lv_obj_align(btn_flash_erase, LV_ALIGN_TOP_MID, 0, 130);
    lv_obj_add_event_cb(btn_flash_erase, btn_flash_erase_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *btn_erase_label = lv_label_create(btn_flash_erase);
    lv_label_set_text(btn_erase_label, "Erase Flash");
    lv_obj_center(btn_erase_label);
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
    if (progress_bar) {
        lv_bar_set_value(progress_bar, progress, LV_ANIM_OFF);
    }
}

void update_ui_update_ble_button_state(bool enabled)
{
    if (btn_ble_fota) {
        lv_obj_t *btn_label = lv_obj_get_child(btn_ble_fota, 0);
        if (enabled) {
            lv_label_set_text(btn_label, "BLE: ON");
            lv_obj_set_style_bg_color(btn_ble_fota, lv_color_hex(0x00AA00), LV_PART_MAIN);
        } else {
            lv_label_set_text(btn_label, "BLE: OFF");
            lv_obj_set_style_bg_color(btn_ble_fota, lv_color_hex(0xAA0000), LV_PART_MAIN);
        }
    }
}

void update_ui_update_usb_button_state(bool enabled)
{
    if (btn_usb_fota) {
        lv_obj_t *btn_label = lv_obj_get_child(btn_usb_fota, 0);
        if (enabled) {
            lv_label_set_text(btn_label, "USB: ON");
            lv_obj_set_style_bg_color(btn_usb_fota, lv_color_hex(0x00AA00), LV_PART_MAIN);
        } else {
            lv_label_set_text(btn_label, "USB: OFF");
            lv_obj_set_style_bg_color(btn_usb_fota, lv_color_hex(0xAA0000), LV_PART_MAIN);
        }
    }
}
