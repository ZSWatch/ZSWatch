#include <zephyr/zbus/zbus.h>

#include "zsw_notification_event.h"

ZBUS_CHAN_DEFINE(zsw_notification_mgr_chan,
                 struct zsw_notification_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(notification_app_lis, main_notification_lis),
                 ZBUS_MSG_INIT()
                );