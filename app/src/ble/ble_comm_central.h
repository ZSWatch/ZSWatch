/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2024.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <zephyr/bluetooth/conn.h>

typedef enum
{
    BLE_COMM_CENTRAL_PERIPH_TYPE_HEART_RATE_MONITOR = 0,
    BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_SPEED_SENSOR,
    BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_CADENCE_SENSOR,
    BLE_COMM_CENTRAL_PERIPH_TYPE_LAST
}ble_comm_central_periph_type_t;

typedef enum
{
    /*  @warning: Member's values must monothonically increaes. Theres should not be any gaps.
     */
    BLE_COMM_CENRAL_PERIPH_STATUS_NO_DEVICE = 0,
    BLE_COMM_CENRAL_PERIPH_STATUS_BONDED_NOT_IN_RANGE,
    BLE_COMM_CENRAL_PERIPH_STATUS_BONDED_CONNECTED,
    BLE_COMM_CENRAL_PERIPH_STATUS_LAST
}ble_comm_central_periph_status_t;

typedef void(*on_new_peripheral_discovered_evt)(const char *name);
typedef void(*on_peripheral_connected_evt)(const char *name, ble_comm_central_periph_type_t periph_type, ble_comm_central_periph_status_t periph_status);

int ble_comm_central_scan_start(ble_comm_central_periph_type_t periph_type,
                                on_new_peripheral_discovered_evt on_new_peripheral_discovered_handler,
                                on_peripheral_connected_evt on_peripheral_connected_handler);
int ble_comm_central_scan_stop(void);
int ble_comm_central_connect(ble_comm_central_periph_type_t periph_type, size_t index);
int ble_comm_central_disconnect(ble_comm_central_periph_type_t periph_type);
int ble_comm_central_get_info(ble_comm_central_periph_type_t periph_type, char name[31]);

bool is_central_connection(const struct bt_conn *conn);
void ble_comm_central_connected(struct bt_conn *conn, uint8_t err);
void ble_comm_central_disconnected(struct bt_conn *conn, uint8_t reason);