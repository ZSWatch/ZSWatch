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
#define SPECTRUM_NUM_BARS_CIRCULAR  24  // Number of frequency bars for circular display
#define SPECTRUM_NUM_BARS_LINEAR    48  // Number of frequency bars for linear display
#define SPECTRUM_SAMPLE_RATE    16000   // Audio sample rate

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
 * @param num_bars Number of output bars (SPECTRUM_NUM_BARS_CIRCULAR or SPECTRUM_NUM_BARS_LINEAR)
 *
 * @return 0 on success, negative error code on failure
 */
int spectrum_analyzer_process(const int16_t *samples, size_t num_samples,
                              uint8_t *magnitudes, size_t num_bars);

/**
 * @brief Get color for a frequency bar based on its index and magnitude
 *
 * @param bar_index Bar index (0 to SPECTRUM_NUM_BARS-1)
 * @param magnitude Bar magnitude (0-255)
 *
 * @return RGB color value (0xRRGGBB)
 */
uint32_t spectrum_get_bar_color(uint8_t bar_index, uint8_t magnitude);

#ifdef __cplusplus
}
#endif
