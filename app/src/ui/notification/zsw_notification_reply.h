/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Dictated replies to notifications. The reply is recorded as a voice memo named
 * "reply_...", the phone transcribes it and sends the text back as a draft. The
 * user reviews it and Send asks the phone to reply through the notification's
 * reply action. Only notifications marked can_reply by the phone get a Reply button.
 */

#ifdef CONFIG_ZSW_NOTIFICATION_REPLY

/** @brief      Start recording a reply. Call from the LVGL thread.
 *  @param id   Notification ID
 *  @return     0 when recording started, negative on error
*/
int zsw_notification_reply_start(uint32_t id);

bool zsw_notification_reply_is_shown(void);

#else

static inline int zsw_notification_reply_start(uint32_t id)
{
    (void)id;
    return -ENOTSUP;
}

static inline bool zsw_notification_reply_is_shown(void)
{
    return false;
}

#endif
