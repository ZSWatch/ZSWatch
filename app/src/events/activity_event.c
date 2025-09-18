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

#include "activity_event.h"
#include <zephyr/zbus/zbus.h>

ZBUS_CHAN_DEFINE(activity_state_data_chan,
                 struct activity_state_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(watchface_activity_state_event),
                 ZBUS_MSG_INIT()
                );
