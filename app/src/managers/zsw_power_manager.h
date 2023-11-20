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
#include <stdint.h>

typedef enum zsw_power_manager_state_t {
    ZSW_ACTIVITY_STATE_ACTIVE,
    ZSW_ACTIVITY_STATE_INACTIVE,
    ZSW_ACTIVITY_STATE_NOT_WORN_STATIONARY,
} zsw_power_manager_state_t;

/** @brief   Resets the inactivity timeout that will make the watch go into inactive mode with display etc. turned off to save power.
 *  @return True if the call caused wakup from inactive state, false otherwise.
*/
bool zsw_power_manager_reset_idle_timout(void);

/** @brief  Returns the time in seconds until the watch will go into inactive state.
 *  @return Time until watch will go into inactive state
*/
uint32_t zsw_power_manager_get_ms_to_inactive(void);

/** @brief  Returns the current power manager state, see @zsw_power_manager_state_t.
 *  @return Power manager state
*/
zsw_power_manager_state_t zsw_power_manager_get_state(void);