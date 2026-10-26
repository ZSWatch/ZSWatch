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

#include "managers/zsw_notification_manager.h"

typedef void (*on_close_notif_cb_t)(uint32_t id);

/** @brief  Show a notification, copied as the popup may wait behind another overlay.
 *  @param close_cb             Called when the user dismissed it
 *  @param close_after_seconds  Auto close time, 0 to stay until dismissed
*/
void zsw_notification_popup_show(const zsw_not_mngr_notification_t *notification, on_close_notif_cb_t close_cb,
                                 uint32_t close_after_seconds);

void zsw_notification_popup_remove(void);

bool zsw_notification_popup_is_shown(void);
