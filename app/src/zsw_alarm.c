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
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/rtc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <zsw_clock.h>

#include "zsw_alarm.h"

LOG_MODULE_REGISTER(zsw_alarm, LOG_LEVEL_DBG);

#define RTC_ALARM_MASK (RTC_ALARM_TIME_MASK_SECOND | RTC_ALARM_TIME_MASK_MINUTE | \
                        RTC_ALARM_TIME_MASK_HOUR | RTC_ALARM_TIME_MASK_MONTHDAY)
#define ALARM_RETRY_DELAY K_SECONDS(5)
static int find_free_alarm_slot(void);
static int start_earliest_alarm(bool force);
static int find_earliest_alarm(void);
static void rtc_alarm_triggered_callback(const struct device *dev, uint16_t id, void *user_data);
static void handle_rtc_alarm_triggered_work(struct k_work *work);
static int compare(const void *rtc_time_a, const void *rtc_time_b);
static void copy_rtc_time_to_tm(struct rtc_time *rtc_time, struct tm *tm_time);
static void copy_tm_to_rtc_time(struct tm *tm_time, struct rtc_time *rtc_time);

static const struct device *rtc = DEVICE_DT_GET(DT_ALIAS(rtc));

static zsw_alarm_t alarms[ZSW_MAX_ALARMS];
static uint16_t generations[ZSW_MAX_ALARMS];
static zsw_alarm_t armed_alarm;
static bool alarm_armed;
static K_MUTEX_DEFINE(alarm_mutex);
static K_WORK_DELAYABLE_DEFINE(rtc_alarm_work, handle_rtc_alarm_triggered_work);

BUILD_ASSERT(ZSW_MAX_ALARMS > 0 && ZSW_MAX_ALARMS < UINT16_MAX &&
             (ZSW_MAX_ALARMS & (ZSW_MAX_ALARMS - 1)) == 0,
             "Alarm slot count must be a power of two");

static int schedule_alarm_work(void)
{
    int ret = k_work_reschedule(&rtc_alarm_work, K_NO_WAIT);

    if (ret < 0) {
        LOG_ERR("Failed to schedule alarm work: %d", ret);
        return ret;
    }
    return 0;
}

static void schedule_alarm_retry(void)
{
    int ret = k_work_reschedule(&rtc_alarm_work, ALARM_RETRY_DELAY);

    if (ret < 0) {
        LOG_ERR("Failed to schedule alarm retry: %d", ret);
    }
}

static int find_alarm(uint32_t id)
{
    if (id >= ZSW_ALARM_INVALID_ID) {
        return -EINVAL;
    }

    int index = id % ZSW_MAX_ALARMS;

    if (!alarms[index].used || alarms[index].id != id) {
        return -EINVAL;
    }

    return index;
}

static int add_alarm(const struct rtc_time *expiry_time, alarm_cb callback, void *user_data)
{
    k_mutex_lock(&alarm_mutex, K_FOREVER);
    int index = find_free_alarm_slot();

    if (index < 0) {
        k_mutex_unlock(&alarm_mutex);
        return index;
    }

    uint16_t id = (uint16_t)(generations[index] * ZSW_MAX_ALARMS + index);
    if (id == ZSW_ALARM_INVALID_ID) {
        generations[index] = 0;
        id = index;
    }
    generations[index] = (generations[index] + 1U) % (UINT16_MAX / ZSW_MAX_ALARMS + 1U);

    alarms[index] = (zsw_alarm_t) {
        .expiry_time = *expiry_time,
        .used = true,
        .enabled = true,
        .id = id,
        .cb = callback,
        .user_data = user_data,
    };
    int ret = start_earliest_alarm(false);
    if (ret < 0) {
        memset(&alarms[index], 0, sizeof(alarms[index]));
        int restore_ret = start_earliest_alarm(true);
        if (restore_ret < 0) {
            LOG_ERR("Failed to restore RTC alarm: %d", restore_ret);
            schedule_alarm_retry();
        }
        k_mutex_unlock(&alarm_mutex);
        return ret;
    }
    k_mutex_unlock(&alarm_mutex);
    return id;
}

