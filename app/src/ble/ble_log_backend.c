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

#include <stdint.h>
#include <string.h>

#include <zephyr/logging/log.h>
#include <zephyr/logging/log_backend.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/logging/log_output.h>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>

#include "ble/ble_comm.h"
#include "ble/ble_log_backend.h"

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/atomic.h>

#define BLE_LOG_BACKEND_BUF_SIZE 256
#define BLE_LOG_PREFIX "<BLELOG>"
#define BLE_LOG_SUFFIX "</BLELOG>"

// Wait this long after connection before sending logs
#define BLE_LOG_CONN_DELAY_MS 3000

static uint8_t output_buf[BLE_LOG_BACKEND_BUF_SIZE];
static bool panic_mode;
static uint32_t log_format_current = LOG_OUTPUT_TEXT;
static bool first_enable;
static bool backend_active;
static int64_t ble_conn_time_ms;
static atomic_t ble_connected = ATOMIC_INIT(0);

static void ble_log_backend_connected(struct bt_conn *conn, uint8_t err)
{
    if (err == 0) {
        ble_conn_time_ms = k_uptime_get();
        atomic_set(&ble_connected, 1);
    }
}

static void ble_log_backend_disconnected(struct bt_conn *conn, uint8_t reason)
{
    atomic_set(&ble_connected, 0);
}

BT_CONN_CB_DEFINE(ble_log_backend_conn_cb) = {
    .connected = ble_log_backend_connected,
    .disconnected = ble_log_backend_disconnected,
};

static const struct log_backend log_backend_ble_comm;

static int line_out(uint8_t *data, size_t length, void *ctx)
{
    ARG_UNUSED(ctx);

    if (!atomic_get(&ble_connected)) {
        // Not connected, pretend to send all
        return length;
    }
    int64_t now = k_uptime_get();
    if ((now - ble_conn_time_ms) < BLE_LOG_CONN_DELAY_MS) {
        // Too soon after connect, pretend to send all
        return length;
    }

    const size_t capped_len = MIN(length, (size_t)UINT16_MAX);

    ble_comm_send((const uint8_t *)BLE_LOG_PREFIX, strlen(BLE_LOG_PREFIX));
    ble_comm_send(data, (uint16_t)capped_len);
    ble_comm_send((const uint8_t *)BLE_LOG_SUFFIX, strlen(BLE_LOG_SUFFIX));

    return length;
}


LOG_OUTPUT_DEFINE(log_output_ble_comm, line_out, output_buf, sizeof(output_buf));

static void process(const struct log_backend *const backend, union log_msg_generic *msg)
{
    ARG_UNUSED(backend);

    if (panic_mode) {
        return;
    }

    uint32_t flags = LOG_OUTPUT_FLAG_LEVEL | LOG_OUTPUT_FLAG_TIMESTAMP;
    log_format_func_t log_output_func = log_format_func_t_get(log_format_current);

    log_output_func(&log_output_ble_comm, &msg->log, flags);
}

static int format_set(const struct log_backend *const backend, uint32_t log_type)
{
    ARG_UNUSED(backend);

    log_format_current = log_type;
    return 0;
}

static void init_backend(struct log_backend const *const backend)
{
    ARG_UNUSED(backend);

    log_backend_deactivate(&log_backend_ble_comm);
}

static void panic(struct log_backend const *const backend)
{
    ARG_UNUSED(backend);

    panic_mode = true;
}

static int backend_ready(const struct log_backend *const backend)
{
    ARG_UNUSED(backend);

    return -EACCES;
}

static const struct log_backend_api ble_log_backend_api = {
    .process = process,
    .dropped = NULL,
    .panic = panic,
    .init = init_backend,
    .is_ready = backend_ready,
    .format_set = format_set,
    .notify = NULL,
};

LOG_BACKEND_DEFINE(log_backend_ble_comm, ble_log_backend_api, false);

void ble_log_backend_set_enabled(bool enable)
{
    if (enable == backend_active) {
        return;
    }

    if (enable) {
        if (!first_enable) {
            log_backend_enable(&log_backend_ble_comm, NULL, CONFIG_LOG_MAX_LEVEL);
            first_enable = true;
        } else {
            log_backend_activate(&log_backend_ble_comm, NULL);
        }
        backend_active = true;
    } else {
        if (first_enable) {
            log_backend_deactivate(&log_backend_ble_comm);
        }
        backend_active = false;
    }
}
