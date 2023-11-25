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

#define NOTIFICATION_INVALID_ID             0xFFFFFFFF
#define NOTIFICATION_INVALID_INDEX          0xFFFFFFFF

static uint32_t find_free_notification_idx(void);
static uint32_t find_notification_idx(uint32_t id);
static uint32_t find_oldest_notification_idx(void);
static uint32_t find_newest_notification_idx(void);
static void notification_mgr_zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void notification_mgr_update_worker(struct k_work *item);

static uint8_t num_notifications;
static zsw_not_mngr_notification_t notifications[ZSW_NOTIFICATION_MGR_MAX_STORED];

static ble_comm_notify_t notification;

static K_WORK_DEFINE(notification_work, notification_mgr_update_worker);
ZBUS_LISTENER_DEFINE(notification_mgr_ble_comm_lis, notification_mgr_zbus_ble_comm_data_callback);
ZBUS_CHAN_DECLARE(zsw_notification_mgr_chan);

static void notification_mgr_update_worker(struct k_work *item)
{
    zsw_not_mngr_notification_t *not;

    not = zsw_notification_manager_add(&notification);
    if (!not) {
        return;
    }

    LOG_DBG("ID: %u", not->id);
    LOG_DBG("Source: %u", not->src);
    LOG_DBG("Sender: %s", not->sender);
    LOG_DBG("Title: %s", not->title);
    LOG_DBG("Body: %s", not->body);
    LOG_DBG("Time: %u", not->timestamp);

    zbus_chan_notify(&zsw_notification_mgr_chan, K_NO_WAIT);
}

static void notification_mgr_zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    // Need to context switch to not get stack overflow.
    // We are here in host bluetooth thread.
    const struct ble_data_event *event = zbus_chan_const_msg(chan);

    if (event->data.type == BLE_COMM_DATA_TYPE_NOTIFY) {
        memcpy(&notification, &event->data.data.notify, sizeof(ble_comm_notify_t));
        k_work_submit(&notification_work);
    } else if (event->data.type == BLE_COMM_DATA_TYPE_NOTIFY_REMOVE) {
        if (zsw_notification_manager_remove(event->data.data.notify_remove.id) != 0) {
            LOG_WRN("Notification %d not found", event->data.data.notify_remove.id);
        }
    }
}

void zsw_notification_manager_init(void)
{
    memset(notifications, 0, sizeof(notifications));
    for (int i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        notifications[i].id = NOTIFICATION_INVALID_ID;
    }
    num_notifications = 0;
}

zsw_not_mngr_notification_t *zsw_notification_manager_add(const ble_comm_notify_t *not)
{
    uint32_t idx = find_free_notification_idx();

    if (idx == NOTIFICATION_INVALID_INDEX) {
        // List full then we replace the oldest
        idx = find_oldest_notification_idx();
        __ASSERT_NO_MSG(idx != NOTIFICATION_INVALID_INDEX);
        notifications[idx].id = NOTIFICATION_INVALID_ID;
        num_notifications--;
    }

    memset(&notifications[idx], 0, sizeof(zsw_not_mngr_notification_t));

    if (strncmp(not->src, "Messenger", not->src_len) == 0) {
        notifications[idx].src = NOTIFICATION_SRC_COMMON_MESSENGER;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].title, not->title, MIN(not->title_len, NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->sender, MIN(not->sender_len, NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else if (strncmp(not->src, "Gmail", not->src_len) == 0) {
        // {t:"notify",id:1670967782,src:"Gmail",title:"Jakob Krantz",body:"Nytt test\nDetta YR NÖTT"

        // TODO Subject is before first \n in body so extract that into title field.
        notifications[idx].src = NOTIFICATION_SRC_GMAIL;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->title, MIN(not->title_len, NOTIFICATION_MGR_MAX_FIELD_LEN - 1));

        memcpy(notifications[idx].title, not->title, MIN(not->title_len, NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    } else {
        // TODO add more
        // For example debug notfication
        // {t:"notify",id:1670967783,src:"Bangle.js Gadgetbridge",subject:"Testar",body:"Testar",sender:"Testar",tel:"Testar"}
        notifications[idx].src = NOTIFICATION_SRC_NONE;
        notifications[idx].id = not->id;
        memcpy(notifications[idx].title, not->src, MIN(not->src_len, NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].body, not->body, MIN(not->body_len, NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
        memcpy(notifications[idx].sender, not->sender, MIN(not->sender_len, NOTIFICATION_MGR_MAX_FIELD_LEN - 1));
    }

    notifications[idx].timestamp = time(NULL);

    if (num_notifications < ZSW_NOTIFICATION_MGR_MAX_STORED) {
        num_notifications++;
    }

    return &notifications[idx];
}

int32_t zsw_notification_manager_remove(uint32_t id)
{
    int msg_len;
    char buf[100];
    uint32_t idx = find_notification_idx(id);

    if (idx != NOTIFICATION_INVALID_INDEX) {
        notifications[idx].id = NOTIFICATION_INVALID_ID;

        // TODO: We have to check the type of notification here to figure out the sender (BLE or other sources).
        memset(buf, 0, sizeof(buf));
        msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"notify\", \"id\": %d, \"n\": %s} \n", id, "\"DISMISS\"");
        ble_comm_send(buf, msg_len);

        if (num_notifications > 0) {
            num_notifications--;
        }

        return 0;
    } else {
        return -ENOENT;
    }
}

int32_t zsw_notification_manager_get_all(zsw_not_mngr_notification_t *nots, int *num_notifications)
{
    int num_stored = 0;
    for (int i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (notifications[i].id != NOTIFICATION_INVALID_ID) {
            nots[num_stored] = notifications[i];
            num_stored++;
        }
    }
    *num_notifications = num_stored;
    return 0;
}

int32_t zsw_notification_manager_get_num(void)
{
    return num_notifications;
}

zsw_not_mngr_notification_t *zsw_notification_manager_get_newest(void)
{
    int idx = find_newest_notification_idx();
    if (idx != NOTIFICATION_INVALID_ID) {
        return &notifications[idx];
    } else {
        return NULL;
    }
}

static uint32_t find_notification_idx(uint32_t id)
{
    for (int i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (notifications[i].id == id) {
            return i;
        }
    }
    return NOTIFICATION_INVALID_INDEX;
}

static uint32_t find_free_notification_idx(void)
{
    for (int i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (notifications[i].id == NOTIFICATION_INVALID_ID) {
            return i;
        }
    }
    return NOTIFICATION_INVALID_INDEX;
}

static uint32_t find_oldest_notification_idx(void)
{
    uint32_t oldest_idx = NOTIFICATION_INVALID_ID;
    uint32_t oldest_id = NOTIFICATION_INVALID_INDEX;

    for (int i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (notifications[i].id != NOTIFICATION_INVALID_ID && notifications[i].id < oldest_id) {
            oldest_idx = i;
            oldest_id = notifications[i].id;
        }
    }

    return oldest_idx;
}

static uint32_t find_newest_notification_idx(void)
{
    uint32_t newest_idx = NOTIFICATION_INVALID_ID;
    uint32_t newest_id = 0;

    for (int i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (notifications[i].id != NOTIFICATION_INVALID_ID && notifications[i].id > newest_id) {
            newest_idx = i;
            newest_id = notifications[i].id;
        }
    }

    return newest_idx;
}