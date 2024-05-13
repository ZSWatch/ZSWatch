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

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/byteorder.h>

#include "ble_comm_central.h"

#include "ble_hrs_central.h"

//LOG_MODULE_REGISTER(ble_comm_central, CONFIG_ZSW_BLE_LOG_LEVEL);
LOG_MODULE_REGISTER(ble_comm_central);

typedef struct peripheral_device_info {
    char name[31];
    bt_addr_le_t addr;
    ble_comm_central_periph_type_t periph_type;
}peripheral_device_info_t;

typedef struct connection_info {
    char name[31];
    struct bt_conn *connection;
} connection_info_t;

#define SCAN_TIMEOUT_10_ms              (1000)
#define DISCOVERED_PERIPHERALS_LIST_LEN     (5)

ble_comm_central_periph_type_t scan_periph_type = BLE_COMM_CENTRAL_PERIPH_TYPE_LAST;


static peripheral_device_info_t discovered_peripherals_list[DISCOVERED_PERIPHERALS_LIST_LEN];
static size_t discovered_peripherals_indx = 0;
static on_new_peripheral_discovered_evt on_new_peripheral_discovered_cb = NULL;
static on_peripheral_connected_evt on_peripheral_connected_cb = NULL;

static connection_info_t connections[BLE_COMM_CENTRAL_PERIPH_TYPE_LAST] = {
    [BLE_COMM_CENTRAL_PERIPH_TYPE_HEART_RATE_MONITOR] = {"", NULL},
    [BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_SPEED_SENSOR] = {"", NULL},
    [BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_CADENCE_SENSOR] = {"", NULL},
};

static bool is_peripheral_device_already_in_list(const bt_addr_le_t *addr);
static void on_peripheral_discovered(const bt_addr_le_t *addr, int8_t rssi, uint8_t type, struct net_buf_simple *ad);
static bool on_extended_inquiry_response(struct bt_data *data, void *user_data);
static void report_new_discovered_peripheral(const peripheral_device_info_t* peripheral);
static ble_comm_central_periph_type_t get_periph_type_from_connection(const struct bt_conn *connection);

int ble_comm_central_scan_start(ble_comm_central_periph_type_t periph_type,
                                on_new_peripheral_discovered_evt on_new_peripheral_discovered_handler,
                                on_peripheral_connected_evt on_peripheral_connected_handler)
{
    if (!bt_is_ready()) {
        return -EINVAL;
    }

    discovered_peripherals_indx = 0;        // delete list

    ble_comm_central_disconnect(periph_type);

    static const struct bt_le_scan_param scan_param = {
        .type       = BT_LE_SCAN_TYPE_ACTIVE,
		.options    = BT_LE_SCAN_OPT_FILTER_DUPLICATE,
		.interval   = BT_GAP_SCAN_FAST_INTERVAL,
		.window     = BT_GAP_SCAN_FAST_WINDOW,
        .timeout    = 0
    };
    int err = bt_le_scan_start(&scan_param, on_peripheral_discovered);
    if (err) {
        LOG_ERR("Scanning start failed due to: %d.", err);
        return -EINVAL;
    }

    scan_periph_type = periph_type;
    on_new_peripheral_discovered_cb = on_new_peripheral_discovered_handler;
    on_peripheral_connected_cb = on_peripheral_connected_handler;
    LOG_INF("Scanning started.");

    return 0;
}

int ble_comm_central_scan_stop(void)
{
    bt_le_scan_stop();
    on_new_peripheral_discovered_cb = NULL;
    LOG_INF("Stop scanning.");
    return 0;
}

int ble_comm_central_connect(ble_comm_central_periph_type_t periph_type, size_t index)
{
    if (index >= discovered_peripherals_indx) {
        return -EINVAL;
    }

    // disconnect from connected devices - also service that is using it
    ble_comm_central_disconnect(periph_type);

    // copy name
    strncpy(connections[periph_type].name, discovered_peripherals_list[index].name, 31);

    const peripheral_device_info_t *device_info = &discovered_peripherals_list[index];
    struct bt_le_conn_param *param = BT_LE_CONN_PARAM_DEFAULT;
    int err = bt_conn_le_create(&(device_info->addr), BT_CONN_LE_CREATE_CONN, param, &connections[periph_type].connection);
    if (err) {
        LOG_ERR("Failed to create central connection due to: %d", err);
        return -EINVAL;
    }
    LOG_INF("Try connect to %s", device_info->name);
    return 0;
}

