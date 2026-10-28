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

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include "cJSON.h"

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include <ble/ble_http.h>
#include "applications/altimeter/altimeter_app.h"
#include "events/accel_event.h"
#include "events/ble_event.h"
#include "events/pressure_event.h"
#include "events/elevation_tracking_event.h"
#include "events/altitude_event.h"
#include "altimeter_ui.h"
#include "history/zsw_history.h"
#include <zsw_clock.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

LOG_MODULE_REGISTER(altimeter_app, LOG_LEVEL_DBG);

#define HTTP_REQUEST_URL_ALTIMETER_FMT "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f&current=temperature_2m,pressure_msl&models=best_match"
#define ALTIMETER_CALIBRATION_INTERVAL_S (30 * 60)
#define ALTIMETER_CALIBRATION_RETRY_INTERVAL_S (60)
#define ALTIMETER_UI_REFRESH_INTERVAL_MS 250

#define DRY_AIR_GAS_CONSTANT 287.05f
#define GRAVITY_M_S2 9.80665f
#define STANDARD_TEMP_K 288.15f
#define ALTITUDE_DISPLAY_TAU_S 2.0f
#define REF_SLP_SLEW_HPA_PER_S 0.001f
#define ALTIMETER_CHART_INTERVAL_MS 10000U
#define FLOOR_HEIGHT_M 2.8f
#define EPISODE_INACTIVITY_MS 30000U
#define EPISODE_SETTLE_MS 1500U
#define COMMIT_DEADBAND_M 0.20f
#define COMMIT_MIN_SPAN_MS 500U
#define COMMIT_MAX_AGE_MS 30000U
#define COMMIT_MIN_CADENCE_CPS 0.5f
#define COMMIT_MAX_SPEED_MPS 1.0f
// The step counter reads ~0.09 m per count on stairs, so the ramp guard sits below that.
#define COMMIT_MIN_M_PER_COUNT 0.05f
#define ALTIMETER_WORK_QUEUE_DEPTH 8

// type definitions for altimeter state, floor count state, and work items
typedef struct {
    float ref_slp;            // Reference mean sea-level pressure (hPa), slews toward target_ref_slp
    float target_ref_slp;     // Latest calibrated reference, applied gradually to avoid altitude jumps
    float temperature_k;      // Latest API ambient temperature (K)
    float displayed_altitude_m;
    uint32_t last_ref_update_ms;
    uint32_t last_display_update_ms;
    uint32_t last_chart_sample_ms;
    bool has_displayed_altitude;
    bool has_ref_update_time;
    bool has_chart_sample;
} altimeter_state_t;

typedef struct {
    float climb_accum;        // Accumulated gain (m) toward next floor up
    float descend_accum;      // Accumulated loss (m) toward next floor down
    uint16_t floors_up;       // Floors climbed today
    uint16_t floors_down;     // Floors descended today
    int last_reset_yday;      // tm_yday of last daily reset, -1 = not yet initialized
} floor_count_state_t;

typedef enum {
    ALTIMETER_FSM_IDLE,
    ALTIMETER_FSM_WAIT_BASELINE,
    ALTIMETER_FSM_TRACKING,
} altimeter_fsm_state_t;

typedef struct {
    altimeter_fsm_state_t state;
    uint32_t episode_start_ms;
    uint32_t last_progress_ms;
    uint32_t last_count;
    uint32_t commit_count;
    uint32_t commit_ms;
    float commit_pressure_pa;
    float commit_temperature_k;
    float latest_pressure_pa;
    bool has_last_count;
} altimeter_fsm_t;

typedef enum {
    ALTIMETER_WORK_PRESSURE,
    ALTIMETER_WORK_CALIBRATION,
    ALTIMETER_WORK_STEP,
    ALTIMETER_WORK_ACTIVITY,
    ALTIMETER_WORK_INACTIVITY_TIMEOUT,
} altimeter_work_type_t;

