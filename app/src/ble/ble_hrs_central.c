/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2024.
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

#include "ble_hrs_central.h"
#include <zephyr/logging/log.h>
#include <bluetooth/services/hrs_client.h>

LOG_MODULE_REGISTER(ble_hrs_central);

static struct bt_hrs_client hrs_c;
static struct bt_hrs_client_measurement cached_measurement;
static int64_t last_measurement_timestamp = 0LL;


int ble_hrs_central_get_measurement(uint16_t* out)
{
	int64_t time_passed_since_last_measurement = k_uptime_get() - last_measurement_timestamp;
	if (time_passed_since_last_measurement > 10*1000) {
		return -EINVAL;
	}

	if (NULL == out)
	{
		return -EINVAL;
	}

	*out = cached_measurement.hr_value;
	return 0;
}

static void hrs_measurement_notify_cb(struct bt_hrs_client *hrs_c, const struct bt_hrs_client_measurement *meas, int err)
{
	if (err) {
		return;
	}
	last_measurement_timestamp = k_uptime_get();
	memcpy((void*)&cached_measurement, (void*)meas, sizeof(struct bt_hrs_client_measurement));
}

static void on_discovery_completed(struct bt_gatt_dm *dm, void *context)
{
	int err;
    struct bt_conn* conn = (struct bt_conn*)context;

	LOG_INF("The HRS discovery procedure succeeded\n");

	bt_gatt_dm_data_print(dm);

    do
    {
        err = bt_hrs_client_init(&hrs_c);
        if (err) {
            LOG_ERR("HRS client init failed (err %d)\n", err);
            break;
        }

	    err = bt_hrs_client_handles_assign(dm, &hrs_c);
	    if (err) {
		LOG_ERR("HRS client init failed (err %d)\n", err);
		break;
	    }

	    err = bt_hrs_client_measurement_subscribe(&hrs_c, hrs_measurement_notify_cb);
	    if (err) {
			LOG_ERR("Subscription to HR measurement char failed (errr %d)", err);
            break;
	    }

	    err = bt_gatt_dm_data_release(dm);
	    if (err) {
		LOG_ERR("Failed to release the discovery data (err %d)", err);
            break;
	    }
    } while (0);

	// Unref connection in case of fail.
    if (err) {
        bt_conn_unref(conn);
    }

}

static void on_discovery_not_found(struct bt_conn *conn, void *context)
{
    bt_conn_unref(conn);
	LOG_ERR("Heart Rate Service could not be found during the discovery\n");
}

static void on_discovery_error_found(struct bt_conn *conn, int err, void *context)
{
    bt_conn_unref(conn);
	LOG_ERR("The discovery procedure failed with %d\n", err);
}

static const struct bt_gatt_dm_cb discovery_cb = {
	.completed = on_discovery_completed,
	.service_not_found = on_discovery_not_found,
	.error_found = on_discovery_error_found
};

int ble_hrs_central_start(struct bt_conn* conn)
{
    int err;

    bt_conn_ref(conn);
    err = bt_gatt_dm_start(conn, BT_UUID_HRS, &discovery_cb, (void*)conn);
    if (err) {
        LOG_ERR("Could not start discovery procedure (%d)", err);
        return -EINVAL;
    }
    return 0;
}

int ble_hrs_central_stop(void)
{
	if (NULL != hrs_c.conn) {
		bt_conn_unref(hrs_c.conn);
	}
    return 0;
}