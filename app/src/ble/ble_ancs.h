#pragma once

#include <zephyr/kernel.h>
#include "ble/ble_comm.h"

int ble_ancs_init(on_data_cb_t data_cb);