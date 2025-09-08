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
#include "spectrum_analyzer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the circular spectrum analyzer UI
 *
 * @param parent Parent LVGL object to attach the spectrum analyzer to
 * @param center_x Center X coordinate of the circular display
 * @param center_y Center Y coordinate of the circular display
 * @param inner_radius Inner radius of the spectrum bars
 * @param outer_radius Outer radius (maximum bar length)
 *
 * @return 0 on success, negative error code on failure
 */
int circular_spectrum_ui_init(lv_obj_t *parent, int16_t center_x, int16_t center_y,
                              uint16_t inner_radius, uint16_t outer_radius);

/**
 * @brief Update the spectrum display with new magnitude data
 *
 * This function should be called from the main LVGL thread to update
 * the visual representation of the spectrum analyzer.
 *
 * @param magnitudes Array of magnitude values [0-255] for each frequency bar
 * @param num_bars Number of bars (should match SPECTRUM_NUM_BARS)
 */
void circular_spectrum_ui_update(const uint8_t *magnitudes, size_t num_bars);

/**
 * @brief Remove and cleanup the circular spectrum analyzer UI
 */
void circular_spectrum_ui_remove(void);

#ifdef __cplusplus
}
#endif
