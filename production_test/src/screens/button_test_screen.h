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

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize the button test screen
 */
void button_test_screen_init(void);

/**
 * @brief Show the button test screen
 */
void button_test_screen_show(void);

/**
 * @brief Update button status when pressed
 * @param button_code Input key code of the button
 */
void button_test_screen_update_button(uint32_t button_code);

/**
 * @brief Update countdown display
 * @param seconds_remaining Seconds remaining in test
 */
void button_test_screen_update_countdown(int seconds_remaining);

/**
 * @brief Start the button test (called when first button pressed)
 */
void button_test_screen_start_test(void);
