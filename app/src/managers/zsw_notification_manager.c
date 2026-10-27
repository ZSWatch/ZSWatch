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

#include <string.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

#include <time.h>
#include <string.h>
#include <stdio.h>

#include "events/ble_event.h"
#include "events/zsw_notification_event.h"
#include "zsw_notification_manager.h"

LOG_MODULE_REGISTER(notification_mgr, LOG_LEVEL_DBG);

#define NOTIFICATION_INVALID_INDEX              0xFFFFFFFF

static void notification_mgr_zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void notification_mgr_update_worker(struct k_work *item);
static int32_t remove_notification(uint32_t id, zsw_notification_remove_reason_t reason);

static uint8_t num_notifications;
static zsw_not_mngr_notification_t notifications[ZSW_NOTIFICATION_MGR_MAX_STORED];
static bool notification_used[ZSW_NOTIFICATION_MGR_MAX_STORED];
static uint8_t notification_order[ZSW_NOTIFICATION_MGR_MAX_STORED];

static K_WORK_DEFINE(notification_work, notification_mgr_update_worker);
ZBUS_LISTENER_DEFINE(notification_mgr_ble_comm_lis, notification_mgr_zbus_ble_comm_data_callback);
ZBUS_CHAN_DECLARE(zsw_notification_mgr_chan);
ZBUS_CHAN_DECLARE(zsw_notification_mgr_remove_chan);

/** @brief      Find a notification index by a given notification ID.
 *  @param id   Notification ID
 *  @return     Notification array index of the notification
*/
static uint32_t find_notification_idx(uint32_t id)
{
    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (notification_used[i] && notifications[i].id == id) {
            return i;
        }
    }

    return NOTIFICATION_INVALID_INDEX;
}

/** @brief  Get the next free notification index.
 *  @return Free notification array index
*/
static uint32_t find_free_notification_idx(void)
{
    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (!notification_used[i]) {
            return i;
        }
    }

    return NOTIFICATION_INVALID_INDEX;
}

/** @brief  Find the oldest notification.
 *  @return Notification index of the oldest notification
*/
static uint32_t find_oldest_notification_idx(void)
{
    return num_notifications ? notification_order[0] : NOTIFICATION_INVALID_INDEX;
}

/** @brief  Find the newest notification.
 *  @return Notification index of the newest notification
*/
static uint32_t find_newest_notification_idx(void)
{
    return num_notifications ? notification_order[num_notifications - 1] : NOTIFICATION_INVALID_INDEX;
}

/** @brief          Copy a field into a zeroed notification field, never splitting a UTF-8 character.
 *  @param dst      Destination field, already zeroed
 *  @param dst_size Size of dst in bytes, including the terminator
 *  @param src      Source text, not required to be null terminated
 *  @param src_len  Source length in bytes
*/
static void copy_field_utf8(char *dst, size_t dst_size, const char *src, int src_len)
{
    size_t len = MIN((size_t)MAX(src_len, 0), dst_size - 1);

    if (src == NULL || len == 0) {
        return;
    }

    if (len < (size_t)src_len) {
        // Cut lands inside a multi-byte character, back off to its first byte.
        while (len > 0 && (src[len] & 0xC0) == 0x80) {
            len--;
        }
    }

    memcpy(dst, src, len);
}

/** @brief
 *  @param item
*/
static void notification_mgr_update_worker(struct k_work *item)
{
    zbus_chan_notify(&zsw_notification_mgr_chan, K_NO_WAIT);
}

