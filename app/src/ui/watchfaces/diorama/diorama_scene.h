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

/*
 * The landscape of the Diorama watchface: a sky, layers of mountains and forest that shift
 * against each other when the watch is tilted, and a lake that mirrors it all.
 *
 * Nothing here is an image. Each layer is a skyline, and the colors follow the height of the
 * sun, so the same scene is drawn for any time of day and any weather. The scene is rendered
 * one row at a time as RGB565 pixels and knows nothing about LVGL.
 */

#define DIORAMA_SIZE            240

typedef enum {
    DIORAMA_WEATHER_CLEAR,
    DIORAMA_WEATHER_CLOUDS,
    DIORAMA_WEATHER_RAIN,
    DIORAMA_WEATHER_SNOW,
    DIORAMA_WEATHER_FOG,
} diorama_weather_t;

typedef struct {
    /* Height of the sun over the horizon, in tenths of a degree */
    int16_t sun_elevation;
    /* Where the sun is on its way over the sky: 0 at the left edge, 255 at the right */
    uint8_t sun_pos;
    int16_t moon_elevation;
    uint8_t moon_pos;
    /* How much of the moon is lit: 0 new moon, 255 full moon */
    uint8_t moon_lit;
    bool moon_waxing;
    diorama_weather_t weather;
    /* 1 (light) to 3 (heavy) */
    uint8_t intensity;
    /* How the watch is held: -127 to 127, x to the right and y down on the display */
    int8_t tilt_x;
    int8_t tilt_y;
    /* 0: the layers are sunk below the lake, 255: in place */
    uint8_t rise;
} diorama_view_t;

/* Called for every area that changed, with display coordinates (inclusive) */
typedef void (*diorama_dirty_cb_t)(int x1, int y1, int x2, int y2);

void diorama_scene_init(uint32_t seed);

/* @return true if what is on the display changes with the new view. */
bool diorama_scene_set_view(const diorama_view_t *view);

/* Moves clouds, water, stars, rain and snow on by elapsed_ms. */
void diorama_scene_step(uint32_t elapsed_ms, diorama_dirty_cb_t dirty);

/* Writes the pixels x1 to x2 of row y to dst. */
void diorama_scene_render_row(int y, int x1, int x2, uint16_t *dst);
