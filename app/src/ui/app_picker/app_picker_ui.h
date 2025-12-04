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

#ifndef APP_PICKER_UI_H
#define APP_PICKER_UI_H

#include <lvgl.h>
#include "managers/zsw_app_manager.h"

/**
 * @brief Callback function type for when an app is selected
 * @param app The selected application
 */
typedef void (*app_picker_on_app_selected_cb)(application_t *app);

/**
 * @brief Callback function type for when the app picker is closed
 */
typedef void (*app_picker_on_close_cb)(void);

/**
 * @brief Initialize and show the app picker UI
 *
 * @param root Parent LVGL object to attach the app picker to
 * @param group LVGL input group for encoder/button navigation
 * @param on_app_selected Callback when an app is selected
 * @param folder_info Array of folder info (size ZSW_APP_CATEGORY_COUNT)
 * @return lv_obj_t* The root object of the app picker UI
 */
lv_obj_t *app_picker_ui_create(lv_obj_t *root, lv_group_t *group,
                               app_picker_on_app_selected_cb on_app_selected,
                               const zsw_app_folder_info_t *folder_info);

/**
 * @brief Delete the app picker UI and clean up resources
 */
void app_picker_ui_delete(void);

/**
 * @brief Check if a folder is currently open in the picker
 * @return true if a folder is open
 */
bool app_picker_ui_is_folder_open(void);

/**
 * @brief Close the currently open folder (if any)
 * Called when back button is pressed while in folder view
 */
void app_picker_ui_close_folder(void);

/*
 * Public event callbacks - these are used by XML-generated UI code
 * and must be accessible externally
 */

/**
 * @brief Callback for app slot click (used by XML event binding)
 */
void app_picker_on_app_clicked(lv_event_t *e);

/**
 * @brief Callback for folder close button (used by XML event binding)
 */
void app_picker_on_folder_close_clicked(lv_event_t *e);

/**
 * @brief Callback for left navigation arrow (used by XML event binding)
 */
void app_picker_on_nav_left_clicked(lv_event_t *e);

/**
 * @brief Callback for right navigation arrow (used by XML event binding)
 */
void app_picker_on_nav_right_clicked(lv_event_t *e);

#endif /* APP_PICKER_UI_H */
