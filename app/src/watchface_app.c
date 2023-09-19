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
#include <zsw_imu.h>
#include <ram_retention_storage.h>
#include <events/ble_data_event.h>
#include <events/accel_event.h>
#include <notification_manager.h>
#include <zephyr/zbus/zbus.h>
#include <zsw_charger.h>
#include <events/chg_event.h>
#include <events/battery_event.h>
#include <events/activity_event.h>
#include <zsw_battery_manager.h>
#include <zsw_pressure_sensor.h>
#include <zsw_env_sensor.h>
#include "../../watchface_app.h"

LOG_MODULE_REGISTER(watcface_app, LOG_LEVEL_WRN);

#define MAX_WATCHFACES  5

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void zbus_accel_data_callback(const struct zbus_channel *chan);
static void zbus_chg_state_data_callback(const struct zbus_channel *chan);
static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);
static void zbus_activity_event_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(watchface_ble_comm_lis, zbus_ble_comm_data_callback);

ZBUS_CHAN_DECLARE(accel_data_chan);
ZBUS_LISTENER_DEFINE(watchface_accel_lis, zbus_accel_data_callback);

ZBUS_CHAN_DECLARE(chg_state_data_chan);
ZBUS_LISTENER_DEFINE(watchface_chg_event, zbus_chg_state_data_callback);

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

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
    .disconnected = disconnected,
};

static delayed_work_item_t clock_work =     { .type = UPDATE_CLOCK };
static delayed_work_item_t date_work =      { .type = UPDATE_SLOW_VALUES };

static delayed_work_item_t general_work_item;
static struct k_work_sync canel_work_sync;

static K_WORK_DEFINE(update_ui_work, update_ui_from_event);
static ble_comm_cb_data_t last_data_update;
static ble_comm_weather_t last_weather_data;
static struct battery_sample_event last_batt_evt = {.percent = 100, .mV = 4300};
static struct chg_state_event last_chg_evt;

static bool running;
static bool is_connected;
static bool is_suspended;

static watchface_ui_api_t *watchfaces[MAX_WATCHFACES];
static uint8_t num_watchfaces;
static uint8_t current_watchface;

static watchface_app_evt_listener watchface_evt_cb;

static int watchface_app_init(void)
{
    k_work_init_delayable(&general_work_item.work, general_work);
    k_work_init_delayable(&clock_work.work, general_work);
    k_work_init_delayable(&date_work.work, general_work);
    running = false;
    is_suspended = false;
    current_watchface = 0;
    return 0;
}

void watchface_app_register_ui(watchface_ui_api_t *ui_api)
{
    __ASSERT(num_watchfaces < MAX_WATCHFACES, "Increase MAX_WATCHFACES, too many.");
    watchfaces[num_watchfaces] = ui_api;
    num_watchfaces++;
}

void watchface_app_start(lv_group_t *group, watchface_app_evt_listener evt_cb)
{
    __ASSERT(num_watchfaces > 0, "Must enable at least one watchface.");
    watchface_evt_cb = evt_cb;
    general_work_item.type = OPEN_WATCHFACE;
    __ASSERT(0 <= k_work_schedule(&general_work_item.work, K_MSEC(100)), "FAIL schedule");
}

void watchface_app_stop(void)
{
    running = false;
    is_suspended = false;
    k_work_cancel_delayable_sync(&clock_work.work, &canel_work_sync);
    k_work_cancel_delayable_sync(&date_work.work, &canel_work_sync);
    watchfaces[current_watchface]->remove();
}

void watchface_change(void)
{
    watchfaces[current_watchface]->remove();
    current_watchface = (current_watchface + 1) % num_watchfaces;

    general_work_item.type = OPEN_WATCHFACE;
    __ASSERT(0 <= k_work_schedule(&general_work_item.work, K_MSEC(100)), "FAIL schedule");
}

