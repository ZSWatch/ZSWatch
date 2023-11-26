/**
 * @file ble_hid.c
 * @author Leonardo Bispo
 *
 * @brief Implements Human Interface Device (HID) over BLE GATT Profile.
 *
 * @see https://www.bluetooth.com/specifications/specs/hid-over-gatt-profile-1-0/
 */

#include <stdio.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/kernel.h>

#include "ble/ble_hid.h"

#define HID_KEY_RIGHT_ARROW     0x4F
#define HID_KEY_LEFT_ARROW      0x50
#define HID_KEYBOARD_REPORT_ID  0x0A

LOG_MODULE_REGISTER(ble_hid, CONFIG_ZSW_BLE_LOG_LEVEL);

enum {
    HIDS_REMOTE_WAKE = BIT(0),
    HIDS_NORMALLY_CONNECTABLE = BIT(1),
};

struct hids_info {
    uint16_t version; /* version number of base USB HID Specification */
    uint8_t code;     /* country HID Device hardware is localized for. */
    uint8_t flags;
} __packed;

struct hids_report {
    uint8_t id;   /* report id */
    uint8_t type; /* report type */
} __packed;

static struct hids_info info = {
    .version = 0x0000,
    .code = 0x00,
    .flags = HIDS_NORMALLY_CONNECTABLE,
};

enum {
    HIDS_INPUT = 0x01,
    HIDS_OUTPUT = 0x02,
    HIDS_FEATURE = 0x03,
};

static struct hids_report input = {
    .id = HID_KEYBOARD_REPORT_ID,
    .type = HIDS_INPUT,
};

static uint8_t ctrl_point;
static uint8_t report_map[] = {
    0x05, 0x01, /* Usage Page (Generic Desktop) */
    0x09, 0x06, /* Usage (Keyboard) */
    0xA1, 0x01, /* Collection (Application) */

    /* Keys */
    0x85, HID_KEYBOARD_REPORT_ID, /* Report ID (10)*/
    0x05, 0x07,                   /* Usage Page (Key Codes) */
    0x19, 0xe0,                   /* Usage Minimum (224) */
    0x29, 0xe7,                   /* Usage Maximum (231) */
    0x15, 0x00,                   /* Logical Minimum (0) */
    0x25, 0x01,                   /* Logical Maximum (1) */
    0x75, 0x01,                   /* Report Size (1) */
    0x95, 0x08,                   /* Report Count (8) */
    0x81, 0x02,                   /* Input (Data, Variable, Absolute) */

    0x95, 0x01, /* Report Count (1) */
    0x75, 0x08, /* Report Size (8) */
    0x81, 0x01, /* Input (Constant) reserved byte(1) */

    0x95, 0x06, /* Report Count (6) */
    0x75, 0x08, /* Report Size (8) */
    0x15, 0x00, /* Logical Minimum (0) */
    0x25, 0x65, /* Logical Maximum (101) */
    0x05, 0x07, /* Usage Page (Key codes) */
    0x19, 0x00, /* Usage Minimum (0) */
    0x29, 0x65, /* Usage Maximum (101) */
    0x81, 0x00, /* Input (Data, Array) Key array(6 bytes) */

    0xC0 /* End Collection (Application) */
};

static void key_release_handle(struct k_work *item);
K_WORK_DELAYABLE_DEFINE(key_release, key_release_handle);

static ssize_t read_info(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len,
                         uint16_t offset)
{
    LOG_DBG("read_info");
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data, sizeof(struct hids_info));
}

static ssize_t read_report_map(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len,
                               uint16_t offset)
{
    LOG_DBG("read_report_map");
    return bt_gatt_attr_read(conn, attr, buf, len, offset, report_map, sizeof(report_map));
}

static ssize_t read_report(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len,
                           uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data, sizeof(struct hids_report));
}

static void input_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    ;
}

static ssize_t read_input_report(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len,
                                 uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, NULL, 0);
}

static ssize_t write_ctrl_point(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len,
                                uint16_t offset, uint8_t flags)
{
    uint8_t *value = attr->user_data;

    if (offset + len > sizeof(ctrl_point)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);

    return len;
}

/* HID Service Declaration */
BT_GATT_SERVICE_DEFINE(hog_svc,
                       BT_GATT_PRIMARY_SERVICE(BT_UUID_HIDS),
                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_INFO, BT_GATT_CHRC_READ,
                                              BT_GATT_PERM_READ, read_info, NULL, &info),
                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT_MAP, BT_GATT_CHRC_READ,
                                              BT_GATT_PERM_READ, read_report_map, NULL, NULL),
                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
                                              BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_READ_ENCRYPT,
                                              read_input_report, NULL, NULL),
                       BT_GATT_CCC(input_ccc_changed,
                                   BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT),
                       BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ,
                                          read_report, NULL, &input),
                       BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_CTRL_POINT,
                                              BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                                              BT_GATT_PERM_WRITE,
                                              NULL, write_ctrl_point, &ctrl_point), );

static void key_release_handle(struct k_work *item)
{
    int8_t report[8];
    memset(report, 0, sizeof(report));
    bt_gatt_notify(NULL, &hog_svc.attrs[5], report, sizeof(report));
}

static void send_key_release(void)
{
    k_work_reschedule(&key_release, K_MSEC(250));
}

void ble_hid_init(void)
{
    ;
}

int ble_hid_next(void)
{
    int ret = 0;

    // Byte 0: Modifier
    // Byte 1: Reserved
    // Byte 2: Keypress
    int8_t report[8] = {0x00, 0x00, HID_KEY_RIGHT_ARROW};
    ret = bt_gatt_notify(NULL, &hog_svc.attrs[5], report, sizeof(report));

    if (ret != 0) {
        LOG_ERR("ble_hid_next err: %d", ret);
        return ret;
    }

    send_key_release();

    return ret;
}

int ble_hid_previous(void)
{
    int ret = 0;

    // Byte 0: Modifier
    // Byte 1: Reserved
    // Byte 2: Keypress
    int8_t report[8] = {0x00, 0x00, HID_KEY_LEFT_ARROW};
    ret = bt_gatt_notify(NULL, &hog_svc.attrs[5], report, sizeof(report));

    if (ret != 0) {
        LOG_ERR("ble_hid_previous err: %d", ret);
        return ret;
    }

    send_key_release();

    return ret;
}