int zsw_alarm_add(struct rtc_time expiry_time, alarm_cb callback, void *user_data)
{
    int ret;
    struct rtc_time current_time;
    struct tm tm_alarm_time;

    if (!zsw_clock_rtc_available()) {
        LOG_WRN("RTC not available, alarm not supported");
        return -ENOTSUP;
    }

    if (callback == NULL) {
        return -EINVAL;
    }

    ret = rtc_get_time(rtc, &current_time);
    if (ret != 0) {
        LOG_ERR("Failed to get current time: %d", ret);
        return ret;
    }

    // If year, month or day is not set, set it to current date.
    // alarms must have a fully valid date.
    if (!expiry_time.tm_year || !expiry_time.tm_mon || !expiry_time.tm_mday) {
        int hour = expiry_time.tm_hour;
        int min = expiry_time.tm_min;
        int sec = expiry_time.tm_sec;
        expiry_time = current_time;
        expiry_time.tm_hour = hour;
        expiry_time.tm_min = min;
        expiry_time.tm_sec = sec;
    }

    if (expiry_time.tm_hour < current_time.tm_hour || (expiry_time.tm_hour == current_time.tm_hour &&
                                                       expiry_time.tm_min < current_time.tm_min) || (expiry_time.tm_hour == current_time.tm_hour &&
                                                                                                     expiry_time.tm_min == current_time.tm_min && expiry_time.tm_sec < current_time.tm_sec)) {
        copy_rtc_time_to_tm(&expiry_time, &tm_alarm_time);
        // Alarm is set for tomorrow
        tm_alarm_time.tm_mday++;
        if (mktime(&tm_alarm_time) == -1) {
            LOG_ERR("Failed to convert time to epoch: %d", ret);
            return -EINVAL;
        }
        copy_tm_to_rtc_time(&tm_alarm_time, &expiry_time);
    }

    LOG_DBG("Alarm added: %d-%d-%d %d:%d:%d", expiry_time.tm_year, expiry_time.tm_mon, expiry_time.tm_mday,
            expiry_time.tm_hour, expiry_time.tm_min, expiry_time.tm_sec);
    return add_alarm(&expiry_time, callback, user_data);
}

int zsw_alarm_add_timer(uint16_t hour, uint16_t min, uint16_t sec, alarm_cb callback, void *user_data)
{
    int ret;
    struct rtc_time alarm_time;
    struct tm tm_alarm_time;

    if (!zsw_clock_rtc_available()) {
        LOG_WRN("RTC not available, timer not supported");
        return -ENOTSUP;
    }

    if (callback == NULL) {
        return -EINVAL;
    }

    ret = rtc_get_time(rtc, &alarm_time);
    if (ret != 0) {
        LOG_ERR("Failed to get current time: %d", ret);
        return ret;
    }

    copy_rtc_time_to_tm(&alarm_time, &tm_alarm_time);

    tm_alarm_time.tm_hour += hour;
    tm_alarm_time.tm_min += min;
    tm_alarm_time.tm_sec += sec;

    // This normalizes the time, i.e. if second gets to 80,
    // it will add one to minute and adjust the seconds to 20.
    if (mktime(&tm_alarm_time) == -1) {
        LOG_ERR("Failed to convert time to epoch: %d", ret);
        return -EINVAL;
    }

    copy_tm_to_rtc_time(&tm_alarm_time, &alarm_time);

    LOG_DBG("Timer added: %d-%d-%d %d:%d:%d", alarm_time.tm_year, alarm_time.tm_mon, alarm_time.tm_mday, alarm_time.tm_hour,
            alarm_time.tm_min, alarm_time.tm_sec);

    return add_alarm(&alarm_time, callback, user_data);
}

int zsw_alarm_set_enabled(uint32_t alarm_id, bool enabled)
{
    k_mutex_lock(&alarm_mutex, K_FOREVER);
    int index = find_alarm(alarm_id);
    if (index < 0) {
        k_mutex_unlock(&alarm_mutex);
        return index;
    }

    int ret = 0;
    if (alarms[index].enabled != enabled) {
        alarms[index].enabled = enabled;
        ret = start_earliest_alarm(false);
        if (ret < 0) {
            alarms[index].enabled = !enabled;
            int restore_ret = start_earliest_alarm(true);
            if (restore_ret < 0) {
                LOG_ERR("Failed to restore RTC alarm: %d", restore_ret);
                schedule_alarm_retry();
            }
        }
    }
    k_mutex_unlock(&alarm_mutex);
    return ret;
}

int zsw_alarm_get_enabled(uint32_t alarm_id, bool *enabled)
{
    if (enabled == NULL) {
        return -EINVAL;
    }

    k_mutex_lock(&alarm_mutex, K_FOREVER);
    int index = find_alarm(alarm_id);
    if (index < 0) {
        k_mutex_unlock(&alarm_mutex);
        return index;
    }
    *enabled = alarms[index].enabled;
    k_mutex_unlock(&alarm_mutex);
    return 0;
}

