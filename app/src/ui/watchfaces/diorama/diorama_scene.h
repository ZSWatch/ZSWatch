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
 * Nothing here is an image. Each layer is a skyline, what stands in it is a few runs of
 * pixels, and the colors follow the height of the sun, so the same scene is drawn for any
 * time of day and any weather. The scene is rendered as RGB565 pixels and knows nothing about
 * LVGL.
 */

#define DIORAMA_SIZE            240

typedef enum {
    DIORAMA_WEATHER_CLEAR,
    DIORAMA_WEATHER_CLOUDS,
    DIORAMA_WEATHER_RAIN,
    DIORAMA_WEATHER_SNOW,
    DIORAMA_WEATHER_FOG,
} diorama_weather_t;

/* An arc of a body that is not in the sky at all, as in the polar night */
#define DIORAMA_ARC_NONE        INT16_MIN

typedef struct {
    /* Height of the sun over the horizon, in tenths of a degree: what the colors follow */
    int16_t sun_elevation;
    /*
     * Where the sun is on its way over the sky: 0 when it rises and 256 when it sets. It is
     * below the horizon outside of that. How high the sun is drawn follows from this, not
     * from its real height: it always comes up clear of the landscape.
     */
    int16_t sun_arc;
    /* How high the sun stands in the middle of the day: 0 for a winter sun, 255 for high summer */
    uint8_t sun_height;
    /* The moon has the night: 0 when the sun sets and 256 when it rises */
    int16_t moon_arc;
    /* How much of the moon is lit: 0 new moon, 255 full moon */
    uint8_t moon_lit;
    bool moon_waxing;
    diorama_weather_t weather;
    /* 1 (light) to 3 (heavy) */
    uint8_t intensity;
    /* How far the right side of the watch is lowered, -127 to 127: the layers slide that way */
    int8_t tilt_x;
    /* 0: the layers are sunk below the lake, 255: in place */
    uint8_t rise;
} diorama_view_t;

/* Called for every area that changed, with display coordinates (inclusive) */
typedef void (*diorama_dirty_cb_t)(int x1, int y1, int x2, int y2);

void diorama_scene_init(uint32_t seed);

/*
 * @param first_row Set to the first row of the display that changes: the sky stays as it is
 *                  when only the tilt is new.
 *
 * @return true if what is on the display changes with the new view.
 */
bool diorama_scene_set_view(const diorama_view_t *view, int *first_row);

/* Moves clouds, water, stars, northern lights, rain and snow on by elapsed_ms. */
void diorama_scene_step(uint32_t elapsed_ms, diorama_dirty_cb_t dirty);

/*
 * Renders the area x1,y1 to x2,y2 (inclusive) as RGB565 pixels. dst is the pixel at x1,y1
 * and stride the number of bytes from one row to the next.
 */
void diorama_scene_render(int x1, int y1, int x2, int y2, uint16_t *dst, int stride);
