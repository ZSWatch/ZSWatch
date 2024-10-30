/*
 * Copyright (c) 2024 Demant A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @note this is adapted from https://github.com/AstraeusLabs/web-broadcast-assistant/tree/f1655305f3f1064fde04ed6f6744ca188bdc1670 */

#ifndef __BROADCAST_ASSISTANT_H__
#define __BROADCAST_ASSISTANT_H__

#include <zephyr/types.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/addr.h>

#define BT_DATA_RSSI         (BT_DATA_MANUFACTURER_DATA - 1)
#define BT_DATA_SID          (BT_DATA_MANUFACTURER_DATA - 2)
#define BT_DATA_PA_INTERVAL  (BT_DATA_MANUFACTURER_DATA - 3)
#define BT_DATA_ERROR_CODE   (BT_DATA_MANUFACTURER_DATA - 4)
#define BT_DATA_BROADCAST_ID (BT_DATA_MANUFACTURER_DATA - 5)
#define BT_DATA_RPA          (BT_DATA_MANUFACTURER_DATA - 6)
#define BT_DATA_IDENTITY     (BT_DATA_MANUFACTURER_DATA - 7)
#define BT_DATA_BASE         (BT_DATA_MANUFACTURER_DATA - 8)
#define BT_NAME_LEN 30

enum {
	BROADCAST_ASSISTANT_SCAN_TARGET_SOURCE = BIT(0),
	BROADCAST_ASSISTANT_SCAN_TARGET_SINK = BIT(1),
	BROADCAST_ASSISTANT_SCAN_TARGET_ALL =
		(BROADCAST_ASSISTANT_SCAN_TARGET_SOURCE | BROADCAST_ASSISTANT_SCAN_TARGET_SINK)
};

typedef struct scan_recv_data {
    char bt_name[BT_NAME_LEN];
    uint8_t bt_name_type;
    char broadcast_name[BT_NAME_LEN];
    uint32_t broadcast_id;
    bool has_bass;
    bool has_pacs;
} scan_recv_data_t;

int start_scan(uint8_t target);
int stop_scanning(void);
int connect_to_sink(bt_addr_le_t *bt_addr_le);
int disconnect_from_sink(bt_addr_le_t *bt_addr_le);
int add_source(uint8_t sid, uint16_t pa_interval, uint32_t broadcast_id, bt_addr_le_t *addr);
int remove_source(void);
int broadcast_assistant_init(void);
int disconnect_unpair_all(void);

#endif /* __BROADCAST_ASSISTANT_H__ */
