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

#include <errno.h>
#include <math.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/zbus/zbus.h>

#include "events/accel_event.h"
#include "events/elevation_tracking_event.h"
#include "events/floor_count_event.h"
#include "events/pressure_event.h"
#include "events/zsw_periodic_event.h"
#include "sensors/zsw_floor_counter.h"
#include "zsw_clock.h"

LOG_MODULE_REGISTER(zsw_floor_counter, CONFIG_ZSW_SENSORS_LOG_LEVEL);

#define DRY_AIR_GAS_CONSTANT 287.05f
#define GRAVITY_M_S2 9.80665f
#define STANDARD_TEMP_K 288.15f
#define FLOOR_HEIGHT_M 2.8f
#define EPISODE_INACTIVITY_MS 30000U
#define EPISODE_SETTLE_MS 1500U
#define COMMIT_DEADBAND_M 0.20f
#define COMMIT_MIN_SPAN_MS 500U
#define COMMIT_MAX_AGE_MS 30000U
#define COMMIT_MIN_CADENCE_CPS 0.5f
#define COMMIT_MAX_SPEED_MPS 1.0f
#define COMMIT_MIN_M_PER_COUNT 0.05f
#define FLOOR_WORK_QUEUE_DEPTH 8

typedef struct {
    float climb_accum;
    float descend_accum;
    uint16_t floors_up;
    uint16_t floors_down;
    uint32_t last_reset_date;
} floor_count_state_t;

typedef enum {
    FLOOR_FSM_IDLE,
    FLOOR_FSM_WAIT_BASELINE,
    FLOOR_FSM_TRACKING,
} floor_fsm_state_t;

typedef struct {
    floor_fsm_state_t state;
    uint32_t episode_start_ms;
    uint32_t last_progress_ms;
    uint32_t last_count;
    uint32_t commit_count;
    uint32_t commit_ms;
    float commit_pressure_pa;
    float commit_temperature_k;
    float latest_pressure_pa;
    bool has_last_count;
} floor_fsm_t;

typedef enum {
    FLOOR_WORK_PRESSURE,
    FLOOR_WORK_STEP,
    FLOOR_WORK_ACTIVITY,
    FLOOR_WORK_INACTIVITY_TIMEOUT,
    FLOOR_WORK_TEMPERATURE,
    FLOOR_WORK_DAY_CHECK,
} floor_work_type_t;

typedef struct {
    floor_work_type_t type;
    uint32_t timestamp_ms;
    union {
        float pressure_pa;
        float temperature_k;
        struct {
            uint32_t count;
            bool snapshot;
        } step;
    } data;
} floor_work_item_t;

static void process_floor_work(struct k_work *work);
static void inactivity_timeout_handler(struct k_work *work);
static void on_pressure_event(const struct zbus_channel *chan);
static void on_accel_event(const struct zbus_channel *chan);
static void on_day_check(const struct zbus_channel *chan);

static floor_count_state_t floor_state;
static floor_fsm_t floor_fsm;
static float temperature_k = STANDARD_TEMP_K;
static atomic_t initialized;
static atomic_t published_counts;
static zsw_floor_counter_changed_cb_t changed_callback;
static bool tracking_published;
static bool tracking_valid = true;

ZBUS_CHAN_DECLARE(floor_count_chan);

K_WORK_DEFINE(floor_work, process_floor_work);
K_WORK_DELAYABLE_DEFINE(floor_inactivity_work, inactivity_timeout_handler);
K_MSGQ_DEFINE(floor_work_queue, sizeof(floor_work_item_t), FLOOR_WORK_QUEUE_DEPTH, 4);

