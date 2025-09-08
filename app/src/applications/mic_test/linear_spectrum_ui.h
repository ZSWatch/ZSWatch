/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2025 Jakob Krantz.
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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LINEAR_SPECTRUM_BARS 48  // More bars for linear view

/**
 * @brief Initialize the linear spectrum analyzer UI
 *
 * @param parent Parent LVGL object to attach the spectrum analyzer to
 * @param x X position of the spectrum area
 * @param y Y position of the spectrum area
 * @param width Width of the spectrum area
 * @param height Height of the spectrum area
 *
 * @return 0 on success, negative error code on failure
 */
int linear_spectrum_ui_init(lv_obj_t *parent, int16_t x, int16_t y,
                            uint16_t width, uint16_t height);

/**
 * @brief Update the linear spectrum display with new magnitude data
 *
 * @param magnitudes Array of magnitude values [0-255] for each frequency bar
 * @param num_bars Number of bars
 */
void linear_spectrum_ui_update(const uint8_t *magnitudes, size_t num_bars);

/**
 * @brief Remove and cleanup the linear spectrum analyzer UI
 */
void linear_spectrum_ui_remove(void);

#ifdef __cplusplus
}
#endif
