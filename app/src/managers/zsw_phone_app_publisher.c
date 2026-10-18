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
#include "events/zsw_notification_event.h"
#include "sensors/zsw_imu.h"
#if defined(CONFIG_ZSW_HR_MANAGER)
#include "drivers/zsw_hr.h"
#endif

LOG_MODULE_REGISTER(zsw_phone_app_publisher, LOG_LEVEL_DBG);

static void zbus_send_status_data_callback(const struct zbus_channel *chan);
static void zbus_notification_remove_callback(const struct zbus_channel *chan);

static void handle_delayed_send_status(struct k_work *item);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_CHAN_DECLARE(zsw_notification_mgr_remove_chan);
ZBUS_LISTENER_DEFINE(zsw_phone_app_publisher_battery_event, zbus_send_status_data_callback);
ZBUS_LISTENER_DEFINE(zsw_phone_app_publisher_notification_remove_event, zbus_notification_remove_callback);
ZBUS_CHAN_ADD_OBS(zsw_notification_mgr_remove_chan, zsw_phone_app_publisher_notification_remove_event, 1);

K_WORK_DELAYABLE_DEFINE(delayed_send_status_work, handle_delayed_send_status);

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);

static bool is_connected;

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
    .disconnected = disconnected,
};

/*
 * Gadgetbridge stores every activity message as a sample at the time it is received.
 * A heart rate is therefore only sent once, when it is measured. The periodic messages
 * carry steps with heart rate 0, meaning no new reading.
 */
static void send_activity_data(uint16_t heart_rate)
{
    uint32_t steps = 0;
    zsw_imu_data_step_activity_t step_activity = ZSW_IMU_EVT_STEP_ACTIVITY_UNKNOWN;

    zsw_imu_fetch_num_steps(&steps);
    zsw_imu_fetch_step_activity(&step_activity);

    ble_gadgetbridge_send_activity_data(heart_rate, steps, step_activity,
                                        zsw_power_manager_get_state());
}

#if defined(CONFIG_ZSW_HR_MANAGER)
ZBUS_CHAN_DECLARE(hr_sample_data_chan);

static int64_t last_hr_notification;

static void publish_hr(struct k_work *work)
{
    ARG_UNUSED(work);
    struct zsw_hr_sample sample;

    if (zbus_chan_read(&hr_sample_data_chan, &sample, K_MSEC(100)) || !is_connected ||
        sample.timestamp_ms <= last_hr_notification) {
        return;
    }
    last_hr_notification = sample.timestamp_ms;
#if defined(CONFIG_BT_HRS)
    bt_hrs_notify(sample.heart_rate_bpm);
#endif
    send_activity_data(sample.heart_rate_bpm);
}
K_WORK_DEFINE(publish_hr_work, publish_hr);

static void hr_measurement_received(const struct zbus_channel *chan)
{
    ARG_UNUSED(chan);
    k_work_submit(&publish_hr_work);
}
ZBUS_LISTENER_DEFINE(phone_hr_listener, hr_measurement_received);
ZBUS_CHAN_ADD_OBS(hr_sample_data_chan, phone_hr_listener, 1);
#endif

static void send_status_update(int mV, int percent, bool is_charging)
{
    int msg_len;
    char buf[100];

    memset(buf, 0, sizeof(buf));
    msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"status\", \"bat\": %d, \"volt\": %d, \"chg\": %d} \n", percent,
                       mV, is_charging);
    ble_comm_send(buf, msg_len);
}

static void zbus_send_status_data_callback(const struct zbus_channel *chan)
{
    struct battery_sample_event *event = zbus_chan_msg(chan);
    bt_bas_set_battery_level(event->percent);
    if (is_connected) {
        send_status_update(event->mV, event->percent, event->is_charging);
        send_activity_data(0);
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

    send_activity_data(0);

    if (zbus_chan_read(&battery_sample_data_chan, &last_sample, K_MSEC(100)) == 0) {
        send_status_update(last_sample.mV, last_sample.percent, last_sample.is_charging);
    }
}

static void zbus_notification_remove_callback(const struct zbus_channel *chan)
{
    const struct zsw_notification_remove_event *evt = zbus_chan_const_msg(chan);

    // Only a dismissal on the watch is the user's decision. Evicting the oldest
    // notification or echoing a phone removal must not clear it on the phone.
    if (evt->notification.id != 0 && evt->reason == ZSW_NOTIFICATION_REMOVED_BY_USER) {
        ble_gadgetbridge_send_notification_action(evt->notification.id, BLE_COMM_NOTIFY_ACTION_DISMISS);
    }
}

static int zsw_phone_app_publisher_init(void)
{
    return 0;
}

SYS_INIT(zsw_phone_app_publisher_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
