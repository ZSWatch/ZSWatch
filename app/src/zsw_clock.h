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
    struct rtc_time tm;     /**< Modified time object with 1900 added to the year*/
#else
    struct tm   tm;         /**< Modified time object with 1900 added to the year*/
#endif
    uint32_t    tv_usec;
} zsw_timeval_t;

/**
 * Sets the time of the clock.
 *
 * @param ztm A pointer to a `zsw_timeval_t` structure representing the desired time.
 */
void zsw_clock_set_time(zsw_timeval_t *ztm);

/**
 * @brief Retrieves the current time from the.
 *
 * This function retrieves the current time from the clock and stores it in the provided zsw_timeval_t structure.
 *
 * @param ztm Pointer to the zsw_timeval_t structure where the current time will be stored.
 */
void zsw_clock_get_time(zsw_timeval_t *ztm);

/**
 * @brief Sets the timezone for the ZSW clock.
 *
 * This function sets the timezone for the clock. The timezone is specified
 * using the provided string in setenv and tzset format.
 *
 * @param tz The timezone string to set.
 */
void zsw_clock_set_timezone(char *tz);