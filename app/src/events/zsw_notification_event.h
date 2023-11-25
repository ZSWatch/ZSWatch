#pragma once

#include "ble/ble_comm.h"
#include "managers/zsw_notification_manager.h"

/** @brief  We use an empty struct, because all listeners were only informed. They have
 *          to fetch the notifications on their own.
*/
struct zsw_notification_event {
};

struct zsw_notification_remove_event {
    zsw_not_mngr_notification_t notification;
};