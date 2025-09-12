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

#include <stddef.h>
#include "../production_test_runner.h"

typedef struct {
    const char *name;
    const test_result_t *result_ptr;
} test_metadata_t;

/**
 * @brief Initialize the sensor scan screen
 */
void sensor_scan_screen_init(void);

/**
 * @brief Show the sensor scan screen
 * @param metadata Array of test metadata
 * @param num_tests Number of tests in the metadata array
 */
void sensor_scan_screen_show(const test_metadata_t *metadata,
                             size_t num_tests);
