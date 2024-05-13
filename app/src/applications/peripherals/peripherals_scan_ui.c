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
#include "peripherals_ui.h"
#include "peripherals_scan_ui.h"

static lv_obj_t *scan_result_list = NULL;
static lv_obj_t *scan_status_label = NULL;
static discovered_peripheral_clicked_evt discovered_peripheral_clicked_handler = NULL;

lv_obj_t *create_scan_page(lv_obj_t *menu, discovered_peripheral_clicked_evt discovered_peripheral_clicked_handler_fp)
{
    discovered_peripheral_clicked_handler = discovered_peripheral_clicked_handler_fp;

    lv_obj_t *page = lv_menu_page_create(menu, NULL);
    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_ON);
    lv_obj_t *cont = lv_menu_cont_create(page);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(cont, &style_outline_primary, LV_STATE_FOCUS_KEY);

    scan_result_list = lv_list_create(page);
    lv_obj_set_size(scan_result_list, LV_PCT(100), LV_PCT(100));
    lv_obj_center(scan_result_list);
    lv_peripherals_clear_discovered_peripherals_list();

    return page;
}

void lv_peripherals_set_scan_status(lv_peripherals_scan_status_t scan_status)
{
    if (scan_status < LV_PERIPHERALS_SCAN_STATUS_LAST) {
        lv_label_set_text(scan_status_label, scan_status_text[scan_status]);
    } else {
        lv_label_set_text(scan_status_label, "");
    }
}

static void discovered_peripheral_clicked(lv_event_t *event)
{
    lv_obj_t *button = lv_event_get_current_target(event);
    uint32_t discovered_peripheral_index = lv_obj_get_index(button) - 1;
    if (NULL != discovered_peripheral_clicked_handler) {
        discovered_peripheral_clicked_handler(discovered_peripheral_index);
    }
}

uint32_t lv_peripherals_add_discovered_peripheral(const char* name)
{
    if (NULL == name) {
        return UINT32_MAX;
    }
    lv_obj_t *button = lv_list_add_btn(scan_result_list, NULL, name);
    lv_obj_add_event_cb(button, discovered_peripheral_clicked, LV_EVENT_CLICKED, NULL);
    return lv_obj_get_index(button);
}

void lv_peripherals_clear_discovered_peripherals_list(void)
{
    lv_obj_clean(scan_result_list);
    scan_status_label = lv_list_add_text(scan_result_list, "");
    lv_peripherals_set_scan_status(LV_PERIPHERALS_SCAN_STATUS_LAST);
}