int zsw_alarm_get_remaining(uint32_t alarm_id, uint32_t *hour, uint32_t *min, uint32_t *sec)
{
    struct rtc_time expiry_time;

    if (!zsw_clock_rtc_available()) {
        LOG_WRN("RTC not available, timer not supported");
        return -ENOTSUP;
    }

    if (hour == NULL || min == NULL || sec == NULL) {
        return -EINVAL;
    }

    k_mutex_lock(&alarm_mutex, K_FOREVER);
    int index = find_alarm(alarm_id);
    if (index >= 0) {
        expiry_time = alarms[index].expiry_time;
    }
    k_mutex_unlock(&alarm_mutex);
    if (index < 0) {
        return index;
    }

    struct tm current_time_tm = {0};
    struct tm alarm_time_tm = {0};
    struct rtc_time current_time;

    int ret = rtc_get_time(rtc, &current_time);
    if (ret != 0) {
        LOG_ERR("Failed to get current time: %d", ret);
        return ret;
    }

    copy_rtc_time_to_tm(&current_time, &current_time_tm);
    copy_rtc_time_to_tm(&expiry_time, &alarm_time_tm);

    time_t current_epoch = mktime(&current_time_tm);
    time_t alarm_epoch = mktime(&alarm_time_tm);

    int diffSecs = (int)difftime(alarm_epoch, current_epoch);
    LOG_DBG("start: %lld, end: %lld, diff: %d", current_epoch, alarm_epoch, diffSecs);
    if (diffSecs < 0) {
        diffSecs = 0;
    }

    *hour = diffSecs / 3600;
    *min = (diffSecs % 3600) / 60;
    *sec = diffSecs % 60;

    return 0;
}

int zsw_alarm_remove(uint32_t alarm_id)
{
    k_mutex_lock(&alarm_mutex, K_FOREVER);
    int index = find_alarm(alarm_id);
    if (index < 0) {
        k_mutex_unlock(&alarm_mutex);
        return index;
    }

    zsw_alarm_t removed = alarms[index];
    memset(&alarms[index], 0, sizeof(alarms[index]));
    int ret = start_earliest_alarm(false);
    if (ret < 0) {
        alarms[index] = removed;
        int restore_ret = start_earliest_alarm(true);
        if (restore_ret < 0) {
            LOG_ERR("Failed to restore RTC alarm: %d", restore_ret);
            schedule_alarm_retry();
        }
    }
    k_mutex_unlock(&alarm_mutex);
    return ret;
}

static int find_earliest_alarm(void)
{
    zsw_alarm_t *earliest_alarm = NULL;
    int index = -1;

    for (int i = 0; i < ZSW_MAX_ALARMS; i++) {
        if (alarms[i].used && alarms[i].enabled &&
            (earliest_alarm == NULL || compare(&alarms[i], earliest_alarm) < 0)) {
            earliest_alarm = &alarms[i];
            index = i;
        }
    }

    return index;
}

/* Called with alarm_mutex held; RTC programming and slot changes are serialized. */
static int start_earliest_alarm(bool force)
{
    zsw_alarm_t now = {0};
    int index = find_earliest_alarm();
    int ret;

    if (index >= 0) {
        ret = rtc_get_time(rtc, &now.expiry_time);
        if (ret != 0) {
            LOG_ERR("Failed to read RTC while scheduling: %d", ret);
            return ret;
        }
        if (compare(&alarms[index], &now) <= 0) {
            return schedule_alarm_work();
        }
        // The RTC does not support month and year alarms, so cache the full date.
        if (!force && alarm_armed && compare(&armed_alarm, &alarms[index]) == 0) {
            return 0;
        }
    } else if (!force && !alarm_armed) {
        return 0;
    }

    // Cancel alarm
    ret = rtc_alarm_set_callback(rtc, 0, NULL, NULL);
    if (ret == 0) {
        ret = rtc_alarm_set_time(rtc, 0, 0, NULL);
    }
    if (ret != 0) {
        alarm_armed = false;
        LOG_ERR("Failed to disable RTC alarm: %d", ret);
        return ret;
    }
    alarm_armed = false;
    if (index < 0) {
        return 0;
    }

    struct rtc_time *expiry = &alarms[index].expiry_time;

    ret = rtc_alarm_set_callback(rtc, 0, rtc_alarm_triggered_callback, NULL);
    if (ret == 0) {
        ret = rtc_alarm_set_time(rtc, 0, RTC_ALARM_MASK, expiry);
    }
    if (ret != 0) {
        LOG_ERR("Failed to program RTC alarm: %d", ret);
        return ret;
    }
    armed_alarm = alarms[index];
    alarm_armed = true;

    ret = rtc_get_time(rtc, &now.expiry_time);
    if (ret != 0) {
        LOG_ERR("Failed to recheck RTC alarm: %d", ret);
        return ret;
    }
    if (compare(&alarms[index], &now) <= 0) {
        return schedule_alarm_work();
    }
    return 0;
}

