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
 * The world of the Snow watchface: a grid of grains inside the round display.
 * Loose snow falls the way gravity points, one cell per step. The digits of the
 * time are grains that are held in place. When the time changes, the digits
 * that changed are let go, and grains from the pile fly up to form the new ones.
 *
 * No drawing here: the UI asks for the cells and the flying grains and draws them.
 */

#define SNOW_GRID_SIZE      120
#define SNOW_CELL_PX        2

/* Rows are grouped in bands for redrawing: each band reports the columns that changed. */
#define SNOW_BAND_ROWS      8
#define SNOW_NUM_BANDS      (SNOW_GRID_SIZE / SNOW_BAND_ROWS)

#define SNOW_MAX_FLYING     192

typedef enum {
    SNOW_CELL_EMPTY,
    SNOW_CELL_SNOW,
    SNOW_CELL_DIGIT,
} snow_cell_type_t;

/* Four shades per grain type give the snow its texture. A grain keeps its shade when it moves. */
#define SNOW_NUM_SHADES     4

#define SNOW_CELL_TYPE(cell)    ((snow_cell_type_t)((cell) & 0x3))
#define SNOW_CELL_SHADE(cell)   (((cell) >> 2) & 0x3)

/* A grain on its way from the pile to a digit, in pixels. */
typedef struct {
    int16_t x;
    int16_t y;
    uint8_t shade;
} snow_flying_t;

/** Empties the world, pours the pile and places the colon. No digits until snow_sim_set_time(). */
void snow_sim_init(uint32_t seed);

/**
 * Sets which way is down on the display, x to the right and y down, 127 is 1 g.
 * A short vector (the watch lies flat) is ignored and the snow keeps falling the way it did.
 *
 * @return true if the snow now falls in another direction than before.
 */
bool snow_sim_set_gravity(int x, int y);

/** Shows a new time: digits that changed crumble and are built again from the pile. */
void snow_sim_set_time(int hour, int minute);

/** Throws the snow upwards for a moment. */
void snow_sim_shake(void);

/**
 * Moves everything one step.
 *
 * @return true if something moved or is about to. When false the world is at rest
 *         until the time or gravity changes.
 */
bool snow_sim_step(void);

/** @return Type and shade of a cell, see SNOW_CELL_TYPE() and SNOW_CELL_SHADE(). */
uint8_t snow_sim_cell(int x, int y);

/** @return The grains in the air right now, valid until the next step. */
int snow_sim_get_flying(snow_flying_t *grains, int max);

/**
 * Gets and clears the columns that changed in a band since it was last asked for.
 *
 * @return false if nothing changed in the band.
 */
bool snow_sim_pop_dirty(int band, int *x_first, int *x_last);

/**
 * @return The row of the top of the pile in a column, when the watch is held upright.
 *         SNOW_GRID_SIZE if there is no snow at the bottom of the column.
 */
int snow_sim_surface(int x);

/** Throws the top grain of a column a little into the air, like spray. */
void snow_sim_kick(int x);

/** @return true if the snow falls straight down on the display and is not being shaken. */
bool snow_sim_is_level(void);