ZBUS_CHAN_DECLARE(pressure_data_chan);
ZBUS_CHAN_DECLARE(accel_data_chan);
ZBUS_CHAN_DECLARE(elevation_tracking_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(floor_day_lis, on_day_check);
ZBUS_LISTENER_DEFINE(floor_pressure_lis, on_pressure_event);
ZBUS_CHAN_ADD_OBS(pressure_data_chan, floor_pressure_lis, 1);
ZBUS_LISTENER_DEFINE(floor_accel_lis, on_accel_event);
ZBUS_CHAN_ADD_OBS(accel_data_chan, floor_accel_lis, 1);

static bool enqueue_floor_work(const floor_work_item_t *item)
{
    if (k_msgq_put(&floor_work_queue, item, K_NO_WAIT) != 0) {
        LOG_DBG("Floor queue full; event %d discarded", item->type);
        return false;
    }

    k_work_submit(&floor_work);
    return true;
}

static void publish_counts(void)
{
    uint32_t counts = (uint32_t)floor_state.floors_up | ((uint32_t)floor_state.floors_down << 16);

    if ((uint32_t)atomic_set(&published_counts, (atomic_val_t)counts) != counts) {
        const struct floor_count_event event = {
            .floors_up = floor_state.floors_up,
            .floors_down = floor_state.floors_down,
        };
        int ret = zbus_chan_pub(&floor_count_chan, &event, K_FOREVER);
        if (ret != 0) {
            LOG_WRN("Floor count publication failed: %d", ret);
        }
        LOG_INF("Floors changed: up=%u down=%u", floor_state.floors_up, floor_state.floors_down);
        if (changed_callback != NULL) {
            changed_callback();
        }
    }
}

static void check_daily_reset(void)
{
    zsw_timeval_t time;

    zsw_clock_get_time(&time);
    if (time.tm.tm_year < 1970 || time.tm.tm_mon < 0 || time.tm.tm_mon > 11 ||
        time.tm.tm_mday < 1 || time.tm.tm_mday > 31) {
        return;
    }
    uint32_t date = (uint32_t)time.tm.tm_year * 10000U + (uint32_t)(time.tm.tm_mon + 1) * 100U +
                    (uint32_t)time.tm.tm_mday;
    if (floor_state.last_reset_date != date) {
        if (floor_state.last_reset_date != 0) {
            LOG_INF("Floor daily reset: date=%u -> %u, up=%u down=%u",
                    floor_state.last_reset_date, date,
                    floor_state.floors_up, floor_state.floors_down);
        }
        floor_state.floors_up = 0;
        floor_state.floors_down = 0;
        floor_state.climb_accum = 0.0f;
        floor_state.descend_accum = 0.0f;
        floor_fsm.commit_pressure_pa = 0.0f;
        floor_state.last_reset_date = date;
    }
}

static void sync_tracking(void)
{
    bool active = floor_fsm.state != FLOOR_FSM_IDLE;
    if (active == tracking_published && tracking_valid) {
        return;
    }

    struct elevation_tracking_event event = { .active = active };
    int ret = zbus_chan_pub(&elevation_tracking_chan, &event, K_MSEC(100));

    if (ret != 0) {
        tracking_valid = false;
        LOG_WRN("Floor tracking request failed: %d", ret);
        k_work_reschedule(&floor_inactivity_work, K_SECONDS(1));
        return;
    }
    tracking_published = active;
    tracking_valid = true;
    if (!active) {
        k_work_cancel_delayable(&floor_inactivity_work);
    }
}

static void set_commit_point(uint32_t now_ms)
{
    floor_fsm.commit_pressure_pa = floor_fsm.latest_pressure_pa;
    floor_fsm.commit_count = floor_fsm.last_count;
    floor_fsm.commit_ms = now_ms;
    floor_fsm.commit_temperature_k = temperature_k;
}

static void start_episode(uint32_t now_ms)
{
    floor_fsm.episode_start_ms = now_ms;
    floor_fsm.last_progress_ms = now_ms;
    floor_fsm.state = FLOOR_FSM_WAIT_BASELINE;
    LOG_INF("Floor episode started: waiting for baseline, steps=%u", floor_fsm.last_count);
    k_work_reschedule(&floor_inactivity_work, K_MSEC(EPISODE_INACTIVITY_MS));
}

static void end_episode_if_inactive(uint32_t now_ms)
{
    if (floor_fsm.state == FLOOR_FSM_IDLE ||
        (uint32_t)(now_ms - floor_fsm.last_progress_ms) < EPISODE_INACTIVITY_MS) {
        return;
    }

    floor_state.climb_accum = 0.0f;
    floor_state.descend_accum = 0.0f;
    floor_fsm.state = FLOOR_FSM_IDLE;
    LOG_INF("Floor episode ended: inactive, up=%u down=%u", floor_state.floors_up, floor_state.floors_down);
}

static void commit_height(uint32_t now_ms)
{
    uint32_t span_ms = now_ms - floor_fsm.commit_ms;
    if (span_ms < COMMIT_MIN_SPAN_MS) {
        return;
    }

    float span_s = (float)span_ms / 1000.0f;
    uint32_t counts = floor_fsm.last_count - floor_fsm.commit_count;
    float cadence = (float)counts / span_s;

    if (floor_fsm.commit_pressure_pa <= 0.0f || floor_fsm.latest_pressure_pa <= 0.0f ||
        cadence < COMMIT_MIN_CADENCE_CPS) {
        set_commit_point(now_ms);
        return;
    }

    float dh_m = (DRY_AIR_GAS_CONSTANT * floor_fsm.commit_temperature_k / GRAVITY_M_S2) *
                 logf(floor_fsm.commit_pressure_pa / floor_fsm.latest_pressure_pa);

    if (fabsf(dh_m) < COMMIT_DEADBAND_M) {
        if (span_ms >= COMMIT_MAX_AGE_MS) {
            set_commit_point(now_ms);
        }
        return;
    }
    if (!isfinite(dh_m) || fabsf(dh_m) / span_s > COMMIT_MAX_SPEED_MPS ||
        fabsf(dh_m) / (float)counts < COMMIT_MIN_M_PER_COUNT) {
        set_commit_point(now_ms);
        return;
    }

    if (dh_m > 0.0f) {
        floor_state.climb_accum += dh_m;
    } else {
        floor_state.descend_accum -= dh_m;
    }

    while (floor_state.climb_accum >= FLOOR_HEIGHT_M) {
        floor_state.climb_accum -= FLOOR_HEIGHT_M;
        if (floor_state.floors_up < UINT16_MAX) {
            floor_state.floors_up++;
        }
    }
    while (floor_state.descend_accum >= FLOOR_HEIGHT_M) {
        floor_state.descend_accum -= FLOOR_HEIGHT_M;
        if (floor_state.floors_down < UINT16_MAX) {
            floor_state.floors_down++;
        }
    }
    set_commit_point(now_ms);
}

static void process_step_event(uint32_t step_count, bool snapshot, uint32_t now_ms)
{
    uint32_t delta = 0;
    bool first_sample = !floor_fsm.has_last_count;

    if (!first_sample) {
        if (step_count < floor_fsm.last_count) {
            floor_fsm.last_count = step_count;
            set_commit_point(now_ms);
            return;
        }
        delta = step_count - floor_fsm.last_count;
    }
    floor_fsm.last_count = step_count;
    floor_fsm.has_last_count = true;

    if (floor_fsm.state == FLOOR_FSM_IDLE) {
        if (!snapshot && step_count != 0U && (first_sample || delta > 0U)) {
            start_episode(now_ms);
        }
        return;
    }
    if (delta == 0U) {
        return;
    }

    floor_fsm.last_progress_ms = now_ms;
    k_work_reschedule(&floor_inactivity_work, K_MSEC(EPISODE_INACTIVITY_MS));
    if (floor_fsm.state == FLOOR_FSM_TRACKING) {
        commit_height(now_ms);
    }
}

static void process_pressure_sample(float pressure_pa, uint32_t now_ms)
{
    if (!isfinite(pressure_pa) || pressure_pa <= 0.0f) {
        return;
    }

    floor_fsm.latest_pressure_pa = pressure_pa;
    if (floor_fsm.state == FLOOR_FSM_WAIT_BASELINE && floor_fsm.has_last_count &&
        (uint32_t)(now_ms - floor_fsm.episode_start_ms) >= EPISODE_SETTLE_MS) {
        set_commit_point(now_ms);
        floor_fsm.state = FLOOR_FSM_TRACKING;
        LOG_INF("Floor baseline ready: tracking, pressure=%d Pa, steps=%u",
                (int)pressure_pa, floor_fsm.last_count);
    }
}

static void process_floor_work(struct k_work *work)
{
    floor_work_item_t item;

    ARG_UNUSED(work);
    while (k_msgq_get(&floor_work_queue, &item, K_NO_WAIT) == 0) {
        check_daily_reset();
        switch (item.type) {
            case FLOOR_WORK_PRESSURE:
                process_pressure_sample(item.data.pressure_pa, item.timestamp_ms);
                break;
            case FLOOR_WORK_STEP:
                process_step_event(item.data.step.count, item.data.step.snapshot, item.timestamp_ms);
                break;
            case FLOOR_WORK_ACTIVITY:
                if (floor_fsm.state == FLOOR_FSM_IDLE) {
                    start_episode(item.timestamp_ms);
                }
                break;
            case FLOOR_WORK_INACTIVITY_TIMEOUT:
                end_episode_if_inactive(k_uptime_get_32());
                if (floor_fsm.state != FLOOR_FSM_IDLE) {
                    uint32_t elapsed_ms = k_uptime_get_32() - floor_fsm.last_progress_ms;
                    k_work_reschedule(&floor_inactivity_work,
                                      K_MSEC(EPISODE_INACTIVITY_MS - MIN(elapsed_ms, EPISODE_INACTIVITY_MS)));
                }
                break;
            case FLOOR_WORK_TEMPERATURE:
                temperature_k = item.data.temperature_k;
                break;
            case FLOOR_WORK_DAY_CHECK:
                if (floor_fsm.state != FLOOR_FSM_IDLE) {
                    LOG_INF("Floor status: state=%s steps=%u pressure=%d Pa up=%u down=%u "
                            "partial_up=%d cm partial_down=%d cm idle=%u ms",
                            floor_fsm.state == FLOOR_FSM_WAIT_BASELINE ? "WAIT_BASELINE" : "TRACKING",
                            floor_fsm.last_count, (int)floor_fsm.latest_pressure_pa,
                            floor_state.floors_up, floor_state.floors_down,
                            (int)(floor_state.climb_accum * 100.0f),
                            (int)(floor_state.descend_accum * 100.0f),
                            (uint32_t)(item.timestamp_ms - floor_fsm.last_progress_ms));
                }
                break;
        }
        publish_counts();
        sync_tracking();
    }
}

static void inactivity_timeout_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    const floor_work_item_t item = {
        .type = FLOOR_WORK_INACTIVITY_TIMEOUT,
        .timestamp_ms = k_uptime_get_32(),
    };
    if (!enqueue_floor_work(&item)) {
        k_work_reschedule(&floor_inactivity_work, K_MSEC(10));
    }
}

