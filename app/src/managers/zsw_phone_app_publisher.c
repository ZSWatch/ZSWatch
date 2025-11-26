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

#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/services/bas.h>
#if defined(CONFIG_BT_HRS)
#include <zephyr/bluetooth/services/hrs.h>
#endif

#include "ble/ble_comm.h"
#include "ble/gadgetbridge/ble_gadgetbridge.h"
#include "events/battery_event.h"
#include "managers/zsw_power_manager.h"
#include "sensors/zsw_imu.h"
#if defined(CONFIG_BT_HRS)
#include "sensors/zsw_health_data.h"
#endif

LOG_MODULE_REGISTER(zsw_phone_app_publisher, LOG_LEVEL_DBG);

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);

static void handle_delayed_send_status(struct k_work *item);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_LISTENER_DEFINE(zsw_phone_app_publisher_battery_event, zbus_battery_sample_data_callback);

K_WORK_DELAYABLE_DEFINE(delayed_send_status_work, handle_delayed_send_status);

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);

static bool is_connected;

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
    .disconnected = disconnected,
};

static void send_activity_data(void)
{
    uint32_t steps = 0;
    zsw_imu_data_step_activity_t step_activity = ZSW_IMU_EVT_STEP_ACTIVITY_UNKNOWN;

    zsw_imu_fetch_num_steps(&steps);
    zsw_imu_fetch_step_activity(&step_activity);

#if defined(CONFIG_BT_HRS)
    uint16_t hr = zsw_health_data_get_heart_rate();
    bt_hrs_notify(hr);
    ble_gadgetbridge_send_activity_data(hr, steps, step_activity,
                                        zsw_power_manager_get_state());
#else
    ble_gadgetbridge_send_activity_data(0, steps, step_activity,
                                        zsw_power_manager_get_state());
#endif
}

static void send_battery_state_update(int mV, int percent, bool is_charging)
{
    int msg_len;
    char buf[100];

    memset(buf, 0, sizeof(buf));
    msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"status\", \"bat\": %d, \"volt\": %d, \"chg\": %d} \n", percent,
                       mV, is_charging);
    ble_comm_send(buf, msg_len);

    send_activity_data();
}

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan)
{
    struct battery_sample_event *event = zbus_chan_msg(chan);
    bt_bas_set_battery_level(event->percent);
    if (is_connected) {
        send_battery_state_update(event->mV, event->percent, event->is_charging);
    }
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        return;
    }
    is_connected = true;
    // Send version info and initial status after a short delay
    // to allow phone to set up notifications
    k_work_reschedule(&delayed_send_status_work, K_MSEC(3000));
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    is_connected = false;
    k_work_cancel_delayable(&delayed_send_status_work);
}

static void handle_delayed_send_status(struct k_work *item)
{
    struct battery_sample_event last_sample;

    ble_gadgetbridge_send_version_info();

    send_activity_data();

    if (zbus_chan_read(&battery_sample_data_chan, &last_sample, K_MSEC(100)) == 0) {
        send_battery_state_update(last_sample.mV, last_sample.percent, last_sample.is_charging);
    }
}

static int zsw_phone_app_publisher_init(void)
{
    return 0;
}

SYS_INIT(zsw_phone_app_publisher_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
