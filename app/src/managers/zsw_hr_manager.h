/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <zephyr/toolchain.h>
#include "drivers/zsw_hr.h"

typedef enum {
    ZSW_HR_SETTING_OFF,
    ZSW_HR_SETTING_BACKGROUND,
    ZSW_HR_SETTING_CONTINUOUS,
} zsw_hr_setting_t;

typedef enum {
    ZSW_HR_STATUS_OFF,
    ZSW_HR_STATUS_WAITING_MOTION,
    ZSW_HR_STATUS_WAITING_INTERVAL,
    ZSW_HR_STATUS_MEASURING,
    ZSW_HR_STATUS_ERROR,
    ZSW_HR_STATUS_UNAVAILABLE,
} zsw_hr_status_t;

struct zsw_hr_manager_snapshot {
    zsw_hr_setting_t mode;
    zsw_hr_status_t status;
    bool manual;
    bool has_sample;
    bool has_report;
    int error;
    uint32_t sequence;
    int64_t next_measurement_ms;
    struct zsw_hr_sample sample;
    /* Latest unfiltered report */
    struct zsw_hr_sample report;
};

#define ZSW_HR_HISTORY_CAPACITY 32
struct zsw_hr_history_point {
    int64_t timestamp_ms;
    uint32_t sequence;
    int16_t bpm;
    uint8_t confidence;
};

#ifdef CONFIG_ZSW_HR_MANAGER
int zsw_hr_manager_init(void);
int zsw_hr_manager_set_mode(zsw_hr_setting_t mode);
int zsw_hr_manager_measure_now(void);
int zsw_hr_manager_cancel_measurement(void);
void zsw_hr_manager_get_snapshot(struct zsw_hr_manager_snapshot *snapshot);
size_t zsw_hr_manager_get_history(struct zsw_hr_history_point *points, size_t capacity);
#else
static inline size_t zsw_hr_manager_get_history(struct zsw_hr_history_point *points, size_t capacity)
{
    ARG_UNUSED(points);
    ARG_UNUSED(capacity);
    return 0;
}
static inline int zsw_hr_manager_init(void)
{
    return -ENODEV;
}
static inline int zsw_hr_manager_set_mode(zsw_hr_setting_t mode)
{
    ARG_UNUSED(mode);
    return -ENODEV;
}
static inline int zsw_hr_manager_measure_now(void)
{
    return -ENODEV;
}
static inline int zsw_hr_manager_cancel_measurement(void)
{
    return -ENODEV;
}
static inline void zsw_hr_manager_get_snapshot(struct zsw_hr_manager_snapshot *snapshot)
{
    *snapshot = (struct zsw_hr_manager_snapshot) {
        .status = ZSW_HR_STATUS_UNAVAILABLE
    };
}
#endif
