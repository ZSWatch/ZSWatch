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

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include <zsw_clock.h>
#include <zsw_retained_ram_storage.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/settings/settings.h>

#include "watchface_app.h"
#include "zsw_settings.h"
#include "events/accel_event.h"
#include "events/battery_event.h"
#include "events/activity_event.h"
#include "events/ble_event.h"
#include "sensors/zsw_imu.h"
#include "sensors/zsw_environment_sensor.h"
#include "sensors/zsw_pressure_sensor.h"
#include "managers/zsw_notification_manager.h"
#include "ui/watchfaces/zsw_watchface_dropdown_ui.h"

LOG_MODULE_REGISTER(watcface_app, LOG_LEVEL_WRN);

#define MAX_WATCHFACES  15
#define NORMAL_TIME_UPDATE_INTERVAL   K_MSEC(1000)
#define SMOOTH_TIME_UPDATE_INTERVAL   K_MSEC(50)

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void zbus_accel_data_callback(const struct zbus_channel *chan);
static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);
static void zbus_activity_event_callback(const struct zbus_channel *chan);
static int settings_load_handler_watchface(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg,
                                           void *param);
static int settings_load_handler_brightness(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg,
                                            void *param);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(watchface_ble_comm_lis, zbus_ble_comm_data_callback);

ZBUS_CHAN_DECLARE(accel_data_chan);
ZBUS_LISTENER_DEFINE(watchface_accel_lis, zbus_accel_data_callback);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_LISTENER_DEFINE(watchface_battery_event, zbus_battery_sample_data_callback);

ZBUS_CHAN_DECLARE(activity_state_data_chan);
ZBUS_LISTENER_DEFINE(watchface_activity_state_event, zbus_activity_event_callback);

#define WORK_STACK_SIZE 3000
#define WORK_PRIORITY   5

#define RENDER_INTERVAL_LVGL    K_MSEC(100)
#define SLOW_UPDATE_INTERVAL    K_MINUTES(1)

typedef enum work_type {
    UPDATE_CLOCK,
    UPDATE_VALUES,
    OPEN_WATCHFACE,
    UPDATE_SLOW_VALUES
} work_type_t;

typedef struct delayed_work_item {
    struct k_work_delayable work;
    work_type_t             type;
} delayed_work_item_t;

static void general_work(struct k_work *item);

static void check_notifications(void);
static void update_ui_from_event(struct k_work *item);
static void watchface_gesture_cb(lv_event_t *e);

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
    .disconnected = disconnected,
};

static delayed_work_item_t clock_work =     { .type = UPDATE_CLOCK };
static delayed_work_item_t update_work =      { .type = UPDATE_VALUES };
static delayed_work_item_t date_work =      { .type = UPDATE_SLOW_VALUES };

static delayed_work_item_t general_work_item;
static struct k_work_sync cancel_work_sync;

static K_WORK_DEFINE(update_ui_work, update_ui_from_event);
static ble_comm_data_type_t last_data_update_type;
static ble_comm_weather_t last_weather_data;
static ble_comm_music_info_t last_music_info;
static struct battery_sample_event last_batt_evt = {.percent = 100, .mV = 4300};

static bool running;
static bool is_connected;
static bool is_suspended;
static lv_obj_t *watchface_root_screen;

static watchface_ui_api_t *watchfaces[MAX_WATCHFACES];
static uint8_t num_watchfaces;
static zsw_settings_watchface_t watchface_settings;
static zsw_settings_brightness_t brightness_setting;

static watchface_app_evt_listener watchface_evt_cb;

static int watchface_app_init(void)
{
    k_work_init_delayable(&general_work_item.work, general_work);
    k_work_init_delayable(&clock_work.work, general_work);
    k_work_init_delayable(&update_work.work, general_work);
    k_work_init_delayable(&date_work.work, general_work);
    running = false;
    is_suspended = false;

    return 0;
}

void watchface_app_register_ui(watchface_ui_api_t *ui_api)
{
    __ASSERT(num_watchfaces < MAX_WATCHFACES, "Increase MAX_WATCHFACES, too many.");
    watchfaces[num_watchfaces] = ui_api;
    num_watchfaces++;
}