int ble_comm_central_disconnect(ble_comm_central_periph_type_t periph_type)
{
    if (NULL != connections[periph_type].connection) {
        bt_conn_unref(connections[periph_type].connection);
        bt_conn_disconnect(connections[periph_type].connection, BT_HCI_ERR_REMOTE_USER_TERM_CONN);

        if (BLE_COMM_CENTRAL_PERIPH_TYPE_HEART_RATE_MONITOR == periph_type) {
            ble_hrs_central_stop();
        } else if (BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_SPEED_SENSOR == periph_type) {

        } else if (BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_CADENCE_SENSOR == periph_type) {

        }
        connections[periph_type].connection = NULL;
        memset(connections[periph_type].name, 0, 31);
    }

    return 0;
}

int ble_comm_central_get_info(ble_comm_central_periph_type_t periph_type, char name[31])
{
    if (NULL == connections[periph_type].connection) {
        return -ENODEV;
    }

    if (NULL != name) {
        strncpy(name, connections[periph_type].name, 31);
    }
    return 0;
}

bool is_central_connection(const struct bt_conn *conn)
{
    for (size_t i = 0; i < BLE_COMM_CENTRAL_PERIPH_TYPE_LAST; i++) {
        if (conn == connections[i].connection) {
            return true;
        }
    }
    return false;
}

void ble_comm_central_connected(struct bt_conn *conn, uint8_t err)
{
    ble_comm_central_periph_type_t periph_type = get_periph_type_from_connection(conn);

    if (BLE_COMM_CENTRAL_PERIPH_TYPE_LAST == periph_type) {
        return;
    }

    if (err) {
        LOG_ERR("Connection failed (err %u)", err);
        connections[periph_type].connection = NULL;
        memset(connections[periph_type].name, 0, 31);
        if (NULL != on_peripheral_connected_cb) {
            on_peripheral_connected_cb(connections[periph_type].name, periph_type, BLE_COMM_CENRAL_PERIPH_STATUS_NO_DEVICE);
        }
        return;
    }

    LOG_INF("Central is connected to peripheral.");
    if (NULL != on_peripheral_connected_cb) {
        on_peripheral_connected_cb(connections[periph_type].name, periph_type, BLE_COMM_CENRAL_PERIPH_STATUS_BONDED_CONNECTED);
    }

    if (BLE_COMM_CENTRAL_PERIPH_TYPE_HEART_RATE_MONITOR == periph_type) {
        ble_hrs_central_start(conn);
    } else if (BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_CADENCE_SENSOR == periph_type) {

    } else if (BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_SPEED_SENSOR == periph_type) {

    }
}

void ble_comm_central_disconnected(struct bt_conn *conn, uint8_t reason)
{
    ble_comm_central_periph_type_t periph_type = get_periph_type_from_connection(conn);

    if (BLE_COMM_CENTRAL_PERIPH_TYPE_LAST == periph_type) {
        return;
    }

    if (BLE_COMM_CENTRAL_PERIPH_TYPE_HEART_RATE_MONITOR == periph_type) {
        ble_hrs_central_stop();
    } else if (BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_CADENCE_SENSOR == periph_type) {

    } else if (BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_SPEED_SENSOR == periph_type) {

    }

    bt_conn_unref(conn);
    connections[periph_type].connection = NULL;
    memset((void*)connections[periph_type].name, 0, 31);
}