static void refresh_ui(void)
{
    uint32_t steps;
    watchfaces[current_watchface]->set_ble_connected(is_connected);
    watchfaces[current_watchface]->set_battery_percent(last_batt_evt.percent, last_batt_evt.mV);
    if (strlen(last_weather_data.report_text) > 0) {
        watchfaces[current_watchface]->set_weather(last_weather_data.temperature_c, last_weather_data.weather_code);
    }
    if (zsw_imu_fetch_num_steps(&steps) == 0) {
        watchfaces[current_watchface]->set_step(steps);
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
            watchfaces[current_watchface]->show(watchface_evt_cb);
            refresh_ui();

            __ASSERT(0 <= k_work_schedule(&clock_work.work, K_NO_WAIT), "FAIL clock_work");
            __ASSERT(0 <= k_work_schedule(&date_work.work, K_SECONDS(1)), "FAIL clock_work");
            general_work_item.type = UPDATE_SLOW_VALUES;
            __ASSERT(0 <= k_work_schedule(&general_work_item.work, K_MSEC(100)), "FAIL schedule");
            break;
        }
        case UPDATE_CLOCK: {
            struct tm *time = zsw_clock_get_time();
            LOG_INF("%d, %d, %d\n", time->tm_hour, time->tm_min, time->tm_sec);
            watchfaces[current_watchface]->set_time(time->tm_hour, time->tm_min, time->tm_sec);

            check_notifications();

            // Realtime update of steps
            if (zsw_imu_fetch_num_steps(&steps) == 0) {
                watchfaces[current_watchface]->set_step(steps);
            }
            __ASSERT(0 <= k_work_schedule(&clock_work.work, K_SECONDS(1)), "FAIL clock_work");
            break;
        }
        case UPDATE_SLOW_VALUES: {
            float pressure;
            float temperature;
            float humidity;

            struct tm *time = zsw_clock_get_time();
            watchfaces[current_watchface]->set_date(time->tm_wday, time->tm_mday);

            zsw_env_sensor_fetch_all(&temperature, &pressure, &humidity);
            zsw_pressure_sensor_fetch_pressure(&pressure);
            watchfaces[current_watchface]->set_watch_env_sensors((int)temperature, (int)humidity, (int)pressure);

            __ASSERT(0 <= k_work_schedule(&date_work.work, SLOW_UPDATE_INTERVAL), "FAIL date_work");
        }
    }
}

static void check_notifications(void)
{
    uint32_t num_unread = notification_manager_get_num();
    watchfaces[current_watchface]->set_num_notifcations(num_unread);
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
    watchfaces[current_watchface]->set_ble_connected(true);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    is_connected = false;
    if (!running | is_suspended) {
        return;
    }
    watchfaces[current_watchface]->set_ble_connected(false);
}

static void update_ui_from_event(struct k_work *item)
{
    if (running && !is_suspended) {
        if (last_data_update.type == BLE_COMM_DATA_TYPE_WEATHER) {
            LOG_DBG("Weather: %s t: %d hum: %d code: %d wind: %d dir: %d", last_data_update.data.weather.report_text,
                    last_data_update.data.weather.temperature_c, last_data_update.data.weather.humidity,
                    last_data_update.data.weather.weather_code,
                    last_data_update.data.weather.wind,
                    last_data_update.data.weather.wind_direction);
            watchfaces[current_watchface]->set_weather(last_data_update.data.weather.temperature_c,
                                                       last_data_update.data.weather.weather_code);
        } else if (last_data_update.type == BLE_COMM_DATA_TYPE_SET_TIME) {
            k_work_reschedule(&date_work.work, K_NO_WAIT);
        }
        return;
    }
}

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);
    // TODO getting this callback again before workqueue has ran will
    // cause previous to be lost.
    memcpy(&last_data_update, &event->data, sizeof(ble_comm_cb_data_t));
    memcpy(&last_weather_data, &last_data_update.data.weather, sizeof(last_data_update.data.weather));
    if (running && !is_suspended) {
        k_work_submit(&update_ui_work);
    }
}

static void zbus_accel_data_callback(const struct zbus_channel *chan)
{
    if (running && !is_suspended) {
        const struct accel_event *event = zbus_chan_const_msg(chan);
        if (event->data.type == ZSW_IMU_EVT_TYPE_STEP) {
            watchfaces[current_watchface]->set_step(event->data.data.step.count);
        }
    }
}

static void zbus_chg_state_data_callback(const struct zbus_channel *chan)
{
    const struct chg_state_event *event = zbus_chan_const_msg(chan);
    memcpy(&last_chg_evt, event, sizeof(struct chg_state_event));

    if (running && !is_suspended) {
        // TODO Show some nice animation or similar
    }
}

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan)
{
    const struct battery_sample_event *event = zbus_chan_const_msg(chan);
    memcpy(&last_batt_evt, event, sizeof(struct battery_sample_event));

    if (running && !is_suspended) {
        watchfaces[current_watchface]->set_battery_percent(event->percent, event->mV);
    }
}

static void zbus_activity_event_callback(const struct zbus_channel *chan)
{
    if (running) {
        const struct activity_state_event *event = zbus_chan_const_msg(chan);
        if (event->state == ZSW_ACTIVITY_STATE_INACTIVE) {
            is_suspended = true;
            k_work_cancel_delayable_sync(&clock_work.work, &canel_work_sync);
            k_work_cancel_delayable_sync(&date_work.work, &canel_work_sync);
        } else if (event->state == ZSW_ACTIVITY_STATE_ACTIVE) {
            is_suspended = false;
            watchfaces[current_watchface]->ui_invalidate_cached();
            refresh_ui();
            __ASSERT(0 <= k_work_schedule(&clock_work.work, K_NO_WAIT), "FAIL clock_work");
            __ASSERT(0 <= k_work_schedule(&date_work.work, K_SECONDS(1)), "FAIL clock_work");
        }
    }
}

SYS_INIT(watchface_app_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
