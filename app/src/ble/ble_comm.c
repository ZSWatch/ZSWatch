/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
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

#include <zephyr/sys/base64.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <zephyr/zbus/zbus.h>

#include "ui/zsw_ui.h"
#include "gadgetbridge/ble_gadgetbridge.h"
#include "gadgetbridge/ble_find_device.h"
#include "chronos/ble_chronos.h"

#ifdef CONFIG_BT_AMS_CLIENT
#include <bluetooth/services/ams_client.h>
#endif
#ifdef CONFIG_BT_ANCS_CLIENT
#include <bluetooth/services/ancs_client.h>
#endif
LOG_MODULE_REGISTER(ble_comm, CONFIG_ZSW_BLE_LOG_LEVEL);

// How long after connecting the peer gets the short interval for discovery.
#define BLE_COMM_DISCOVERY_SHORT_INTERVAL_MS    5000
// 15 ms in 1.25 ms units.
#define BLE_COMM_SHORT_INTERVAL                 12

static void ble_connected(struct bt_conn *conn, uint8_t err);
static void ble_disconnected(struct bt_conn *conn, uint8_t reason);
static void ble_recycled(void);
static void bt_receive_cb(struct bt_conn *conn, const uint8_t *const data, uint16_t len);
static void conn_interval_update_handler(struct k_work *item);
static void discovery_done_handler(struct k_work *item);
static int update_adv_interval(uint16_t interval_min, uint16_t interval_max);
static void param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);
static void phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param);
static void le_data_length_updated(struct bt_conn *conn, struct bt_conn_le_data_len_info *info);

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = ble_connected,
    .disconnected = ble_disconnected,
    .recycled = ble_recycled,
    .le_param_updated = param_updated,
    .le_phy_updated = phy_updated,
    .le_data_len_updated = le_data_length_updated
};

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE,
                  (CONFIG_BT_DEVICE_APPEARANCE >> 0) & 0xff,
                  (CONFIG_BT_DEVICE_APPEARANCE >> 8) & 0xff),
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
                  BT_UUID_16_ENCODE(BT_UUID_DIS_VAL)),
#ifdef CONFIG_BT_ANCS_CLIENT
    BT_DATA_BYTES(BT_DATA_SOLICIT128, BT_UUID_ANCS_VAL),
#endif
};

static const struct bt_data ad_nus[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BLE_UUID_TRANSPORT_VAL),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static struct bt_le_adv_param adv_param = {
    .options = BT_LE_ADV_OPT_CONN,
    .interval_min = BT_GAP_ADV_SLOW_INT_MIN,
    .interval_max = BT_GAP_ADV_SLOW_INT_MAX,
};

K_WORK_DEFINE(conn_interval_update_work, conn_interval_update_handler);
K_WORK_DELAYABLE_DEFINE(discovery_done_work, discovery_done_handler);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_CHAN_DECLARE(music_control_data_chan);

static struct bt_conn *current_conn;
static uint32_t max_send_len;

static atomic_t pairing_enabled;

static struct ble_transport_cb ble_transport_callbacks = {
    .data_receive = bt_receive_cb,
};

static void auth_cancel(struct bt_conn *conn)
{
    LOG_ERR("Pairing cancelled");
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
    LOG_DBG("Pairing complete");
    struct bt_conn_info info;
    char addr[BT_ADDR_LE_STR_LEN];

    if (bt_conn_get_info(conn, &info) < 0) {
        addr[0] = '\0';
    }

    bt_addr_le_to_str(info.le.remote, addr, sizeof(addr));
    zsw_popup_show("Pairing successful", addr, NULL, 5, false);
    ble_comm_set_pairable(false);
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    struct bt_conn_info info;
    char addr[BT_ADDR_LE_STR_LEN];

    if (bt_conn_get_info(conn, &info) < 0) {
        addr[0] = '\0';
    }

    bt_addr_le_to_str(info.le.remote, addr, sizeof(addr));
    if (atomic_get(&pairing_enabled)) {
        zsw_popup_show("Pairing Failed", "Address:", NULL, 5, false);
    }
    LOG_WRN("Pairing Failed (%d). Disconnecting.", reason);
    bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
}

