#ifndef __NOTIFICATION_MANAGERH_
#define __NOTIFICATION_MANAGERH_
#include <inttypes.h>
#include <ble_comm.h>

#define NOTIFICATION_MGR_MAX_FIELD_LEN  50
#define NOTIFICATION_MANAGER_MAX_STORED 5

typedef enum notification_src {
    NOTIFICATION_SRC_MESSENGER,
    NOTIFICATION_SRC_GMAIL,
    NOTIFICATION_SRC_NONE
} notification_src_t;

typedef struct not_mngr_notification {
    uint32_t id;
    char sender[NOTIFICATION_MGR_MAX_FIELD_LEN];
    char title[NOTIFICATION_MGR_MAX_FIELD_LEN];
    char body[NOTIFICATION_MGR_MAX_FIELD_LEN];
    notification_src_t src;
} not_mngr_notification_t;

void notification_manager_init(void);
not_mngr_notification_t *notification_manager_add(ble_comm_notify_t *notification);
int32_t notification_manager_remove(uint32_t id);
int32_t notification_manager_get(uint32_t id, not_mngr_notification_t *notifcation);
int32_t notification_manager_get_all(not_mngr_notification_t *notifcations, int *num_notifications);
int32_t notification_manager_get_num(void);
not_mngr_notification_t *notification_manager_get_newest(void);

#endif