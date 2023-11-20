/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>

#include "ble/ble_transport.h"

LOG_MODULE_REGISTER(ble_transport, CONFIG_ZSW_BLE_LOG_LEVEL);

static struct ble_transport_cb *callbacks;

static ssize_t on_receive(struct bt_conn *conn,
                          const struct bt_gatt_attr *attr,
                          const void *buf,
                          uint16_t len,
                          uint16_t offset,
                          uint8_t flags)
{
    LOG_DBG("Received data, handle %d, conn %p, len: %d",
            attr->handle, (void *)conn, len);

    if (callbacks->data_receive) {
        callbacks->data_receive(conn, buf, len);
    }
    return len;
}

BT_GATT_SERVICE_DEFINE(nus_service,
                       BT_GATT_PRIMARY_SERVICE(BLE_TRANSPORT_UUID_SERVICE),
                       BT_GATT_CHARACTERISTIC(BLE_TRANSPORT_UUID_TX,
                                              BT_GATT_CHRC_NOTIFY,
#if CONFIG_BLE_DISABLE_PAIRING_REQUIRED
                                              BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
#else
                                              BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT,
#endif
                                              NULL, NULL, NULL),
                       BT_GATT_CCC(NULL,
#if CONFIG_BLE_DISABLE_PAIRING_REQUIRED
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
#else
                                   BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT),
#endif
                       BT_GATT_CHARACTERISTIC(BLE_TRANSPORT_UUID_RX,
                                              BT_GATT_CHRC_WRITE |
                                              BT_GATT_CHRC_WRITE_WITHOUT_RESP,
#if CONFIG_BLE_DISABLE_PAIRING_REQUIRED
                                              BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
#else
                                              BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT,
#endif
                                              NULL, on_receive, NULL),
                      );

int ble_transport_init(struct ble_transport_cb *callback)
{
    if (callback) {
        callbacks = callback;
    }

    return 0;
}

int ble_transport_send(struct bt_conn *connection, const uint8_t *data, uint16_t length)
{
    struct bt_gatt_notify_params params = {0};
    const struct bt_gatt_attr *attr = &nus_service.attrs[2];

    params.attr = attr;
    params.data = data;
    params.len = length;

    if (connection && bt_gatt_is_subscribed(connection, attr, BT_GATT_CCC_NOTIFY)) {
        return bt_gatt_notify_cb(connection, &params);
    } else {
        return -EINVAL;
    }
}
