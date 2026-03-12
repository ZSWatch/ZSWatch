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

#ifdef CONFIG_APPLICATIONS_USE_VOICE_MEMO

#include "zsw_voice_memo_event.h"
#include <zephyr/zbus/zbus.h>

ZBUS_CHAN_DEFINE(voice_memo_recording_chan,
                 struct zsw_voice_memo_recording_event,
                 NULL, NULL,
                 ZBUS_OBSERVERS(ble_voice_memo_recording_lis, voice_memo_app_recording_lis),
                 ZBUS_MSG_INIT());

ZBUS_CHAN_DEFINE(voice_memo_result_chan,
                 struct zsw_voice_memo_result_event,
                 NULL, NULL,
                 ZBUS_OBSERVERS(voice_memo_popup_result_lis, voice_memo_app_result_lis),
                 ZBUS_MSG_INIT());

#endif /* CONFIG_APPLICATIONS_USE_VOICE_MEMO */