void watchface_app_start(lv_obj_t *root_screen, lv_group_t *group, watchface_app_evt_listener evt_cb)
{
    __ASSERT(num_watchfaces > 0, "Must enable at least one watchface.");
    int err = settings_load_subtree_direct(ZSW_SETTINGS_WATCHFACE, settings_load_handler_watchface, &watchface_settings);
    if (err != 0) {
        LOG_ERR("Failed loading watchface settings");
    }

    err = settings_load_subtree_direct(ZSW_SETTINGS_BRIGHTNESS, settings_load_handler_brightness, &brightness_setting);
    if (err != 0) {
        LOG_ERR("Failed loading brightness settings");
    }

    if (watchface_settings.watchface_index >= num_watchfaces) {
        watchface_settings.watchface_index = 0;
    }

    watchface_root_screen = root_screen;
    watchface_evt_cb = evt_cb;

    lv_obj_add_event_cb(watchface_root_screen, watchface_gesture_cb, LV_EVENT_GESTURE, NULL);

    general_work_item.type = OPEN_WATCHFACE;
    __ASSERT(0 <= k_work_schedule(&general_work_item.work, K_MSEC(100)), "FAIL schedule");
}

void watchface_app_stop(void)
{
    running = false;
    is_suspended = false;
    k_work_cancel_delayable_sync(&clock_work.work, &cancel_work_sync);
    k_work_cancel_delayable_sync(&date_work.work, &cancel_work_sync);
    k_work_cancel_delayable_sync(&general_work_item.work, &cancel_work_sync);
    watchfaces[watchface_settings.watchface_index]->remove();
    zsw_watchface_dropdown_ui_remove();

    lv_obj_remove_event_cb(watchface_root_screen, watchface_gesture_cb);
}

void watchface_change(int index)
{
    if (num_watchfaces == 1) {
        return;
    }

    if ((index < 0) || (index >= num_watchfaces)) {
        return;
    }

    watchfaces[watchface_settings.watchface_index]->remove();

    // Make sure we have the latest settings
    int err = settings_load_subtree_direct(ZSW_SETTINGS_WATCHFACE, settings_load_handler_watchface, &watchface_settings);
    if (err != 0) {
        LOG_ERR("Failed loading watchface settings");
    }

    watchface_settings.watchface_index = index;

    err = settings_save_one(ZSW_SETTINGS_WATCHFACE, &watchface_settings, sizeof(watchface_settings));
    if (err != 0) {
        LOG_ERR("Failed saving watchface settings");
    }
    if (running) {
        general_work_item.type = OPEN_WATCHFACE;
        __ASSERT(0 <= k_work_schedule(&general_work_item.work, K_MSEC(100)), "FAIL schedule");
    }
}

int watchface_app_get_current_face(void)
{
    return watchface_settings.watchface_index;
}

int watchface_app_get_num_faces(void)
{
    return num_watchfaces;

}

int watchface_app_get_face_info(int index, const lv_img_dsc_t **preview,  const char **name)
{
    if (index >= num_watchfaces) {
        return -EEXIST;
    }

    *name = watchfaces[index]->name;
    *preview = watchfaces[index]->get_preview_img();

    return 0;
}

static void refresh_ui(void)
{
    uint32_t steps;
    watchfaces[watchface_settings.watchface_index]->set_ble_connected(is_connected);
    watchfaces[watchface_settings.watchface_index]->set_battery_percent(last_batt_evt.percent, last_batt_evt.mV);
    zsw_watchface_dropdown_ui_set_battery_info(last_batt_evt.percent, last_batt_evt.is_charging, last_batt_evt.tte,
                                               last_batt_evt.ttf);
    if (strlen(last_weather_data.report_text) > 0) {
        watchfaces[watchface_settings.watchface_index]->set_weather(last_weather_data.temperature_c,
                                                                    last_weather_data.weather_code);
    }
    if (zsw_imu_fetch_num_steps(&steps) == 0) {
        // TODO: Add calculation for distance and kcal
        watchfaces[watchface_settings.watchface_index]->set_step(steps, 0, 0);
    }
    if (strlen(last_music_info.track_name) > 0) {
        zsw_watchface_dropdown_ui_set_music_info(last_music_info.track_name, last_music_info.artist);
    }
}

