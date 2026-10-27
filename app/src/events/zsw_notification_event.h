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
    zsw_notification_remove_reason_t reason;
};

/** @brief Longest reply text kept on the watch, including the terminator. */
#define ZSW_NOTIFICATION_REPLY_MAX_TEXT 512

enum zsw_notification_reply_event_type {
    ZSW_NOTIFICATION_REPLY_DRAFT,           /**< The phone transcribed a dictated reply, text is the draft. */
    ZSW_NOTIFICATION_REPLY_DRAFT_FAILED,    /**< The phone could not transcribe it, text says why. */
    ZSW_NOTIFICATION_REPLY_SENT,            /**< The phone tried to send a reply, ok says if it worked. */
};

/** @brief  Phone answers for dictated replies. Published from the BLE thread. */
struct zsw_notification_reply_event {
    enum zsw_notification_reply_event_type type;
    uint32_t id;                                    /**< Notification the reply was for (SENT). */
    bool ok;                                        /**< The reply reached the app (SENT). */
    char filename[32];                              /**< Recording the draft is from (DRAFT, DRAFT_FAILED). */
    char text[ZSW_NOTIFICATION_REPLY_MAX_TEXT];
};