typedef struct {
    altimeter_work_type_t type;
    float pressure_pa;       // ALTIMETER_WORK_PRESSURE
    float pressure_msl_hpa;  // ALTIMETER_WORK_CALIBRATION
    float temperature_2m_c;  // ALTIMETER_WORK_CALIBRATION
    uint32_t step_count;     // ALTIMETER_WORK_STEP
    uint32_t timestamp_ms;
    bool step_snapshot;
} altimeter_work_item_t;

// Functions needed for all applications
static void altimeter_app_start(lv_obj_t *root, lv_group_t *group);
static void altimeter_app_stop(void);
static void altimeter_app_ui_available(void);
static void altimeter_app_ui_unavailable(void);
static void on_zbus_ble_data_callback(const struct zbus_channel *chan);
static void on_zbus_pressure_callback(const struct zbus_channel *chan);
static void on_zbus_accel_callback(const struct zbus_channel *chan);
static void periodic_calibration_handler(struct k_work *work);
static void http_rsp_cb(ble_http_status_code_t status, char *response);
static void update_altimeter_ui(lv_timer_t *timer);
static void copy_ui_snapshot(void);
static void process_altimeter_work(struct k_work *work);
static float get_current_altitude(float pressure_sensor);
static void check_daily_floor_reset(void);
static void process_pressure_sample(const altimeter_work_item_t *work_item);
static void process_step_event(uint32_t step_count, bool step_snapshot, uint32_t now_ms);
static void process_activity_event(uint32_t now_ms);
static void start_episode(const char *reason, uint32_t now_ms);
static void end_episode_if_inactive(uint32_t now_ms);
static void set_commit_point(uint32_t now_ms);
static void commit_height(uint32_t now_ms);
static void publish_altitude_event(void);
static const char *floor_fsm_state_name(altimeter_fsm_state_t state);
static void set_floor_fsm_state(altimeter_fsm_state_t state, const char *reason);
static bool enqueue_altimeter_work(const altimeter_work_item_t *work_item);
static void request_tracking(bool active);
static void inactivity_timeout_handler(struct k_work *work);
static void reschedule_calibration_work(bool success);

// Static variables
static altimeter_state_t alt_state = {
    .ref_slp = 1013.25f,
    .target_ref_slp = 1013.25f,
    .temperature_k = STANDARD_TEMP_K,
};
static floor_count_state_t floor_state = { .last_reset_yday = -1 };
static altimeter_fsm_t floor_fsm = { .state = ALTIMETER_FSM_IDLE };
static float chart_samples[HISTORY_SAMPLES];
static zsw_history_t chart_history;
static K_MUTEX_DEFINE(ui_data_mutex);
static lv_timer_t *refresh_timer;
static bool ui_data_pending;
static bool ui_refresh_requested;
static float snapshot_samples[HISTORY_SAMPLES];
static altimeter_ui_data_t ui_snapshot;
static altimeter_ui_data_t ui_data = {
    .altitude = 0.0f,
    .raw_pressure = 0.0f,
    .history_data = chart_samples,
    .start_idx = 0,
    .valid_count = 0,
    .is_calibrated = false,
    .has_data = false
};

ZSW_LV_IMG_DECLARE(altimeter_icon);

static application_t app = {
    .name = "Altimeter",
    .icon = ZSW_LV_IMG_USE(altimeter_icon),
    .start_func = altimeter_app_start,
    .stop_func = altimeter_app_stop,
    .ui_available_func = altimeter_app_ui_available,
    .ui_unavailable_func = altimeter_app_ui_unavailable,
    .category = ZSW_APP_CATEGORY_SENSORS,
};

// ZBUS channel and listener for BLE communication
ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(altimeter_ble_comm_lis, on_zbus_ble_data_callback);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, altimeter_ble_comm_lis, 1);

