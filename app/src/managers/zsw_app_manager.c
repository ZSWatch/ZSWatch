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
#include <zephyr/zbus/zbus.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "ui/zsw_ui.h"
#include "ui/app_picker/app_picker_ui.h"
#include "managers/zsw_app_manager.h"
#include "events/activity_event.h"

LOG_MODULE_REGISTER(app_manager, LOG_LEVEL_INF);

#define MAX_APPS        25
#define INVALID_APP_ID  0xFF

static void draw_app_and_folder_view(void);
static void on_app_selected(application_t *app);
static void async_app_start(lv_timer_t *timer);
static void async_app_close(lv_timer_t *timer);
static void transition_app_to_ui_hidden(application_t *app);
static void transition_app_to_ui_visible(application_t *app);
static void zbus_activity_event_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(activity_state_data_chan);
ZBUS_LISTENER_DEFINE(app_manager_activity_state_event_lis, zbus_activity_event_callback);

ZSW_LV_IMG_DECLARE(folder_icon);

static application_t *apps[MAX_APPS];
static uint8_t num_apps;
static uint8_t num_visible_apps;
static uint8_t current_app;
static lv_obj_t *root_obj;
static lv_group_t *group_obj;
static on_app_manager_cb_fn close_cb_func;
static lv_obj_t *app_picker_root;
static uint8_t last_index; // Last focused position in root view
static bool app_launch_only;
static lv_timer_t *async_app_start_timer;
static lv_timer_t *async_app_close_timer;
static zsw_app_category_t current_folder = ZSW_APP_CATEGORY_INVALID;
static bool screen_is_on = true;

// TODO: Add icons for app folders
static const zsw_app_folder_info_t app_folders[ZSW_APP_CATEGORY_COUNT] = {
    [ZSW_APP_CATEGORY_ROOT] = {
        .name = "Root",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .color = LV_COLOR_MAKE(0x9E, 0x9E, 0x9E),
        .category = ZSW_APP_CATEGORY_ROOT
    },
    [ZSW_APP_CATEGORY_TOOLS] = {
        .name = "Tools",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .color = LV_COLOR_MAKE(0xFF, 0x98, 0x00),
        .category = ZSW_APP_CATEGORY_TOOLS
    },
    [ZSW_APP_CATEGORY_FITNESS] = {
        .name = "Fitness",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .color = LV_COLOR_MAKE(0x4C, 0xAF, 0x50),
        .category = ZSW_APP_CATEGORY_FITNESS
    },
    [ZSW_APP_CATEGORY_SYSTEM] = {
        .name = "System",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .color = LV_COLOR_MAKE(0xF4, 0x43, 0x36),
        .category = ZSW_APP_CATEGORY_SYSTEM
    },
    [ZSW_APP_CATEGORY_RANDOM] = {
        .name = "Random",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .color = LV_COLOR_MAKE(0xE9, 0x1E, 0x63),
        .category = ZSW_APP_CATEGORY_RANDOM
    },
    [ZSW_APP_CATEGORY_SENSORS] = {
        .name = "Sensors",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .color = LV_COLOR_MAKE(0x00, 0xBC, 0xD4),
        .category = ZSW_APP_CATEGORY_SENSORS
    },
    [ZSW_APP_CATEGORY_GAMES] = {
        .name = "Games",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .color = LV_COLOR_MAKE(0x21, 0x96, 0xF3),
        .category = ZSW_APP_CATEGORY_GAMES
    },
};

static void delete_root_object(void)
{
    if (app_picker_root != NULL) {
        app_picker_ui_delete();
        app_picker_root = NULL;
    }
}

static void on_app_selected(application_t *app)
{
    if (app == NULL) {
        return;
    }

    for (int i = 0; i < num_apps; i++) {
        if (apps[i] == app) {
            current_app = i;

            if (async_app_start_timer == NULL) {
                async_app_start_timer = lv_timer_create(async_app_start, 50, NULL);
                lv_timer_set_repeat_count(async_app_start_timer, 1);
            }
            return;
        }
    }

    LOG_WRN("Selected app not found in registry");
}

static void async_app_start(lv_timer_t *timer)
{
    async_app_start_timer = NULL;
    LOG_DBG("Start %d", current_app);
    delete_root_object();

    application_t *app = apps[current_app];
    __ASSERT(screen_is_on, "Screen expected to be on when starting app.");
    app->current_state = ZSW_APP_STATE_UI_VISIBLE;

    app->start_func(root_obj, group_obj);
}

static void async_app_close(lv_timer_t *timer)
{
    if (current_app < num_apps) {
        LOG_DBG("Stop %d", current_app);
        bool back_button_consumed = false;
        if (apps[current_app]->back_func) {
            back_button_consumed = apps[current_app]->back_func();
        }

        if (!back_button_consumed) {
            apps[current_app]->current_state = ZSW_APP_STATE_STOPPED;
            apps[current_app]->stop_func();
            current_app = INVALID_APP_ID;
            if (app_launch_only) {
                zsw_app_manager_delete();
                close_cb_func();
            } else {
                draw_app_and_folder_view();
            }
        }
    } else {
        // No app running, check if we are in a folder
        if (app_picker_root != NULL && app_picker_ui_is_folder_open()) {
            LOG_DBG("Close folder in picker");
            app_picker_ui_close_folder();
        } else {
            LOG_DBG("Exit application manager");
            zsw_app_manager_delete();
            close_cb_func();
        }
    }
    async_app_close_timer = NULL;
}

