#pragma once

#include <zephyr/kernel.h>
#include "ble/ble_comm.h"

/**
 * @brief Init Apple Notification Center Service, ANCS
 */
int ble_ancs_init(void);

/**
 * @brief Check if ANCS service is present on the device connected to the watch
 *
 * @return true if ANCS is present
 */
bool ble_ancs_present(void);