static void on_peripheral_discovered(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
    char dev[BT_ADDR_LE_STR_LEN];
    peripheral_device_info_t *new_peripheral = NULL;

    if (is_peripheral_device_already_in_list(addr)) {
        return;
    }

    if ((discovered_peripherals_indx + 1) >= DISCOVERED_PERIPHERALS_LIST_LEN) {
        LOG_ERR("New peripheral discovered but dropped because of list overflow.");
        return;
    }

    new_peripheral = &discovered_peripherals_list[discovered_peripherals_indx];
    memset((void*)new_peripheral, 0x00, sizeof(peripheral_device_info_t));

	if (type == BT_GAP_ADV_TYPE_ADV_IND ||
	    type == BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
        new_peripheral->periph_type = BLE_COMM_CENTRAL_PERIPH_TYPE_LAST;
		bt_data_parse(ad, on_extended_inquiry_response, (void*)new_peripheral);

        if (BLE_COMM_CENTRAL_PERIPH_TYPE_LAST != new_peripheral->periph_type) {
            /*  Left discovered device info in the list only if device type is recognized
             *  and it is supported.
             */
            discovered_peripherals_indx++;

            memcpy(&(new_peripheral->addr), addr, sizeof(bt_addr_le_t));

            bt_addr_le_to_str(addr, dev, sizeof(dev));
            LOG_INF("New peripheral device discovered: %s, AD evt type %u, AD data len %u, RSSI %i, Name: %s\n",
	               dev, type, ad->len, rssi, new_peripheral->name);

            report_new_discovered_peripheral(new_peripheral);
        }
	}
}

static bool on_extended_inquiry_response(struct bt_data *data, void *user_data)
{
    peripheral_device_info_t* new_peripheral = (peripheral_device_info_t*)(user_data);

	switch (data->type) {
	case BT_DATA_UUID16_SOME:
	case BT_DATA_UUID16_ALL:
    {
        // TODO: check whether it is required that UUID of the service is included in the advertisement packet
        if (0U != (data->data_len % sizeof(uint16_t))) {
            printk("AD malformed\r\n");
            return true;
        }

        for (int i = 0; i < data->data_len; i += sizeof(uint16_t)) {
            const struct bt_uuid *uuid;
            uint16_t u16;

            memcpy(&u16, &data->data[i], sizeof(u16));
            uuid = BT_UUID_DECLARE_16(sys_le16_to_cpu(u16));
            if ((0 == bt_uuid_cmp(uuid, BT_UUID_HRS))
            &&  (BLE_COMM_CENTRAL_PERIPH_TYPE_HEART_RATE_MONITOR == scan_periph_type)) {
                new_peripheral->periph_type = BLE_COMM_CENTRAL_PERIPH_TYPE_HEART_RATE_MONITOR;
            } else if ((0 == bt_uuid_cmp(uuid, BT_UUID_CSC))
            &&  (   (BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_CADENCE_SENSOR == scan_periph_type)
            ||      (BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_SPEED_SENSOR == scan_periph_type))) {
                new_peripheral->periph_type = scan_periph_type;
            }
        }
    }
    break;

    case BT_DATA_NAME_COMPLETE:
    case BT_DATA_NAME_SHORTENED:
    {
        size_t characters_to_copy = data->data_len;
        if (characters_to_copy > ARRAY_SIZE(new_peripheral->name)) {
            characters_to_copy = ARRAY_SIZE(new_peripheral->name);
        }
        memcpy(new_peripheral->name, data->data, characters_to_copy);
    }
    break;
    default:
        break;
	}

	return true;
}

static bool is_peripheral_device_already_in_list(const bt_addr_le_t *addr)
{
    size_t discovered_devices_list_n_entries = discovered_peripherals_indx;
    for (size_t i = 0; i < discovered_devices_list_n_entries; i++) {
        if (0 == bt_addr_le_cmp(&(discovered_peripherals_list[i].addr), addr)) {
            return true;
        }
    }
    return false;
}

static void report_new_discovered_peripheral(const peripheral_device_info_t* peripheral)
{
    if (NULL != on_new_peripheral_discovered_cb) {
        const char *new_peripheral_name = peripheral->name;
        on_new_peripheral_discovered_cb(new_peripheral_name);
    }
}

static ble_comm_central_periph_type_t get_periph_type_from_connection(const struct bt_conn *connection)
{
    if (connection == NULL) {
        return BLE_COMM_CENTRAL_PERIPH_TYPE_LAST;
    } else if (connection == connections[BLE_COMM_CENTRAL_PERIPH_TYPE_HEART_RATE_MONITOR].connection) {
        return BLE_COMM_CENTRAL_PERIPH_TYPE_HEART_RATE_MONITOR;
    } else if (connection == connections[BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_CADENCE_SENSOR].connection) {
        return BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_CADENCE_SENSOR;
    } else if (connection == connections[BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_SPEED_SENSOR].connection) {
        return BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_SPEED_SENSOR;
    }
    return BLE_COMM_CENTRAL_PERIPH_TYPE_LAST;
}
