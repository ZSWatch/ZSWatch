#ifndef __BLE_ANCS_H
#define __BLE_ANCS_H

#include <zephyr/kernel.h>
#include "ble/ble_comm.h"

int ble_ancs_init(on_data_cb_t data_cb);

#endif