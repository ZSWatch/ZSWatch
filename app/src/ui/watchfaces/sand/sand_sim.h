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
 * The world of the Sand watchface: a grid of grains inside the round display.
 * Loose sand falls the way gravity points, one cell per step. The digits of the
 * time are grains that are held in place. When the time changes, the digits
 * that changed are let go, and grains from the pile fly up to form the new ones.
 *
 * No drawing here: the UI asks for the cells and the flying grains and draws them.
 */

#define SAND_GRID_SIZE      120
#define SAND_CELL_PX        2

/* Rows are grouped in bands for redrawing: each band reports the columns that changed. */
#define SAND_BAND_ROWS      8
#define SAND_NUM_BANDS      (SAND_GRID_SIZE / SAND_BAND_ROWS)

#define SAND_MAX_FLYING     192

typedef enum {
    SAND_CELL_EMPTY,
    SAND_CELL_SAND,
    SAND_CELL_DIGIT,
} sand_cell_type_t;

/* Four shades per grain type give the sand its texture. A grain keeps its shade when it moves. */
#define SAND_NUM_SHADES     4

#define SAND_CELL_TYPE(cell)    ((sand_cell_type_t)((cell) & 0x3))
#define SAND_CELL_SHADE(cell)   (((cell) >> 2) & 0x3)

/* A grain on its way from the pile to a digit, in pixels. */
typedef struct {
    int16_t x;
    int16_t y;
    uint8_t shade;
} sand_flying_t;

/** Empties the world, pours the pile and places the colon. No digits until sand_sim_set_time(). */
void sand_sim_init(uint32_t seed);

/**
 * Sets which way is down on the display, x to the right and y down, 127 is 1 g.
 * A short vector (the watch lies flat) is ignored and the sand keeps falling the way it did.
 *
 * @return true if the sand now falls in another direction than before.
 */
bool sand_sim_set_gravity(int x, int y);

/** Shows a new time: digits that changed crumble and are built again from the pile. */
void sand_sim_set_time(int hour, int minute);

/** Throws the sand upwards for a moment. */
void sand_sim_shake(void);

/**
 * Moves everything one step.
 *
 * @return true if something moved or is about to. When false the world is at rest
 *         until the time or gravity changes.
 */
bool sand_sim_step(void);

/** @return Type and shade of a cell, see SAND_CELL_TYPE() and SAND_CELL_SHADE(). */
uint8_t sand_sim_cell(int x, int y);

/** @return The grains in the air right now, valid until the next step. */
int sand_sim_get_flying(sand_flying_t *grains, int max);

/**
 * Gets and clears the columns that changed in a band since it was last asked for.
 *
 * @return false if nothing changed in the band.
 */
bool sand_sim_pop_dirty(int band, int *x_first, int *x_last);
