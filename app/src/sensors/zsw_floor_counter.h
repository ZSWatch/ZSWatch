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

#include <stdint.h>

typedef struct {
    uint16_t floors_up;
    uint16_t floors_down;
} zsw_floor_counter_data_t;

/** @brief Notification of changed floor totals, including daily resets. */
typedef void (*zsw_floor_counter_changed_cb_t)(void);

/** @brief Start floor tracking and daily reset checks after sensor initialization. */
void zsw_floor_counter_init(void);

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