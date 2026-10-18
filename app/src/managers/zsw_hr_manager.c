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

#include "zsw_hr_manager.h"

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/zbus/zbus.h>
#include "events/accel_event.h"
#include "sensors/zsw_imu.h"
#include "zsw_settings.h"

LOG_MODULE_REGISTER(zsw_hr_manager, LOG_LEVEL_INF);

#define BACKGROUND_INTERVAL_MS (CONFIG_ZSW_HR_BACKGROUND_INTERVAL_SECONDS * 1000LL)
#define ATTEMPT_TIMEOUT_MS (CONFIG_ZSW_HR_ATTEMPT_TIMEOUT_SECONDS * 1000LL)
#define RETRY_DELAY_MS 30000LL
#define MOTION_QUALIFY_MS 2000LL
#define MAX_REPORT_AGE_MS 3000LL

typedef enum {
    HR_ACTION_NONE,
    HR_ACTION_START,
    HR_ACTION_STOP,
} hr_action_t;

typedef struct {
    struct zsw_hr_manager_snapshot state;
    bool ready;
    // The sensor is running
    bool active;
    // A failed sensor stop is retried after retry_after
    bool stop_pending;
    // Stop the running attempt, for example after a mode change
    bool cancel_attempt;
    bool has_pending_sample;
    struct zsw_hr_sample pending_sample;
    uint8_t good_reports;
    // Start of the next background measurement
    int64_t next_due;
    // No new attempt before this time, after errors or an aborted attempt
    int64_t retry_after;
    // End of the current one-shot attempt
    int64_t deadline;
    int64_t moving_since;
    int64_t manual_since;
    int64_t last_processed;
    // Last fresh report from the sensor, used to detect a stalled sensor
    int64_t last_frame_ms;
    struct zsw_hr_history_point history[ZSW_HR_HISTORY_CAPACITY];
    size_t history_count;
    size_t history_next;
} hr_manager_t;

static K_MUTEX_DEFINE(manager_lock);
static K_MUTEX_DEFINE(mode_save_lock);
static K_SEM_DEFINE(manager_wake, 0, 1);
static hr_manager_t mgr = {
    .state = {.mode = ZSW_HR_SETTING_BACKGROUND, .status = ZSW_HR_STATUS_WAITING_MOTION},
};
ZBUS_CHAN_DECLARE(hr_sample_data_chan);

static void motion_event(const struct zbus_channel *chan)
{
    const struct accel_event *event = zbus_chan_const_msg(chan);
    if (event->data.type == ZSW_IMU_EVT_TYPE_ANY_MOTION ||
        event->data.type == ZSW_IMU_EVT_TYPE_NO_MOTION) {
        k_sem_give(&manager_wake);
    }
}
ZBUS_CHAN_DECLARE(accel_data_chan);
ZBUS_LISTENER_DEFINE(hr_manager_motion_listener, motion_event);
ZBUS_CHAN_ADD_OBS(accel_data_chan, hr_manager_motion_listener, 2);

static void sample_received(const struct zsw_hr_sample *sample, void *user_data)
{
    ARG_UNUSED(user_data);
    k_mutex_lock(&manager_lock, K_FOREVER);
    mgr.pending_sample = *sample;
    mgr.has_pending_sample = true;
    k_mutex_unlock(&manager_lock);
    k_sem_give(&manager_wake);
}

static bool sample_accepted(const struct zsw_hr_sample *sample)
{
    return sample->skin_contact_state == 3 &&
           sample->heart_rate_confidence >= CONFIG_ZSW_HR_MIN_CONFIDENCE &&
           sample->heart_rate_confidence <= 100 &&
           sample->heart_rate_bpm >= 30 && sample->heart_rate_bpm <= 210;
}

static void add_to_history_locked(const struct zsw_hr_sample *sample)
{
    mgr.history[mgr.history_next] = (struct zsw_hr_history_point) {
        .timestamp_ms = sample->timestamp_ms, .sequence = mgr.state.sequence,
        .bpm = sample->heart_rate_bpm, .confidence = sample->heart_rate_confidence,
    };
    mgr.history_next = (mgr.history_next + 1) % ZSW_HR_HISTORY_CAPACITY;
    if (mgr.history_count < ZSW_HR_HISTORY_CAPACITY) {
        mgr.history_count++;
    }
}

