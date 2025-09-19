/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
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

/**
 * @brief Initialize the microphone test screen
 */
void microphone_test_screen_init(void);

/**
 * @brief Show the microphone test screen
 */
void microphone_test_screen_show(void);

/**
 * @brief Update countdown display
 * @param seconds_remaining Seconds remaining in test
 */
void microphone_test_screen_update_countdown(int seconds_remaining);

/**
 * @brief Update spectrum display with FFT data
 * @param magnitudes Array of FFT magnitude values [0-255]
 * @param count Number of magnitude values (should be 30)
 */
void microphone_test_screen_update_spectrum(const uint8_t *magnitudes, size_t count);
