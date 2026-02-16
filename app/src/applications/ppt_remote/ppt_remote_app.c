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
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "ppt_remote_ui.h"
#include "managers/zsw_app_manager.h"
#include "ble/ble_hid.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(ppt_remote_app, CONFIG_ZSW_REMOTE_APP_LOG_LEVEL);

// Functions needed for all applications
static void ppt_remote_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void ppt_remote_app_stop(void *user_data);
static void on_next(void);
static void on_prev(void);

// Functions related to app functionality
static void timer_callback(lv_timer_t *timer);

ZSW_LV_IMG_DECLARE(remote_control);

static application_t app = {
    .name = "Ppt_remote",
    .icon = ZSW_LV_IMG_USE(remote_control),
    .start_func = ppt_remote_app_start,
    .stop_func = ppt_remote_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

static lv_timer_t *counter_timer = NULL;
static int64_t start_time;
static bool is_first_time = true;

static void ppt_remote_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    ppt_remote_ui_show(root, on_next, on_prev);
}

static void ppt_remote_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    if (counter_timer != NULL) {
        lv_timer_del(counter_timer);
    }
    is_first_time = true;
    ppt_remote_ui_remove();
}

static void timer_callback(lv_timer_t *timer)
{
    int64_t now = k_uptime_get();
    ppt_remote_ui_set_timer_counter_value((int)((now - start_time) / 1000));
}

static int ppt_remote_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

static void on_next(void)
{
    // Start the timer when the presentation starts
    if (is_first_time == true) {
        counter_timer = lv_timer_create(timer_callback, 1000, NULL);
        is_first_time = false;
        start_time = k_uptime_get();
    }

    ble_hid_next();
}

static void on_prev(void)
{
    ble_hid_previous();
}

SYS_INIT(ppt_remote_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
