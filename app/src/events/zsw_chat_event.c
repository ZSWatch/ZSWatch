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

#include <zephyr/autoconf.h>

#ifdef CONFIG_APPLICATIONS_USE_CHAT

#include "zsw_chat_event.h"
#include <zephyr/zbus/zbus.h>

ZBUS_CHAN_DEFINE(chat_state_chan,
                 struct zsw_chat_state_event,
                 NULL, NULL,
                 ZBUS_OBSERVERS(chat_app_state_lis),
                 ZBUS_MSG_INIT());

ZBUS_CHAN_DEFINE(chat_transcript_chan,
                 struct zsw_chat_transcript_event,
                 NULL, NULL,
                 ZBUS_OBSERVERS(chat_app_transcript_lis),
                 ZBUS_MSG_INIT());

ZBUS_CHAN_DEFINE(chat_reply_ready_chan,
                 struct zsw_chat_reply_ready_event,
                 NULL, NULL,
                 ZBUS_OBSERVERS(chat_app_reply_ready_lis),
                 ZBUS_MSG_INIT());

ZBUS_CHAN_DEFINE(chat_error_chan,
                 struct zsw_chat_error_event,
                 NULL, NULL,
                 ZBUS_OBSERVERS(chat_app_error_lis),
                 ZBUS_MSG_INIT());

#endif /* CONFIG_APPLICATIONS_USE_CHAT */
