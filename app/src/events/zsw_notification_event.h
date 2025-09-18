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

#include "ble/ble_comm.h"
#include "managers/zsw_notification_manager.h"

/** @brief  We use an empty struct, because all listeners were only informed. They have
 *          to fetch the notifications on their own.
*/
struct zsw_notification_event {
};

struct zsw_notification_remove_event {
    zsw_not_mngr_notification_t notification;
};