static void general_work(struct k_work *item)
{
    struct k_work_delayable *delayable_work = CONTAINER_OF(item, struct k_work_delayable, work);

    delayed_work_item_t *the_work = CONTAINER_OF(delayable_work, delayed_work_item_t, work);
    uint32_t steps;

    switch (the_work->type) {
        case OPEN_WATCHFACE: {
            running = true;
            is_suspended = false;
            // Dropdown
            watchfaces[watchface_settings.watchface_index]->show(watchface_root_screen, watchface_evt_cb, &watchface_settings);
            zsw_watchface_dropdown_ui_add(watchface_root_screen, watchface_evt_cb, brightness_setting);
            refresh_ui();

            __ASSERT(0 <= k_work_schedule(&clock_work.work, K_NO_WAIT), "FAIL clock_work");
            __ASSERT(0 <= k_work_schedule(&update_work.work, K_SECONDS(1)), "FAIL update_work");
            __ASSERT(0 <= k_work_schedule(&date_work.work, K_SECONDS(1)), "FAIL date_work");
            general_work_item.type = UPDATE_SLOW_VALUES;
            __ASSERT(0 <= k_work_schedule(&general_work_item.work, K_MSEC(100)), "FAIL schedule");
            break;
        }
        case UPDATE_VALUES: {
            check_notifications();

            // Realtime update of steps
            if (zsw_imu_fetch_num_steps(&steps) == 0) {
                // TODO: Add calculation for distance and kcal
                watchfaces[watchface_settings.watchface_index]->set_step(steps, 0, 0);
            }
            __ASSERT(0 <= k_work_schedule(&update_work.work, K_SECONDS(1)), "FAIL update_work");
            break;
        }
        case UPDATE_CLOCK: {
            zsw_timeval_t time;
            zsw_clock_get_time(&time);

            if (watchfaces[watchface_settings.watchface_index]->set_datetime) {
                // TODO: Add support for AM and 12/24 h mode
                watchfaces[watchface_settings.watchface_index]->set_datetime(time.tm.tm_wday, time.tm.tm_mday, time.tm.tm_mday,
                                                                             time.tm.tm_mon,
                                                                             time.tm.tm_year, time.tm.tm_wday, time.tm.tm_hour, time.tm.tm_min, time.tm.tm_sec, time.tv_usec, false, false);
            }

            __ASSERT(0 <= k_work_schedule(&clock_work.work,
                                          watchface_settings.smooth_second_hand ? SMOOTH_TIME_UPDATE_INTERVAL : NORMAL_TIME_UPDATE_INTERVAL), "FAIL clock_work");
            break;
        }
        case UPDATE_SLOW_VALUES: {
            float pressure = 0.0;
            float temperature = 0.0;
            float humidity = 0.0;
            float iaq = 0.0;
            float co2 = 0.0;
            zsw_timeval_t time;
            zsw_clock_get_time(&time);

            zsw_environment_sensor_get(&temperature, &humidity, &pressure);
            zsw_environment_sensor_get_co2(&co2);
            zsw_environment_sensor_get_iaq(&iaq);

            zsw_pressure_sensor_get_pressure(&pressure);
            watchfaces[watchface_settings.watchface_index]->set_watch_env_sensors((int)temperature, (int)humidity, (int)pressure,
                                                                                  iaq,
                                                                                  co2);

            __ASSERT(0 <= k_work_schedule(&date_work.work, SLOW_UPDATE_INTERVAL), "FAIL date_work");
        }
    }
}

static void check_notifications(void)
{
    uint32_t num_unread = zsw_notification_manager_get_num();
    watchfaces[watchface_settings.watchface_index]->set_num_notifcations(num_unread);
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    is_connected = true;
    if (!running || is_suspended) {
        return;
    }
    if (err) {
        LOG_ERR("Connection failed (err %u)", err);
        return;
    }
    watchfaces[watchface_settings.watchface_index]->set_ble_connected(true);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    is_connected = false;
    if (!running | is_suspended) {
        return;
    }
    watchfaces[watchface_settings.watchface_index]->set_ble_connected(false);
}

static void update_ui_from_event(struct k_work *item)
{
    if (running && !is_suspended) {
        if (last_data_update_type == BLE_COMM_DATA_TYPE_WEATHER) {
            LOG_DBG("Weather: %s t: %d hum: %d code: %d wind: %d dir: %d", last_weather_data.report_text,
                    last_weather_data.temperature_c, last_weather_data.humidity,
                    last_weather_data.weather_code,
                    last_weather_data.wind,
                    last_weather_data.wind_direction);
            watchfaces[watchface_settings.watchface_index]->set_weather(last_weather_data.temperature_c,
                                                                        last_weather_data.weather_code);
        } else if (last_data_update_type == BLE_COMM_DATA_TYPE_SET_TIME) {
            k_work_reschedule(&date_work.work, K_NO_WAIT);
        } else if (last_data_update_type == BLE_COMM_DATA_TYPE_MUSIC_INFO) {
            zsw_watchface_dropdown_ui_set_music_info(last_music_info.track_name, last_music_info.artist);
        }
        return;
    }
}

