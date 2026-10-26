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
#include "zsw_weather.h"

/*
 * Living Sky effect layer: a time of day sky tint plus weather effects (rain, snow,
 * fog, clouds, stars, sun, lightning), all drawn by one LVGL object in its draw event.
 *
 * Effects only move during a short burst started with living_sky_fx_play(). Outside a
 * burst no LVGL timer exists and the last frame stays on screen as a static image.
 * All state is static and bounded (MAX particles), nothing is allocated per frame.
 */

/**
 * @brief Create the effect layer as a full size, non clickable child of parent.
 *
 * Only one effect layer can exist at a time.
 */
lv_obj_t *living_sky_fx_create(lv_obj_t *parent);

/**
 * @brief Stop any burst and forget the effect object. Call before deleting its parent.
 */
void living_sky_fx_delete(void);

/**
 * @brief Set the weather from an OpenWeatherMap condition code.
 *
 * @param weather_code OpenWeatherMap condition code (0 = unknown, shown as clear sky)
 * @param wind Wind speed as reported by the phone (km/h)
 * @param wind_direction Direction the wind comes from in degrees (0 = north)
 * @return true if the visible effect changed
 */
bool living_sky_fx_set_weather(int weather_code, uint16_t wind, uint16_t wind_direction);

/**
 * @brief Set the local time used for the sky tint and day/night effects.
 */
void living_sky_fx_set_time(int hour, int minute);

/**
 * @brief Animate the effects for duration_ms, then stop and keep the last frame.
 *
 * Restarts the burst if one is already running.
 */
void living_sky_fx_play(uint32_t duration_ms);

/**
 * @brief Stop the burst now. The current frame stays on screen.
 */
void living_sky_fx_stop(void);

bool living_sky_fx_is_playing(void);

zsw_weather_type_t living_sky_fx_get_weather(void);
