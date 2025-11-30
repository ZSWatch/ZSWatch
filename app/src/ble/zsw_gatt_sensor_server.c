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

#include "sensor_fusion/zsw_sensor_fusion.h"
#include "sensors/zsw_imu.h"
#include "sensors/zsw_light_sensor.h"
#include "sensors/zsw_magnetometer.h"
#include "sensors/zsw_environment_sensor.h"

LOG_MODULE_REGISTER(zsw_gatt_sensor_server, CONFIG_ZSW_BLE_LOG_LEVEL);

static ssize_t on_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);
static void on_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value);
static void disconnected(struct bt_conn *conn, uint8_t reason);

static void zbus_periodic_fast_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(periodic_event_100ms_chan);
ZBUS_LISTENER_DEFINE(azsw_gatt_sensor_server_lis, zbus_periodic_fast_callback);

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .disconnected = disconnected,
};

// Number of 100ms periods to skip for sending data.
// 1 = 100ms, 5 = 500ms, 10 = 1s etc.
#define ZSW_GATT_SENSOR_NOTIFY_INTERVAL_PERIODS    2

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

BT_GATT_SERVICE_DEFINE(sensor_fusion_service,
                       BT_GATT_PRIMARY_SERVICE(ADAFRUIT_SERVICE_3D),
                       BT_GATT_CHARACTERISTIC(ADAFRUIT_CHAR_3D,
                                              BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_READ,
                                              ZSW_GATT_READ_WRITE_PERM,
                                              on_read, NULL, NULL),
                       BT_GATT_CCC(on_ccc_cfg_changed, ZSW_GATT_READ_WRITE_PERM)
                      );
BT_GATT_SERVICE_DEFINE(light_service,
                       BT_GATT_PRIMARY_SERVICE(ADAFRUIT_SERVICE_LIGHT),
                       BT_GATT_CHARACTERISTIC(ADAFRUIT_CHAR_LIGHT,
                                              BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_READ,
                                              ZSW_GATT_READ_WRITE_PERM,
                                              on_read, NULL, NULL),
                       BT_GATT_CCC(on_ccc_cfg_changed, ZSW_GATT_READ_WRITE_PERM)
                      );

static bool notif_enabled;
static uint8_t notify_period_counter;

static const struct bt_gatt_attr *const notify_attrs[] = {
    &temp_service.attrs[2],
    &accel_service.attrs[2],
    &humidity_service.attrs[2],
    &pressure_service.attrs[2],
    &mag_service.attrs[2],
    &gyro_service.attrs[2],
    &sensor_fusion_service.attrs[2],
    &light_service.attrs[2],
};

struct notif_scan_ctx {
    bool enabled;
};

static void notif_conn_scan(struct bt_conn *conn, void *data)
{
    struct notif_scan_ctx *state = data;
    struct bt_conn_info info;

    if (state->enabled || bt_conn_get_info(conn, &info) != 0 ||
        info.state != BT_CONN_STATE_CONNECTED) {
        return;
    }

    for (size_t i = 0; i < sizeof(notify_attrs) / sizeof(notify_attrs[0]); i++) {
        if (bt_gatt_is_subscribed(conn, notify_attrs[i], BT_GATT_CCC_NOTIFY)) {
            state->enabled = true;
            return;
        }
    }
}

static bool any_notification_enabled(uint16_t value)
{
    if ((value & BT_GATT_CCC_NOTIFY) != 0U) {
        return true;
    }

    struct notif_scan_ctx ctx = { 0 };

    bt_conn_foreach(BT_CONN_TYPE_LE, notif_conn_scan, &ctx);

    return ctx.enabled;
}

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
    } else if (bt_gatt_attr_get_handle(attr) == bt_gatt_attr_get_handle(&sensor_fusion_service.attrs[2])) {
        zsw_quat_t quat;
        if (zsw_sensor_fusion_get_quaternion(&quat) == 0) {
            f_ptr[0] = quat.w;
            f_ptr[1] = quat.x;
            f_ptr[2] = quat.y;
            f_ptr[3] = quat.z;
            write_len = 4 * sizeof(float);
        }
    } else if (bt_gatt_attr_get_handle(attr) == bt_gatt_attr_get_handle(&light_service.attrs[2])) {
        zsw_light_sensor_get_light(&f_ptr[0]);
        write_len = sizeof(float);
    }

    return write_len;
}

