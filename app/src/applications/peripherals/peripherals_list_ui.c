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
//#include <zephyr/logging/log.h>

#include "peripherals_list_ui.h"

//LOG_MODULE_REGISTER(peripherals_app);   // TODO: remove Zephyr dependency from LVGL file

/*  TODO: which objects needs to be released when application exits.
 *
 */


static peripheral_selected_evt peripheral_selected_handler = NULL;

static void peripheral_button_pressed(lv_event_t *event)
{
    lv_obj_t *button = lv_event_get_current_target(event);
    uint32_t peripheral_index = lv_obj_get_index(button);
    if (peripheral_index < LV_PERIPHERALS_TYPE_LAST) {
        lv_peripherals_type_t peripheral = (lv_peripherals_type_t)(peripheral_index);
        if (NULL != peripheral_selected_handler) {
            peripheral_selected_handler(peripheral);
        }
    }
}

lv_obj_t *create_list_page(lv_obj_t *menu, peripheral_selected_evt fp)
{
    peripheral_selected_handler = fp;
    lv_obj_t *page = lv_menu_page_create(menu, NULL);
    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_t *cont = lv_menu_cont_create(page);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(cont, &style_outline_primary, LV_STATE_FOCUS_KEY);
    lv_obj_t *peripherals_list = lv_list_create(page);
    lv_obj_set_size(peripherals_list, LV_PCT(100), LV_PCT(100));
    lv_obj_center(peripherals_list);

    for(size_t i = 0; i < LV_PERIPHERALS_TYPE_LAST; i++) {
        lv_obj_t *button = lv_list_add_btn(peripherals_list, NULL, peripheral_names_text[i]);
        lv_obj_add_event_cb(button, peripheral_button_pressed, LV_EVENT_CLICKED, NULL);
    }

    return page;
}
