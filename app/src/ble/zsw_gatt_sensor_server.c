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

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/sensor.h>

#include "events/periodic_event.h"
#include "events/zsw_periodic_event.h"

#include "ble/ble_comm.h"
#include <ble/zsw_gatt_sensor_server.h>

#include "sensors/zsw_imu.h"
#include "sensors/zsw_magnetometer.h"
#include "sensors/zsw_environment_sensor.h"

LOG_MODULE_REGISTER(zsw_gatt_sensor_server, CONFIG_ZSW_BLE_LOG_LEVEL);

static ssize_t on_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);
static void on_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value);
static void disconnected(struct bt_conn *conn, uint8_t reason);

static void zbus_periodic_fast_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(periodic_event_fast_chan);
ZBUS_LISTENER_DEFINE(azsw_gatt_sensor_server_lis, zbus_periodic_fast_callback);

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .disconnected = disconnected,
};

#if CONFIG_BLE_DISABLE_PAIRING_REQUIRED
#define ZSW_GATT_READ_WRITE_PERM    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE
#else
#define ZSW_GATT_READ_WRITE_PERM    BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT
#endif

BT_GATT_SERVICE_DEFINE(temp_service,
                       BT_GATT_PRIMARY_SERVICE(ADAFRUIT_SERVICE_TEMPERATURE),
                       BT_GATT_CHARACTERISTIC(ADAFRUIT_CHAR_TEMPERATURE,
                                              BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_READ,
                                              ZSW_GATT_READ_WRITE_PERM,
                                              on_read, NULL, NULL),
                       BT_GATT_CCC(on_ccc_cfg_changed, ZSW_GATT_READ_WRITE_PERM)
                      );

BT_GATT_SERVICE_DEFINE(accel_service,
                       BT_GATT_PRIMARY_SERVICE(ADAFRUIT_SERVICE_ACCEL),
                       BT_GATT_CHARACTERISTIC(ADAFRUIT_CHAR_ACCEL,
                                              BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_READ,
                                              ZSW_GATT_READ_WRITE_PERM,
                                              on_read, NULL, NULL),
                       BT_GATT_CCC(on_ccc_cfg_changed, ZSW_GATT_READ_WRITE_PERM)
                      );

BT_GATT_SERVICE_DEFINE(humidity_service,
                       BT_GATT_PRIMARY_SERVICE(ADAFRUIT_SERVICE_HUMIDITY),
                       BT_GATT_CHARACTERISTIC(ADAFRUIT_CHAR_HUMIDITY,
                                              BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_READ,
                                              ZSW_GATT_READ_WRITE_PERM,
                                              on_read, NULL, NULL),
                       BT_GATT_CCC(on_ccc_cfg_changed, ZSW_GATT_READ_WRITE_PERM)
                      );

BT_GATT_SERVICE_DEFINE(pressure_service,
                       BT_GATT_PRIMARY_SERVICE(ADAFRUIT_SERVICE_PRESSURE),
                       BT_GATT_CHARACTERISTIC(ADAFRUIT_CHAR_PRESSURE,
                                              BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_READ,
                                              ZSW_GATT_READ_WRITE_PERM,
                                              on_read, NULL, NULL),
                       BT_GATT_CCC(on_ccc_cfg_changed, ZSW_GATT_READ_WRITE_PERM)
                      );

BT_GATT_SERVICE_DEFINE(mag_service,
                       BT_GATT_PRIMARY_SERVICE(ADAFRUIT_SERVICE_MAG),
                       BT_GATT_CHARACTERISTIC(ADAFRUIT_CHAR_MAG,
                                              BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_READ,
                                              ZSW_GATT_READ_WRITE_PERM,
                                              on_read, NULL, NULL),
                       BT_GATT_CCC(on_ccc_cfg_changed, ZSW_GATT_READ_WRITE_PERM)
                      );

BT_GATT_SERVICE_DEFINE(gyro_service,
                       BT_GATT_PRIMARY_SERVICE(ADAFRUIT_SERVICE_GYRO),
                       BT_GATT_CHARACTERISTIC(ADAFRUIT_CHAR_GYRO,
                                              BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_READ,
                                              ZSW_GATT_READ_WRITE_PERM,
                                              on_read, NULL, NULL),
                       BT_GATT_CCC(on_ccc_cfg_changed, ZSW_GATT_READ_WRITE_PERM)
                      );

static bool notif_enabled;

