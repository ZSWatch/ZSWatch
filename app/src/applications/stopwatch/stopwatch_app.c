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

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "stopwatch_ui.h"
#include "events/zsw_periodic_event.h"

LOG_MODULE_REGISTER(stopwatch_app, LOG_LEVEL_INF);

// Functions needed for all applications
static void stopwatch_app_start(lv_obj_t *root, lv_group_t *group);
static void stopwatch_app_stop(void);

// Stopwatch event callbacks
static void on_start_cb(void);
static void on_pause_cb(void);
static void on_reset_cb(void);
static void on_lap_cb(void);

static void zbus_periodic_100ms_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(periodic_event_100ms_chan);
ZBUS_LISTENER_DEFINE(stopwatch_app_100ms_event_listener, zbus_periodic_100ms_callback);

ZSW_LV_IMG_DECLARE(stopwatch_app_icon);

typedef struct {
    stopwatch_state_t state;
    uint64_t start_time_ms;
    uint64_t pause_time_ms;
    uint32_t elapsed_ms;
    uint32_t last_lap_time_ms;
} stopwatch_data_t;

static stopwatch_data_t stopwatch_data = {
    .state = STOPWATCH_STATE_STOPPED,
    .start_time_ms = 0,
    .pause_time_ms = 0,
    .elapsed_ms = 0,
    .last_lap_time_ms = 0
};

static application_t app = {
    .name = "Stopwatch",
    .icon = ZSW_LV_IMG_USE(stopwatch_app_icon),
    .start_func = stopwatch_app_start,
    .stop_func = stopwatch_app_stop,
    .category = ZSW_APP_CATEGORY_ROOT
};

static void stopwatch_app_start(lv_obj_t *root, lv_group_t *group)
{
    stopwatch_ui_show(root, on_start_cb, on_pause_cb, on_reset_cb, on_lap_cb);
    stopwatch_ui_update_state(stopwatch_data.state);
    stopwatch_ui_update_time(stopwatch_data.elapsed_ms);

    zsw_periodic_chan_add_obs(&periodic_event_100ms_chan, &stopwatch_app_100ms_event_listener);
}

static void stopwatch_app_stop(void)
{
    zsw_periodic_chan_rm_obs(&periodic_event_100ms_chan, &stopwatch_app_100ms_event_listener);
    stopwatch_ui_remove();
}

static void on_start_cb(void)
{
    uint64_t current_time = k_uptime_get();

    switch (stopwatch_data.state) {
        case STOPWATCH_STATE_STOPPED:
            stopwatch_data.start_time_ms = current_time;
            stopwatch_data.elapsed_ms = 0;
            stopwatch_data.last_lap_time_ms = 0;
            stopwatch_data.state = STOPWATCH_STATE_RUNNING;
            break;

        case STOPWATCH_STATE_PAUSED:
            // Resume: adjust start time to account for pause duration
            stopwatch_data.start_time_ms += (current_time - stopwatch_data.pause_time_ms);
            stopwatch_data.state = STOPWATCH_STATE_RUNNING;
            break;

        case STOPWATCH_STATE_RUNNING:
            // Should not happen.
            LOG_ERR("Stopwatch already running");
            break;
    }

    stopwatch_ui_update_state(stopwatch_data.state);
    LOG_DBG("Stopwatch started/resumed");
}

static void on_pause_cb(void)
{
    if (stopwatch_data.state == STOPWATCH_STATE_RUNNING) {
        stopwatch_data.pause_time_ms = k_uptime_get();
        stopwatch_data.state = STOPWATCH_STATE_PAUSED;
        stopwatch_ui_update_state(stopwatch_data.state);
        LOG_DBG("Stopwatch paused");
    }
}

static void on_reset_cb(void)
{
    stopwatch_data.state = STOPWATCH_STATE_STOPPED;
    stopwatch_data.start_time_ms = 0;
    stopwatch_data.pause_time_ms = 0;
    stopwatch_data.elapsed_ms = 0;
    stopwatch_data.last_lap_time_ms = 0;

    stopwatch_ui_update_state(stopwatch_data.state);
    stopwatch_ui_update_time(stopwatch_data.elapsed_ms);
    LOG_DBG("Stopwatch reset");
}

static void on_lap_cb(void)
{
    if (stopwatch_data.state == STOPWATCH_STATE_RUNNING) {
        uint32_t current_elapsed = stopwatch_data.elapsed_ms;
        uint32_t lap_time = current_elapsed - stopwatch_data.last_lap_time_ms;

        stopwatch_ui_add_lap_time(lap_time, current_elapsed);
        stopwatch_data.last_lap_time_ms = current_elapsed;

        LOG_DBG("Lap time %u ms (total: %u ms)", lap_time, current_elapsed);
    }
}

static void update_elapsed_time(void)
{
    if (stopwatch_data.state == STOPWATCH_STATE_RUNNING) {
        uint64_t current_time = k_uptime_get();
        stopwatch_data.elapsed_ms = (uint32_t)(current_time - stopwatch_data.start_time_ms);

        stopwatch_ui_update_time(stopwatch_data.elapsed_ms);
    }
}

static void zbus_periodic_100ms_callback(const struct zbus_channel *chan)
{
    update_elapsed_time();
}

static int stopwatch_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(stopwatch_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
