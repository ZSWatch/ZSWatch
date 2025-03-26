#pragma once

#include <zephyr/bluetooth/bluetooth.h>

#include "broadcast_assistant.h"

typedef struct lea_assistant_device_t {
    char name[BT_NAME_LEN];
    /** Advertising Set Identifier. */
    uint8_t sid;
    uint16_t pa_interval;
    uint32_t broadcast_id;
    bt_addr_le_t addr;
} lea_assistant_device_t;

void lea_assistant_app_add_source_entry(lea_assistant_device_t *device);

void lea_assistant_app_add_sink_entry(lea_assistant_device_t *device);

void lea_assistant_app_sink_connected(void);
