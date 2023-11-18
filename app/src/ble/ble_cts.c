/**
 * @file ble_cts.c
 * @author Leonardo Bispo
 *
 * @brief Implements Current Time Service (CTS) client.
 *
 * @see https://www.bluetooth.com/specifications/specs/current-time-service-1-1/
 *
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/timeutil.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/zbus/zbus.h>

#include <bluetooth/gatt_dm.h>
#include <bluetooth/services/cts_client.h>

#include "ble/ble_comm.h"
#include "ble/ble_cts.h"
#include "events/ble_data_event.h"

LOG_MODULE_REGISTER(ble_cts, LOG_LEVEL_INF);
ZBUS_CHAN_DECLARE(ble_comm_data_chan);

static struct bt_cts_client cts_c;

static bool has_cts;

static const char *day_of_week[] = { "Unknown",   "Monday",   "Tuesday",
                                     "Wednesday", "Thursday", "Friday",
                                     "Saturday",  "Sunday"
                                   };

static const char *month_of_year[] = { "Unknown",   "January", "February",
                                       "March",     "April",   "May",
                                       "June",      "July",    "August",
                                       "September", "October", "November",
                                       "December"
                                     };

static void read_current_time_cb(struct bt_cts_client *cts_c, struct bt_cts_current_time *current_time, int err);

static void current_time_print(struct bt_cts_current_time *current_time)
{
    LOG_DBG("\tDay of week   %s", day_of_week[current_time->exact_time_256.day_of_week]);

    if (current_time->exact_time_256.day == 0) {
        LOG_DBG("\tDay of month  Unknown");
    } else {
        LOG_DBG("\tDay of month  %u", current_time->exact_time_256.day);
    }

    LOG_DBG("\tMonth of year %s", month_of_year[current_time->exact_time_256.month]);

    if (current_time->exact_time_256.year == 0) {
        LOG_DBG("\tYear          Unknown");
    } else {
        LOG_DBG("\tYear          %u", current_time->exact_time_256.year);
    }
    LOG_DBG("\tTime:");
    LOG_DBG("\tHours     %u", current_time->exact_time_256.hours);
    LOG_DBG("\tMinutes   %u", current_time->exact_time_256.minutes);
    LOG_DBG("\tSeconds   %u", current_time->exact_time_256.seconds);

    LOG_DBG("\tAdjust reason:");
    LOG_DBG("\tDaylight savings %x", current_time->adjust_reason.change_of_daylight_savings_time);
    LOG_DBG("\tTime zone        %x", current_time->adjust_reason.change_of_time_zone);
    LOG_DBG("\tExternal update  %x", current_time->adjust_reason.external_reference_time_update);
    LOG_DBG("\tManual update    %x", current_time->adjust_reason.manual_time_update);
}

void publish_time_event(struct bt_cts_current_time *current_time)
{
    struct ble_data_event evt_time_inf = { 0 };

    struct tm tm = {
        .tm_sec = current_time->exact_time_256.seconds,
        .tm_min = current_time->exact_time_256.minutes,
        .tm_hour = current_time->exact_time_256.hours,
        .tm_mday = current_time->exact_time_256.day,
        .tm_mon = current_time->exact_time_256.month - 1,
        .tm_year = current_time->exact_time_256.year - 1900,
    };

    evt_time_inf.data.type = BLE_COMM_DATA_TYPE_SET_TIME;
    evt_time_inf.data.data.time.seconds = timeutil_timegm(&tm);

    LOG_DBG("EPOCH %d", evt_time_inf.data.data.time.seconds);

    zbus_chan_pub(&ble_comm_data_chan, &evt_time_inf, K_MSEC(250));
}

static void notify_current_time_cb(struct bt_cts_client *cts_c,
                                   struct bt_cts_current_time *current_time)
{
    current_time_print(current_time);

    publish_time_event(current_time);
}

static void enable_notifications(void)
{
    LOG_DBG("Enable CTS notifications");

    if (has_cts && (bt_conn_get_security(cts_c.conn) >= BT_SECURITY_L2)) {
        int err = bt_cts_subscribe_current_time(&cts_c, notify_current_time_cb);
        if (err) {
            LOG_ERR("Cannot subscribe to current time value notification (err %d)", err);
        }
    }
}

static void discover_completed_cb(struct bt_gatt_dm *dm, void *ctx)
{
    LOG_DBG("The discovery procedure succeeded");

    bt_gatt_dm_data_print(dm);

    int err = bt_cts_handles_assign(dm, &cts_c);
    if (err) {
        LOG_ERR("Could not assign CTS client handles, error: %d", err);
    } else {
        has_cts = true;

        if (bt_conn_get_security(cts_c.conn) >= BT_SECURITY_L2) {
            enable_notifications();

            err = bt_cts_read_current_time(&cts_c, read_current_time_cb);
            if (err) {
                LOG_ERR("Failed reading current time (err: %d)", err);
            }
        }
    }

    err = bt_gatt_dm_data_release(dm);
    if (err) {
        LOG_ERR("Could not release the discovery data, error code: %d", err);
    }
}

static void discover_service_not_found_cb(struct bt_conn *conn, void *ctx)
{
    LOG_WRN("The service could not be found during the discovery");
}

static void discover_error_found_cb(struct bt_conn *conn, int err, void *ctx)
{
    LOG_ERR("The discovery procedure failed, err %d", err);
}

static const struct bt_gatt_dm_cb discover_cb = {
    .completed = discover_completed_cb,
    .service_not_found = discover_service_not_found_cb,
    .error_found = discover_error_found_cb,
};

static void read_current_time_cb(struct bt_cts_client *cts_c,
                                 struct bt_cts_current_time *current_time,
                                 int err)
{
    if (err) {
        LOG_ERR("Cannot read Current Time: error: %d", err);
        return;
    }

    current_time_print(current_time);

    publish_time_event(current_time);
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
    LOG_DBG("Pairing completed, get time");

    int err = bt_cts_read_current_time(&cts_c, read_current_time_cb);
    if (err) {
        LOG_ERR("Failed reading current time (err: %d)", err);
    }
}

static struct bt_conn_auth_info_cb conn_auth_info_callbacks = {
    .pairing_complete = pairing_complete,
};

static void security_changed(struct bt_conn *conn, bt_security_t level,
                             enum bt_security_err err)
{
    if (!err) {
        enable_notifications();
    } else {
        LOG_ERR("Security failed: level %u err %d", level, err);
    }
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Connection failed (err 0x%02x)", err);
        return;
    }

    has_cts = false;

    err = bt_gatt_dm_start(conn, BT_UUID_CTS, &discover_cb, NULL);
    if (err) {
        LOG_ERR("Failed to start discovery (err %d)", err);
    }
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .security_changed = security_changed,
};

void ble_cts_init(void)
{
    int err = bt_cts_client_init(&cts_c);
    if (err) {
        LOG_ERR("CTS client init failed (err %d)", err);
        return;
    }

    err = bt_conn_auth_info_cb_register(&conn_auth_info_callbacks);
    if (err) {
        LOG_ERR("Failed to register authorization info callbacks.");
        return;
    }

    LOG_INF("Starting Current Time client");
}