static void handle_rtc_alarm_triggered_work(struct k_work *work)
{
    struct {
        alarm_cb callback;
        void *user_data;
    } due[ZSW_MAX_ALARMS];
    zsw_alarm_t now = {0};
    int count = 0;

    ARG_UNUSED(work);
    k_mutex_lock(&alarm_mutex, K_FOREVER);
    int ret = rtc_get_time(rtc, &now.expiry_time);
    if (ret != 0) {
        LOG_ERR("Failed to read RTC for alarm dispatch: %d", ret);
        schedule_alarm_retry();
        k_mutex_unlock(&alarm_mutex);
        return;
    }

    for (int i = 0; i < ZSW_MAX_ALARMS; i++) {
        if (alarms[i].used && alarms[i].enabled && compare(&alarms[i], &now) <= 0) {
            due[count].callback = alarms[i].cb;
            due[count].user_data = alarms[i].user_data;
            count++;
            memset(&alarms[i], 0, sizeof(alarms[i]));
        }
    }
    // An early monthly RTC match must remain armed for the intended date.
    ret = start_earliest_alarm(count > 0);
    if (ret < 0) {
        LOG_ERR("Failed to start next alarm: %d", ret);
        schedule_alarm_retry();
    }
    k_mutex_unlock(&alarm_mutex);

    for (int i = 0; i < count; i++) {
        due[i].callback(due[i].user_data);
    }
}

static void rtc_alarm_triggered_callback(const struct device *dev, uint16_t id, void *user_data)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(id);
    ARG_UNUSED(user_data);
    schedule_alarm_work();
}

static int find_free_alarm_slot(void)
{
    for (int i = 0; i < ZSW_MAX_ALARMS; i++) {
        if (!alarms[i].used) {
            return i;
        }
    }
    return -ENOMEM;
}

static int compare(const void *rtc_time_a, const void *rtc_time_b)
{
    zsw_alarm_t *a = (zsw_alarm_t *)rtc_time_a;
    zsw_alarm_t *b = (zsw_alarm_t *)rtc_time_b;

    if (a != NULL && b == NULL) {
        return 1;
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

static void copy_rtc_time_to_tm(struct rtc_time *rtc_time, struct tm *tm_time)
{
    tm_time->tm_year = rtc_time->tm_year;
    tm_time->tm_mon = rtc_time->tm_mon;
    tm_time->tm_mday = rtc_time->tm_mday;
    tm_time->tm_hour = rtc_time->tm_hour;
    tm_time->tm_min = rtc_time->tm_min;
    tm_time->tm_sec = rtc_time->tm_sec;
    tm_time->tm_isdst = rtc_time->tm_isdst;
    tm_time->tm_wday = rtc_time->tm_wday;
    tm_time->tm_yday = rtc_time->tm_yday;
}

static void copy_tm_to_rtc_time(struct tm *tm_time, struct rtc_time *rtc_time)
{
    rtc_time->tm_year = tm_time->tm_year;
    rtc_time->tm_mon = tm_time->tm_mon;
    rtc_time->tm_mday = tm_time->tm_mday;
    rtc_time->tm_hour = tm_time->tm_hour;
    rtc_time->tm_min = tm_time->tm_min;
    rtc_time->tm_sec = tm_time->tm_sec;
    rtc_time->tm_isdst = tm_time->tm_isdst;
    rtc_time->tm_wday = tm_time->tm_wday;
    rtc_time->tm_yday = tm_time->tm_yday;
}

static int zsw_alarm_init(void)
{
    memset(alarms, 0, sizeof(alarms));
    if (!zsw_clock_rtc_available()) {
        LOG_WRN("RTC not available, alarm subsystem disabled");
        return 0;
    }

    return start_earliest_alarm(true);
}

SYS_INIT(zsw_alarm_init, APPLICATION, 2); // Just after zsw_periodic_event init
