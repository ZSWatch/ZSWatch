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

typedef bool zsw_settings_ble_aoa_en_t;
#define ZSW_SETTINGS_KEY_BLE_AOA_EN "aoa_en"
#define ZSW_SETTINGS_BLE_AOA_EN (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_BLE_AOA_EN)

typedef int32_t zsw_settings_ble_aoa_int_t;
#define ZSW_SETTINGS_KEY_BLE_AOA_INT "aoa_int"
#define ZSW_SETTINGS_BLE_AOA_INT (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_BLE_AOA_INT)

typedef struct {
    bool animations_on;
    uint8_t default_index;
} zsw_settings_watchface_t;
#define ZSW_SETTINGS_KEY_WATCHFACE "watchface"
#define ZSW_SETTINGS_WATCHFACE (ZSW_SETTINGS_PATH "/" ZSW_SETTINGS_KEY_WATCHFACE)