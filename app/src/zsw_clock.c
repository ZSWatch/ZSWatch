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

#include <zsw_clock.h>
#include <inttypes.h>
#include <sys/time.h>
#include <time.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <unistd.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <zsw_retained_ram_storage.h>

#include "events/zsw_periodic_event.h"

static void zbus_periodic_slow_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(periodic_event_slow_chan);
ZBUS_LISTENER_DEFINE(zsw_clock_lis, zbus_periodic_slow_callback);

void zsw_clock_init(uint64_t start_time_seconds, char *timezone)
{
    struct timespec tspec;
    tspec.tv_sec = start_time_seconds;
    tspec.tv_nsec = 0;
    clock_settime(CLOCK_REALTIME, &tspec);
    if (timezone && strlen(timezone) > 0) {
        setenv("TZ", timezone, 1);
        tzset();
    }

    zsw_periodic_chan_add_obs(&periodic_event_slow_chan, &zsw_clock_lis);
}

struct tm *zsw_clock_get_time(void)
{
    struct timeval tv;
    struct tm *tm;
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    return tm;
}

time_t zsw_clock_get_time_unix(void)
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
