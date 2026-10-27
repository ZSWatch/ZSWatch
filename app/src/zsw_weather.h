/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

typedef enum {
    ZSW_WEATHER_UNKNOWN,
    ZSW_WEATHER_CLEAR,
    ZSW_WEATHER_CLOUDS,
    ZSW_WEATHER_DRIZZLE,
    ZSW_WEATHER_RAIN,
    ZSW_WEATHER_SNOW,
    ZSW_WEATHER_FOG,
    ZSW_WEATHER_THUNDER,
} zsw_weather_type_t;

typedef struct {
    zsw_weather_type_t type;
    uint8_t intensity; // 1 (light) to 3 (heavy)
} zsw_weather_condition_t;

zsw_weather_condition_t zsw_weather_classify(int code);
int wmo_code_to_weather_code(int wmo_code);
