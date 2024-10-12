/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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

typedef enum zsw_buzzer_melody_t {
    ZSW_BUZZER_PATTERN_BEEP,
    ZSW_BUZZER_PATTERN_MARIO,
    ZSW_BUZZER_PATTERN_ALARM,
} zsw_buzzer_melody_t;

/*
* @brief Run a melody pattern
*
* @details Run a buzzer melody, the melody will run asynchronously and
* any try to run a melody while another is running will resutlt in -EBUSY.
*
* @param melody The melody to run
* @return 0 on success, negative error code on failure
*/
int zsw_buzzer_run_melody(zsw_buzzer_melody_t melody);
