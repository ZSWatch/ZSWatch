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

#include <lvgl.h>

#include "ble/chronos/ble_chronos.h"

#define CHRONOS_FONT_DEFAULT &lv_font_montserrat_16

#if LV_FONT_MONTSERRAT_14 == 1
#define CHRONOS_FONT_14 &lv_font_montserrat_14
#else
#define CHRONOS_FONT_14 CHRONOS_FONT_DEFAULT
#endif

#if LV_FONT_MONTSERRAT_16 == 1
#define CHRONOS_FONT_16 &lv_font_montserrat_16
#else
#define CHRONOS_FONT_16 CHRONOS_FONT_DEFAULT
#endif

#if LV_FONT_MONTSERRAT_18 == 1
#define CHRONOS_FONT_18 &lv_font_montserrat_18
#else
#define CHRONOS_FONT_18 CHRONOS_FONT_DEFAULT
#endif

#if LV_FONT_MONTSERRAT_20 == 1
#define CHRONOS_FONT_20 &lv_font_montserrat_20
#else
#define CHRONOS_FONT_20 CHRONOS_FONT_DEFAULT
#endif

#if LV_FONT_MONTSERRAT_30
#define CHRONOS_FONT_30 &lv_font_montserrat_30
#else
#define CHRONOS_FONT_30 CHRONOS_FONT_DEFAULT
#endif

#if LV_FONT_MONTSERRAT_40 == 1
#define CHRONOS_FONT_40 &lv_font_montserrat_40
#else
#define CHRONOS_FONT_40 CHRONOS_FONT_DEFAULT
#endif

void chronos_check_recommended();

void chronos_ui_init(lv_obj_t *root);

void chronos_ui_deinit();

void chronos_ui_add_app_title(lv_obj_t *parent, const char *title, const void *src);
lv_obj_t *chronos_ui_add_page(lv_obj_t *parent);
lv_obj_t *chronos_ui_add_page_static(lv_obj_t *parent);

void chronos_ui_camera_panel_init(lv_obj_t *parent);
void chronos_ui_call_panel_init(lv_obj_t *parent);

void chronos_ui_about_init(lv_obj_t *page);
void chronos_ui_set_app_info(chronos_app_info_t info);
void chronos_ui_set_phone_info(chronos_phone_info_t info);

void chronos_ui_remote_touch_init(lv_obj_t *page);
void chronos_ui_set_remote_touch(chronos_remote_touch_t touch);

void chronos_ui_notifications_init(lv_obj_t *page);
void chronos_ui_add_notification(chronos_notification_t notification);
void chronos_ui_clear_notifications(bool info);
void chronos_ui_hide_notification_empty();

void chronos_ui_weather_init(lv_obj_t *page);
void chronos_ui_set_weather_city(const char *city);
void chronos_ui_set_weather_info(int temp, uint8_t icon, chronos_time_t updated);
void chronos_ui_add_weather_weekly(chronos_weather_t weather);
void chronos_ui_clear_weather_weekly(bool info);
void chronos_ui_clear_weather_hourly(bool info);
void chronos_ui_add_weather_hourly(chronos_hourly_forecast_t forecast);

void chronos_ui_navigation_init(lv_obj_t *page);
void chronos_ui_set_nav_info(const char *text, const char *title, const char *directions);
void chronos_ui_set_nav_icon_state(bool show);
void chronos_ui_set_nav_icon_px(uint16_t x, uint16_t y, bool on);
