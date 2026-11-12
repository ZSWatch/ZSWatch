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

#include "diorama_scene.h"

typedef struct {
    int year;
    int month;              /* 1 to 12 */
    int day;
    int minute_of_day;      /* Local time */
    int utc_offset_min;
    /* Without a place the sun rises at six and sets at six, as on the equator */
    bool has_place;
    float latitude;
    float longitude;
} diorama_sky_time_t;

typedef struct {
    /* Local time in minutes, or -1 when the sun does not rise or set that day */
    int sunrise;
    int sunset;
    /* When the sun does neither: true if it is up all day, false if it is down all day */
    bool polar_day;
} diorama_sky_events_t;

/*
 * Fills in where the sun and the moon are on their way over the sky and the phase of the moon,
 * and optionally when the sun rises and sets.
 */
void diorama_sky_compute(const diorama_sky_time_t *when, diorama_view_t *view, diorama_sky_events_t *events);
