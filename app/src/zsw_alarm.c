#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/rtc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <zsw_clock.h>

#include "zsw_alarm.h"

LOG_MODULE_REGISTER(zsw_alarm, LOG_LEVEL_DBG);

#define RTC_ALARM_MASK_COMPARE_ALL (RTC_ALARM_TIME_MASK_SECOND | RTC_ALARM_TIME_MASK_MINUTE | RTC_ALARM_TIME_MASK_HOUR)

static void zbus_periodic_slow_callback(const struct zbus_channel *chan);
static int find_free_alarm_slot(void);
static void start_earliest_alarm(void);
static int find_earliest_alarm(void);
static void rtc_alarm_triggered_callback(const struct device *dev, uint16_t id, void *user_data);
int compare(const void* rtc_time_a, const void* rtc_time_b);

ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_LISTENER_DEFINE(timer_app_slow_listener, zbus_periodic_slow_callback);

static const struct device *rtc = DEVICE_DT_GET(DT_ALIAS(rtc));

static zsw_alarm_t alarms[MAX_ALARMS];

int zsw_alarm_add(struct rtc_time expiry_time, alarm_cb callback, void* user_data)
{
    int alarm_index = find_free_alarm_slot();

    if (alarm_index < 0) {
        return -ENOMEM;
    }

    alarms[alarm_index].expiry_time = expiry_time;
    alarms[alarm_index].used = true;
    alarms[alarm_index].cb = callback;
    alarms[alarm_index].user_data = user_data;

    start_earliest_alarm();

    return alarm_index;
}

int zsw_alarm_add_timer(uint16_t hour, uint16_t min, uint16_t sec, alarm_cb callback, void* user_data)
{
    int ret;
    struct rtc_time alarm_time;
    struct tm tm_alarm_time;

    ret = rtc_get_time(rtc, &alarm_time);
    __ASSERT(ret == 0, "Failed to get current time");

    tm_alarm_time.tm_year = alarm_time.tm_year;
    tm_alarm_time.tm_mon = alarm_time.tm_mon;
    tm_alarm_time.tm_mday = alarm_time.tm_mday;
    tm_alarm_time.tm_hour = alarm_time.tm_hour;
    tm_alarm_time.tm_min = alarm_time.tm_min;
    tm_alarm_time.tm_sec = alarm_time.tm_sec;
    tm_alarm_time.tm_isdst = alarm_time.tm_isdst;
    tm_alarm_time.tm_wday = alarm_time.tm_wday;
    tm_alarm_time.tm_yday = alarm_time.tm_yday;

    tm_alarm_time.tm_hour += hour;
    tm_alarm_time.tm_min += min;
    tm_alarm_time.tm_sec += sec;

    // This normalizes the time, i.e. if second gets to 80,
    // it will add one to minute and adjust the seconds to 20.
    if(mktime(&tm_alarm_time) == -1) {
        LOG_ERR("Failed to convert time to epoch: %d", ret);
        return -EINVAL;
    }

    alarm_time.tm_year = tm_alarm_time.tm_year;
    alarm_time.tm_mon = tm_alarm_time.tm_mon;
    alarm_time.tm_mday = tm_alarm_time.tm_mday;
    alarm_time.tm_hour = tm_alarm_time.tm_hour;
    alarm_time.tm_min = tm_alarm_time.tm_min;
    alarm_time.tm_sec = tm_alarm_time.tm_sec;
    alarm_time.tm_isdst = tm_alarm_time.tm_isdst;
    alarm_time.tm_wday = tm_alarm_time.tm_wday;
    alarm_time.tm_yday = tm_alarm_time.tm_yday;

    int alarm_index = find_free_alarm_slot();

    if (alarm_index < 0) {
        return -ENOMEM;
    }

    alarms[alarm_index].expiry_time = alarm_time;
    alarms[alarm_index].used = true;
    alarms[alarm_index].cb = callback;
    alarms[alarm_index].user_data = user_data;

    start_earliest_alarm();

    return alarm_index;
}

int zsw_alarm_remove(uint32_t alarm_id)
{
    if (alarm_id >= MAX_ALARMS) {
        return -EINVAL;
    }

    int earliest_alarm_index = find_earliest_alarm();
    __ASSERT(earliest_alarm_index >= 0, "Failed to find earliest alarm");

    alarms[alarm_id].used = false;

    // Check if new alarm is the new earliest, if so change the rtc alarm time to this.
    if (earliest_alarm_index == alarm_id) {
        start_earliest_alarm();
    }

    return 0;
}

static int find_earliest_alarm(void)
{
    zsw_alarm_t* earliest_alarm = NULL;
    int index = -1;

    for (int i = 0; i < MAX_ALARMS; i++) {
        if (alarms[i].used && compare(&alarms[i], earliest_alarm) < 0) {
            earliest_alarm = &alarms[i];
            index = i;
        }
    }

    return index;
}

