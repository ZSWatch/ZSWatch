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

#include "drivers/zsw_hr.h"

#include <errno.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/pm/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/max32664c.h>

LOG_MODULE_REGISTER(zsw_hr, LOG_LEVEL_DBG);

#define ZSW_HR_THREAD_STACK_SIZE 1024
#define ZSW_HR_THREAD_PRIORITY   K_PRIO_PREEMPT(10)

static const struct device *const sensor_hub = DEVICE_DT_GET_OR_NULL(DT_ALIAS(hr_hub));

static K_THREAD_STACK_DEFINE(hr_thread_stack, ZSW_HR_THREAD_STACK_SIZE);
static K_MUTEX_DEFINE(hr_lock);

struct zsw_hr_state {
    bool initialized;
    bool running;
    zsw_hr_mode_t mode;
    uint32_t sample_interval_ms;
    struct zsw_hr_sample last_sample;
    zsw_hr_sample_cb_t callback;
    void *callback_user_data;
    k_tid_t thread_id;
    struct k_thread thread;
};

static struct zsw_hr_state hr_state = {
    .sample_interval_ms = ZSW_HR_DEFAULT_INTERVAL_MS,
    .mode = ZSW_HR_MODE_PERIODIC,
};

static uint32_t sanitize_interval(zsw_hr_mode_t mode, uint32_t interval_ms)
{
    uint32_t interval = interval_ms;

    if (interval == 0) {
        interval = (mode == ZSW_HR_MODE_CONTINUOUS) ? ZSW_HR_REALTIME_INTERVAL_MS
                                                    : ZSW_HR_DEFAULT_INTERVAL_MS;
    }

    if (interval < ZSW_HR_MIN_INTERVAL_MS) {
        interval = ZSW_HR_MIN_INTERVAL_MS;
    }

    return interval;
}

static int configure_sensor_mode(void)
{
    int rc;
    struct sensor_value mode = {
#ifdef CONFIG_MAX32664C_USE_EXTENDED_REPORTS
        .val1 = MAX32664C_OP_MODE_ALGO_AEC_EXT,
#else
        .val1 = MAX32664C_OP_MODE_ALGO_AEC,
#endif
        .val2 = MAX32664C_ALGO_MODE_CONT_HRM,
    };

#if IS_ENABLED(CONFIG_PM_DEVICE)
    rc = pm_device_action_run(sensor_hub, PM_DEVICE_ACTION_RESUME);
    if (rc) {
        LOG_ERR("Failed to resume heart rate hub: %d", rc);
        //return rc;
    }
#endif

    rc = sensor_attr_set(sensor_hub, SENSOR_CHAN_MAX32664C_HEARTRATE, SENSOR_ATTR_MAX32664C_OP_MODE,
                         &mode);
    if (rc) {
        LOG_ERR("Failed to set MAX32664C op mode: %d", rc);
        return rc;
    }

    return 0;
}

static int disable_sensor(void)
{
    int rc;
    struct sensor_value mode = {
        .val1 = MAX32664C_OP_MODE_IDLE,
        .val2 = 0,
    };

    rc = sensor_attr_set(sensor_hub, SENSOR_CHAN_MAX32664C_HEARTRATE, SENSOR_ATTR_MAX32664C_OP_MODE,
                         &mode);
    if (rc) {
        LOG_ERR("Failed to stop MAX32664C: %d", rc);
    }

#if IS_ENABLED(CONFIG_PM_DEVICE)
    int pm_rc = pm_device_action_run(sensor_hub, PM_DEVICE_ACTION_SUSPEND);
    if (pm_rc) {
        LOG_WRN("Failed to suspend heart rate hub: %d", pm_rc);
        if (rc == 0) {
            rc = pm_rc;
        }
    }
#endif

    return rc;
}

