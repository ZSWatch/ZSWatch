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
#include "peripherals_list_ui.h"
#include "peripherals_info_ui.h"
#include "peripherals_scan_ui.h"

static lv_obj_t *menu = NULL;
static const lv_peripherals_handlers_t *evt_handlers = NULL;
static lv_obj_t *pages[LV_PERIPHERALS_PAGE_LAST] = {
    [LV_PERIPHERALS_PAGE_LIST] = NULL,
    [LV_PERIPHERALS_PAGE_INFO] = NULL,
    [LV_PERIPHERALS_PAGE_SCAN] = NULL,
};
static lv_peripherals_page_t active_page = LV_PERIPHERALS_PAGE_LIST;

static lv_peripherals_handlers_t dummy_handler = {
    .close_button_handler = NULL,
    .peripheral_selected_handler = NULL,
    .scan_start_handler = NULL,
    .peripheral_remove_handler = NULL,
    .discovered_peripheral_clicked_handler = NULL
};

static void create_styles(void)
{
    lv_style_init(&style_outline_primary);
    lv_style_set_border_color(&style_outline_primary, lv_color_hex(0xF99B7D));
    lv_style_set_border_width(&style_outline_primary, lv_disp_dpx(lv_disp_get_next(NULL), 3));
    lv_style_set_border_opa(&style_outline_primary, LV_OPA_50);
    lv_style_set_border_side(&style_outline_primary, LV_BORDER_SIDE_BOTTOM);
}

static void close_button_pressed(lv_event_t *event)
{
    if ((NULL != evt_handlers) && (NULL != evt_handlers->close_button_handler)) {
        evt_handlers->close_button_handler(active_page);
    }
}

static void create_menu(void)
{
    menu = lv_menu_create(lv_scr_act());
    lv_menu_set_mode_root_back_btn(menu, LV_MENU_ROOT_BACK_BTN_ENABLED);
    lv_obj_add_event_cb(menu, close_button_pressed, LV_EVENT_CLICKED, menu);
    lv_obj_set_size(menu, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_top(menu, 25, LV_PART_MAIN);
    lv_obj_set_style_pad_left(menu, 20, LV_PART_MAIN);
    lv_obj_center(menu);
}

void lv_peripherals_create(lv_group_t *input_group, const lv_peripherals_handlers_t *handlers)
{
    create_styles();
    create_menu();

    if (NULL == handlers) {
        handlers = &dummy_handler;
    }

    pages[LV_PERIPHERALS_PAGE_LIST] = create_list_page(menu, handlers->peripheral_selected_handler);
    pages[LV_PERIPHERALS_PAGE_INFO] = create_info_page(menu, handlers->scan_start_handler, handlers->peripheral_remove_handler);
    pages[LV_PERIPHERALS_PAGE_SCAN] = create_scan_page(menu, handlers->discovered_peripheral_clicked_handler);

    active_page = LV_PERIPHERALS_PAGE_LIST;
    lv_peripherals_set_page(active_page);

    evt_handlers = handlers;
}

void lv_peripherals_remove(void)
{
    if (NULL != menu) {
        /*  All widgets are children of menu so all should be cleared?
         */
        lv_obj_del(menu);
    }
    menu = NULL;
}

void lv_peripherals_set_page(lv_peripherals_page_t page)
{
    active_page = page;
    switch(active_page)
    {
        case LV_PERIPHERALS_PAGE_LIST:
        break;
        case LV_PERIPHERALS_PAGE_INFO:
        break;
        case LV_PERIPHERALS_PAGE_SCAN:
        lv_peripherals_clear_discovered_peripherals_list();
        break;
        case LV_PERIPHERALS_PAGE_LAST:
        break;
        default:
        break;
    }

    lv_menu_set_page(menu, pages[page]);
}
