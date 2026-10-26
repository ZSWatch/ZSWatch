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

#include "zsw_weather.h"

zsw_weather_condition_t zsw_weather_classify(int code)
{
    zsw_weather_condition_t condition = { .type = ZSW_WEATHER_UNKNOWN, .intensity = 2 };

    switch (code / 100) {
        case 2:
            condition.type = ZSW_WEATHER_THUNDER;
            condition.intensity = 3;
            break;
        case 3:
            condition.type = ZSW_WEATHER_DRIZZLE;
            condition.intensity = 1;
            break;
        case 5:
            if (code == 511) {
                condition.type = ZSW_WEATHER_SNOW;
            } else {
                condition.type = ZSW_WEATHER_RAIN;
                if (code == 500 || code == 520) {
                    condition.intensity = 1;
                } else if (code != 501 && code != 521) {
                    condition.intensity = 3;
                }
            }
            break;
        case 6:
            condition.type = ZSW_WEATHER_SNOW;
            if (code == 600 || code == 620) {
                condition.intensity = 1;
            } else if (code == 602 || code == 622) {
                condition.intensity = 3;
            }
            break;
        case 7:
            condition.type = ZSW_WEATHER_FOG;
            break;
        case 8:
            if (code == 800) {
                condition.type = ZSW_WEATHER_CLEAR;
            } else {
                condition.type = ZSW_WEATHER_CLOUDS;
                condition.intensity = code == 801 ? 1 : (code == 802 ? 2 : 3);
            }
            break;
        default:
            condition.type = ZSW_WEATHER_UNKNOWN;
            break;
    }
    return condition;
}

int wmo_code_to_weather_code(int wmo_code)
{
    switch (wmo_code) {
        case 0: {
            return 800;
        }
        case 1: {
            return 800;
        }
        case 2: {
            return 800;
        }
        case 3: {
            return 803;
        }
        case 45: {
            return 700;
        }
        case 48: {
            return 700;
        }
        case 51: {
            return 700;
        }
        case 53: {
            return 512;
        }
        case 55: {
            return 512;
        }
        case 80: {
            return 512;
        }
        case 81: {
            return 512;
        }
        case 82: {
            return 512;
        }
        case 61: {
            return 512;
        }
        case 63: {
            return 512;
        }
        case 65: {
            return 512;
        }
        case 56: {
            return 512;
        }
        case 57: {
            return 512;
        }
        case 66: {
            return 512;
        }
        case 67: {
            return 512;
        }
        case 71: {
            return 511;
        }
        case 73: {
            return 511;
        }
        case 75: {
            return 511;
        }
        case 77: {
            return 511;
        }
        case 85: {
            return 511;
        }
        case 86: {
            return 511;
        }
        case 95: {
            return 200;
        }
        case 96: {
            return 200;
        }
        case 99: {
            return 200;
        }
    }
    return -1;
}