// ZBUS channel and listener for Pressure sensor data
ZBUS_CHAN_DECLARE(pressure_data_chan);
ZBUS_LISTENER_DEFINE(altimeter_pressure_lis, on_zbus_pressure_callback);
ZBUS_CHAN_ADD_OBS(pressure_data_chan, altimeter_pressure_lis, 1);

ZBUS_CHAN_DECLARE(accel_data_chan);
ZBUS_LISTENER_DEFINE(altimeter_accel_lis, on_zbus_accel_callback);
ZBUS_CHAN_ADD_OBS(accel_data_chan, altimeter_accel_lis, 1);

// ZBUS channel this app publishes calibrated altitude to
ZBUS_CHAN_DECLARE(altitude_data_chan);
ZBUS_CHAN_DECLARE(elevation_tracking_chan);
K_WORK_DELAYABLE_DEFINE(altimeter_calibration_work, periodic_calibration_handler);
K_WORK_DEFINE(altimeter_work, process_altimeter_work);
K_WORK_DELAYABLE_DEFINE(altimeter_inactivity_work, inactivity_timeout_handler);
K_MSGQ_DEFINE(altimeter_work_queue, sizeof(altimeter_work_item_t), ALTIMETER_WORK_QUEUE_DEPTH, 4);

static float get_current_altitude(float pressure_sensor)
{
    if (pressure_sensor <= 0.0f || alt_state.ref_slp <= 0.0f || alt_state.temperature_k <= 0.0f) {
        return 0.0f;
    }

    return (DRY_AIR_GAS_CONSTANT * alt_state.temperature_k / GRAVITY_M_S2) *
           logf(alt_state.ref_slp / pressure_sensor);
}

static void reschedule_calibration_work(bool success)
{
    uint32_t delay_s = success ? ALTIMETER_CALIBRATION_INTERVAL_S : ALTIMETER_CALIBRATION_RETRY_INTERVAL_S;

    k_work_reschedule(&altimeter_calibration_work, K_SECONDS(delay_s));
}

static void periodic_calibration_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    int ret = ble_comm_request_gps_status(true);

    if (ret != 0) {
        LOG_ERR("Failed to request GPS for calibration: %d", ret);
        // No async continuation will occur for this cycle, retry sooner
        reschedule_calibration_work(false);
    }
}

static void http_rsp_cb(ble_http_status_code_t status, char *response)
{
    bool success = false;

    if (status == BLE_HTTP_STATUS_OK) {
        cJSON *parsed_response = cJSON_Parse(response);
        if (parsed_response == NULL) {
            LOG_ERR("Failed to parse JSON");
            reschedule_calibration_work(false);
            return;
        }

        cJSON *current = cJSON_GetObjectItem(parsed_response, "current");
        if (current) {
            cJSON *pressure_msl_node = cJSON_GetObjectItem(current, "pressure_msl");
            cJSON *temperature_2m_node = cJSON_GetObjectItem(current, "temperature_2m");

            if (pressure_msl_node && cJSON_IsNumber(pressure_msl_node) &&
                temperature_2m_node && cJSON_IsNumber(temperature_2m_node)) {
                altimeter_work_item_t work_item = {
                    .type = ALTIMETER_WORK_CALIBRATION,
                    .pressure_msl_hpa = (float)pressure_msl_node->valuedouble,
                    .temperature_2m_c = (float)temperature_2m_node->valuedouble,
                };

                if (k_msgq_put(&altimeter_work_queue, &work_item, K_NO_WAIT) != 0) {
                    LOG_WRN("Altimeter work queue full; calibration discarded");
                } else {
                    k_work_submit(&altimeter_work);
                    success = true;
                    LOG_INF("Calibrated! New MSL Reference: %.2f hPa (Temp 2m: %.1f C)",
                            work_item.pressure_msl_hpa, work_item.temperature_2m_c);
                }
            } else {
                LOG_WRN("Missing or invalid pressure_msl/temperature_2m in JSON response.");
            }
        } else {
            LOG_WRN("Missing current object in JSON response.");
        }

        cJSON_Delete(parsed_response); // CRITICAL: prevent memory leaks
    } else {
        LOG_ERR("HTTP request failed with status: %d", status);
    }

    reschedule_calibration_work(success);
}