static void on_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    bool notifications_active = any_notification_enabled(value);

    if (!notif_enabled && notifications_active) {
        notif_enabled = true;
        zsw_imu_feature_enable(ZSW_IMU_FEATURE_GYRO, false);
        if (zsw_sensor_fusion_init() != 0) {
            LOG_ERR("Failed to start sensor fusion for BLE notifications");
        }
        ble_comm_set_short_connection_interval();
        zsw_periodic_chan_add_obs(&periodic_event_100ms_chan, &azsw_gatt_sensor_server_lis);
    } else if (notif_enabled && !notifications_active) {
        ble_comm_set_default_connection_interval();
        zsw_periodic_chan_rm_obs(&periodic_event_100ms_chan, &azsw_gatt_sensor_server_lis);
        zsw_imu_feature_disable(ZSW_IMU_FEATURE_GYRO);
        zsw_sensor_fusion_deinit();
        notif_enabled = false;
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    ARG_UNUSED(conn);
    ARG_UNUSED(reason);

    if (!notif_enabled || any_notification_enabled(0)) {
        return;
    }

    ble_comm_set_default_connection_interval();
    zsw_periodic_chan_rm_obs(&periodic_event_100ms_chan, &azsw_gatt_sensor_server_lis);
    zsw_imu_feature_disable(ZSW_IMU_FEATURE_GYRO);
    zsw_sensor_fusion_deinit();
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
    zsw_quat_t quat;
    uint8_t buf[CONFIG_BT_L2CAP_TX_MTU];

    notify_period_counter++;
    if (notify_period_counter < ZSW_GATT_SENSOR_NOTIFY_INTERVAL_PERIODS) {
        return;
    }
    notify_period_counter = 0;

    f_ptr = (float *)buf;

    zsw_environment_sensor_get(&temperature, &humidity, &pressure);
    f_ptr[0] = temperature;
    write_len = sizeof(float);
    bt_gatt_notify(NULL, &temp_service.attrs[2], &buf, write_len);

    f_ptr[0] = humidity;
    write_len = sizeof(float);
    bt_gatt_notify(NULL, &humidity_service.attrs[2], &buf, write_len);

    f_ptr[0] = pressure;
    write_len = sizeof(float);
    bt_gatt_notify(NULL, &pressure_service.attrs[2], &buf, write_len);

    if (zsw_imu_fetch_accel(&x, &y, &z) == 0) {
        f_ptr[0] = x;
        f_ptr[1] = y;
        f_ptr[2] = z;
        write_len = 3 * sizeof(float);
        bt_gatt_notify(NULL, &accel_service.attrs[2], &buf, write_len);
    }

    if (zsw_imu_fetch_gyro(&x, &y, &z) == 0) {
        f_ptr[0] = x;
        f_ptr[1] = y;
        f_ptr[2] = z;
        write_len = 3 * sizeof(float);
        bt_gatt_notify(NULL, &gyro_service.attrs[2], &buf, write_len);
    }

    if (zsw_magnetometer_set_enable(true) == 0) {
        zsw_magnetometer_get_all(&f_ptr[0], &f_ptr[1], &f_ptr[2]);
        zsw_magnetometer_set_enable(false);
        write_len = 3 * sizeof(float);
        bt_gatt_notify(NULL, &mag_service.attrs[2], &buf, write_len);
    }

    if (zsw_light_sensor_get_light(&f_ptr[0]) == 0) {
        write_len = sizeof(float);
        bt_gatt_notify(NULL, &light_service.attrs[2], &buf, write_len);
    }

    if (zsw_sensor_fusion_get_quaternion(&quat) == 0) {
        f_ptr[0] = quat.w;
        f_ptr[1] = quat.x;
        f_ptr[2] = quat.y;
        f_ptr[3] = quat.z;
        write_len = 4 * sizeof(float);
        bt_gatt_notify(NULL, &sensor_fusion_service.attrs[2], &buf, write_len);
    }
}
