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
#include "managers/zsw_smp_manager.h"

#ifdef CONFIG_BT_AMS_CLIENT
#include <bluetooth/services/ams_client.h>
#endif
#ifdef CONFIG_BT_ANCS_CLIENT
#include <bluetooth/services/ancs_client.h>
#endif
LOG_MODULE_REGISTER(ble_comm, CONFIG_ZSW_BLE_LOG_LEVEL);

#define BLE_COMM_LONG_INT_MIN_MS                (400 / 1.25)
#define BLE_COMM_LONG_INT_MAX_MS                (500 / 1.25)
#define BLE_COMM_CONN_INT_UPDATE_TIMEOUT_MS     5000

static void ble_connected(struct bt_conn *conn, uint8_t err);
static void ble_disconnected(struct bt_conn *conn, uint8_t reason);
static void ble_recycled(void);
static void bt_receive_cb(struct bt_conn *conn, const uint8_t *const data, uint16_t len);
static void update_conn_interval_slow_handler(struct k_work *item);
static void update_conn_interval_short_handler(struct k_work *item);
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

K_WORK_DELAYABLE_DEFINE(conn_interval_slow_work, update_conn_interval_slow_handler);
K_WORK_DELAYABLE_DEFINE(conn_interval_fast_work, update_conn_interval_short_handler);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_CHAN_DECLARE(music_control_data_chan);

static struct bt_conn *current_conn;
static struct k_spinlock ble_tx_lock;
static uint32_t ble_tx_generation;
static bool ble_tx_failed;

static int pairing_enabled;

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
    if (pairing_enabled) {
        zsw_popup_show("Pairing Failed", "Address:", NULL, 5, false);
    }
    LOG_WRN("Pairing Failed (%d). Disconnecting.", reason);
    bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
}

static void pairing_deny(struct bt_conn *conn)
{
    LOG_ERR("Pairing deny");
    bt_conn_auth_cancel(conn);
}

static void pairing_accept(struct bt_conn *conn)
{
    LOG_WRN("Pairing accept");
    bt_conn_auth_pairing_confirm(conn);
}

static struct bt_conn_auth_cb auth_cb_display = {
    .passkey_display = NULL,
    .passkey_entry = NULL,
    .pairing_confirm = pairing_deny,
    .cancel = auth_cancel,
};

static struct bt_conn_auth_info_cb auth_cb_info = {
    .pairing_complete = pairing_complete,
    .pairing_failed = pairing_failed,
};