static void on_zbus_ble_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);

    if (event->data.type != BLE_COMM_DATA_TYPE_GPS) {
        return;
    }

    ble_comm_request_gps_status(false);

    float lat = event->data.data.gps.lat;
    float lon = event->data.data.gps.lon;

    LOG_INF("GPS received: %.4f, %.4f. Requesting Altimeter data...", lat, lon);

    char http_url[256];
    snprintf(http_url, sizeof(http_url), HTTP_REQUEST_URL_ALTIMETER_FMT, lat, lon);

    int ret = zsw_ble_http_get(http_url, http_rsp_cb);
    // On failure, reschedule calibration work (short timeout).
    if (ret != 0) {
        LOG_ERR("Failed to proxy HTTP request: %d", ret);
        reschedule_calibration_work(false);
    }
}

static void copy_ui_snapshot(void)
{
    ui_snapshot = ui_data;
    memcpy(snapshot_samples, chart_samples, sizeof(snapshot_samples));
    ui_snapshot.history_data = snapshot_samples;
}

static void update_altimeter_ui(lv_timer_t *timer)
{
    ARG_UNUSED(timer);

    if (app.current_state != ZSW_APP_STATE_UI_VISIBLE) {
        return;
    }

    k_mutex_lock(&ui_data_mutex, K_FOREVER);
    if (!ui_data_pending && !ui_refresh_requested) {
        k_mutex_unlock(&ui_data_mutex);
        return;
    }
    bool fresh_data = ui_data_pending;
    copy_ui_snapshot();
    ui_data_pending = false;
    ui_refresh_requested = false;
    k_mutex_unlock(&ui_data_mutex);

    altimeter_ui_update(&ui_snapshot);
    if (fresh_data) {
        altimeter_ui_notify_data_ready();
    }
}

static void process_altimeter_work(struct k_work *work)
{
    altimeter_work_item_t work_item;

    ARG_UNUSED(work);

    while (k_msgq_get(&altimeter_work_queue, &work_item, K_NO_WAIT) == 0) {
        switch (work_item.type) {
        case ALTIMETER_WORK_CALIBRATION:
            LOG_INF("MSL calibration update: target %.2f -> %.2f hPa, T_API %.2f C",
                    (double)alt_state.target_ref_slp,
                    (double)work_item.pressure_msl_hpa,
                    (double)work_item.temperature_2m_c);
            alt_state.target_ref_slp = work_item.pressure_msl_hpa;
            alt_state.temperature_k = work_item.temperature_2m_c + 273.15f;
            k_mutex_lock(&ui_data_mutex, K_FOREVER);
            ui_data.is_calibrated = true;
            ui_data_pending = true;
            k_mutex_unlock(&ui_data_mutex);
            publish_altitude_event();
            break;
        case ALTIMETER_WORK_PRESSURE:
            process_pressure_sample(&work_item);
            break;
        case ALTIMETER_WORK_STEP:
            process_step_event(work_item.step_count, work_item.step_snapshot, work_item.timestamp_ms);
            break;
        case ALTIMETER_WORK_ACTIVITY:
            process_activity_event(work_item.timestamp_ms);
            break;
        case ALTIMETER_WORK_INACTIVITY_TIMEOUT:
            end_episode_if_inactive(work_item.timestamp_ms);
            break;
        }
    }
}

static void check_daily_floor_reset(void)
{
    zsw_timeval_t time;

    zsw_clock_get_time(&time);

    if (floor_state.last_reset_yday != time.tm.tm_yday) {
        floor_state.floors_up = 0;
        floor_state.floors_down = 0;
        floor_state.climb_accum = 0.0f;
        floor_state.descend_accum = 0.0f;
        floor_state.last_reset_yday = time.tm.tm_yday;
    }
}

