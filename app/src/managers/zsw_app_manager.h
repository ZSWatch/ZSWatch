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

#include <lvgl.h>

typedef void(*application_start_fn)(lv_obj_t *root, lv_group_t *group);
typedef void(*application_stop_fn)(void);

typedef void(*on_app_manager_cb_fn)(void);

typedef struct application_t {
    application_start_fn    start_func;
    application_stop_fn     stop_func;
    char                   *name;
    const void             *icon;
    bool                    hidden;
    uint8_t                 private_list_index;
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

/** @brief
 *  @param index
*/
void zsw_app_manager_set_index(int index);

/** @brief Get number of registrated applications
*/
int zsw_app_manager_get_num_apps(void);