/* Returns true when the pending sample completes a measurement. */
static bool process_sample_locked(int64_t now)
{
    if (!mgr.has_pending_sample) {
        return false;
    }
    struct zsw_hr_sample sample = mgr.pending_sample;
    mgr.has_pending_sample = false;
    if (!mgr.active || sample.timestamp_ms <= mgr.last_processed ||
        sample.timestamp_ms <= 0 || sample.timestamp_ms > now) {
        return false;
    }
    mgr.last_processed = sample.timestamp_ms;
    mgr.state.report = sample;
    mgr.state.has_report = true;
    if (now - sample.timestamp_ms > MAX_REPORT_AGE_MS) {
        mgr.good_reports = 0;
        return false;
    }
    mgr.last_frame_ms = now;
    if (!sample_accepted(&sample)) {
        mgr.good_reports = 0;
        return false;
    }
    if (mgr.good_reports < CONFIG_ZSW_HR_GOOD_REPORTS) {
        mgr.good_reports++;
    }
    if (mgr.good_reports < CONFIG_ZSW_HR_GOOD_REPORTS) {
        return false;
    }
    mgr.state.sample = sample;
    mgr.state.has_sample = true;
    mgr.state.sequence++;
    mgr.state.error = 0;
    add_to_history_locked(&sample);
    if (mgr.state.manual && sample.timestamp_ms > mgr.manual_since) {
        mgr.state.manual = false;
        mgr.next_due = now + BACKGROUND_INTERVAL_MS;
    }
    return true;
}

static void apply_mode_locked(zsw_hr_setting_t mode)
{
    if (mgr.state.mode == mode && mode != ZSW_HR_SETTING_OFF) {
        return;
    }
    mgr.state.mode = mode;
    mgr.state.manual = false;
    mgr.cancel_attempt = true;
    mgr.next_due = 0;
    mgr.retry_after = 0;
    mgr.state.error = 0;
}

static int load_mode(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg, void *param)
{
    zsw_hr_setting_t *mode = param;
    uint8_t value;

    ARG_UNUSED(key);
    // A corrupt setting must never select a running mode.
    if (len != sizeof(value) || read_cb(cb_arg, &value, sizeof(value)) != sizeof(value) ||
        value > ZSW_HR_SETTING_CONTINUOUS) {
        *mode = ZSW_HR_SETTING_OFF;
        return -EINVAL;
    }
    *mode = value;
    return 0;
}

int zsw_hr_manager_set_mode(zsw_hr_setting_t mode)
{
    if (mode < ZSW_HR_SETTING_OFF || mode > ZSW_HR_SETTING_CONTINUOUS) {
        return -EINVAL;
    }
    k_mutex_lock(&mode_save_lock, K_FOREVER);
    k_mutex_lock(&manager_lock, K_FOREVER);
    bool changed = mgr.state.mode != mode;
    bool initialized = mgr.ready;
    k_mutex_unlock(&manager_lock);
    int rc = initialized ? 0 : -EAGAIN;
    if (!rc && changed) {
        uint8_t value = mode;
        rc = settings_save_one(ZSW_SETTINGS_HR_MODE, &value, sizeof(value));
    }
    if (!rc) {
        k_mutex_lock(&manager_lock, K_FOREVER);
        apply_mode_locked(mode);
        k_mutex_unlock(&manager_lock);
        k_sem_give(&manager_wake);
    }
    k_mutex_unlock(&mode_save_lock);
    return rc;
}

int zsw_hr_manager_measure_now(void)
{
    k_mutex_lock(&manager_lock, K_FOREVER);
    int rc = !mgr.ready ? -EAGAIN : mgr.state.manual ? -EALREADY : 0;
    if (!rc) {
        mgr.state.manual = true;
        if (!mgr.active && !mgr.stop_pending) {
            mgr.retry_after = 0;
        }
        mgr.manual_since = k_uptime_get();
        if (mgr.active) {
            mgr.deadline = mgr.manual_since + ATTEMPT_TIMEOUT_MS;
        }
    }
    k_mutex_unlock(&manager_lock);
    if (!rc) {
        k_sem_give(&manager_wake);
    }
    return rc;
}

int zsw_hr_manager_cancel_measurement(void)
{
    k_mutex_lock(&manager_lock, K_FOREVER);
    int rc = mgr.state.manual ? 0 : -EALREADY;
    if (!rc) {
        mgr.state.manual = false;
        mgr.cancel_attempt = true;
        mgr.retry_after = k_uptime_get() + RETRY_DELAY_MS;
    }
    k_mutex_unlock(&manager_lock);
    if (!rc) {
        k_sem_give(&manager_wake);
    }
    return rc;
}

void zsw_hr_manager_get_snapshot(struct zsw_hr_manager_snapshot *snapshot)
{
    k_mutex_lock(&manager_lock, K_FOREVER);
    *snapshot = mgr.state;
    k_mutex_unlock(&manager_lock);
}

