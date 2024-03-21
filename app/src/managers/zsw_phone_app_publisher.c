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

#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/services/bas.h>

#include "ble/ble_comm.h"
#include "battery/battery.h"
#include "events/chg_event.h"
#include "events/battery_event.h"
#include "managers/zsw_battery_manager.h"

LOG_MODULE_REGISTER(zsw_phone_app_publisher, LOG_LEVEL_DBG);

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);
static void zbus_chg_state_data_callback(const struct zbus_channel *chan);

static void handle_delayed_send_status(struct k_work *item);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_LISTENER_DEFINE(zsw_phone_app_publisher_battery_event, zbus_battery_sample_data_callback);

ZBUS_CHAN_DECLARE(chg_state_data_chan);
ZBUS_LISTENER_DEFINE(zsw_phone_app_publisher_chg_event, zbus_chg_state_data_callback);

K_WORK_DELAYABLE_DEFINE(delayed_send_status_work, handle_delayed_send_status);

static void connected(struct bt_conn *conn, uint8_t err);

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
};

static void send_battery_state_update(int mV, int percent, bool is_charging)
{
    int msg_len;
    char buf[100];

    memset(buf, 0, sizeof(buf));
    msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"status\", \"bat\": %d, \"volt\": %d, \"chg\": %d} \n", percent,
                       mV, is_charging);
    ble_comm_send(buf, msg_len);
}

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan)
{
    struct battery_sample_event *event = zbus_chan_msg(chan);
    bt_bas_set_battery_level(event->percent);
    send_battery_state_update(event->mV, event->percent, false);
}

static void zbus_chg_state_data_callback(const struct zbus_channel *chan)
{
    struct chg_state_event *event = zbus_chan_msg(chan);
    int rc;
    int batt_mv;
    int batt_percent;

    /*
    rc = zsw_battery_manager_sample_battery(&batt_mv, &batt_percent);
    if (rc == 0) {
        send_battery_state_update(batt_mv, batt_percent, event->is_charging);
    }
    */
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    // Due to MTU not yet exchanged when this callback is called
    // we need to wait a bit for it to finish.
    // TODO this should be handled somewhere else and only notify
    // after MTU eschange is finished.
    k_work_reschedule(&delayed_send_status_work, K_SECONDS(5));
}

static void handle_delayed_send_status(struct k_work *item)
{
    int rc;
    int batt_mv;
    int batt_percent;

    // When new connection, send latest data to phone.
    rc = zsw_battery_manager_sample_battery(&batt_mv, &batt_percent);
    if (rc == 0) {
        send_battery_state_update(batt_mv, batt_percent, false);
    }
}

static int zsw_phone_app_publisher_init(void)
{
    return 0;
}

SYS_INIT(zsw_phone_app_publisher_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);