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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPECTRUM_FFT_SIZE       64      // FFT points for analysis

/**
 * @brief Initialize the spectrum analyzer
 *
 * @return 0 on success, negative error code on failure
 */
int spectrum_analyzer_init(void);

/**
 * @brief Process audio samples and compute frequency spectrum
 *
 * @param samples Pointer to 16-bit audio samples
 * @param num_samples Number of samples (should be >= SPECTRUM_FFT_SIZE)
 * @param magnitudes Output array for frequency magnitudes [0-255]
 * @param num_bars Number of output bars (typically NUM_SPECTRUM_BARS = 30)
 * @param gain_multiplier Gain multiplier for sensitivity adjustment
 *
 * @return 0 on success, negative error code on failure
 */
int spectrum_analyzer_process(const int16_t *samples, size_t num_samples,
                              uint8_t *magnitudes, size_t num_bars, float gain_multiplier);

#ifdef __cplusplus
}
#endif