size_t zsw_hr_manager_get_history(struct zsw_hr_history_point *points, size_t capacity)
{
    if (!points || !capacity) {
        return 0;
    }
    k_mutex_lock(&manager_lock, K_FOREVER);
    size_t count = MIN(capacity, mgr.history_count);
    size_t first = (mgr.history_next + ZSW_HR_HISTORY_CAPACITY - count) % ZSW_HR_HISTORY_CAPACITY;
    for (size_t i = 0; i < count; i++) {
        points[i] = mgr.history[(first + i) % ZSW_HR_HISTORY_CAPACITY];
    }
    k_mutex_unlock(&manager_lock);
    return count;
}

/* Moving for at least MOTION_QUALIFY_MS, so short movements don't start a measurement. */
static bool update_moving_locked(int64_t now)
{
    if (mgr.state.mode == ZSW_HR_SETTING_OFF || zsw_imu_get_motion_state() != ZSW_IMU_MOTION_MOVING) {
        mgr.moving_since = 0;
        return false;
    }
    if (!mgr.moving_since) {
        mgr.moving_since = now;
    }
    return now - mgr.moving_since >= MOTION_QUALIFY_MS;
}

/* Decides whether the running attempt ends, and records why. */
static bool attempt_ends_locked(int64_t now, bool moving, bool continuous, bool completed, bool manual_requested)
{
    bool permitted = mgr.state.manual || moving;
    // The attempt timeout only applies to one-shot and manual measurements.
    bool one_shot = mgr.state.mode != ZSW_HR_SETTING_CONTINUOUS;
    bool expired = (one_shot || mgr.state.manual) && now >= mgr.deadline;
    bool stalled = continuous && now - mgr.last_frame_ms >= ATTEMPT_TIMEOUT_MS;

    if (expired) {
        mgr.state.manual = false;
        mgr.state.error = -ETIMEDOUT;
        if (!continuous) {
            mgr.retry_after = now + RETRY_DELAY_MS;
        }
    }
    if (stalled) {
        mgr.state.error = -ENODATA;
        mgr.state.manual = false;
        mgr.retry_after = now + RETRY_DELAY_MS;
    }

    bool ends = mgr.cancel_attempt || !permitted || stalled || (expired && !continuous) ||
                (completed && !continuous && !mgr.state.manual);

    if (ends && !permitted && !completed && !expired && !manual_requested) {
        // Motion stopped mid-attempt: measure again as soon as the watch moves.
        if (mgr.state.mode == ZSW_HR_SETTING_BACKGROUND) {
            mgr.next_due = 0;
        }
        mgr.retry_after = MAX(mgr.retry_after, now + RETRY_DELAY_MS);
    }
    return ends;
}

static void begin_attempt_locked(int64_t now, bool continuous)
{
    mgr.good_reports = 0;
    mgr.has_pending_sample = false;
    mgr.last_processed = 0;
    mgr.last_frame_ms = now;
    mgr.deadline = now + ATTEMPT_TIMEOUT_MS;
    if (!mgr.state.manual && !continuous) {
        mgr.next_due = now + BACKGROUND_INTERVAL_MS;
    }
}

static void update_status_locked(bool starting, bool moving)
{
    struct zsw_hr_manager_snapshot *state = &mgr.state;

    if (state->error && !mgr.active && !starting) {
        state->status = ZSW_HR_STATUS_ERROR;
    } else if (state->mode == ZSW_HR_SETTING_OFF && !state->manual) {
        state->status = ZSW_HR_STATUS_OFF;
    } else if (mgr.active || starting) {
        state->status = ZSW_HR_STATUS_MEASURING;
    } else if (state->error) {
        state->status = ZSW_HR_STATUS_ERROR;
    } else if (!moving && !state->manual) {
        state->status = ZSW_HR_STATUS_WAITING_MOTION;
    } else {
        state->status = ZSW_HR_STATUS_WAITING_INTERVAL;
    }
    state->next_measurement_ms = MAX(mgr.next_due, mgr.retry_after);
}

/* Returns the next time the manager has something to do, INT64_MAX for nothing. */
static int64_t next_wakeup_locked(int64_t now, bool moving, bool continuous, bool requested, bool due)
{
    int64_t wake_at = INT64_MAX;

    if (mgr.active) {
        wake_at = continuous ? mgr.last_frame_ms + ATTEMPT_TIMEOUT_MS : mgr.deadline;
        if (mgr.state.manual) {
            wake_at = MIN(wake_at, mgr.deadline);
        }
    } else {
        if ((requested || mgr.stop_pending) && now < mgr.retry_after) {
            wake_at = mgr.retry_after;
        }
        if (moving && mgr.state.mode == ZSW_HR_SETTING_BACKGROUND && !due) {
            wake_at = MIN(wake_at, MAX(mgr.next_due, mgr.retry_after));
        }
    }
    if (mgr.moving_since && !moving) {
        wake_at = MIN(wake_at, mgr.moving_since + MOTION_QUALIFY_MS);
    }
    return wake_at;
}

