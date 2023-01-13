#include <watchface_ui.h>

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include <clock.h>
#include <battery.h>
#include <heart_rate_sensor.h>
#include <accelerometer.h>
#include <vibration_motor.h>
#include <ram_retention_storage.h>
#include <events/ble_data_event.h>
#include <events/accel_event.h>
#include <notification_manager.h>

LOG_MODULE_REGISTER(watcface_app, LOG_LEVEL_WRN);

#define WORK_STACK_SIZE 3000
#define WORK_PRIORITY   5

#define RENDER_INTERVAL_LVGL    K_MSEC(100)
#define ACCEL_INTERVAL          K_MSEC(100)
#define BATTERY_INTERVAL        K_SECONDS(10)
#define SEND_STATUS_INTERVAL    K_SECONDS(30) // TODO move out from here
#define DATE_UPDATE_INTERVAL    K_MINUTES(1)

typedef enum work_type {
    UPDATE_CLOCK,
    OPEN_WATCHFACE,
    BATTERY,
    SEND_STATUS_UPDATE,
    UPDATE_DATE
} work_type_t;

typedef struct delayed_work_item {
    struct k_work_delayable work;
    work_type_t             type;
} delayed_work_item_t;

void general_work(struct k_work *item);

static void check_notifications(void);
static int read_battery(int *batt_mV, int *percent);

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = connected,
    .disconnected = disconnected,
};

static delayed_work_item_t battery_work =   { .type = BATTERY };
static delayed_work_item_t clock_work =     { .type = UPDATE_CLOCK };
static delayed_work_item_t status_work =    { .type = SEND_STATUS_UPDATE };
static delayed_work_item_t date_work =      { .type = UPDATE_DATE };

static delayed_work_item_t general_work_item;
static struct k_work_sync canel_work_sync;

static bool running;

static int watchface_app_init(const struct device *arg)
{
    k_work_init_delayable(&general_work_item.work, general_work);
    k_work_init_delayable(&battery_work.work, general_work);
    k_work_init_delayable(&clock_work.work, general_work);
    k_work_init_delayable(&status_work.work, general_work);
    k_work_init_delayable(&date_work.work, general_work);
    running = false;

    return 0;
}

void watchface_app_start(lv_group_t *group)
{
    general_work_item.type = OPEN_WATCHFACE;
    __ASSERT(0 <= k_work_schedule(&general_work_item.work, K_MSEC(100)), "FAIL schedule");
}

void watchface_app_stop(void)
{
    running = false;
    k_work_cancel_delayable_sync(&battery_work.work, &canel_work_sync);
    k_work_cancel_delayable_sync(&clock_work.work, &canel_work_sync);
    k_work_cancel_delayable_sync(&date_work.work, &canel_work_sync);
    watchface_remove();
}

void general_work(struct k_work *item)
{
    delayed_work_item_t *the_work = CONTAINER_OF(item, delayed_work_item_t, work);

    switch (the_work->type) {
        case OPEN_WATCHFACE: {
            running = true;
            watchface_show();
            __ASSERT(0 <= k_work_schedule(&battery_work.work, K_NO_WAIT), "FAIL battery_work");
            __ASSERT(0 <= k_work_schedule(&clock_work.work, K_NO_WAIT), "FAIL clock_work");
            __ASSERT(0 <= k_work_schedule(&date_work.work, K_SECONDS(1)), "FAIL clock_work");
            break;
        }
        case UPDATE_CLOCK: {
            struct tm *time = clock_get_time();
            LOG_INF("%d, %d, %d\n", time->tm_hour, time->tm_min, time->tm_sec);
            watchface_set_time(time->tm_hour, time->tm_min, time->tm_sec);

            // TODO move from this file
            retained.current_time_seconds = clock_get_time_unix();
            retained_update();
            check_notifications();
            __ASSERT(0 <= k_work_schedule(&clock_work.work, K_SECONDS(1)), "FAIL clock_work");
            break;
        }
        case UPDATE_DATE: {
            struct tm *time = clock_get_time();
            watchface_set_date(time->tm_wday, time->tm_mday);
            __ASSERT(0 <= k_work_schedule(&date_work.work, DATE_UPDATE_INTERVAL), "FAIL date_work");
        }
        case BATTERY: {
            int batt_mv;
            int batt_percent;
            static uint32_t count;

            if (read_battery(&batt_mv, &batt_percent) == 0) {
                watchface_set_battery_percent(batt_percent, batt_mv);
            }
            watchface_set_hrm(count % 220);
            //heart_rate_sensor_fetch(&hr_sample);
            count++;
            __ASSERT(0 <= k_work_schedule(&battery_work.work, BATTERY_INTERVAL),
                     "FAIL battery_work");
            break;
        }
        case SEND_STATUS_UPDATE: {
            // TODO move to main
            int batt_mv;
            int batt_percent;
            int msg_len;
            char buf[100];
            memset(buf, 0, sizeof(buf));

            if (read_battery(&batt_mv, &batt_percent) == 0) {
                msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"status\", \"bat\": %d, \"volt\": %d, \"chg\": %d} \n", batt_percent,
                                   batt_mv, 0);
                ble_comm_send(buf, msg_len);
            }
            __ASSERT(0 <= k_work_schedule(&status_work.work, SEND_STATUS_INTERVAL),
                     "Failed schedule status work");
            break;
        }
    }
}

