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
#include <lvgl.h>

/*
 * Overlays are short-lived views shown on top of the watchface or any app,
 * triggered from BLE, timers or buttons. One is visible at a time. While it is
 * shown, the buttons go to its LVGL group and back closes it.
 *
 * Unless noted, functions must be called from the LVGL thread.
 */

typedef struct zsw_overlay zsw_overlay_t;

struct zsw_overlay {
    const char *name;
    /* A higher priority replaces the shown overlay, otherwise it waits. */
    uint8_t priority;
    /* Block the main button, watchface gestures and watchface events. */
    bool blocks_input;
    /* A swipe on the watchface acts as back. */
    bool swipe_is_back;
    /* Auto-dismiss after this long, 0 to stay until closed. */
    uint32_t timeout_ms;
    /* Build the view on parent (lv_layer_top()) and add buttons to group. */
    void (*show)(lv_obj_t *parent, lv_group_t *group);
    /* Delete everything show() created. */
    void (*close)(void);
    /* Optional. Return false to let the manager close the overlay. */
    bool (*back)(void);
    /* Internal */
    bool queued;
};

/* Shows the overlay, or queues it behind one with the same or a higher
 * priority. Showing the visible overlay again rebuilds it.
 */
void zsw_overlay_show(zsw_overlay_t *overlay);

/* Wakes the display and shows the overlay. Any thread. */
void zsw_overlay_show_async(zsw_overlay_t *overlay);

/* Closes the overlay, or removes it from the queue. */
void zsw_overlay_close(zsw_overlay_t *overlay);

bool zsw_overlay_is_shown(const zsw_overlay_t *overlay);

/* Restarts the auto-dismiss timer with a new timeout. */
void zsw_overlay_set_timeout(zsw_overlay_t *overlay, uint32_t timeout_ms);

/* For the UI controller. */
bool zsw_overlay_active(void);
bool zsw_overlay_handle_back(void);
bool zsw_overlay_handle_swipe(void);
