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

#include <stdbool.h>
#include <stdint.h>
#include <lvgl.h>

#define TILES_TEXT_LEN      64
#define TILES_HR_POINTS     32

typedef enum {
    TILE_WEATHER,
    TILE_ACTIVITY,
    TILE_HEART_RATE,
    TILE_MUSIC,
    TILE_BATTERY,
    TILE_COUNT,
} tiles_tile_t;

typedef struct {
    bool weather_valid;
    int8_t temperature_c;
    int weather_code;
    uint16_t humidity;
    char weather_text[TILES_TEXT_LEN];

    uint32_t steps;
    uint32_t step_goal;

    bool hr_available;
    int16_t hr_bpm;                     /* 0 when there is no recent measurement */
    int32_t hr_age_s;
    uint8_t hr_num_points;
    int16_t hr_points[TILES_HR_POINTS];

    bool music_valid;
    bool music_playing;
    char track[TILES_TEXT_LEN];
    char artist[TILES_TEXT_LEN];

    int battery_percent;
    int battery_mv;
    bool battery_charging;
    int battery_minutes_left;           /* To empty, or to full while charging. -1 unknown */
} tiles_data_t;

typedef enum {
    TILES_MUSIC_PREVIOUS,
    TILES_MUSIC_PLAY_PAUSE,
    TILES_MUSIC_NEXT,
} tiles_music_action_t;

/** @brief Tapping a tile opens its app. */
typedef void (*tiles_open_cb_t)(tiles_tile_t tile);
typedef void (*tiles_music_cb_t)(tiles_music_action_t action);

/** @brief Build the tiles. The heart rate tile is left out without a sensor. */
void tiles_ui_show(lv_obj_t *root, lv_group_t *group, tiles_open_cb_t open_cb, tiles_music_cb_t music_cb,
                   const tiles_data_t *data);

void tiles_ui_update(const tiles_data_t *data);

void tiles_ui_remove(void);
