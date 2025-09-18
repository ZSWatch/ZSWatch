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

#include "zsw_notification_event.h"

ZBUS_CHAN_DEFINE(zsw_notification_mgr_chan,
                 struct zsw_notification_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(notification_app_lis, main_notification_lis),
                 ZBUS_MSG_INIT()
                );

/** @brief  This event informs all listeners about a removed notification. The event will contain a copy
 *          of the removed notification.
*/
ZBUS_CHAN_DEFINE(zsw_notification_mgr_remove_chan,
                 struct zsw_notification_remove_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(notification_app_remove_lis),
                 ZBUS_MSG_INIT()
                );
