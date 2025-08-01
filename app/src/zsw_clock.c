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

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "zsw_clock.h"
#include "events/zsw_periodic_event.h"

#if CONFIG_RTC
#include <zephyr/drivers/rtc.h>
#else
#include "zsw_retained_ram_storage.h"
#endif

#if CONFIG_RTC
static const struct device *const rtc = DEVICE_DT_GET(DT_ALIAS(rtc));
// Needed to get rid of strptime warning
char *strptime(const char *buf, const char *format, struct tm *tm);
#else
static void zbus_periodic_slow_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_LISTENER_DEFINE(zsw_clock_lis, zbus_periodic_slow_callback);
#endif

LOG_MODULE_REGISTER(zsw_clock, LOG_LEVEL_WRN);

#ifndef CONFIG_RTC

static time_t zsw_clock_get_time_unix(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return tv.tv_sec;
}

static void zbus_periodic_slow_callback(const struct zbus_channel *chan)
{
    retained.current_time_seconds = zsw_clock_get_time_unix();
    zsw_retained_ram_update();
}
#endif

void zsw_clock_set_time(zsw_timeval_t *ztm)
{
#if CONFIG_RTC
    rtc_set_time(rtc, &ztm->tm);
#else
    struct timespec tspec;

    tspec.tv_nsec = 0;
    tspec.tv_sec = mktime(&ztm->tm);

    clock_settime(CLOCK_REALTIME, &tspec);
#endif
}

void zsw_clock_get_time(zsw_timeval_t *ztm)
{
#if CONFIG_RTC
    struct rtc_time tm;

    memset(&tm, 0, sizeof(struct rtc_time));
    if (rtc_get_time(rtc, &tm)) {
        // Set the time struct to zero to prevent invalid values
        // in case the time reading fails.
        memset(ztm, 0, sizeof(zsw_timeval_t));

        return;
    }
    memcpy(ztm, &tm, sizeof(struct rtc_time));
#else
    struct tm *tm;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    memcpy(ztm, tm, sizeof(struct tm));
#endif

    // Add 1900 to the year because we want to count from 1900
    ztm->tm.tm_year += 1900;
}

void zsw_clock_set_timezone(char *tz)
{
    if (strlen(tz) > 0) {
        setenv("TZ", tz, 1);
        tzset();
    }
}

void zsw_timeval_to_tm(zsw_timeval_t *ztm, struct tm *tm)
{
    tm->tm_sec = ztm->tm.tm_sec;
    tm->tm_min = ztm->tm.tm_min;
    tm->tm_hour = ztm->tm.tm_hour;
    tm->tm_mday = ztm->tm.tm_mday;
    tm->tm_mon = ztm->tm.tm_mon;
    tm->tm_year = ztm->tm.tm_year - 1900;
    tm->tm_wday = ztm->tm.tm_wday;
    tm->tm_yday = ztm->tm.tm_yday;
    tm->tm_isdst = ztm->tm.tm_isdst;
}

static int zsw_clock_init(void)
{
#if CONFIG_RTC
    if (!device_is_ready(rtc)) {
        LOG_ERR("Device not ready!");
        return -EBUSY;
    }
    struct rtc_time tm;

    memset(&tm, 0, sizeof(struct rtc_time));
    if (rtc_get_time(rtc, &tm) == -ENODATA) {
        // Parse __DATE__ and __TIME__ to set RTC to build time
        struct tm build_tm = {0};
        char build_time_str[32];
        snprintf(build_time_str, sizeof(build_time_str), "%s %s", __DATE__, __TIME__);
        strptime(build_time_str, "%b %d %Y %H:%M:%S", &build_tm);

        tm.tm_sec  = build_tm.tm_sec;
        tm.tm_min  = build_tm.tm_min;
        tm.tm_hour = build_tm.tm_hour;
        tm.tm_mday = build_tm.tm_mday;
        tm.tm_mon  = build_tm.tm_mon;
        tm.tm_year = build_tm.tm_year - 1900; // RTC expects year since 1900
        tm.tm_wday = build_tm.tm_wday;
        tm.tm_yday = build_tm.tm_yday;
        tm.tm_isdst = build_tm.tm_isdst;

        rtc_set_time(rtc, &tm);
#ifdef CONFIG_ARCH_POSIX
        struct tm *tp;
        time_t t;
        t = time(NULL);
        tp = localtime(&t);
        t = mktime(tp);
        rtc_set_time(rtc, (struct rtc_time *)tp);
#else
        rtc_set_time(rtc, &tm);
#endif
    }
#else
    struct timespec tspec;

    tspec.tv_sec = retained.current_time_seconds;
    tspec.tv_nsec = 0;

    clock_settime(CLOCK_REALTIME, &tspec);
    zsw_clock_set_timezone(retained.timezone);

    zsw_periodic_chan_add_obs(&periodic_event_1s_chan, &zsw_clock_lis);
#endif

    return 0;
}

SYS_INIT(zsw_clock_init, APPLICATION, 2);