/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

typedef struct {
    uint16_t floors_up;
    uint16_t floors_down;
} zsw_floor_counter_data_t;

typedef enum {
    ZSW_FLOOR_COUNTER_STOPPED,
    ZSW_FLOOR_COUNTER_IDLE,
    ZSW_FLOOR_COUNTER_WAIT_BASELINE,
    ZSW_FLOOR_COUNTER_TRACKING,
} zsw_floor_counter_state_t;

typedef struct {
    bool enabled;
    zsw_floor_counter_state_t state;
    bool pressure_fast_request_owned;
    bool step_snapshot_request_owned;
} zsw_floor_counter_status_t;

/** @brief Notification of changed floor totals, including daily resets. */
typedef void (*zsw_floor_counter_changed_cb_t)(void);

/** @brief Initialize floor tracking and daily reset checks after sensor initialization. */
void zsw_floor_counter_init(void);

/**
 * @brief Persist and apply the desired floor-counter state.
 * @return 0 on acceptance, or a negative error if persistence or scheduling fails.
 */
int zsw_floor_counter_set_enabled(bool enabled);

/** @brief Get the accepted desired floor-counter state. */
bool zsw_floor_counter_is_enabled(void);

/** @brief Get desired state, applied FSM state, and floor-owned request status. */
zsw_floor_counter_status_t zsw_floor_counter_get_status(void);

/** @brief Get the display name for a floor-counter FSM state. */
const char *zsw_floor_counter_state_name(zsw_floor_counter_state_t state);

/**
 * @brief Register the count-change callback before initialization.
 * The callback runs on the system workqueue; it must not block or access LVGL.
 */
void zsw_floor_counter_set_callback(zsw_floor_counter_changed_cb_t callback);

/** @brief Get coherent daily totals from any thread, even before initialization. */
zsw_floor_counter_data_t zsw_floor_counter_get_counts(void);

/**
 * @brief Queue ambient temperature in kelvin (173.15 to 373.15).
 * @return 0 if queued, -EINVAL for invalid input, or -ENOMSG if the queue is full.
 */
int zsw_floor_counter_set_temperature(float ambient_temperature_k);