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

#include <inttypes.h>

#include "ble/ble_comm.h"

/** @brief Size of the sender and title fields, including the terminator.
*/
#define ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN      50

/** @brief Size of the body field, including the terminator.
*/
#define ZSW_NOTIFICATION_MGR_MAX_BODY_LEN       160

/** @brief Maximum number of notification stored at a time.
*/
#define ZSW_NOTIFICATION_MGR_MAX_STORED         10

/** @brief Notification sources definitions.
*/
typedef enum {
    NOTIFICATION_SRC_FB_MESSENGER,                              /**< */
    NOTIFICATION_SRC_COMMON_MESSENGER,                          /**< */
    NOTIFICATION_SRC_WHATSAPP,                                  /**< */
    NOTIFICATION_SRC_COMMON_MAIL,                               /**< */
    NOTIFICATION_SRC_GMAIL,                                     /**< */
    NOTIFICATION_SRC_YOUTUBE,                                   /**< */
    NOTIFICATION_SRC_HOME_ASSISTANT,                            /**< */
    NOTIFICATION_SRC_DISCORD,                                   /**< */
    NOTIFICATION_SRC_LINKEDIN,                                  /**< */
    NOTIFICATION_SRC_REDDIT,                                    /**< */
    NOTIFICATION_SRC_CALENDAR,                                  /**< */
    NOTIFICATION_SRC_NONE                                       /**< */
} zsw_notification_src_t;

/** @brief Notification object definition.
*/
typedef struct not_mngr_notification {
    uint32_t id;                                                /**< Notification ID. */
    uint32_t timestamp;                                         /**< Active notification time in seconds. */
    char sender[ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN];            /**< Contains the notification sender (e-mail address or name in WhatsApp). */
    char title[ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN];             /**< */
    char body[ZSW_NOTIFICATION_MGR_MAX_BODY_LEN];               /**< */
    zsw_notification_src_t src;                                 /**< */
    bool can_reply;                                             /**< The phone can send a dictated reply to it. */
} zsw_not_mngr_notification_t;

/** @brief Why a notification left the watch. Only a user dismissal is reported to the phone.
*/
typedef enum {
    ZSW_NOTIFICATION_REMOVED_BY_USER,                           /**< Dismissed on the watch. */
    ZSW_NOTIFICATION_REMOVED_BY_PHONE,                          /**< Removed on the phone. */
    ZSW_NOTIFICATION_REMOVED_EVICTED,                           /**< Dropped to make room for a newer one. */
} zsw_notification_remove_reason_t;

/** @brief
*/
void zsw_notification_manager_init(void);

/** @brief
 *  @param notification
 *  @return
*/
zsw_not_mngr_notification_t *zsw_notification_manager_add(const ble_comm_notify_t *notification);

/** @brief      Remove a notification the user dismissed on the watch.
 *  @param id   Notification ID
 *  @return     0 when successful
*/
int32_t zsw_notification_manager_remove(uint32_t id);

/** @brief                  Copy a stored notification.
 *  @param id               Notification ID
 *  @param notification     Output
 *  @return                 0 when found, -ENOENT otherwise
*/
int32_t zsw_notification_manager_get(uint32_t id, zsw_not_mngr_notification_t *notification);

/** @brief              Called by zsw_notification_manager_foreach().
 *  @param notification Only valid during the call
 *  @param user_data
*/
typedef void (*zsw_notification_manager_foreach_cb_t)(const zsw_not_mngr_notification_t *notification,
                                                      void *user_data);

/** @brief          Call cb for every stored notification, oldest first, without copying them.
 *  @param cb
 *  @param user_data
*/
void zsw_notification_manager_foreach(zsw_notification_manager_foreach_cb_t cb, void *user_data);

/** @brief
 *  @return
*/
int32_t zsw_notification_manager_get_num(void);

/** @brief
 *  @return
*/
zsw_not_mngr_notification_t *zsw_notification_manager_get_newest(void);