static enum bt_security_err pairing_accept(struct bt_conn *conn,
                                           const struct bt_conn_pairing_feat *const feat)
{
    if (!atomic_get(&pairing_enabled)) {
        LOG_WRN("Pairing rejected: pairing window closed");
        return BT_SECURITY_ERR_PAIR_NOT_ALLOWED;
    }

    LOG_INF("Pairing accepted: pairing window open");
    return BT_SECURITY_ERR_SUCCESS;
}

static void pairing_confirm(struct bt_conn *conn)
{
    if (atomic_get(&pairing_enabled)) {
        bt_conn_auth_pairing_confirm(conn);
    } else {
        bt_conn_auth_cancel(conn);
    }
}

static const struct bt_conn_auth_cb auth_cb_display = {
    .passkey_display = NULL,
    .passkey_entry = NULL,
    .pairing_accept = pairing_accept,
    .pairing_confirm = pairing_confirm,
    .cancel = auth_cancel,
};

static struct bt_conn_auth_info_cb auth_cb_info = {
    .pairing_complete = pairing_complete,
    .pairing_failed = pairing_failed,
};

int ble_comm_init(void)
{
    ble_comm_set_pairable(false);

    int err = bt_conn_auth_cb_register(&auth_cb_display);
    if (err) {
        LOG_ERR("Failed to register pairing callbacks (err: %d)", err);
        return err;
    }

    err = bt_conn_auth_info_cb_register(&auth_cb_info);
    if (err) {
        LOG_ERR("Failed to register pairing info callbacks (err: %d)", err);
        return err;
    }

    err = ble_transport_init(&ble_transport_callbacks);
    if (err) {
        LOG_ERR("Failed to initialize UART service (err: %d)", err);
        return err;
    }

    err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), ad_nus, ARRAY_SIZE(ad_nus));
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)", err);
    } else {
        LOG_DBG("Advertising successfully started");
    }

    return err;
}

int ble_comm_send(const uint8_t *data, uint16_t len)
{
    uint16_t offset = 0;
    int ret = 0;

    if (current_conn == NULL) {
        return -ENOTCONN;
    }

    while (offset < len) {
        uint16_t chunk_len = len - offset;
        if (chunk_len > max_send_len) {
            chunk_len = max_send_len;
        }

        ret = ble_transport_send(current_conn, data + offset, chunk_len);
        if (ret < 0) {
            // Running out of TX buffers mid-message leaves the phone with an incomplete
            // message. -EINVAL before the phone enables notifications is expected.
            if (offset > 0 || ret == -ENOMEM) {
                LOG_ERR("BLE send failed after %u of %u bytes: %d", offset, len, ret);
            } else {
                LOG_DBG("BLE send failed: %d", ret);
            }
            return ret;
        }
        offset += chunk_len;
    }
    return 0;
}

void ble_comm_set_pairable(bool pairable)
{
    atomic_set(&pairing_enabled, pairable);
    if (pairable) {
        LOG_WRN("Enable Pairable");
    } else {
        LOG_WRN("Disable Pairable");
    }
}

// Bit per enum ble_comm_short_interval_reason. The connection uses the short
// interval while any bit is set and the long idle interval otherwise.
static atomic_t short_interval_reasons;
// What was last requested on the current connection: -1 nothing, 0 long, 1 short.
static int requested_short = -1;

void ble_comm_short_interval_request(enum ble_comm_short_interval_reason reason)
{
    if (!atomic_test_and_set_bit(&short_interval_reasons, reason)) {
        k_work_submit(&conn_interval_update_work);
    }
}

void ble_comm_short_interval_release(enum ble_comm_short_interval_reason reason)
{
    if (atomic_test_and_clear_bit(&short_interval_reasons, reason)) {
        k_work_submit(&conn_interval_update_work);
    }
}