static bool enqueue_altimeter_work(const altimeter_work_item_t *work_item)
{
    if (k_msgq_put(&altimeter_work_queue, work_item, K_NO_WAIT) != 0) {
        LOG_WRN("Altimeter work queue full; event discarded (type %d)", work_item->type);
        return false;
    }

    k_work_submit(&altimeter_work);
    return true;
}

static void request_tracking(bool active)
{
    struct elevation_tracking_event event = { .active = active };
    int ret = zbus_chan_pub(&elevation_tracking_chan, &event, K_MSEC(100));
    if (ret != 0) {
        LOG_ERR("Failed to request tracking %s: %d", active ? "on" : "off", ret);
    }
}

static void inactivity_timeout_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    const altimeter_work_item_t event = {
        .type = ALTIMETER_WORK_INACTIVITY_TIMEOUT,
        .timestamp_ms = (uint32_t)k_uptime_get_32(),
    };
    if (!enqueue_altimeter_work(&event)) {
        k_work_reschedule(&altimeter_inactivity_work, K_MSEC(10));
    }
}

static void process_pressure_sample(const altimeter_work_item_t *work_item)
{
    if (!isfinite(work_item->pressure_pa) || work_item->pressure_pa <= 0.0f) {
        return;
    }

    uint32_t timestamp_ms = work_item->timestamp_ms;
    floor_fsm.latest_pressure_pa = work_item->pressure_pa;
    LOG_DBG("Pressure event: state=%s pressure=%.2f Pa",
            floor_fsm_state_name(floor_fsm.state),
            (double)work_item->pressure_pa);

    // The first samples after the ODR switch still carry the slow filter's old state.
    if (floor_fsm.state == ALTIMETER_FSM_WAIT_BASELINE && floor_fsm.has_last_count &&
        (uint32_t)(timestamp_ms - floor_fsm.episode_start_ms) >= EPISODE_SETTLE_MS) {
        set_commit_point(timestamp_ms);
        set_floor_fsm_state(ALTIMETER_FSM_TRACKING, "pressure settled");
        LOG_INF("Episode baseline: P=%.2f Pa count=%u T_API=%.2f K",
                (double)floor_fsm.commit_pressure_pa, (unsigned)floor_fsm.commit_count,
                (double)floor_fsm.commit_temperature_k);
    }

    check_daily_floor_reset();

    if (!alt_state.has_ref_update_time) {
        alt_state.last_ref_update_ms = timestamp_ms;
        alt_state.has_ref_update_time = true;
    } else {
        uint32_t elapsed_ms = timestamp_ms - alt_state.last_ref_update_ms;
        float max_slew_hpa = REF_SLP_SLEW_HPA_PER_S * ((float)elapsed_ms / 1000.0f);
        if (alt_state.ref_slp < alt_state.target_ref_slp) {
            alt_state.ref_slp = MIN(alt_state.ref_slp + max_slew_hpa, alt_state.target_ref_slp);
        } else if (alt_state.ref_slp > alt_state.target_ref_slp) {
            alt_state.ref_slp = MAX(alt_state.ref_slp - max_slew_hpa, alt_state.target_ref_slp);
        }
        alt_state.last_ref_update_ms = timestamp_ms;
    }

    float pressure_hpa = work_item->pressure_pa / 100.0f;
    float raw_altitude = get_current_altitude(pressure_hpa);
    if (!alt_state.has_displayed_altitude) {
        alt_state.displayed_altitude_m = raw_altitude;
        alt_state.has_displayed_altitude = true;
    } else {
        uint32_t elapsed_ms = timestamp_ms - alt_state.last_display_update_ms;
        float elapsed_s = (float)elapsed_ms / 1000.0f;
        float alpha = 1.0f - expf(-elapsed_s / ALTITUDE_DISPLAY_TAU_S);
        alt_state.displayed_altitude_m += alpha * (raw_altitude - alt_state.displayed_altitude_m);
    }
    alt_state.last_display_update_ms = timestamp_ms;

    k_mutex_lock(&ui_data_mutex, K_FOREVER);
    ui_data.altitude = alt_state.displayed_altitude_m;
    ui_data.raw_pressure = pressure_hpa;
    ui_data.floors_up = floor_state.floors_up;
    ui_data.floors_down = floor_state.floors_down;

    if (!alt_state.has_chart_sample ||
        (uint32_t)(timestamp_ms - alt_state.last_chart_sample_ms) >= ALTIMETER_CHART_INTERVAL_MS) {
        zsw_history_add(&chart_history, &ui_data.altitude);
        ui_data.start_idx = (uint8_t)chart_history.write_index;
        ui_data.valid_count = (uint8_t)chart_history.num_samples;
        ui_data.has_data = zsw_history_samples(&chart_history) > 0;
        alt_state.last_chart_sample_ms = timestamp_ms;
        alt_state.has_chart_sample = true;
    }
    ui_data_pending = true;
    k_mutex_unlock(&ui_data_mutex);

    publish_altitude_event();
}

