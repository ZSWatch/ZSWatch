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

#define ZSW_NOTIFICATION_MGR_INVALID_ID         0xFFFFFFFF
#define NOTIFICATION_INVALID_INDEX              0xFFFFFFFF

static void notification_mgr_zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void notification_mgr_update_worker(struct k_work *item);

static uint8_t num_notifications;
static zsw_not_mngr_notification_t notifications[ZSW_NOTIFICATION_MGR_MAX_STORED];

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
        if (notifications[i].id == id) {
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
        if (notifications[i].id == ZSW_NOTIFICATION_MGR_INVALID_ID) {
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
    uint32_t oldest_idx = ZSW_NOTIFICATION_MGR_INVALID_ID;
    uint32_t oldest_id = NOTIFICATION_INVALID_INDEX;

    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if ((notifications[i].id != ZSW_NOTIFICATION_MGR_INVALID_ID) && (notifications[i].id < oldest_id)) {
            oldest_idx = i;
            oldest_id = notifications[i].id;
        }
    }

    return oldest_idx;
}

/** @brief  Find the newest notification.
 *  @return Notification index of the newest notification
*/
static uint32_t find_newest_notification_idx(void)
{
    uint32_t newest_idx = ZSW_NOTIFICATION_MGR_INVALID_ID;
    uint32_t newest_id = 0;

    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if ((notifications[i].id != ZSW_NOTIFICATION_MGR_INVALID_ID) && (notifications[i].id > newest_id)) {
            newest_idx = i;
            newest_id = notifications[i].id;
        }
    }

    return newest_idx;
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

        if (zsw_notification_manager_remove(event->data.data.notify_remove.id) != 0) {
            LOG_WRN("Notification %d not found", event->data.data.notify_remove.id);
        }
    }
}

void zsw_notification_manager_init(void)
{
    memset(notifications, 0, sizeof(notifications));
    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        notifications[i].id = ZSW_NOTIFICATION_MGR_INVALID_ID;
    }
    num_notifications = 0;
}

zsw_not_mngr_notification_t *zsw_notification_manager_add(const ble_comm_notify_t *not)
{
    uint32_t idx;

    // Prevent double notifications with the same ID. Return the double notification when found.
    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (notifications[i].id == not->id) {
            return &notifications[i];
        }
    }

    idx = find_free_notification_idx();
    // List full. We remove the oldest notification.
    if (idx == NOTIFICATION_INVALID_INDEX) {
        LOG_DBG("Notification buffer full");

        idx = find_oldest_notification_idx();
        zsw_notification_manager_remove(notifications[idx].id);

        // No check needed because we have removed one notification.
        idx = find_free_notification_idx();
    }

    memset(&notifications[idx], 0, sizeof(zsw_not_mngr_notification_t));

    if (strncmp(not->src, "Messenger", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_FB_MESSENGER;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else if (strncmp(not->src, "WhatsApp", not->src_len) == 0) {
        // {"t":"notify","id":1700974318,"src":"WhatsApp","title":"Daniel Kampert","subject":"","body":"H","sender":""}

        notifications[idx].src = NOTIFICATION_SRC_WHATSAPP;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else if (strncmp(not->src, "Gmail", not->src_len) == 0) {
        // {t:"notify",id:1670967782,src:"Gmail",title:"Jakob Krantz",body:"Nytt test\nDetta YR NÖTT"

        // TODO Subject is before first \n in body so extract that into title field.
        notifications[idx].src = NOTIFICATION_SRC_GMAIL;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else if (strncmp(not->src, "Home Assistant", not->src_len) == 0) {
        // {"t":"notify","id":1701847200,"src":"Home Assistant","title":"Message.","subject":"","body":"Gute Nacht","sender":""}

        notifications[idx].src = NOTIFICATION_SRC_HOME_ASSISTANT;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else if (strncmp(not->src, "Discord", not->src_len) == 0) {
        // {"t":"notify","id":1701847200,"src":"Discord","title":"Message.","subject":"","body":"Gute Nacht","sender":""}

        notifications[idx].src = NOTIFICATION_SRC_DISCORD;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else if (strncmp(not->src, "LinkedIn", not->src_len) == 0) {
        // {"t":"notify","id":1701847200,"src":"LinkedIn","title":"Message.","subject":"","body":"Gute Nacht","sender":""}

        notifications[idx].src = NOTIFICATION_SRC_LINKEDIN;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else if (strncmp(not->src, "Reddit", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_REDDIT;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else if (strncmp(not->src, "YouTube", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_YOUTUBE;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else if (strncmp(not->src, "Messages", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_COMMON_MESSENGER;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else if (strncmp(not->src, "Calendar", not->src_len) == 0 || strncmp(not->src, "Kalender", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_CALENDAR;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else {
        // TODO add more
        // For example debug notfication
        // {t:"notify",id:1670967783,src:"Bangle.js Gadgetbridge",subject:"Testar",body:"Testar",sender:"Testar",tel:"Testar"}
        notifications[idx].src = NOTIFICATION_SRC_NONE;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].title, not->src, MIN(not->src_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->sender, MIN(not->sender_len, ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    }

    notifications[idx].timestamp = time(NULL);

    if (num_notifications < ZSW_NOTIFICATION_MGR_MAX_STORED) {
        num_notifications++;
    }

    LOG_DBG("Notifications: %u", num_notifications);

    return &notifications[idx];
}

int32_t zsw_notification_manager_remove(uint32_t id)
{
    uint32_t idx = find_notification_idx(id);

    if (idx != NOTIFICATION_INVALID_INDEX) {
        struct zsw_notification_remove_event evt;

        LOG_DBG("Remove notification on place %u with ID: %u", idx, notifications[idx].id);

        // NOTE: We pass a copy of the notification into the ZBUS event. This help the listeners to
        // handle the notification, because the data in the notification buffer can change before
        // the listeners have ececuted their operations.
        memcpy(&evt.notification, &notifications[idx], sizeof(zsw_not_mngr_notification_t));
        zbus_chan_pub(&zsw_notification_mgr_remove_chan, &evt, K_NO_WAIT);

        notifications[idx].id = ZSW_NOTIFICATION_MGR_INVALID_ID;

        if (num_notifications > 0) {
            num_notifications--;
        }

        LOG_DBG("Notifications: %u", num_notifications);

        return 0;
    }

    return -ENOENT;
}

void zsw_notification_manager_get_all(zsw_not_mngr_notification_t *nots, uint32_t *num_notifications)
{
    uint32_t num_stored = 0;
    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (notifications[i].id != ZSW_NOTIFICATION_MGR_INVALID_ID) {
            nots[num_stored] = notifications[i];
            num_stored++;
        }
    }

    *num_notifications = num_stored;
}

int32_t zsw_notification_manager_get_num(void)
{
    return num_notifications;
}

zsw_not_mngr_notification_t *zsw_notification_manager_get_newest(void)
{
    int idx = find_newest_notification_idx();
    if (idx != ZSW_NOTIFICATION_MGR_INVALID_ID) {
        return &notifications[idx];
    }

    return NULL;
}