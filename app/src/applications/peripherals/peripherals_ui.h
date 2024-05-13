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
#pragma once

#include <lvgl.h>

#include "peripherals_common_ui.h"
#include "peripherals_info_ui.h"
#include "peripherals_scan_ui.h"

typedef struct
{
    close_button_evt close_button_handler;
    peripheral_selected_evt peripheral_selected_handler;
    scan_start_evt scan_start_handler;
    peripheral_remove_evt peripheral_remove_handler;
    discovered_peripheral_clicked_evt discovered_peripheral_clicked_handler;
}lv_peripherals_handlers_t;

void lv_peripherals_create(lv_group_t *input_group, const lv_peripherals_handlers_t *handlers);
void lv_peripherals_remove(void);
void lv_peripherals_set_page(lv_peripherals_page_t page);

void lv_peripherals_update_info_page(const lv_peripheral_info_t *info);
void lv_peripherals_update_info_page_status(lv_peripherals_status_t status);

void lv_peripherals_set_scan_status(lv_peripherals_scan_status_t scan_status);
uint32_t lv_peripherals_add_discovered_peripheral(const char* name);
void lv_peripherals_clear_discovered_peripherals_list(void);