static void set_commit_point(uint32_t now_ms)
{
    floor_fsm.commit_pressure_pa = floor_fsm.latest_pressure_pa;
    floor_fsm.commit_count = floor_fsm.last_count;
    floor_fsm.commit_ms = now_ms;
    floor_fsm.commit_temperature_k = alt_state.temperature_k;
}

static void start_episode(const char *reason, uint32_t now_ms)
{
    floor_fsm.episode_start_ms = now_ms;
    floor_fsm.last_progress_ms = now_ms;
    set_floor_fsm_state(ALTIMETER_FSM_WAIT_BASELINE, reason);
    LOG_INF("Floor episode starts (%s): count=%u", reason, (unsigned)floor_fsm.last_count);
    request_tracking(true);
    k_work_reschedule(&altimeter_inactivity_work, K_MSEC(EPISODE_INACTIVITY_MS));
}

static void end_episode_if_inactive(uint32_t now_ms)
{
    // A step may have re-armed the timer after this timeout was already queued.
    if (floor_fsm.state == ALTIMETER_FSM_IDLE ||
        (uint32_t)(now_ms - floor_fsm.last_progress_ms) < EPISODE_INACTIVITY_MS) {
        return;
    }

    LOG_INF("Floor episode ends: residual up=%.2f m down=%.2f m",
            (double)floor_state.climb_accum, (double)floor_state.descend_accum);
    floor_state.climb_accum = 0.0f;
    floor_state.descend_accum = 0.0f;
    set_floor_fsm_state(ALTIMETER_FSM_IDLE, "inactivity timeout");
    request_tracking(false);
    publish_altitude_event();
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
    float dh_m = 0.0f;
    const char *skip = NULL;

    if (floor_fsm.commit_pressure_pa > 0.0f && floor_fsm.latest_pressure_pa > 0.0f) {
        dh_m = (DRY_AIR_GAS_CONSTANT * floor_fsm.commit_temperature_k / GRAVITY_M_S2) *
               logf(floor_fsm.commit_pressure_pa / floor_fsm.latest_pressure_pa);
    } else {
        skip = "no pressure";
    }

    if (skip == NULL && cadence < COMMIT_MIN_CADENCE_CPS) {
        skip = "low cadence";
    } else if (skip == NULL && fabsf(dh_m) < COMMIT_DEADBAND_M) {
        if (span_ms < COMMIT_MAX_AGE_MS) {
            return;
        }
        skip = "no height change";
    } else if (skip == NULL && fabsf(dh_m) / span_s > COMMIT_MAX_SPEED_MPS) {
        skip = "too fast";
    } else if (skip == NULL && fabsf(dh_m) / (float)counts < COMMIT_MIN_M_PER_COUNT) {
        skip = "slope too small";
    }

    if (skip != NULL) {
        LOG_INF("Height skipped (%s): span=%.1fs counts=%u cadence=%.2f dH=%.3f m",
                skip, (double)span_s, (unsigned)counts, (double)cadence, (double)dh_m);
        set_commit_point(now_ms);
        return;
    }

    check_daily_floor_reset();
    if (dh_m > 0.0f) {
        floor_state.climb_accum += dh_m;
    } else {
        floor_state.descend_accum -= dh_m;
    }

    bool paid = false;
    while (floor_state.climb_accum >= FLOOR_HEIGHT_M) {
        floor_state.climb_accum -= FLOOR_HEIGHT_M;
        floor_state.floors_up++;
        paid = true;
    }
    while (floor_state.descend_accum >= FLOOR_HEIGHT_M) {
        floor_state.descend_accum -= FLOOR_HEIGHT_M;
        floor_state.floors_down++;
        paid = true;
    }

    LOG_INF("Height commit: span=%.1fs counts=%u cadence=%.2f dH=%.3f m up=%.2f m down=%.2f m floors=%u/%u",
            (double)span_s, (unsigned)counts, (double)cadence, (double)dh_m,
            (double)floor_state.climb_accum, (double)floor_state.descend_accum,
            floor_state.floors_up, floor_state.floors_down);
    set_commit_point(now_ms);
    if (paid) {
        publish_altitude_event();
    }
}