static ssize_t on_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    int16_t x;
    int16_t y;
    int16_t z;
    int write_len;
    float temperature = 0.0;
    float pressure = 0.0;
    float humidity = 0.0;
    float *f_ptr;

    f_ptr = (float *)buf;
    write_len = 0;

    zsw_environment_sensor_get(&temperature, &humidity, &pressure);

    if (bt_gatt_attr_get_handle(attr) == bt_gatt_attr_get_handle(&temp_service.attrs[2])) {
        f_ptr[0] = temperature;
        write_len = sizeof(float);
    } else if (bt_gatt_attr_get_handle(attr) == bt_gatt_attr_get_handle(&accel_service.attrs[2])) {
        zsw_imu_fetch_accel(&x, &y, &z);
        f_ptr[0] = x;
        f_ptr[1] = y;
        f_ptr[2] = z;
        write_len = 3 * sizeof(float);
    } else if (bt_gatt_attr_get_handle(attr) == bt_gatt_attr_get_handle(&humidity_service.attrs[2])) {
        f_ptr[0] = humidity;
        write_len = sizeof(float);
    } else if (bt_gatt_attr_get_handle(attr) == bt_gatt_attr_get_handle(&pressure_service.attrs[2])) {
        f_ptr[0] = pressure;
        write_len = sizeof(float);
    } else if (bt_gatt_attr_get_handle(attr) == bt_gatt_attr_get_handle(&mag_service.attrs[2])) {
        zsw_magnetometer_set_enable(true);
        zsw_magnetometer_get_all(&f_ptr[0], &f_ptr[1], &f_ptr[2]);
        zsw_magnetometer_set_enable(false);
        write_len = 3 * sizeof(float);
    } else if (bt_gatt_attr_get_handle(attr) == bt_gatt_attr_get_handle(&gyro_service.attrs[2])) {
        zsw_imu_fetch_gyro(&x, &y, &z);
        f_ptr[0] = x;
        f_ptr[1] = y;
        f_ptr[2] = z;
        write_len = 3 * sizeof(float);
    }

    return write_len;
}

static void on_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    // TODO handle notifications per connection per service.
    if (!notif_enabled && (value == BT_GATT_CCC_NOTIFY)) {
        notif_enabled = true;
        zsw_imu_feature_enable(ZSW_IMU_FEATURE_GYRO, false);
        ble_comm_short_connection_interval();
        zsw_periodic_chan_add_obs(&periodic_event_fast_chan, &azsw_gatt_sensor_server_lis);
    } else if (notif_enabled && (value != BT_GATT_CCC_NOTIFY)) {
        // If any char get notify off, then stop sending at all.
        // TODO Keep track of which ones have notify on.
        ble_comm_long_connection_interval();
        zsw_periodic_chan_rm_obs(&periodic_event_fast_chan, &azsw_gatt_sensor_server_lis);
        zsw_imu_feature_disable(ZSW_IMU_FEATURE_GYRO);
        notif_enabled = false;
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    // TODO Handle multiple connections
    zsw_periodic_chan_rm_obs(&periodic_event_fast_chan, &azsw_gatt_sensor_server_lis);
    notif_enabled = false;
}

static void zbus_periodic_fast_callback(const struct zbus_channel *chan)
{
    int16_t x;
    int16_t y;
    int16_t z;
    int write_len;
    float *f_ptr;
    float pressure = 0.0;
    float humidity = 0.0;
    float temperature = 0.0;
    uint8_t buf[CONFIG_BT_L2CAP_TX_MTU];

    f_ptr = (float *)buf;

    // TODO use bt_gatt_notify_multiple instead of many bt_gatt_notify
    zsw_environment_sensor_get(&temperature, &humidity, &pressure);
    f_ptr[0] = temperature;
    write_len = sizeof(float);
    bt_gatt_notify(NULL, &temp_service.attrs[1], &buf, write_len);

    f_ptr[0] = humidity;
    write_len = sizeof(float);
    bt_gatt_notify(NULL, &humidity_service.attrs[1], &buf, write_len);

    f_ptr[0] = pressure;
    write_len = sizeof(float);
    bt_gatt_notify(NULL, &pressure_service.attrs[1], &buf, write_len);

    if (zsw_imu_fetch_accel(&x, &y, &z) == 0) {
        f_ptr[0] = x;
        f_ptr[1] = y;
        f_ptr[2] = z;
        write_len = 3 * sizeof(float);
        bt_gatt_notify(NULL, &accel_service.attrs[1], &buf, write_len);
    }

    if (zsw_imu_fetch_gyro(&x, &y, &z) == 0) {
        f_ptr[0] = x;
        f_ptr[1] = y;
        f_ptr[2] = z;
        write_len = 3 * sizeof(float);
        bt_gatt_notify(NULL, &gyro_service.attrs[1], &buf, write_len);
    }

    if (zsw_magnetometer_set_enable(true) == 0) {
        zsw_magnetometer_get_all(&f_ptr[0], &f_ptr[1], &f_ptr[2]);
        zsw_magnetometer_set_enable(false);
        write_len = 3 * sizeof(float);
        bt_gatt_notify(NULL, &mag_service.attrs[1], &buf, write_len);
    }
}