static void watchface_gesture_cb(lv_event_t *e)
{
    lv_dir_t  dir;
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_GESTURE) {
        dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        watchface_evt_cb((watchface_app_evt_t) {
            .type = WATCHFACE_APP_EVENT_GESTURE,
            .data.gesture_direction = dir
        });
    }
}

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);
    last_data_update_type = event->data.type;
    if (event->data.type == BLE_COMM_DATA_TYPE_WEATHER) {
        memcpy(&last_weather_data, &event->data.data.weather, sizeof(event->data.data.weather));
    }
    if (event->data.type == BLE_COMM_DATA_TYPE_MUSIC_INFO) {
        memcpy(&last_music_info, &event->data.data.music_info, sizeof(event->data.data.music_info));
    }
    if (running && !is_suspended) {
        k_work_submit(&update_ui_work);
    }
}

static void zbus_accel_data_callback(const struct zbus_channel *chan)
{
    if (running && !is_suspended) {
        const struct accel_event *event = zbus_chan_const_msg(chan);
        if (event->data.type == ZSW_IMU_EVT_TYPE_STEP) {
            // TODO: Add calculation for distance and kcal
            watchfaces[watchface_settings.watchface_index]->set_step(event->data.data.step.count, 0, 0);
        }
    }
}

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan)
{
    const struct battery_sample_event *event = zbus_chan_const_msg(chan);
    memcpy(&last_batt_evt, event, sizeof(struct battery_sample_event));

    if (running && !is_suspended) {
        watchfaces[watchface_settings.watchface_index]->set_battery_percent(event->percent, event->mV);
        zsw_watchface_dropdown_ui_set_battery_info(last_batt_evt.percent, event->is_charging, event->tte, event->ttf);
    }
}

static void zbus_activity_event_callback(const struct zbus_channel *chan)
{
    if (running) {
        const struct activity_state_event *event = zbus_chan_const_msg(chan);
        if (event->state == ZSW_ACTIVITY_STATE_INACTIVE) {
            is_suspended = true;
            k_work_cancel_delayable_sync(&clock_work.work, &cancel_work_sync);
            k_work_cancel_delayable_sync(&date_work.work, &cancel_work_sync);
            k_work_cancel_delayable_sync(&update_work.work, &cancel_work_sync);
        } else if (event->state == ZSW_ACTIVITY_STATE_ACTIVE) {
            is_suspended = false;
            watchfaces[watchface_settings.watchface_index]->ui_invalidate_cached();
            refresh_ui();
            __ASSERT(0 <= k_work_schedule(&clock_work.work, K_NO_WAIT), "FAIL clock_work");
            __ASSERT(0 <= k_work_schedule(&date_work.work, K_SECONDS(1)), "FAIL clock_work");
            __ASSERT(0 <= k_work_schedule(&update_work.work, K_SECONDS(1)), "FAIL clock_work");
        }
    }
}

static int settings_load_handler_watchface(const char *key, size_t len,
                                           settings_read_cb read_cb, void *cb_arg, void *param)
{
    int rc;
    zsw_settings_watchface_t *settings = (zsw_settings_watchface_t *)param;
    if (len != sizeof(zsw_settings_watchface_t)) {
        return -EINVAL;
    }

    rc = read_cb(cb_arg, settings, sizeof(zsw_settings_watchface_t));
    if (rc >= 0) {
        return 0;
    }

    return -ENODATA;
}

static int settings_load_handler_brightness(const char *key, size_t len,
                                            settings_read_cb read_cb, void *cb_arg, void *param)
{
    int rc;
    zsw_settings_brightness_t *settings = (zsw_settings_brightness_t *)param;
    if (len != sizeof(zsw_settings_brightness_t)) {
        return -EINVAL;
    }

    rc = read_cb(cb_arg, settings, sizeof(zsw_settings_brightness_t));
    if (rc >= 0) {
        return 0;
    }

    return -ENODATA;
}

SYS_INIT(watchface_app_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