/** @brief
 *  @param chan
*/
static void notification_mgr_zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    zsw_not_mngr_notification_t *not;

    // Need to context switch to not get stack overflow.
    // We are here in host bluetooth thread.
    const struct ble_data_event *event = zbus_chan_const_msg(chan);

    if (event->data.type == BLE_COMM_DATA_TYPE_NOTIFY) {
        // Notification source is empty, ignore.
        if (event->data.data.notify.src_len == 0) {
            return;
        }
        not = zsw_notification_manager_add(&event->data.data.notify);
        if (!not) {
            return;
        }

        LOG_DBG("ID: %u", not->id);
        LOG_DBG("Source: %u", not->src);
        LOG_DBG("Sender: %s", not->sender);
        LOG_DBG("Title: %s", not->title);
        LOG_DBG("Body: %s", not->body);
        LOG_DBG("Time: %u", not->timestamp);

        k_work_submit(&notification_work);
    } else if (event->data.type == BLE_COMM_DATA_TYPE_NOTIFY_REMOVE) {
        LOG_DBG("Remove notification with ID %u", event->data.data.notify_remove.id);

        if (remove_notification(event->data.data.notify_remove.id, ZSW_NOTIFICATION_REMOVED_BY_PHONE) != 0) {
            LOG_WRN("Notification %d not found", event->data.data.notify_remove.id);
        }
    }
}

void zsw_notification_manager_init(void)
{
    memset(notifications, 0, sizeof(notifications));
    memset(notification_used, 0, sizeof(notification_used));
    memset(notification_order, 0, sizeof(notification_order));
    num_notifications = 0;
}

