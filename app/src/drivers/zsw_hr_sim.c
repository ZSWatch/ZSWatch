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

/* Simulated heart rate sensor for native_sim. Produces on-skin reports with a slowly
 * varying heart rate, so the HR manager, Tiles and the HR app have data to show.
 */

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "zsw_hr.h"

LOG_MODULE_REGISTER(zsw_hr_sim, LOG_LEVEL_INF);

static void sim_work_handler(struct k_work *work);

static K_WORK_DELAYABLE_DEFINE(sim_work, sim_work_handler);
static K_MUTEX_DEFINE(sim_lock);

static bool running;
static uint32_t interval_ms = ZSW_HR_DEFAULT_INTERVAL_MS;
static uint32_t tick;
static struct zsw_hr_sample latest;
static bool has_latest;
static zsw_hr_sample_cb_t sample_cb;
static void *sample_cb_data;

static void sim_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);
    // Triangle wave between 64 and 82 bpm with a period of 36 reports.
    static const int8_t wave[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    struct zsw_hr_sample sample = {
        .heart_rate_bpm = 64 + 2 * wave[tick % ARRAY_SIZE(wave)] + (tick / ARRAY_SIZE(wave)) % 2,
        .heart_rate_confidence = 95,
        .spo2_percent = 97 + tick % 2,
        .spo2_confidence = 90,
        .skin_contact = true,
        .skin_contact_state = 3,
        .timestamp_ms = k_uptime_get(),
    };
    zsw_hr_sample_cb_t cb;
    void *cb_data;

    k_mutex_lock(&sim_lock, K_FOREVER);
    if (!running) {
        k_mutex_unlock(&sim_lock);
        return;
    }
    tick++;
    latest = sample;
    has_latest = true;
    cb = sample_cb;
    cb_data = sample_cb_data;
    k_work_reschedule(&sim_work, K_MSEC(interval_ms));
    k_mutex_unlock(&sim_lock);

    if (cb) {
        cb(&sample, cb_data);
    }
}

int zsw_hr_start(const struct zsw_hr_config *config)
{
    k_mutex_lock(&sim_lock, K_FOREVER);
    interval_ms = config && config->sample_interval_ms ? MAX(config->sample_interval_ms, ZSW_HR_MIN_INTERVAL_MS) :
                  ZSW_HR_DEFAULT_INTERVAL_MS;
    running = true;
    k_work_reschedule(&sim_work, K_MSEC(interval_ms));
    k_mutex_unlock(&sim_lock);
    LOG_INF("Simulated HR started, %u ms", interval_ms);
    return 0;
}

int zsw_hr_stop(void)
{
    k_mutex_lock(&sim_lock, K_FOREVER);
    running = false;
    k_work_cancel_delayable(&sim_work);
    k_mutex_unlock(&sim_lock);
    return 0;
}

int zsw_hr_set_sampling_interval(uint32_t new_interval_ms)
{
    k_mutex_lock(&sim_lock, K_FOREVER);
    interval_ms = MAX(new_interval_ms, ZSW_HR_MIN_INTERVAL_MS);
    k_mutex_unlock(&sim_lock);
    return 0;
}

int zsw_hr_get_latest(struct zsw_hr_sample *sample)
{
    int rc = -ENODATA;

    k_mutex_lock(&sim_lock, K_FOREVER);
    if (has_latest) {
        *sample = latest;
        rc = 0;
    }
    k_mutex_unlock(&sim_lock);
    return rc;
}

int zsw_hr_register_callback(zsw_hr_sample_cb_t callback, void *user_data)
{
    k_mutex_lock(&sim_lock, K_FOREVER);
    sample_cb = callback;
    sample_cb_data = user_data;
    k_mutex_unlock(&sim_lock);
    return 0;
}

bool zsw_hr_is_available(void)
{
    return true;
}

bool zsw_hr_is_running(void)
{
    return running;
}