int ble_comm_init(void)
{
    bt_conn_auth_cb_register(&auth_cb_display);
    bt_conn_auth_info_cb_register(&auth_cb_info);

    ble_comm_set_pairable(false);

    int err = ble_transport_init(&ble_transport_callbacks);
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

/* A complete NUS message is admitted before sending any bytes. The BT RX and
 * system work queues cannot wait for ATT buffers; a recording list can require
 * more packets than that pool contains. A nonblocking sender resumes at the
 * same offset when ATT buffers become available and serializes whole messages. */
#define BLE_TX_QUEUE_BYTES 12288
#define BLE_TX_MESSAGE_BYTES 8192

struct ble_tx_message {
    void *fifo_reserved;
    struct bt_conn *connection;
    uint32_t generation;
    uint16_t length;
    uint8_t data[];
};

static K_FIFO_DEFINE(ble_tx_queue);
static size_t ble_tx_bytes;

static struct ble_tx_message *ble_tx_active;
static size_t ble_tx_offset;
static int64_t ble_tx_deadline;
static void ble_tx_work_fn(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(ble_tx_work, ble_tx_work_fn);

static void ble_tx_release(struct ble_tx_message *message)
{
    bt_conn_unref(message->connection);
    k_spinlock_key_t key = k_spin_lock(&ble_tx_lock);
    ble_tx_bytes -= sizeof(*message) + message->length;
    k_spin_unlock(&ble_tx_lock, key);
    k_free(message);
}

static void ble_tx_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    while (true) {
        if (!ble_tx_active) {
            ble_tx_active = k_fifo_get(&ble_tx_queue, K_NO_WAIT);
            if (!ble_tx_active) {
                return;
            }
            ble_tx_offset = 0;
            ble_tx_deadline = k_uptime_get() + 30000;
        }
        struct ble_tx_message *message = ble_tx_active;
        k_spinlock_key_t key = k_spin_lock(&ble_tx_lock);
        bool current = !ble_tx_failed && message->connection == current_conn && message->generation == ble_tx_generation;
        k_spin_unlock(&ble_tx_lock, key);
        uint16_t mtu = bt_gatt_get_mtu(message->connection);
        if (current && mtu > 3 && k_uptime_get() < ble_tx_deadline) {
            uint16_t chunk = MIN(message->length - ble_tx_offset, mtu - 3);
            int error = ble_transport_send(message->connection, message->data + ble_tx_offset, chunk);
            if (error == -ENOMEM || error == -ENOBUFS || error == -EAGAIN) {
                /* Waiting here would deadlock the system work queue. A rejected
                 * notification has sent no bytes; retry exactly this fragment. */
                k_work_reschedule(&ble_tx_work, K_MSEC(5));
                return;
            }
            if (error == 0) {
                ble_tx_offset += chunk;
                if (ble_tx_offset < message->length) {
                    continue;
                }
            }
        }
        if (current && ble_tx_offset > 0 && ble_tx_offset < message->length) {
            /* A terminal partial frame must not consume the next JSON message.
             * Fence this connection's queue and let the peer reconnect/resync. */
            key = k_spin_lock(&ble_tx_lock);
            if (message->connection == current_conn && message->generation == ble_tx_generation) {
                ble_tx_failed = true;
                ble_tx_generation++;
            }
            k_spin_unlock(&ble_tx_lock, key);
            bt_conn_disconnect(message->connection, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        }
        ble_tx_active = NULL;
        ble_tx_release(message);
    }
}

int ble_comm_send(const uint8_t *data, uint16_t len)
{
    if (!data || !len || len > BLE_TX_MESSAGE_BYTES || k_is_in_isr()) {
        return -EINVAL;
    }
    size_t size = sizeof(struct ble_tx_message) + len;
    k_spinlock_key_t key = k_spin_lock(&ble_tx_lock);
    if (!current_conn || ble_tx_failed || ble_tx_bytes + size > BLE_TX_QUEUE_BYTES) {
        int error = current_conn && !ble_tx_failed ? -ENOBUFS : -ENOTCONN;
        k_spin_unlock(&ble_tx_lock, key);
        return error;
    }
    struct bt_conn *connection = bt_conn_ref(current_conn);
    uint32_t generation = ble_tx_generation;
    ble_tx_bytes += size;
    k_spin_unlock(&ble_tx_lock, key);
    struct ble_tx_message *message = k_malloc(size);
    if (!message) {
        bt_conn_unref(connection);
        key = k_spin_lock(&ble_tx_lock);
        ble_tx_bytes -= size;
        k_spin_unlock(&ble_tx_lock, key);
        return -ENOMEM;
    }
    message->connection = connection;
    message->generation = generation;
    message->length = len;
    memcpy(message->data, data, len);
    k_fifo_put(&ble_tx_queue, message);
    k_work_schedule(&ble_tx_work, K_NO_WAIT);
    return 0;
}

void ble_comm_set_pairable(bool pairable)
{
    if (pairable) {
        LOG_WRN("Enable Pairable");
        auth_cb_display.pairing_confirm = pairing_accept;
        bt_conn_auth_cb_register(&auth_cb_display);
    } else {
        LOG_WRN("Disable Pairable");
        auth_cb_display.pairing_confirm = pairing_deny;
        bt_conn_auth_cb_register(&auth_cb_display);
    }
    pairing_enabled = pairable;
}

int ble_comm_set_short_connection_interval(void)
{
    int err;
    struct bt_le_conn_param param = {
        .interval_min = CONFIG_BT_PERIPHERAL_PREF_MIN_INT,
        .interval_max = CONFIG_BT_PERIPHERAL_PREF_MAX_INT,
        .latency = CONFIG_BT_PERIPHERAL_PREF_LATENCY,
        .timeout = CONFIG_BT_PERIPHERAL_PREF_TIMEOUT,
    };

    // If someone explicitly requested short connection interval,
    // don't change it back.
    k_work_cancel_delayable(&conn_interval_slow_work);

    // Zephyr's SMP transport owns the faster 7.5-11.25 ms request while
    // enabled. Do not overwrite it with the ordinary 30-50 ms preference.
    if (IS_ENABLED(CONFIG_MCUMGR_TRANSPORT_BT_CONN_PARAM_CONTROL) && zsw_smp_manager_is_enabled()) {
        return 0;
    }

    if (current_conn == NULL) {
        // Need context switch as this function can get called before
        // the connection is propogated to this file.
        // For example in CCC notify callbacks, triggered by a connect.
        k_work_schedule(&conn_interval_fast_work, K_MSEC(1));
        return 0;
    }

    LOG_DBG("Set short conection interval");

    err = bt_conn_le_param_update(current_conn, &param);
    if (err && err != -EALREADY) {
        LOG_ERR("bt_conn_le_param_update failed: %d", err);
    }

    return err;
}

int ble_comm_set_default_connection_interval(void)
{
    // SMP can stay registered across the service-change reconnect. Its own
    // parameter controller assumes its last request remains in force, so a
    // later idle/sensor callback must not replace it with 400-500 ms.
    if (IS_ENABLED(CONFIG_MCUMGR_TRANSPORT_BT_CONN_PARAM_CONTROL) && zsw_smp_manager_is_enabled()) {
        k_work_cancel_delayable(&conn_interval_slow_work);
        return 0;
    }
    if (current_conn == NULL) {
        return -ENOTCONN;
    }

    int err;
    struct bt_le_conn_param param = {
        .interval_min = BLE_COMM_LONG_INT_MIN_MS,
        .interval_max = BLE_COMM_LONG_INT_MAX_MS,
        .latency = CONFIG_BT_PERIPHERAL_PREF_LATENCY,
        .timeout = 500,
    };

    LOG_DBG("Set long conection interval");

    err = bt_conn_le_param_update(current_conn, &param);
    if (err && err != -EALREADY) {
        LOG_ERR("bt_conn_le_param_update failed %d", err);
    }

    return err;
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

static void update_conn_interval_slow_handler(struct k_work *item)
{
    LOG_DBG("Change to long connection interval");
    ble_comm_set_default_connection_interval();
}

static void update_conn_interval_short_handler(struct k_work *item)
{
    LOG_DBG("Change to fast connection interval");
    if (current_conn != NULL) {
        ble_comm_set_short_connection_interval();
    }
}

static void mtu_exchange_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
    if (!err) {
        LOG_INF("MTU exchange done. %d", bt_gatt_get_mtu(current_conn) - 3);

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
    k_spinlock_key_t key = k_spin_lock(&ble_tx_lock);
    current_conn = bt_conn_ref(conn);
    ble_tx_failed = false;
    ble_tx_generation++;
    k_spin_unlock(&ble_tx_lock, key);

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("Connected %s", addr);
    request_mtu_exchange();
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);
    LOG_INF("Interval: %d, latency: %d, timeout: %d", info.le.interval, info.le.latency, info.le.timeout);

    // Right after a new connection we want short connection interval
    // to let the peer discover services etc. quickly.
    // After some time assume the peer is done and change to longer intervals
    // to save power.
    if (!IS_ENABLED(CONFIG_MCUMGR_TRANSPORT_BT_CONN_PARAM_CONTROL) || !zsw_smp_manager_is_enabled()) {
        k_work_schedule(&conn_interval_slow_work, K_MSEC(BLE_COMM_CONN_INT_UPDATE_TIMEOUT_MS));
    }

    if (pairing_enabled) {
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
    }

    k_spinlock_key_t key = k_spin_lock(&ble_tx_lock);
    struct bt_conn *previous = current_conn == conn ? current_conn : NULL;
    if (previous) {
        current_conn = NULL;
        ble_tx_generation++;
    }
    k_spin_unlock(&ble_tx_lock, key);
    if (previous) {
        k_work_cancel_delayable(&conn_interval_slow_work);
        k_work_cancel_delayable(&conn_interval_fast_work);
        bt_conn_unref(previous);
    }

    ble_chronos_state(false);
}

static void ble_recycled(void)
{
    int err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), ad_nus, ARRAY_SIZE(ad_nus));
    if (err) {
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