zsw_not_mngr_notification_t *zsw_notification_manager_add(const ble_comm_notify_t *not)
{
    uint32_t idx;

    // Prevent double notifications with the same ID.
    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (notification_used[i] && notifications[i].id == not->id) {
            LOG_DBG("Duplicate notification ID %u, ignoring", not->id);
            return NULL;
        }
    }

    idx = find_free_notification_idx();
    // List full. We remove the oldest notification.
    if (idx == NOTIFICATION_INVALID_INDEX) {
        LOG_DBG("Notification buffer full");

        idx = find_oldest_notification_idx();
        remove_notification(notifications[idx].id, ZSW_NOTIFICATION_REMOVED_EVICTED);

        // No check needed because we have removed one notification.
        idx = find_free_notification_idx();
    }

    memset(&notifications[idx], 0, sizeof(zsw_not_mngr_notification_t));

    if (strncmp(not->src, "Messenger", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_FB_MESSENGER;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
        copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
    } else if (strncmp(not->src, "WhatsApp", not->src_len) == 0) {
        // {"t":"notify","id":1700974318,"src":"WhatsApp","title":"Daniel Kampert","subject":"","body":"H","sender":""}

        notifications[idx].src = NOTIFICATION_SRC_WHATSAPP;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
    } else if (strncmp(not->src, "Gmail", not->src_len) == 0) {
        // {t:"notify",id:1670967782,src:"Gmail",title:"Jakob Krantz",body:"Nytt test\nDetta YR NÖTT"

        // TODO Subject is before first \n in body so extract that into title field.
        notifications[idx].src = NOTIFICATION_SRC_GMAIL;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
        copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
    } else if (strncmp(not->src, "Home Assistant", not->src_len) == 0) {
        // {"t":"notify","id":1701847200,"src":"Home Assistant","title":"Message.","subject":"","body":"Gute Nacht","sender":""}

        notifications[idx].src = NOTIFICATION_SRC_HOME_ASSISTANT;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
        copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
    } else if (strncmp(not->src, "Discord", not->src_len) == 0) {
        // {"t":"notify","id":1701847200,"src":"Discord","title":"Message.","subject":"","body":"Gute Nacht","sender":""}

        notifications[idx].src = NOTIFICATION_SRC_DISCORD;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
        copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
    } else if (strncmp(not->src, "LinkedIn", not->src_len) == 0) {
        // {"t":"notify","id":1701847200,"src":"LinkedIn","title":"Message.","subject":"","body":"Gute Nacht","sender":""}

        notifications[idx].src = NOTIFICATION_SRC_LINKEDIN;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
        copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
    } else if (strncmp(not->src, "Reddit", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_REDDIT;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
        copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
    } else if (strncmp(not->src, "YouTube", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_YOUTUBE;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
        copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
    } else if (strncmp(not->src, "Messages", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_COMMON_MESSENGER;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
        copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
    } else if (strncmp(not->src, "Calendar", not->src_len) == 0 || strncmp(not->src, "Kalender", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_CALENDAR;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
        copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
    } else {
        // TODO add more
        // For example debug notfication
        // {t:"notify",id:1670967783,src:"Bangle.js Gadgetbridge",subject:"Testar",body:"Testar",sender:"Testar",tel:"Testar"}
        notifications[idx].src = NOTIFICATION_SRC_NONE;
        notifications[idx].id = not->id;
        copy_field_utf8(notifications[idx].title, sizeof(notifications[idx].title), not->src, not->src_len);
        copy_field_utf8(notifications[idx].body, sizeof(notifications[idx].body), not->body, not->body_len);
        // Android companions send who wrote the message as the title, not as the sender.
        if (not->sender_len > 0) {
            copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->sender, not->sender_len);
        } else {
            copy_field_utf8(notifications[idx].sender, sizeof(notifications[idx].sender), not->title, not->title_len);
        }
    }

    notifications[idx].can_reply = not->can_reply;
    notifications[idx].timestamp = time(NULL);

    notification_used[idx] = true;
    notification_order[num_notifications++] = idx;

    LOG_DBG("Notifications: %u", num_notifications);

    return &notifications[idx];
}

int32_t zsw_notification_manager_remove(uint32_t id)
{
    return remove_notification(id, ZSW_NOTIFICATION_REMOVED_BY_USER);
}

static int32_t remove_notification(uint32_t id, zsw_notification_remove_reason_t reason)
{
    uint32_t idx = find_notification_idx(id);

    if (idx != NOTIFICATION_INVALID_INDEX) {
        LOG_DBG("Remove notification on place %u with ID: %u", idx, notifications[idx].id);

        // NOTE: We pass a copy of the notification into the ZBUS event. This help the listeners to
        // handle the notification, because the data in the notification buffer can change before
        // the listeners have ececuted their operations.
        // Written straight into the channel, a copy on the stack is too large for the BLE thread.
        if (zbus_chan_claim(&zsw_notification_mgr_remove_chan, K_NO_WAIT) == 0) {
            struct zsw_notification_remove_event *evt = zbus_chan_msg(&zsw_notification_mgr_remove_chan);

            memcpy(&evt->notification, &notifications[idx], sizeof(zsw_not_mngr_notification_t));
            evt->reason = reason;
            zbus_chan_finish(&zsw_notification_mgr_remove_chan);
            zbus_chan_notify(&zsw_notification_mgr_remove_chan, K_NO_WAIT);
        } else {
            LOG_WRN("Removal of %u not published", id);
        }

        notification_used[idx] = false;
        for (uint32_t i = 0; i < num_notifications; i++) {
            if (notification_order[i] == idx) {
                memmove(&notification_order[i], &notification_order[i + 1],
                        (num_notifications - i - 1) * sizeof(notification_order[0]));
                num_notifications--;
                break;
            }
        }

        LOG_DBG("Notifications: %u", num_notifications);

        return 0;
    }

    return -ENOENT;
}

void zsw_notification_manager_foreach(zsw_notification_manager_foreach_cb_t cb, void *user_data)
{
    for (uint32_t i = 0; i < num_notifications; i++) {
        cb(&notifications[notification_order[i]], user_data);
    }
}

int32_t zsw_notification_manager_get(uint32_t id, zsw_not_mngr_notification_t *notification)
{
    uint32_t idx = find_notification_idx(id);

    if (idx == NOTIFICATION_INVALID_INDEX) {
        return -ENOENT;
    }

    memcpy(notification, &notifications[idx], sizeof(zsw_not_mngr_notification_t));

    return 0;
}

int32_t zsw_notification_manager_get_num(void)
{
    return num_notifications;
}

zsw_not_mngr_notification_t *zsw_notification_manager_get_newest(void)
{
    uint32_t idx = find_newest_notification_idx();
    if (idx != NOTIFICATION_INVALID_INDEX) {
        return &notifications[idx];
    }

    return NULL;
}
