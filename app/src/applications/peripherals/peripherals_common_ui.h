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

#include <stdint.h>
#include "lvgl.h"

typedef enum
{
    /*  @warning: Member's values must monothonically increaes. Theres should not be any gaps.
     */
    LV_PERIPHERALS_TYPE_HEART_RATE_MONITOR = 0,
    LV_PERIPHERALS_TYPE_CYCLING_SPEED_SENSOR,
    LV_PERIPHERALS_TYPE_CYCLING_CANDENCE_SENSOR,
    LV_PERIPHERALS_TYPE_LAST
}lv_peripherals_type_t;

typedef enum
{
    /*  @warning: Member's values must monothonically increaes. Theres should not be any gaps.
     */
    LV_PERIPHERALS_STATUS_NO_DEVICE = 0,
    LV_PERIPHERALS_STATUS_BONDED_NOT_IN_RANGE,
    LV_PERIPHERALS_STATUS_BONDED_CONNECTED,
    LV_PERIPHERALS_STATUS_LAST
}lv_peripherals_status_t;

typedef enum
{
    /*  @warning: Member's values must monothonically increaes. Theres should not be any gaps.
     */
    LV_PERIPHERALS_SCAN_STATUS_IN_PROGRESS = 0,
    LV_PERIPHERALS_SCAN_STATUS_DONE,
    LV_PERIPHERALS_SCAN_STATUS_ERROR,
    LV_PERIPHERALS_SCAN_STATUS_LAST
}lv_peripherals_scan_status_t;

typedef struct
{
    lv_peripherals_type_t peripheral_type;
    const char *peripheral_name;
    lv_peripherals_status_t status;
}lv_peripheral_info_t;

typedef enum
{
    /*  @warning: Member's values must monothonically increaes. Theres should not be any gaps.
     */
    LV_PERIPHERALS_PAGE_LIST = 0,
    LV_PERIPHERALS_PAGE_INFO,
    LV_PERIPHERALS_PAGE_SCAN,
    LV_PERIPHERALS_PAGE_LAST
}lv_peripherals_page_t;

typedef void(*close_button_evt)(lv_peripherals_page_t page);
typedef void(*peripheral_selected_evt)(lv_peripherals_type_t peripheral_type);
typedef void(*scan_start_evt)(void);
typedef void(*peripheral_remove_evt)(void);
typedef void(*discovered_peripheral_clicked_evt)(uint32_t index);

extern const char *peripheral_names_text[LV_PERIPHERALS_TYPE_LAST];
extern const char *peripheral_status_text[LV_PERIPHERALS_STATUS_LAST];
extern const char *scan_status_text[LV_PERIPHERALS_SCAN_STATUS_LAST];

extern lv_style_t style_outline_primary;
