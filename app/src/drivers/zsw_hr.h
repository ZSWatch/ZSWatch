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

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ZSW_HR_REALTIME_INTERVAL_MS 10U
#define ZSW_HR_DEFAULT_INTERVAL_MS 1000U
#define ZSW_HR_MIN_INTERVAL_MS 10U

typedef enum {
    ZSW_HR_MODE_PERIODIC = 0,
    ZSW_HR_MODE_CONTINUOUS,
} zsw_hr_mode_t;

struct zsw_hr_config {
    zsw_hr_mode_t mode;
    /* 0 uses mode dependent default. */
    uint32_t sample_interval_ms;
};

struct zsw_hr_sample {
    int32_t heart_rate_bpm;
    uint8_t heart_rate_confidence;
    int32_t spo2_percent;
    uint8_t spo2_confidence;
    int32_t respiration_rate;
    uint8_t respiration_confidence;
    bool skin_contact;
    int32_t activity_class;
    int64_t timestamp_ms;
};

typedef void (*zsw_hr_sample_cb_t)(const struct zsw_hr_sample *sample, void *user_data);

int zsw_hr_start(const struct zsw_hr_config *config);
int zsw_hr_stop(void);
int zsw_hr_set_sampling_interval(uint32_t interval_ms);
int zsw_hr_get_latest(struct zsw_hr_sample *sample);
int zsw_hr_register_callback(zsw_hr_sample_cb_t callback, void *user_data);
bool zsw_hr_is_running(void);

#ifdef __cplusplus
}
#endif
