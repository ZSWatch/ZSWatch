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

#include "../production_test_runner.h"

/**
 * @brief Initialize the result screen
 */
void result_screen_init(void);

/**
 * @brief Show the final result screen
 * @param context Test context with final results
 * @param test_names Array of test names corresponding to test results
 * @param num_tests Number of tests in the arrays
 */
void result_screen_show(production_test_runner_context_t *context, const char **test_names, int num_tests);
