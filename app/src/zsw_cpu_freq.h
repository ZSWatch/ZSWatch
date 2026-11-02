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

#include <stdbool.h>

/**
 * @brief Run the app core at 128 MHz until the matching zsw_cpu_release_fast().
 * Requests nest. The clock goes back to 64 MHz when the last one is released.
 */
void zsw_cpu_request_fast(void);

/** @brief Release a zsw_cpu_request_fast(). */
void zsw_cpu_release_fast(void);

/** @return true when the app core runs at 128 MHz. */
bool zsw_cpu_is_fast(void);
