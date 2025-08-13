/*
 * This file is part of ZSWatch project <https://github.com/ZSWatch/ZSWatch/>.
 * Copyright (c) 2025 Jakob Krantz.
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

#ifndef ZSW_UI_CONTROLLER_H
#define ZSW_UI_CONTROLLER_H

/**
 * @brief Initialize the UI controller system
 *
 * Initializes the main UI system including:
 * - Root screen setup
 * - Input handling for buttons and touch events
 * - LVGL encoder input device configuration
 * - Input groups for managing focus
 * - Screen gesture event callbacks
 * - Watchface application startup
 *
 * @return 0 on success, negative error code on failure
 */
int zsw_ui_controller_init();

/**
 * @brief Enable notification mode for UI controller
 *
 * Switches the UI to notification mode by:
 * - Setting the temporary input group as default
 * - Routing encoder input to the temporary group
 * - Enabling LVGL button handling for notifications
 *
 * This allows notification popups to receive and handle input events.
 */
void zsw_ui_controller_set_notification_mode(void);

/**
 * @brief Clear notification mode and restore normal controller operation
 *
 * Restores normal UI navigation by:
 * - Setting the main input group as default
 * - Routing encoder input back to the main group
 * - Disabling LVGL button handling if in watchface state
 * - Keeping LVGL button handling enabled if in application manager state
 *
 * This returns input handling to the appropriate state based on current UI mode.
 */
void zsw_ui_controller_clear_notification_mode(void);

#endif /* ZSW_UI_CONTROLLER_H */