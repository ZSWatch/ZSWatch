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

#include <lvgl.h>
#include "zsw_weather.h"

#define LIVING_SKY_SIZE 240
#define LIVING_SKY_SUN_X 62
#define LIVING_SKY_SUN_Y 56
#define LIVING_SKY_FP_SHIFT 4
#define LIVING_SKY_MAX_PARTICLES 36
#define LIVING_SKY_MAX_BLOBS 4
#define LIVING_SKY_FOG_BAND_WIDTH 170

typedef struct {
    int16_t x;
    int16_t y;
    int8_t vx;
    int8_t vy;
    uint8_t size;
    uint8_t phase;
} living_sky_particle_t;

typedef struct {
    zsw_weather_type_t weather;
    uint8_t intensity;
    uint16_t minute_of_day;
    bool night;
    uint8_t num_particles;
    uint8_t num_blobs;
    living_sky_particle_t particles[LIVING_SKY_MAX_PARTICLES];
    living_sky_particle_t blobs[LIVING_SKY_MAX_BLOBS];
    uint16_t frame;
    lv_opa_t flash_opa;
    int8_t bolt_x;
} living_sky_scene_t;

static inline bool living_sky_scene_has_stars(const living_sky_scene_t *scene)
{
    return scene->night && (scene->weather == ZSW_WEATHER_CLEAR ||
                            (scene->weather == ZSW_WEATHER_CLOUDS && scene->intensity < 3));
}

static inline bool living_sky_scene_has_sun(const living_sky_scene_t *scene)
{
    return !scene->night && (scene->weather == ZSW_WEATHER_CLEAR ||
                             (scene->weather == ZSW_WEATHER_CLOUDS && scene->intensity == 1));
}

void living_sky_scene_ui_draw(lv_layer_t *layer, const lv_area_t *coords,
                              const living_sky_scene_t *scene, bool playing);
