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

typedef enum zsw_vibration_pattern {
    ZSW_VIBRATION_PATTERN_CLICK,
    ZSW_VIBRATION_PATTERN_NOTIFICATION,
    ZSW_VIBRATION_PATTERN_ALARM,
} zsw_vibration_pattern_t;

/*
* @brief Run a vibration pattern
*
* @details Run a vibration pattern, the pattern will run asynchronously and
* any try to run a pattern while another is running will resutlt in -EBUSY.
*
* @param pattern The pattern to run
* @return 0 on success, negative error code on failure
*/
int zsw_vibration_run_pattern(zsw_vibration_pattern_t pattern);

/*
* @brief Enable or disable the vibration motor
*
* @details Used to globally disable the vibration motor, for example when the watch
* is in DND mode or when battery is low.
*
* @param enable true to enable, false to disable
*/
int zsw_vibration_set_enabled(bool enable);
