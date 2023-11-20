#include <string.h>

#include "zsw_notification_manager.h"

#define NOTIFICATION_INVALID_ID             0xFFFFFFFF
#define NOTIFICATION_INVALID_INDEX          0xFFFFFFFF

static uint32_t find_free_notification_idx(void);
static uint32_t find_notification_idx(uint32_t id);
static uint32_t find_oldest_notification_idx(void);
static uint32_t find_newest_notification_idx(void);

static zsw_not_mngr_notification_t notifications[NOTIFICATION_MANAGER_MAX_STORED];
static uint8_t num_notifications;
static zsw_not_mngr_notification_t *active_notification;

void zsw_notification_manager_init(void)
{
    memset(notifications, 0, sizeof(notifications));
    for (int i = 0; i < NOTIFICATION_MANAGER_MAX_STORED; i++) {
        notifications[i].id = NOTIFICATION_INVALID_ID;
    }
    num_notifications = 0;
    active_notification = NULL;
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
        notifications[idx].src = NOTIFICATION_SRC_MESSENGER;
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

    num_notifications++;

    if (!active_notification) {
        // Show it!
    }

    return &notifications[idx];
}

int32_t zsw_notification_manager_remove(uint32_t id)
{
    uint32_t idx = find_notification_idx(id);
    if (idx != NOTIFICATION_INVALID_INDEX) {
        notifications[idx].id = NOTIFICATION_INVALID_ID;
        num_notifications--;
        return 0;
    } else {
        return -ENOENT;
    }
}

int32_t zsw_notification_manager_get_all(zsw_not_mngr_notification_t *nots, int *num_notifications)
{
    int num_stored = 0;
    for (int i = 0; i < NOTIFICATION_MANAGER_MAX_STORED; i++) {
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
    for (int i = 0; i < NOTIFICATION_MANAGER_MAX_STORED; i++) {
        if (notifications[i].id == id) {
            return i;
        }
    }
    return NOTIFICATION_INVALID_INDEX;
}

static uint32_t find_free_notification_idx(void)
{
    for (int i = 0; i < NOTIFICATION_MANAGER_MAX_STORED; i++) {
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

    for (int i = 0; i < NOTIFICATION_MANAGER_MAX_STORED; i++) {
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

    for (int i = 0; i < NOTIFICATION_MANAGER_MAX_STORED; i++) {
        if (notifications[i].id != NOTIFICATION_INVALID_ID && notifications[i].id > newest_id) {
            newest_idx = i;
            newest_id = notifications[i].id;
        }
    }

    return newest_idx;
}