static int fetch_sample(struct zsw_hr_sample *sample)
{
    int rc;
    struct sensor_value hr;
    struct sensor_value spo2;
    struct sensor_value rr;
    struct sensor_value skin_contact;
    struct sensor_value activity;

    rc = sensor_sample_fetch(sensor_hub);
    if (rc) {
        return rc;
    }

    rc = sensor_channel_get(sensor_hub, SENSOR_CHAN_MAX32664C_HEARTRATE, &hr);
    if (rc) {
        return rc;
    }

    rc = sensor_channel_get(sensor_hub, SENSOR_CHAN_MAX32664C_BLOOD_OXYGEN_SATURATION, &spo2);
    if (rc) {
        return rc;
    }

    rc = sensor_channel_get(sensor_hub, SENSOR_CHAN_MAX32664C_RESPIRATION_RATE, &rr);
    if (rc) {
        return rc;
    }

    rc = sensor_channel_get(sensor_hub, SENSOR_CHAN_MAX32664C_SKIN_CONTACT, &skin_contact);
    if (rc) {
        return rc;
    }

    rc = sensor_channel_get(sensor_hub, SENSOR_CHAN_MAX32664C_ACTIVITY, &activity);
    if (rc) {
        return rc;
    }

    sample->timestamp_ms = k_uptime_get();
    sample->heart_rate_bpm = hr.val1;
    sample->heart_rate_confidence = (uint8_t)hr.val2;
    sample->spo2_percent = spo2.val1;
    sample->spo2_confidence = sensor_max32664c_value_to_confidence(&spo2);
    sample->respiration_rate = rr.val1;
    sample->respiration_confidence = (uint8_t)rr.val2;
    sample->skin_contact = (skin_contact.val1 != 0);
    sample->activity_class = activity.val1;

    return 0;
}

static void hr_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    struct zsw_hr_sample sample;
    uint32_t failure_count = 0;

    while (true) {
        uint32_t interval_ms;
        zsw_hr_sample_cb_t cb;
        void *cb_data;
        bool running;
        int rc;

        k_mutex_lock(&hr_lock, K_FOREVER);
        running = hr_state.running;
        interval_ms = hr_state.sample_interval_ms;
        cb = hr_state.callback;
        cb_data = hr_state.callback_user_data;
        k_mutex_unlock(&hr_lock);

        if (!running) {
            break;
        }

        if (!sensor_hub) {
            LOG_ERR("Heart rate hub missing");
            break;
        }

        rc = fetch_sample(&sample);
        if (rc == 0) {
            failure_count = 0;

            k_mutex_lock(&hr_lock, K_FOREVER);
            hr_state.last_sample = sample;
            cb = hr_state.callback;
            cb_data = hr_state.callback_user_data;
            k_mutex_unlock(&hr_lock);

            if (cb) {
                cb(&sample, cb_data);
            }
        } else {
            failure_count++;
            if ((failure_count == 1) || (failure_count % 50 == 0)) {
                LOG_WRN("Failed to read heart rate sample (%d, %u)", rc, failure_count);
            }
        }

        k_msleep(interval_ms);
    }
}

