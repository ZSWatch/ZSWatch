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

#pragma once

#include <zephyr/drivers/rtc.h>

#include <inttypes.h>
#include <time.h>

#if CONFIG_RTC
typedef void (*zsw_clock_on_update)(void);
#endif

typedef struct {
#if CONFIG_RTC
    struct rtc_time
        tm;                 /**< Modified time object with 1900 added to the year and the month increased by one. */
#else
    struct tm   tm;                 /**< Modified time object with 1900 added to the year and the month increased by one. */
#endif
    uint32_t    tv_usec;
} zsw_timeval_t;

/** @brief
 *              NOTE: This function needs the time as seconds
 *  @param ztm
 */
void zsw_clock_set_time(zsw_timeval_t *ztm);

/** @brief
 *  @param ztm
 */
void zsw_clock_get_time(zsw_timeval_t *ztm);

/** @brief
 *  @param tz
 */
void zsw_clock_set_timezone(char *tz);