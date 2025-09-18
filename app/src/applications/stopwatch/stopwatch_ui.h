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

#include <inttypes.h>
#include <lvgl.h>

typedef enum {
    STOPWATCH_STATE_STOPPED,
    STOPWATCH_STATE_RUNNING,
    STOPWATCH_STATE_PAUSED
} stopwatch_state_t;

typedef void (*stopwatch_event_cb_t)(void);

void stopwatch_ui_show(lv_obj_t *root, stopwatch_event_cb_t start_cb, stopwatch_event_cb_t pause_cb,
                       stopwatch_event_cb_t reset_cb, stopwatch_event_cb_t lap_cb);
void stopwatch_ui_remove(void);
void stopwatch_ui_update_time(uint32_t elapsed_ms);
void stopwatch_ui_update_state(stopwatch_state_t state);
void stopwatch_ui_add_lap_time(uint32_t lap_time, uint32_t total_time);
