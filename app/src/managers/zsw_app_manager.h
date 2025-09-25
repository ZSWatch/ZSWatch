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

#include <lvgl.h>

typedef void(*application_start_fn)(lv_obj_t *root, lv_group_t *group);
typedef void(*application_stop_fn)(void);
/*
* Return true if the back button was consumed by the application.
*/
typedef bool(*application_back_fn)(void);

typedef void(*on_app_manager_cb_fn)(void);

typedef enum {
    ZSW_APP_CATEGORY_ROOT = 0,
    ZSW_APP_CATEGORY_TOOLS,
    ZSW_APP_CATEGORY_FITNESS,
    ZSW_APP_CATEGORY_SYSTEM,
    ZSW_APP_CATEGORY_GAMES,
    ZSW_APP_CATEGORY_SENSORS,
    ZSW_APP_CATEGORY_RANDOM,
    ZSW_APP_CATEGORY_COUNT,
    ZSW_APP_CATEGORY_INVALID
} zsw_app_category_t;

typedef enum {
    ZSW_APP_STATE_STOPPED,      // App is not running
    ZSW_APP_STATE_UI_VISIBLE,   // App UI is visible and safe to use
    ZSW_APP_STATE_UI_HIDDEN     // App is running but UI is not safe to call
} zsw_app_state_t;

typedef void(*application_ui_unavailable_fn)(void);
typedef void(*application_ui_available_fn)(void);

typedef struct application_t {
    application_start_fn            start_func;
    application_stop_fn             stop_func;
    application_back_fn             back_func;
    application_ui_unavailable_fn   ui_unavailable_func;
    application_ui_available_fn     ui_available_func;
    char                            *name;
    const void                      *icon;
    bool                            hidden;
    zsw_app_category_t              category;
    uint8_t                         private_list_index;
    zsw_app_state_t                 current_state;
} application_t;

/** @brief
 *  @param close_cb
 *  @param root
 *  @param group
 *  @param app_name
*/
int zsw_app_manager_show(on_app_manager_cb_fn close_cb, lv_obj_t *root, lv_group_t *group, char *app_name);

/** @brief
*/
void zsw_app_manager_delete(void);

/** @brief
 *  @param app
*/
void zsw_app_manager_add_application(application_t *app);

/** @brief
 *  @param app
*/
void zsw_app_manager_app_close_request(application_t *app);

/** @brief
*/
void zsw_app_manager_exit_app(void);

/** @brief Get number of registrated applications
*/
int zsw_app_manager_get_num_apps(void);

/** @brief Get current app state
 *  @param app Application to check
 */
zsw_app_state_t zsw_app_manager_get_app_state(application_t *app);
