/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2024.
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
#include <string.h>

#include "peripherals_info_ui.h"
#include "peripherals_common_ui.h"
#include "peripherals_ui.h"


static lv_obj_t *peripheral_type_label = NULL;
static lv_obj_t *peripheral_name_label = NULL;
static lv_obj_t *peripheral_status_label = NULL;
static lv_obj_t *scan_start_button = NULL;
static lv_obj_t *remove_peripheral_button = NULL;   // TODO: align name

static scan_start_evt scan_start_handler = NULL;
static peripheral_remove_evt peripheral_remove_handler = NULL;

const lv_peripheral_info_t default_info = {
    LV_PERIPHERALS_TYPE_LAST,    // invalid value on purpose
    "",
    LV_PERIPHERALS_STATUS_NO_DEVICE,
};

static void scan_start_button_pressed(lv_event_t *event)
{
    if (NULL != scan_start_handler) {
        scan_start_handler();
    }
}

static void remove_peripheral_pressed(lv_event_t *event)
{
    if (NULL != peripheral_remove_handler) {
        peripheral_remove_handler();
    }
}

lv_obj_t *create_info_page(lv_obj_t *menu, scan_start_evt scan_start_handler_fp, peripheral_remove_evt peripheral_remove_handler_fp)
{
    scan_start_handler = scan_start_handler_fp;
    peripheral_remove_handler = peripheral_remove_handler_fp;

    lv_obj_t *page = lv_menu_page_create(menu, NULL);
    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_ON);
    lv_obj_t *cont = lv_menu_cont_create(page);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(cont, &style_outline_primary, LV_STATE_FOCUS_KEY);

    peripheral_type_label = lv_label_create(cont);
    peripheral_name_label = lv_label_create(cont);
    peripheral_status_label = lv_label_create(cont);

    remove_peripheral_button = lv_btn_create(cont);
    lv_label_set_text(lv_label_create(remove_peripheral_button), "Remove");
    lv_obj_add_event_cb(remove_peripheral_button, remove_peripheral_pressed, LV_EVENT_CLICKED, NULL);

    scan_start_button = lv_btn_create(cont);
    lv_label_set_text(lv_label_create(scan_start_button), "Scan");
    lv_obj_add_event_cb(scan_start_button, scan_start_button_pressed, LV_EVENT_CLICKED, NULL);

    lv_peripherals_update_info_page(&default_info);
    return page;
}

void lv_peripherals_update_info_page(const lv_peripheral_info_t *info)
{
    #define TEXT_BUFFER_SIZE        ((size_t)(40))
    static char text_buffer[TEXT_BUFFER_SIZE];
    static size_t text_buffer_consumption;

    if (NULL == info) {
        return;
    }

    if (info->peripheral_type < LV_PERIPHERALS_TYPE_LAST) {
        lv_label_set_text(peripheral_type_label, peripheral_names_text[info->peripheral_type]);
    } else {
        lv_label_set_text(peripheral_type_label, "Invalid");
    }

    if ((NULL != info->peripheral_name)
    && ((LV_PERIPHERALS_STATUS_BONDED_NOT_IN_RANGE == info->status) || (LV_PERIPHERALS_STATUS_BONDED_CONNECTED == info->status)))
    {
        text_buffer_consumption = lv_snprintf(text_buffer, TEXT_BUFFER_SIZE, "Name: %s", info->peripheral_name);
    } else {
        text_buffer_consumption = lv_snprintf(text_buffer, TEXT_BUFFER_SIZE, "Name: N/A");  // TODO: N/A not on if device does not exists.
    }
    lv_label_set_text(peripheral_name_label, text_buffer);
    if (text_buffer_consumption >= TEXT_BUFFER_SIZE) {
        // TODO: add error log
    }

    if (info->status < LV_PERIPHERALS_STATUS_LAST) {
        text_buffer_consumption = lv_snprintf(text_buffer, TEXT_BUFFER_SIZE, "Status: %s", peripheral_status_text[info->status]);
    } else {
        text_buffer_consumption = lv_snprintf(text_buffer, TEXT_BUFFER_SIZE, "Status: unknown");
    }
    lv_label_set_text(peripheral_status_label, text_buffer);
    if (text_buffer_consumption >= TEXT_BUFFER_SIZE) {
        // TODO: add error log
    }

    // TODO: disable remove button if there is no device.
}

void lv_peripherals_update_info_page_status(lv_peripherals_status_t status)
{
    #define TEXT_BUFFER_SIZE        ((size_t)(40))
    static char text_buffer[TEXT_BUFFER_SIZE];
    static size_t text_buffer_consumption;

    if (status < LV_PERIPHERALS_STATUS_LAST) {
        text_buffer_consumption = lv_snprintf(text_buffer, TEXT_BUFFER_SIZE, "Status: %s", peripheral_status_text[status]);
    } else {
        text_buffer_consumption = lv_snprintf(text_buffer, TEXT_BUFFER_SIZE, "Status: unknown");
    }
    lv_label_set_text(peripheral_status_label, text_buffer);
    if (text_buffer_consumption >= TEXT_BUFFER_SIZE) {
        // TODO: add error log
    }
}
