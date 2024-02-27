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

#define CONCATINATE_(a, b) a##b
#define CONCATINATE(a, b) CONCATINATE_(a, b)

#if CONFIG_ZSWATCH_PCB_REV > 3
#define ZSW_LV_IMG_DECLARE(var_name)
#define ZSW_LV_IMG_USE(var_name)        "S:"#var_name".bin"
#else
#define ZSW_LV_IMG_DECLARE(var_name) LV_IMG_DECLARE(var_name)
#define ZSW_LV_IMG_USE(var_name) &var_name
#endif

extern const lv_img_dsc_t *global_watchface_bg_img;

const lv_img_dsc_t *zsw_ui_utils_icon_from_weather_code(int code, lv_color_t *icon_color);