static void conn_interval_update_handler(struct k_work *item)
{
    ARG_UNUSED(item);
    bool want_short = atomic_get(&short_interval_reasons) != 0;

    if (current_conn == NULL || requested_short == want_short) {
        return;
    }

    // 15 ms is the shortest interval iOS accepts. The long one is the idle
    // interval from CONFIG_BT_PERIPHERAL_PREF_*.
    struct bt_le_conn_param param = {
        .interval_min = want_short ? BLE_COMM_SHORT_INTERVAL : CONFIG_BT_PERIPHERAL_PREF_MIN_INT,
        .interval_max = want_short ? BLE_COMM_SHORT_INTERVAL : CONFIG_BT_PERIPHERAL_PREF_MAX_INT,
        .latency = CONFIG_BT_PERIPHERAL_PREF_LATENCY,
        .timeout = CONFIG_BT_PERIPHERAL_PREF_TIMEOUT,
    };

    LOG_DBG("Request %s connection interval (reasons 0x%lx)", want_short ? "short" : "long",
            atomic_get(&short_interval_reasons));

    int err = bt_conn_le_param_update(current_conn, &param);
    if (err && err != -EALREADY) {
        LOG_ERR("bt_conn_le_param_update failed: %d", err);
        return;
    }
    requested_short = want_short;
}

static void discovery_done_handler(struct k_work *item)
{
    ARG_UNUSED(item);
    ble_comm_short_interval_release(BLE_COMM_SHORT_INTERVAL_DISCOVERY);
}

int ble_comm_set_fast_adv_interval(void)
{
    return update_adv_interval(BT_GAP_ADV_FAST_INT_MIN_1, BT_GAP_ADV_FAST_INT_MAX_1);
}

int ble_comm_set_default_adv_interval(void)
{
    return update_adv_interval(BT_GAP_ADV_SLOW_INT_MIN, BT_GAP_ADV_SLOW_INT_MAX);
}

int ble_comm_get_mtu(void)
{
    if (current_conn == NULL) {
        return 0;
    }
    return bt_gatt_get_mtu(current_conn);
}

int ble_comm_request_gps_status(bool enable)
{
    char gps_status[50];
    int len = snprintf(gps_status, sizeof(gps_status), "{\"t\":\"gps_power\", \"status\":%s} \n",
                       enable ? "true" : "false");
    LOG_DBG("Request GPS: %s", gps_status);
    return ble_comm_send(gps_status, len);
}

static int update_adv_interval(uint16_t interval_min, uint16_t interval_max)
{
    int err;

    if (interval_min > interval_max) {
        LOG_ERR("Invalid advertising interval: min (%d) > max (%d)", interval_min, interval_max);
        return -EINVAL;
    }

    LOG_INF("Updating advertising interval: min=%d, max=%d", interval_min, interval_max);

    adv_param.interval_min = interval_min;
    adv_param.interval_max = interval_max;

    // If currently advertising, restart with new parameters
    if (current_conn == NULL) {
        err = bt_le_adv_stop();
        if (err) {
            LOG_ERR("Failed to stop advertising (err %d)", err);
            return err;
        }

        err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), ad_nus, ARRAY_SIZE(ad_nus));
        if (err) {
            LOG_ERR("Failed to restart advertising (err %d)", err);
            return err;
        }

        LOG_DBG("Advertising restarted with new interval");
    }

    return 0;
}

static void mtu_exchange_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
    if (!err) {
        LOG_INF("MTU exchange done. %d", bt_gatt_get_mtu(current_conn) - 3);

        max_send_len = bt_gatt_get_mtu(current_conn) - 3;
    } else {
        LOG_ERR("MTU exchange failed (err %" PRIu8 ")", err);
    }
}