static void transition_app_to_ui_hidden(application_t *app)
{
    if (app && app->current_state == ZSW_APP_STATE_UI_VISIBLE) {
        app->current_state = ZSW_APP_STATE_UI_HIDDEN;
        LOG_DBG("App '%s' UI now hidden", app->name);

        if (app->ui_unavailable_func) {
            app->ui_unavailable_func();
        }
    }
}

static void transition_app_to_ui_visible(application_t *app)
{
    if (app && app->current_state == ZSW_APP_STATE_UI_HIDDEN) {
        app->current_state = ZSW_APP_STATE_UI_VISIBLE;
        LOG_DBG("App '%s' UI now visible", app->name);

        if (app->ui_available_func) {
            app->ui_available_func();
        }
    }
}

static void zbus_activity_event_callback(const struct zbus_channel *chan)
{
    const struct activity_state_event *event = zbus_chan_const_msg(chan);
    bool new_screen_state = (event->state == ZSW_ACTIVITY_STATE_ACTIVE);

    if (screen_is_on == new_screen_state) {
        return; // No change
    }

    screen_is_on = new_screen_state;

    // Find currently running app and transition it
    if (current_app < num_apps) {
        application_t *running_app = apps[current_app];

        if (screen_is_on) {
            transition_app_to_ui_visible(running_app);
        } else {
            transition_app_to_ui_hidden(running_app);
        }
    }
}

static void draw_app_and_folder_view(void)
{
    /* Use new circular app picker UI */
    app_picker_root = app_picker_ui_create(root_obj, group_obj, on_app_selected, app_folders);

    LOG_DBG("Created circular app picker UI");
}

int zsw_app_manager_show(on_app_manager_cb_fn close_cb, lv_obj_t *root, lv_group_t *group, char *app_name)
{
    int err = 0;
    bool app_found;
    close_cb_func = close_cb;
    root_obj = root;
    group_obj = group;
    app_launch_only = false;

    if (app_name == NULL) {
        draw_app_and_folder_view();
    } else {
        app_found = false;
        for (int i = 0; i < num_apps; i++) {
            if (strcmp(apps[i]->name, app_name) == 0) {
                app_launch_only = true;
                current_app = i;
                if (async_app_start_timer == NULL) {
                    async_app_start_timer = lv_timer_create(async_app_start, 1,  NULL);
                    lv_timer_set_repeat_count(async_app_start_timer, 1);
                }
            }
        }
    }

    if (app_name != NULL && !app_found) {
        err = -ENOENT;
    }

    return err;
}

void zsw_app_manager_delete(void)
{
    if (current_app < num_apps) {
        LOG_DBG("Stop force %d", current_app);
        apps[current_app]->current_state = ZSW_APP_STATE_STOPPED;
        apps[current_app]->stop_func();
    }
    delete_root_object();
}

void zsw_app_manager_add_application(application_t *app)
{
    __ASSERT_NO_MSG(num_apps < MAX_APPS);

    app->current_state = ZSW_APP_STATE_STOPPED;
    apps[num_apps] = app;
    num_apps++;
    if (!app->hidden) {
        app->private_list_index = num_visible_apps;
        num_visible_apps++;
    }
}

void zsw_app_manager_exit_app(void)
{
    if (async_app_close_timer != NULL) {
        return;
    }
    async_app_close_timer = lv_timer_create(async_app_close, 500,  NULL);
    lv_timer_set_repeat_count(async_app_close_timer, 1);
}

void zsw_app_manager_app_close_request(application_t *app)
{
    LOG_DBG("zsw_app_manager_app_close_request");
    zsw_app_manager_exit_app();
}

int zsw_app_manager_get_num_apps(void)
{
    return num_apps;
}

application_t *zsw_app_manager_get_app(int index)
{
    if (index < 0 || index >= num_apps) {
        return NULL;
    }
    return apps[index];
}

zsw_app_state_t zsw_app_manager_get_app_state(application_t *app)
{
    __ASSERT_NO_MSG(app != NULL);
    return app->current_state;
}

static int application_manager_init(void)
{
    memset(apps, 0, sizeof(apps));
    num_apps = 0;
    current_app = INVALID_APP_ID;
    async_app_start_timer = NULL;
    current_folder = ZSW_APP_CATEGORY_INVALID;
    last_index = 0;
    screen_is_on = true;

    // Subscribe to activity events to track screen state
    zbus_chan_add_obs(&activity_state_data_chan, &app_manager_activity_state_event_lis, K_MSEC(100));

    return 0;
}

SYS_INIT(application_manager_init, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY);
