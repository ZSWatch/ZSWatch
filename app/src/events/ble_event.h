#pragma once

#include "ble/ble_comm.h"

struct ble_data_event {
    ble_comm_cb_data_t data;
};

struct ble_connect_event {
};

struct ble_disconnect_event {
    uint8_t reason;
};