static void request_mtu_exchange(void)
{
    int err;
    static struct bt_gatt_exchange_params exchange_params;
    exchange_params.func = mtu_exchange_cb;

    err = bt_gatt_exchange_mtu(current_conn, &exchange_params);
    if (err) {
        LOG_ERR("MTU exchange failed (err %d)", err);
    } else {
        LOG_INF("MTU exchange pending");
    }
}

static void ble_connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    if (err) {
        LOG_ERR("Connection failed (err %u)", err);
        return;
    }
    current_conn = bt_conn_ref(conn);
    max_send_len = 20;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Connected %s", addr);
    request_mtu_exchange();
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);
    LOG_INF("Interval: %d, latency: %d, timeout: %d", info.le.interval, info.le.latency, info.le.timeout);

    // Right after a new connection we want short connection interval
    // to let the peer discover services etc. quickly. A central such as BlueZ
    // reuses the long interval from the previous connection otherwise.
    // After some time assume the peer is done; other reasons may keep it short.
    requested_short = -1;
    ble_comm_short_interval_request(BLE_COMM_SHORT_INTERVAL_DISCOVERY);
    k_work_submit(&conn_interval_update_work);
    k_work_schedule(&discovery_done_work, K_MSEC(BLE_COMM_DISCOVERY_SHORT_INTERVAL_MS));

    if (atomic_get(&pairing_enabled)) {
        int rc = bt_conn_set_security(conn, BT_SECURITY_L2);
        if (rc != 0) {
            LOG_ERR("Failed to set security: %d", rc);
            bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
        }
    }
    ble_chronos_state(true);
}

static void ble_disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_INF("Disconnected: %s (reason %u)", addr, reason);

    if (current_conn == conn) {
        ble_find_device_disconnected();
        k_work_cancel_delayable(&discovery_done_work);
        // Sensor notifications release their own reason when they stop.
        atomic_clear_bit(&short_interval_reasons, BLE_COMM_SHORT_INTERVAL_DISCOVERY);
        atomic_clear_bit(&short_interval_reasons, BLE_COMM_SHORT_INTERVAL_SMP);
        bt_conn_unref(current_conn);
        current_conn = NULL;
    }

    ble_chronos_state(false);
}

static void ble_recycled(void)
{
    int err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), ad_nus, ARRAY_SIZE(ad_nus));
    // update_adv_interval() restarts advertising itself; the connection object
    // it released is recycled afterwards, while the new advertiser holds the
    // only one (CONFIG_BT_MAX_CONN=1).
    if (err == -EALREADY || err == -ENOMEM) {
        LOG_DBG("Already advertising (err %d)", err);
    } else if (err) {
        LOG_ERR("Advertising failed to start (err %d)", err);
    } else {
        LOG_DBG("Advertising successfully started");
    }
}

static void param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
    LOG_INF("Updated => Interval: %d, latency: %d, timeout: %d", interval, latency, timeout);
    ble_chronos_connection_update();
}

static const char *phy2str(uint8_t phy)
{
    switch (phy) {
        case 0:
            return "No packets";
        case BT_GAP_LE_PHY_1M:
            return "LE 1M";
        case BT_GAP_LE_PHY_2M:
            return "LE 2M";
        case BT_GAP_LE_PHY_CODED:
            return "LE Coded";
        default:
            return "Unknown";
    }
}

static void le_data_length_updated(struct bt_conn *conn, struct bt_conn_le_data_len_info *info)
{
    LOG_INF("LE data len updated: TX (len: %d time: %d)" " RX (len: %d time: %d)\n", info->tx_max_len, info->tx_max_time,
            info->rx_max_len, info->rx_max_time);
}

static void phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param)
{
    LOG_INF("LE PHY updated: TX PHY %s, RX PHY %s", phy2str(param->tx_phy), phy2str(param->rx_phy));
}

static void bt_receive_cb(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
    LOG_HEXDUMP_DBG(data, len, "RX");

    ble_gadgetbridge_input(data, len);

    ble_chronos_input(data, len);
}