static void on_pressure_event(const struct zbus_channel *chan)
{
    if (!atomic_get(&initialized)) {
        return;
    }

    const struct pressure_event *event = zbus_chan_const_msg(chan);
    const floor_work_item_t item = {
        .type = FLOOR_WORK_PRESSURE,
        .timestamp_ms = k_uptime_get_32(),
        .data.pressure_pa = event->pressure,
    };
    enqueue_floor_work(&item);
}

static void on_accel_event(const struct zbus_channel *chan)
{
    if (!atomic_get(&initialized)) {
        return;
    }

    const struct accel_event *event = zbus_chan_const_msg(chan);
    floor_work_item_t item = { .timestamp_ms = k_uptime_get_32() };

    if (event->data.type == ZSW_IMU_EVT_TYPE_STEP_ACTIVITY) {
        if (event->data.data.step_activity != ZSW_IMU_EVT_STEP_ACTIVITY_WALK &&
            event->data.data.step_activity != ZSW_IMU_EVT_STEP_ACTIVITY_RUN) {
            return;
        }
        item.type = FLOOR_WORK_ACTIVITY;
    } else if (event->data.type == ZSW_IMU_EVT_TYPE_STEP || event->data.type == ZSW_IMU_EVT_TYPE_STEP_SNAPSHOT) {
        item.type = FLOOR_WORK_STEP;
        item.data.step.count = event->data.data.step.count;
        item.data.step.snapshot = event->data.type == ZSW_IMU_EVT_TYPE_STEP_SNAPSHOT;
    } else {
        return;
    }
    enqueue_floor_work(&item);
}

