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

#pragma once
#include <inttypes.h>
#include <stdbool.h>

#define ZSW_SETTINGS_PATH "settings"

typedef int32_t zsw_settings_brightness_t;
#define ZSW_SETTINGS_KEY_BRIGHTNESS "bri"
#define ZSW_SETTINGS_BRIGHTNESS (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_BRIGHTNESS)

typedef bool zsw_settings_vib_on_press_t;
#define ZSW_SETTINGS_KEY_VIBRATION_ON_PRESS "vib"
#define ZSW_SETTINGS_VIBRATE_ON_PRESS (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_VIBRATION_ON_PRESS)

typedef bool zsw_settings_display_always_on_t;
#define ZSW_SETTINGS_KEY_DISPLAY_ALWAYS_ON "disp_on"
#define ZSW_SETTINGS_DISPLAY_ALWAYS_ON (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_DISPLAY_ALWAYS_ON)

typedef bool zsw_settings_ble_log_en_t;
#define ZSW_SETTINGS_KEY_BLE_LOG_EN "ble_log"
#define ZSW_SETTINGS_BLE_LOG_EN (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_BLE_LOG_EN)

typedef bool zsw_settings_ble_aoa_en_t;
#define ZSW_SETTINGS_KEY_BLE_AOA_EN "aoa_en"
#define ZSW_SETTINGS_BLE_AOA_EN (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_BLE_AOA_EN)

typedef int32_t zsw_settings_ble_aoa_int_t;
#define ZSW_SETTINGS_KEY_BLE_AOA_INT "aoa_int"
#define ZSW_SETTINGS_BLE_AOA_INT (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_BLE_AOA_INT)

typedef struct {
    bool animations_on;
    uint8_t watchface_index;
    bool smooth_second_hand;
    bool relative_battery;
} zsw_settings_watchface_t;
#define ZSW_SETTINGS_KEY_WATCHFACE "watchface"
#define ZSW_SETTINGS_WATCHFACE (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_WATCHFACE)

typedef bool zsw_settings_onboarding_done_t;
#define ZSW_SETTINGS_KEY_ONBOARDING_DONE "onboard"
#define ZSW_SETTINGS_ONBOARDING_DONE (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_ONBOARDING_DONE)

int zsw_settings_erase_all(void);