static void process_step_event(uint32_t step_count, bool step_snapshot, uint32_t now_ms)
{
    uint32_t delta = 0;
    bool first_sample = !floor_fsm.has_last_count;

    if (!first_sample) {
        if (step_count < floor_fsm.last_count) {
            LOG_INF("Step counter reset/rebase: %u -> %u", (unsigned)floor_fsm.last_count, (unsigned)step_count);
            floor_fsm.last_count = step_count;
            set_commit_point(now_ms);
            return;
        }
        delta = step_count - floor_fsm.last_count;
    }
    floor_fsm.last_count = step_count;
    floor_fsm.has_last_count = true;

    if (floor_fsm.state == ALTIMETER_FSM_IDLE) {
        // Snapshots only run while tracking, so a late one must not restart an ended episode.
        if (!step_snapshot && step_count != 0U && (first_sample || delta > 0U)) {
            start_episode("step event", now_ms);
        }
        return;
    }

    if (delta == 0U) {
        return;
    }

    floor_fsm.last_progress_ms = now_ms;
    k_work_reschedule(&altimeter_inactivity_work, K_MSEC(EPISODE_INACTIVITY_MS));
    if (floor_fsm.state == ALTIMETER_FSM_TRACKING) {
        commit_height(now_ms);
    }
}

static void process_activity_event(uint32_t now_ms)
{
    if (floor_fsm.state == ALTIMETER_FSM_IDLE) {
        start_episode("activity", now_ms);
    }
}

static const char *floor_fsm_state_name(altimeter_fsm_state_t state)
{
    switch (state) {
    case ALTIMETER_FSM_IDLE:
        return "IDLE";
    case ALTIMETER_FSM_WAIT_BASELINE:
        return "WAIT_BASELINE";
    case ALTIMETER_FSM_TRACKING:
        return "TRACKING";
    default:
        return "UNKNOWN";
    }
}

static void set_floor_fsm_state(altimeter_fsm_state_t state, const char *reason)
{
    if (floor_fsm.state == state) {
        return;
    }

    LOG_INF("Floor FSM: %s -> %s (%s)",
            floor_fsm_state_name(floor_fsm.state),
            floor_fsm_state_name(state),
            reason);
    floor_fsm.state = state;
}

static void publish_altitude_event(void)
{
    struct altitude_event event;

    k_mutex_lock(&ui_data_mutex, K_FOREVER);
    ui_data.floors_up = floor_state.floors_up;
    ui_data.floors_down = floor_state.floors_down;
    ui_data_pending = true;
    event = (struct altitude_event) {
        .altitude_m = ui_data.altitude,
        .is_calibrated = ui_data.is_calibrated,
        .floors_up = floor_state.floors_up,
        .floors_down = floor_state.floors_down,
    };
    k_mutex_unlock(&ui_data_mutex);

    int ret = zbus_chan_pub(&altitude_data_chan, &event, K_MSEC(250));
    if (ret != 0) {
        LOG_WRN("Failed to publish altitude event: %d", ret);
    }
}

