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
 *
 * Sand trickles in all day, each part of the day in its own color, so the pile grows in
 * layers and shows how far the day has come. It is emptied at midnight. Loose sand falls
 * the way gravity points, one cell per step, so turning the watch mixes the layers.
 *
 * When a digit of the time changes the old one falls apart into dust, which fades on its
 * way down.
 *
 * No drawing here: the UI asks for the cells and draws them.
 */

#define SAND_GRID_SIZE      120
#define SAND_CELL_PX        2

/* Rows are grouped in bands for redrawing: each band reports the columns that changed. */
#define SAND_BAND_ROWS      8
#define SAND_NUM_BANDS      (SAND_GRID_SIZE / SAND_BAND_ROWS)

/* What a cell holds: nothing, sand in one of the colors of the day, or dust of a digit. */
#define SAND_CELL_EMPTY     0
#define SAND_NUM_COLORS     12
#define SAND_CELL_DUST      (SAND_NUM_COLORS + 1)

/** Empties the world. The pile is poured by the first sand_sim_set_time(). */
void sand_sim_init(uint32_t seed);

/**
 * Sets which way is down on the display, x to the right and y down, 127 is 1 g.
 * A short vector (the watch lies flat) is ignored and the sand keeps falling the way it did.
 *
 * @return true if the sand now falls in another direction than before.
 */
bool sand_sim_set_gravity(int x, int y);

/**
 * Tells the time of day. The first call pours the pile for the day so far at once.
 * After that the sand that is missing trickles in, and at midnight the pile blows away.
 */
void sand_sim_set_time(int hour, int minute);

/**
 * Turns a shape into dust: one grain for each set bit, where there is room for it.
 *
 * @param cell_x,cell_y Top left cell of the shape.
 * @param rows          One entry per row, the leftmost cell in the highest of the width bits.
 */
void sand_sim_dissolve(int cell_x, int cell_y, const uint16_t *rows, int width, int height);

/** Throws the sand up in a cloud for a moment. */
void sand_sim_shake(void);

/**
 * Moves everything one step.
 *
 * @return true if something moved or is about to. When false the world is at rest
 *         until the time or gravity changes.
 */
bool sand_sim_step(void);

/** @return SAND_CELL_EMPTY, a color 1..SAND_NUM_COLORS or SAND_CELL_DUST. */
uint8_t sand_sim_cell(int x, int y);

/**
 * Gets and clears the columns that changed in a band since it was last asked for.
 *
 * @return false if nothing changed in the band.
 */
bool sand_sim_pop_dirty(int band, int *x_first, int *x_last);
