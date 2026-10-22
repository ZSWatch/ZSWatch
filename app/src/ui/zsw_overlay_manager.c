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

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "ui/zsw_overlay_manager.h"
#include "ui/zsw_ui_controller.h"
#include "managers/zsw_power_manager.h"

LOG_MODULE_REGISTER(zsw_overlay_manager, LOG_LEVEL_INF);

#define MAX_QUEUED 4

static zsw_overlay_t *active;
static lv_timer_t *timeout_timer;
static zsw_overlay_t *queue[MAX_QUEUED];

static void timeout_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    timeout_timer = NULL;
    if (active) {
        LOG_DBG("%s timed out", active->name);
        zsw_overlay_close(active);
    }
}

static void start_timeout(uint32_t timeout_ms)
{
    if (timeout_timer) {
        lv_timer_delete(timeout_timer);
        timeout_timer = NULL;
    }
    if (timeout_ms > 0) {
        timeout_timer = lv_timer_create(timeout_timer_cb, timeout_ms, NULL);
        lv_timer_set_repeat_count(timeout_timer, 1);
    }
}

static void enqueue(zsw_overlay_t *overlay)
{
    if (overlay->queued) {
        return;
    }
    for (int i = 0; i < MAX_QUEUED; i++) {
        if (!queue[i]) {
            queue[i] = overlay;
            overlay->queued = true;
            LOG_DBG("%s queued", overlay->name);
            return;
        }
    }
    LOG_WRN("Queue full, dropping %s", overlay->name);
}

static void dequeue(zsw_overlay_t *overlay)
{
    for (int i = 0; i < MAX_QUEUED; i++) {
        if (queue[i] == overlay) {
            memmove(&queue[i], &queue[i + 1], (MAX_QUEUED - i - 1) * sizeof(queue[0]));
            queue[MAX_QUEUED - 1] = NULL;
            break;
        }
    }
    overlay->queued = false;
}

/* Highest priority first, oldest first among equals. */
static zsw_overlay_t *next_queued(void)
{
    zsw_overlay_t *next = NULL;

    for (int i = 0; i < MAX_QUEUED && queue[i]; i++) {
        if (!next || queue[i]->priority > next->priority) {
            next = queue[i];
        }
    }
    return next;
}

static void open_overlay(zsw_overlay_t *overlay)
{
    LOG_DBG("%s shown", overlay->name);
    active = overlay;
    zsw_ui_controller_set_notification_mode();
    overlay->show(lv_layer_top(), lv_group_get_default());
    start_timeout(overlay->timeout_ms);
}

/* Closes the active overlay without showing the next one. */
static void close_active(void)
{
    zsw_overlay_t *overlay = active;

    /* Cleared first so a close() that calls zsw_overlay_close() does nothing. */
    active = NULL;
    start_timeout(0);
    overlay->close();
    zsw_ui_controller_clear_notification_mode();
    LOG_DBG("%s closed", overlay->name);
}

void zsw_overlay_show(zsw_overlay_t *overlay)
{
    if (active == overlay) {
        close_active();
    } else if (active) {
        if (overlay->priority <= active->priority) {
            enqueue(overlay);
            return;
        }
        LOG_DBG("%s replaces %s", overlay->name, active->name);
        close_active();
    }
    dequeue(overlay);
    open_overlay(overlay);
}

static void show_async_cb(void *overlay)
{
    zsw_overlay_show(overlay);
}

void zsw_overlay_show_async(zsw_overlay_t *overlay)
{
    /* lv_async_call does not run while the display sleeps. */
    zsw_power_manager_reset_idle_timout();
    lv_async_call(show_async_cb, overlay);
}

void zsw_overlay_close(zsw_overlay_t *overlay)
{
    if (overlay->queued) {
        dequeue(overlay);
        return;
    }
    if (active != overlay) {
        return;
    }
    close_active();

    zsw_overlay_t *next = next_queued();
    if (next) {
        dequeue(next);
        open_overlay(next);
    }
}

bool zsw_overlay_is_shown(const zsw_overlay_t *overlay)
{
    return active == overlay;
}

void zsw_overlay_set_timeout(zsw_overlay_t *overlay, uint32_t timeout_ms)
{
    if (active == overlay) {
        start_timeout(timeout_ms);
    }
}

bool zsw_overlay_active(void)
{
    return active && active->blocks_input;
}

bool zsw_overlay_handle_back(void)
{
    zsw_overlay_t *overlay = active;

    if (!overlay) {
        return false;
    }
    if (!overlay->back || !overlay->back()) {
        zsw_overlay_close(overlay);
    }
    return true;
}

bool zsw_overlay_handle_swipe(void)
{
    if (!active) {
        return false;
    }
    if (active->swipe_is_back) {
        zsw_overlay_handle_back();
    }
    return true;
}