static void start_earliest_alarm(void)
{
    LOG_DBG("start_earliest_alarm");
    struct rtc_time current_alarm_time;
    uint16_t current_alarm_mask;
    int earliest_alarm_index = find_earliest_alarm();

    int ret = rtc_alarm_get_time(rtc, 0, &current_alarm_mask, &current_alarm_time);
    if (ret != 0) {
        LOG_ERR("Failed to get current alarm time");
        return;
    }

    // No alarms active, and RTC have alarm => disable it
    if (earliest_alarm_index < 0 && current_alarm_mask != 0) {
        LOG_DBG("No alarms active, and RTC have alarm => disable it");
        rtc_alarm_set_callback(rtc, 0, NULL, NULL);
        rtc_alarm_set_time(rtc, 0, 0, NULL);
        return;
    } else if (earliest_alarm_index < 0) {
        // No alarms active
        LOG_DBG("No alarms active");
        return;
    }

    if (current_alarm_mask != 0 && compare(&alarms[earliest_alarm_index], &current_alarm_time) > 0) {
        // The current active alarm is already the earliest
        LOG_DBG("Current alarm is already the earliest");
        return;
    } else {
        // RTC alarm should be updated
        LOG_DBG("RTC alarm should be updated");
        rtc_alarm_set_callback(rtc, 0, NULL, NULL);
        rtc_alarm_set_time(rtc, 0, 0, NULL);
        rtc_alarm_set_time(rtc, 0, RTC_ALARM_MASK_COMPARE_ALL, &alarms[earliest_alarm_index].expiry_time);
        int ret = rtc_alarm_set_callback(rtc, 0, rtc_alarm_triggered_callback, (void*)earliest_alarm_index);
        __ASSERT(ret == 0, "Failed to set alarm callback");
    }
}

static void rtc_alarm_triggered_callback(const struct device *dev, uint16_t id, void *user_data)
{
    LOG_DBG("RTC alarm callback");
    int alarm_index = (int)user_data;
    alarms[alarm_index].used = false;
    // Cancel alarm
    rtc_alarm_set_callback(rtc, 0, NULL, NULL);
    rtc_alarm_set_time(rtc, 0, 0, NULL);
    start_earliest_alarm();
    alarms[alarm_index].cb(alarms[alarm_index].user_data);
}

static void zbus_periodic_slow_callback(const struct zbus_channel *chan)
{

}

static int find_free_alarm_slot(void)
{
    for (int i = 0; i < MAX_ALARMS; i++) {
        if (!alarms[i].used) {
            return i;
        }
    }
    return -ENOMEM;
}

int compare(const void* rtc_time_a, const void* rtc_time_b) {
    zsw_alarm_t *a = (zsw_alarm_t*)rtc_time_a;
    zsw_alarm_t *b = (zsw_alarm_t*)rtc_time_b;

    if (a != NULL && b == NULL) {
        return -1;
    }

    if (b != NULL && a == NULL) {
        return -1;
    }

    if (!a && !b) {
        return 0;
    }

    if (a->expiry_time.tm_year != b->expiry_time.tm_year) {
        return a->expiry_time.tm_year - b->expiry_time.tm_year;
    }
    if (a->expiry_time.tm_mon != b->expiry_time.tm_mon) {
        return a->expiry_time.tm_mon - b->expiry_time.tm_mon;
    }
    if (a->expiry_time.tm_mday != b->expiry_time.tm_mday) {
        return a->expiry_time.tm_mday - b->expiry_time.tm_mday;
    }
    if (a->expiry_time.tm_hour != b->expiry_time.tm_hour) {
        return a->expiry_time.tm_hour - b->expiry_time.tm_hour;
    }
    if (a->expiry_time.tm_min != b->expiry_time.tm_min) {
        return a->expiry_time.tm_min - b->expiry_time.tm_min;
    }
    if (a->expiry_time.tm_sec != b->expiry_time.tm_sec) {
        return a->expiry_time.tm_sec - b->expiry_time.tm_sec;
    }
    return 0;
}

static int zsw_alarm_init(void)
{
    memset(alarms, 0, sizeof(alarms));

    // TODO should be handled somewhere else, maybe in main.
    struct tm *tp;
    time_t t;
    t = time(NULL);
    tp = localtime(&t);
    t = mktime(tp);
    rtc_set_time(rtc, (struct rtc_time*)tp);

    // TODO if any timers
    //zsw_periodic_chan_add_obs(&periodic_event_1s_chan, &timer_app_slow_listener);
    return 0;
}

SYS_INIT(zsw_alarm_init, APPLICATION, 2); // Just after zsw_periodic_event init