static void on_day_check(const struct zbus_channel *chan)
{
    ARG_UNUSED(chan);

    const floor_work_item_t item = {
        .type = FLOOR_WORK_DAY_CHECK,
        .timestamp_ms = k_uptime_get_32(),
    };
    enqueue_floor_work(&item);
}

void zsw_floor_counter_init(void)
{
    atomic_set(&initialized, 1);
    int ret = zsw_periodic_chan_add_obs(&periodic_event_10s_chan, &floor_day_lis);
    if (ret < 0 && ret != -EALREADY) {
        LOG_WRN("Floor daily reset observer failed: %d", ret);
    }
    on_day_check(NULL);
}

void zsw_floor_counter_set_callback(zsw_floor_counter_changed_cb_t callback)
{
    __ASSERT(!atomic_get(&initialized), "Register floor callback before init");
    changed_callback = callback;
}

zsw_floor_counter_data_t zsw_floor_counter_get_counts(void)
{
    uint32_t counts = (uint32_t)atomic_get(&published_counts);

    return (zsw_floor_counter_data_t) {
        .floors_up = (uint16_t)counts,
        .floors_down = (uint16_t)(counts >> 16),
    };
}

int zsw_floor_counter_set_temperature(float ambient_temperature_k)
{
    if (!isfinite(ambient_temperature_k) || ambient_temperature_k < 173.15f || ambient_temperature_k > 373.15f) {
        return -EINVAL;
    }

    const floor_work_item_t item = {
        .type = FLOOR_WORK_TEMPERATURE,
        .data.temperature_k = ambient_temperature_k,
    };
    return enqueue_floor_work(&item) ? 0 : -ENOMSG;
}