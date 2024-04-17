/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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
#include <zephyr/init.h>
#include "../../zsw_settings.h"

// UI need to be initialized after watchface_app
#define WATCHFACE_UI_INIT_PRIO 99

typedef enum watchface_app_evt_t {
    WATCHFACE_APP_EVT_CLICK_BATT,
    WATCHFACE_APP_EVT_CLICK_STEP,
    WATCHFACE_APP_EVT_CLICK_WEATHER,
} watchface_app_evt_t;

typedef void(*watchface_app_evt_listener)(watchface_app_evt_t);

typedef struct watchface_ui_api_t {
    void (*show)(watchface_app_evt_listener, zsw_settings_watchface_t *settings);
    void (*remove)(void);
    void (*set_battery_percent)(int32_t percent, int32_t battery);
    void (*set_hrm)(int32_t bpm, int32_t oxygen);
    void (*set_step)(int32_t steps, int32_t distance, int32_t kcal);
    void (*set_ble_connected)(bool connected);
    void (*set_num_notifcations)(int32_t number);
    void (*set_weather)(int8_t temperature, int weather_code);
    void (*set_datetime)(int day_of_week, int date, int day, int month, int year, int weekday, int32_t hour, int32_t minute,
                         int32_t second, uint32_t usec);
    void (*set_watch_env_sensors)(int temperature, int humidity, int pressure, float iaq, float co2);
    void (*ui_invalidate_cached)(void);
} watchface_ui_api_t;

void watchface_app_start(lv_group_t *group, watchface_app_evt_listener evt_cb);
void watchface_app_stop(void);
void watchface_change(void);
void watchface_app_register_ui(watchface_ui_api_t *ui);
