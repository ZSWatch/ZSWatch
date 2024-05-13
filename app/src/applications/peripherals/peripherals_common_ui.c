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

#include "peripherals_common_ui.h"

const char *peripheral_names_text[LV_PERIPHERALS_TYPE_LAST] = {       // TODO: add _text to name
    [LV_PERIPHERALS_TYPE_HEART_RATE_MONITOR] = "Heart-Rate Monitor",
    [LV_PERIPHERALS_TYPE_CYCLING_SPEED_SENSOR] = "Cycling-Speed Sensor",
    [LV_PERIPHERALS_TYPE_CYCLING_CANDENCE_SENSOR] = "Cadence-Speed Sensor",
};

const char *peripheral_status_text[LV_PERIPHERALS_STATUS_LAST] = {
    [LV_PERIPHERALS_STATUS_NO_DEVICE] = "no-device",
    [LV_PERIPHERALS_STATUS_BONDED_NOT_IN_RANGE] = "not in range",
    [LV_PERIPHERALS_STATUS_BONDED_CONNECTED] = "connected"
};

const char *scan_status_text[LV_PERIPHERALS_SCAN_STATUS_LAST] = {
    [LV_PERIPHERALS_SCAN_STATUS_IN_PROGRESS] = "In progress...",
    [LV_PERIPHERALS_SCAN_STATUS_DONE] = "Done",
    [LV_PERIPHERALS_SCAN_STATUS_ERROR] = "Error"
};

lv_style_t style_outline_primary;
