#pragma once

#include "ble/ble_comm.h"

/** @brief  We use an empty struct, because all listeners were only informed. They have
 *          to fetch the notifications on their own.
*/
struct zsw_notification_event {
};