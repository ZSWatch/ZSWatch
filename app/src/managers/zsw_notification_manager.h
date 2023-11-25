/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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

#include <inttypes.h>

#include "ble/ble_comm.h"

/** @brief The field "LabelHeader" allows 19 characters. So the maximum length is 20 bytes.
*/
#define ZSW_NOTIFICATION_MGR_MAX_SENDER_LEN     20

#define NOTIFICATION_MGR_MAX_FIELD_LEN          50

/** @brief Maximum number of notification stored at a time.
*/
#define ZSW_NOTIFICATION_MGR_MAX_STORED         20

typedef enum {
    NOTIFICATION_SRC_COMMON_MESSENGER,
    NOTIFICATION_SRC_WHATSAPP,
    NOTIFICATION_SRC_COMMON_MAIL,
    NOTIFICATION_SRC_GMAIL,
    NOTIFICATION_SRC_NONE
} zsw_notification_src_t;

typedef struct not_mngr_notification {
    uint32_t id;                                                /**< Notification ID. */
    uint32_t timestamp;                                         /**< Active notification time in seconds. */
    char sender[ZSW_NOTIFICATION_MGR_MAX_SENDER_LEN];           /**< */
    char title[NOTIFICATION_MGR_MAX_FIELD_LEN];//TODO: Maybe remove?
    char body[NOTIFICATION_MGR_MAX_FIELD_LEN];                  /**< */
    zsw_notification_src_t src;                                 /**< */
} zsw_not_mngr_notification_t;

/** @brief
*/
void zsw_notification_manager_init(void);

/** @brief
 *  @param notification
 *  @return
*/
zsw_not_mngr_notification_t *zsw_notification_manager_add(const ble_comm_notify_t *notification);

/** @brief
 *  @param id
 *  @return
*/
int32_t zsw_notification_manager_remove(uint32_t id);

/** @brief
 *  @param id
 *  @param notification
 *  @return
*/
int32_t zsw_notification_manager_get(uint32_t id, zsw_not_mngr_notification_t *notifcation);

/** @brief
 *  @param notifications
 *  @param num_notifications
 *  @return
*/
int32_t zsw_notification_manager_get_all(zsw_not_mngr_notification_t *notifcations, int *num_notifications);

/** @brief
 *  @return
*/
int32_t zsw_notification_manager_get_num(void);

/** @brief
 *  @return
*/
zsw_not_mngr_notification_t *zsw_notification_manager_get_newest(void);