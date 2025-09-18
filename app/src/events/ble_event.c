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

#include <zephyr/zbus/zbus.h>

#include "ble_event.h"

ZBUS_CHAN_DEFINE(ble_comm_data_chan,
                 struct ble_data_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(notification_mgr_ble_comm_lis, main_ble_comm_lis, /*music_app_ble_comm_lis,*/ watchface_ble_comm_lis),
                 ZBUS_MSG_INIT()
                );
