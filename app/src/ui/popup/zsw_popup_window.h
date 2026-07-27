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

/** @brief Callback fired when a popup closes.
 *  @param confirmed true only when the user pressed "Yes".
 */
typedef void(*on_close_popup_cb_t)(bool confirmed);

/** @brief Show a popup on the top layer.
 *  Requests are queued when another popup is already visible.
 *  @param title Popup title string.
 *  @param body Popup body string.
 *  @param close_cb Callback called when popup closes.
 *  @param close_after_seconds Auto-close timeout in seconds.
 *  @param display_yes_no true to show Yes/No buttons, false for close button.
 */
void zsw_popup_show(char *title, char *body, on_close_popup_cb_t close_cb, uint32_t close_after_seconds,
                    bool display_yes_no);

/** @brief Remove the active popup.
 *  If queued requests exist, the next popup is shown automatically.
 */
void zsw_popup_remove(void);