static void stop_sensor(void)
{
    int rc = zsw_hr_stop();

    LOG_INF("HR stopped: %d", rc);
    k_mutex_lock(&manager_lock, K_FOREVER);
    if (rc && rc != -EALREADY) {
        mgr.stop_pending = true;
        mgr.state.error = rc;
        mgr.state.status = ZSW_HR_STATUS_ERROR;
        mgr.retry_after = k_uptime_get() + RETRY_DELAY_MS;
    } else {
        if (mgr.stop_pending) {
            mgr.state.error = 0;
        }
        mgr.stop_pending = false;
    }
    k_mutex_unlock(&manager_lock);
}

static void start_sensor(void)
{
    struct zsw_hr_config config = {.mode = ZSW_HR_MODE_CONTINUOUS, .sample_interval_ms = 1000};
    int rc = zsw_hr_start(&config);

    k_mutex_lock(&manager_lock, K_FOREVER);
    mgr.active = rc == 0;
    mgr.state.error = rc;
    if (rc) {
        mgr.state.manual = false;
        mgr.state.status = ZSW_HR_STATUS_ERROR;
        mgr.retry_after = k_uptime_get() + RETRY_DELAY_MS;
    }
    k_mutex_unlock(&manager_lock);
    LOG_INF("HR started: %d", rc);
}

/*
 * One manager pass: process the latest report, decide whether to start or stop the
 * sensor, and calculate when to run again. Sensor calls are made without the lock.
 * Returns the delay until the next pass in ms, -1 to wait for an event, 0 to run again.
 */
static int64_t run_once(void)
{
    int64_t now = k_uptime_get();
    hr_action_t action = HR_ACTION_NONE;
    struct zsw_hr_sample published;

    k_mutex_lock(&manager_lock, K_FOREVER);
    bool moving = update_moving_locked(now);
    bool continuous = mgr.state.mode == ZSW_HR_SETTING_CONTINUOUS && moving;
    bool manual_requested = mgr.state.manual;
    bool completed = !mgr.cancel_attempt && process_sample_locked(now);

    if (completed) {
        published = mgr.state.sample;
    }
    if ((mgr.stop_pending && now >= mgr.retry_after) ||
        (mgr.active && attempt_ends_locked(now, moving, continuous, completed, manual_requested))) {
        action = HR_ACTION_STOP;
        mgr.active = false;
        mgr.has_pending_sample = false;
    }
    mgr.cancel_attempt = false;

    bool due = mgr.state.mode == ZSW_HR_SETTING_BACKGROUND && now >= mgr.next_due;
    bool requested = mgr.state.manual || continuous || (due && moving);

    if (action == HR_ACTION_NONE && !mgr.active && !mgr.stop_pending && requested &&
        now >= mgr.retry_after) {
        action = HR_ACTION_START;
        begin_attempt_locked(now, continuous);
    }
    update_status_locked(action == HR_ACTION_START, moving);
    int64_t wake_at = next_wakeup_locked(now, moving, continuous, requested, due);
    k_mutex_unlock(&manager_lock);

    if (completed) {
        zbus_chan_pub(&hr_sample_data_chan, &published, K_MSEC(20));
    }
    if (action == HR_ACTION_STOP) {
        stop_sensor();
        return 0;
    }
    if (action == HR_ACTION_START) {
        start_sensor();
        return 0;
    }
    return wake_at == INT64_MAX ? -1 : MAX(1, wake_at - k_uptime_get());
}

static void manager_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a);
    ARG_UNUSED(b);
    ARG_UNUSED(c);
    while (true) {
        int64_t delay = run_once();
        if (!delay) {
            continue;
        }
        k_sem_take(&manager_wake, delay < 0 ? K_FOREVER : K_MSEC(delay));
    }
}
K_THREAD_DEFINE(hr_manager_id, 1536, manager_thread, NULL, NULL, NULL, 9, 0, SYS_FOREVER_MS);

int zsw_hr_manager_init(void)
{
    zsw_hr_setting_t mode = ZSW_HR_SETTING_BACKGROUND;

    if (!zsw_hr_is_available()) {
        mgr.state.status = ZSW_HR_STATUS_UNAVAILABLE;
        return -ENODEV;
    }
    if (settings_subsys_init() || settings_load_subtree_direct(ZSW_SETTINGS_HR_MODE, load_mode, &mode)) {
        LOG_ERR("Failed to load HR mode");
    }
    int rc = zsw_hr_register_callback(sample_received, NULL);
    if (rc) {
        return rc;
    }
    k_mutex_lock(&manager_lock, K_FOREVER);
    apply_mode_locked(mode);
    mgr.ready = true;
    k_mutex_unlock(&manager_lock);
    k_thread_start(hr_manager_id);
    return 0;
}