static void on_zbus_pressure_callback(const struct zbus_channel *chan)
{
    const struct pressure_event *event = zbus_chan_const_msg(chan);
    altimeter_work_item_t work_item = {
        .type = ALTIMETER_WORK_PRESSURE,
        .pressure_pa = event->pressure,
        .timestamp_ms = (uint32_t)k_uptime_get_32(),
    };

    enqueue_altimeter_work(&work_item);
}

static void on_zbus_accel_callback(const struct zbus_channel *chan)
{
    const struct accel_event *event = zbus_chan_const_msg(chan);

    if (event->data.type == ZSW_IMU_EVT_TYPE_STEP_ACTIVITY) {
        if (event->data.data.step_activity == ZSW_IMU_EVT_STEP_ACTIVITY_WALK ||
            event->data.data.step_activity == ZSW_IMU_EVT_STEP_ACTIVITY_RUN) {
            const altimeter_work_item_t activity_item = {
                .type = ALTIMETER_WORK_ACTIVITY,
                .timestamp_ms = (uint32_t)k_uptime_get_32(),
            };
            enqueue_altimeter_work(&activity_item);
        }
        return;
    }

    if (event->data.type != ZSW_IMU_EVT_TYPE_STEP &&
        event->data.type != ZSW_IMU_EVT_TYPE_STEP_SNAPSHOT) {
        return;
    }

    altimeter_work_item_t work_item = {
        .type = ALTIMETER_WORK_STEP,
        .step_count = event->data.data.step.count,
        .timestamp_ms = (uint32_t)k_uptime_get_32(),
        .step_snapshot = event->data.type == ZSW_IMU_EVT_TYPE_STEP_SNAPSHOT,
    };

    enqueue_altimeter_work(&work_item);
}

static void altimeter_app_start(lv_obj_t *root, lv_group_t *group)
{
    ARG_UNUSED(group);

    LOG_INF("Altimeter app started!");
    altimeter_ui_show(root);
    k_mutex_lock(&ui_data_mutex, K_FOREVER);
    copy_ui_snapshot();
    ui_data_pending = false;
    ui_refresh_requested = false;
    k_mutex_unlock(&ui_data_mutex);
    altimeter_ui_update(&ui_snapshot);
    refresh_timer = lv_timer_create(update_altimeter_ui, ALTIMETER_UI_REFRESH_INTERVAL_MS, NULL);

}

static void altimeter_app_stop(void)
{
    LOG_INF("Altimeter app stopped!");
    if (refresh_timer != NULL) {
        lv_timer_del(refresh_timer);
        refresh_timer = NULL;
    }
    altimeter_ui_remove();
}

static void altimeter_app_ui_available(void)
{
    if (refresh_timer != NULL) {
        k_mutex_lock(&ui_data_mutex, K_FOREVER);
        ui_refresh_requested = true;
        k_mutex_unlock(&ui_data_mutex);
        lv_timer_resume(refresh_timer);
        lv_timer_ready(refresh_timer);
    }
}

static void altimeter_app_ui_unavailable(void)
{
    if (refresh_timer != NULL) {
        lv_timer_pause(refresh_timer);
    }
}

static int altimeter_app_add(void)
{
    zsw_history_init(&chart_history, HISTORY_SAMPLES, sizeof(float), chart_samples, "altimeter_hist");
    zsw_app_manager_add_application(&app);
    return 0;
}

void zsw_altimeter_calibration_start(void)
{
    k_work_reschedule(&altimeter_calibration_work, K_NO_WAIT);
}

SYS_INIT(altimeter_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);