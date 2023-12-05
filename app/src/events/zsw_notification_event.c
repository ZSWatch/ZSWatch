#include <zephyr/zbus/zbus.h>

#include "zsw_notification_event.h"

ZBUS_CHAN_DEFINE(zsw_notification_mgr_chan,
                 struct zsw_notification_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(notification_app_lis, main_notification_lis),
                 ZBUS_MSG_INIT()
                );

/** @brief  This event informs all listeners about a removed notification. The event will contain a copy
 *          of the removed notification.
*/
ZBUS_CHAN_DEFINE(zsw_notification_mgr_remove_chan,
                 struct zsw_notification_remove_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(notification_app_remove_lis),
                 ZBUS_MSG_INIT()
                );