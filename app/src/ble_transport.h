#ifndef __BLE_TRANSPORT_H
#define __BLE_TRANSPORT_H

#include <zephyr/types.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

// Using Nordic NUS as the Gadgetbridge app supports that
#define BLE_TRANSPORT_UUID_SERVICE   BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x6e400001, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e))
#define BLE_TRANSPORT_UUID_RX        BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x6e400002, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e))
#define BLE_TRANSPORT_UUID_TX        BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x6e400003, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e))

struct ble_transport_cb {
    void (*data_receive)(struct bt_conn *connection, const uint8_t *const data, uint16_t length);
};

int ble_transport_init(struct ble_transport_cb *callback);
int ble_transport_send(struct bt_conn *connection, const uint8_t *data, uint16_t length);

#endif