int zsw_hr_start(const struct zsw_hr_config *config)
{
    int rc;
    zsw_hr_mode_t mode;
    uint32_t interval_ms;

    if (!sensor_hub) {
        return -ENODEV;
    }

    if (!device_is_ready(sensor_hub)) {
        LOG_ERR("Sensor hub not ready");
        return -ENODEV;
    }

    if (config && config->mode > ZSW_HR_MODE_CONTINUOUS) {
        return -EINVAL;
    }

    mode = config ? config->mode : hr_state.mode;
    interval_ms = sanitize_interval(mode,
                                    config ? config->sample_interval_ms : hr_state.sample_interval_ms);

    k_mutex_lock(&hr_lock, K_FOREVER);
    hr_state.mode = mode;
    hr_state.sample_interval_ms = interval_ms;

    if (hr_state.running) {
        k_mutex_unlock(&hr_lock);
        LOG_DBG("Heart rate sensor already running, updated interval to %u ms", interval_ms);
        return 0;
    }

    hr_state.running = true;
    k_mutex_unlock(&hr_lock);

    rc = configure_sensor_mode();
    if (rc) {
        k_mutex_lock(&hr_lock, K_FOREVER);
        hr_state.running = false;
        k_mutex_unlock(&hr_lock);
        disable_sensor();
        return rc;
    }

    memset(&hr_state.last_sample, 0, sizeof(hr_state.last_sample));

    hr_state.thread_id = k_thread_create(&hr_state.thread, hr_thread_stack,
                                         K_THREAD_STACK_SIZEOF(hr_thread_stack), hr_thread, NULL,
                                         NULL, NULL, ZSW_HR_THREAD_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(hr_state.thread_id, "zsw_hr");

    LOG_INF("Heart rate sampling started (%s, %u ms)",
            mode == ZSW_HR_MODE_CONTINUOUS ? "continuous" : "periodic", interval_ms);

    return 0;
}

int zsw_hr_stop(void)
{
    int rc = 0;
    k_tid_t tid;

    if (!sensor_hub) {
        return -ENODEV;
    }

    k_mutex_lock(&hr_lock, K_FOREVER);
    if (!hr_state.running) {
        k_mutex_unlock(&hr_lock);
        return -EALREADY;
    }

    hr_state.running = false;
    tid = hr_state.thread_id;
    k_mutex_unlock(&hr_lock);

    if (tid) {
        k_wakeup(tid);
        rc = k_thread_join(tid, K_FOREVER);
        if (rc) {
            LOG_WRN("Failed to join heart rate thread: %d", rc);
        }
    }

    k_mutex_lock(&hr_lock, K_FOREVER);
    hr_state.thread_id = NULL;
    k_mutex_unlock(&hr_lock);

    rc = disable_sensor();

    LOG_INF("Heart rate sampling stopped");

    return rc;
}

int zsw_hr_set_sampling_interval(uint32_t interval_ms)
{
    k_tid_t tid;

    k_mutex_lock(&hr_lock, K_FOREVER);
    hr_state.sample_interval_ms = sanitize_interval(hr_state.mode, interval_ms);
    tid = hr_state.thread_id;
    k_mutex_unlock(&hr_lock);

    if (tid) {
        k_wakeup(tid);
    }

    return 0;
}

int zsw_hr_get_latest(struct zsw_hr_sample *sample)
{
    if (sample == NULL) {
        return -EINVAL;
    }

    k_mutex_lock(&hr_lock, K_FOREVER);
    *sample = hr_state.last_sample;
    bool has_data = hr_state.last_sample.timestamp_ms != 0;
    k_mutex_unlock(&hr_lock);

    return has_data ? 0 : -ENODATA;
}

int zsw_hr_register_callback(zsw_hr_sample_cb_t callback, void *user_data)
{
    k_mutex_lock(&hr_lock, K_FOREVER);
    hr_state.callback = callback;
    hr_state.callback_user_data = user_data;
    k_mutex_unlock(&hr_lock);

    if (hr_state.thread_id) {
        k_wakeup(hr_state.thread_id);
    }

    return 0;
}

bool zsw_hr_is_running(void)
{
    bool running;

    k_mutex_lock(&hr_lock, K_FOREVER);
    running = hr_state.running;
    k_mutex_unlock(&hr_lock);

    return running;
}

static int zsw_hr_init(const struct device *unused)
{
    ARG_UNUSED(unused);

    if (!sensor_hub) {
        LOG_WRN("Heart rate hub alias not defined");
        return -ENODEV;
    }

    if (!device_is_ready(sensor_hub)) {
        LOG_WRN("Heart rate hub not ready");
        return -ENODEV;
    }

    k_mutex_lock(&hr_lock, K_FOREVER);
    hr_state.initialized = true;
    k_mutex_unlock(&hr_lock);

    LOG_INF("Heart rate hub ready");

    return 0;
}

SYS_INIT(zsw_hr_init, APPLICATION, CONFIG_DEFAULT_CONFIGURATION_DRIVER_INIT_PRIORITY);