/** A discharge curve specific to the power source. */
static const struct battery_level_point levels[] = {
    /*
    Battery supervisor cuts power at 3500mA so treat that as 0%
    TODO analyze more to get a better curve.
    */
    { 10000, 4150 },
    { 0, 3500 },
};

static int read_battery(int *batt_mV, int *percent)
{
    int rc = battery_measure_enable(true);
    if (rc != 0) {
        LOG_ERR("Failed initialize battery measurement: %d\n", rc);
        return -1;
    }
    // From https://github.com/zephyrproject-rtos/zephyr/blob/main/samples/boards/nrf/battery/src/main.c
    *batt_mV = battery_sample();

    if (*batt_mV < 0) {
        LOG_ERR("Failed to read battery voltage: %d\n", *batt_mV);
        return -1;
    }

    unsigned int batt_pptt = battery_level_pptt(*batt_mV, levels);

    LOG_DBG("%d mV; %u pptt\n", *batt_mV, batt_pptt);
    *percent = batt_pptt / 100;

    rc = battery_measure_enable(false);
    if (rc != 0) {
        LOG_ERR("Failed disable battery measurement: %d\n", rc);
        return -1;
    }
    return 0;
}

static void check_notifications(void)
{
    uint32_t num_unread = notification_manager_get_num();
    watchface_set_num_notifcations(num_unread);
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Connection failed (err %u)", err);
        return;
    }
    __ASSERT(0 <= k_work_schedule(&status_work.work, K_MSEC(1000)), "FAIL status");

    watchface_set_ble_connected(true);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    watchface_set_ble_connected(false);
}

static bool app_event_handler(const struct app_event_header *aeh)
{
    if (running && is_ble_data_event(aeh)) {
        struct ble_data_event *event = cast_ble_data_event(aeh);
        if (event->data.type == BLE_COMM_DATA_TYPE_WEATHER) {
            LOG_DBG("Weather: %s t: %d hum: %d code: %d wind: %d dir: %d", event->data.data.weather.report_text,
                    event->data.data.weather.temperature_c, event->data.data.weather.humidity, event->data.data.weather.weather_code,
                    event->data.data.weather.wind,
                    event->data.data.weather.wind_direction);
            watchface_set_weather(event->data.data.weather.temperature_c, event->data.data.weather.weather_code);
        } else if (event->data.type == BLE_COMM_DATA_TYPE_SET_TIME) {
            k_work_reschedule(&date_work.work, K_SECONDS(1));
        }
        return false;
    } if (running && is_accel_event(aeh)) {
        struct accel_event *event = cast_accel_event(aeh);
        if (event->data.type == ACCELEROMETER_EVT_TYPE_STEP) {
            watchface_set_step(event->data.data.step.count);
        }
    }
    return false;
}

SYS_INIT(watchface_app_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
APP_EVENT_LISTENER(watchface_app, app_event_handler);
APP_EVENT_SUBSCRIBE(watchface_app, ble_data_event);
APP_EVENT_SUBSCRIBE(watchface_app, accel_event);