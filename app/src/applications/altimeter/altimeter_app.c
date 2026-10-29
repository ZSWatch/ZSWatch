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
#include <zephyr/sys/atomic.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include "cJSON.h"

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include <ble/ble_http.h>
#include "applications/altimeter/altimeter_app.h"
#include "events/ble_event.h"
#include "events/pressure_event.h"
#include "events/altitude_event.h"
#include "sensors/zsw_floor_counter.h"
#include "altimeter_ui.h"
#include "history/zsw_history.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

LOG_MODULE_REGISTER(altimeter_app, CONFIG_ZSW_APP_LOG_LEVEL);

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
#define ALTIMETER_WORK_QUEUE_DEPTH 8

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

typedef enum {
    ALTIMETER_WORK_PRESSURE,
    ALTIMETER_WORK_CALIBRATION,
} altimeter_work_type_t;

typedef struct {
    altimeter_work_type_t type;
    uint32_t timestamp_ms;
    union {
        float pressure_pa;
        struct {
            float pressure_msl_hpa;
            float temperature_2m_c;
        } calibration;
    } data;
} altimeter_work_item_t;

// Functions needed for all applications
static void altimeter_app_start(lv_obj_t *root, lv_group_t *group);
static void altimeter_app_stop(void);
static void altimeter_app_ui_available(void);
static void altimeter_app_ui_unavailable(void);
static void on_zbus_ble_data_callback(const struct zbus_channel *chan);
static void on_zbus_pressure_callback(const struct zbus_channel *chan);
static void periodic_calibration_handler(struct k_work *work);
static void http_rsp_cb(ble_http_status_code_t status, char *response);
static void update_altimeter_ui(lv_timer_t *timer);
static void copy_ui_snapshot(void);
static void process_altimeter_work(struct k_work *work);
static float get_current_altitude(float pressure_sensor);
static void process_pressure_sample(const altimeter_work_item_t *work_item);
static void publish_altitude_event(void);
static void on_floor_count_changed(void);
static bool enqueue_altimeter_work(const altimeter_work_item_t *work_item);
static void reschedule_calibration_work(bool success);

// Static variables
static altimeter_state_t alt_state = {
    .ref_slp = 1013.25f,
    .target_ref_slp = 1013.25f,
    .temperature_k = STANDARD_TEMP_K,
};
static atomic_t floor_update_pending;
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

// ZBUS channel this app publishes calibrated altitude to
ZBUS_CHAN_DECLARE(altitude_data_chan);
K_WORK_DELAYABLE_DEFINE(altimeter_calibration_work, periodic_calibration_handler);
K_WORK_DEFINE(altimeter_work, process_altimeter_work);
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
        LOG_DBG("Calibration GPS request failed: %d", ret);
        reschedule_calibration_work(false);
    }
}

static void http_rsp_cb(ble_http_status_code_t status, char *response)
{
    bool success = false;

    if (status == BLE_HTTP_STATUS_OK) {
        cJSON *parsed_response = cJSON_Parse(response);
        if (parsed_response == NULL) {
            LOG_DBG("Invalid calibration response");
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
                    .data.calibration = {
                        .pressure_msl_hpa = (float)pressure_msl_node->valuedouble,
                        .temperature_2m_c = (float)temperature_2m_node->valuedouble,
                    },
                };

                if (!isfinite(work_item.data.calibration.pressure_msl_hpa) ||
                    work_item.data.calibration.pressure_msl_hpa < 800.0f ||
                    work_item.data.calibration.pressure_msl_hpa > 1200.0f ||
                    !isfinite(work_item.data.calibration.temperature_2m_c) ||
                    work_item.data.calibration.temperature_2m_c < -100.0f ||
                    work_item.data.calibration.temperature_2m_c > 100.0f) {
                    LOG_DBG("Invalid calibration values discarded");
                } else {
                    success = enqueue_altimeter_work(&work_item);
                }
            }
        }

        cJSON_Delete(parsed_response);
    } else {
        LOG_DBG("Calibration HTTP failed: %d", status);
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

    char http_url[256];
    snprintf(http_url, sizeof(http_url), HTTP_REQUEST_URL_ALTIMETER_FMT, lat, lon);

    int ret = zsw_ble_http_get(http_url, http_rsp_cb);
    if (ret != 0) {
        LOG_DBG("Calibration HTTP request failed: %d", ret);
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
                if (zsw_floor_counter_set_temperature(work_item.data.calibration.temperature_2m_c + 273.15f) != 0) {
                    reschedule_calibration_work(false);
                    break;
                }
                alt_state.target_ref_slp = work_item.data.calibration.pressure_msl_hpa;
                alt_state.temperature_k = work_item.data.calibration.temperature_2m_c + 273.15f;
                LOG_DBG("Calibration updated");
                k_mutex_lock(&ui_data_mutex, K_FOREVER);
                ui_data.is_calibrated = true;
                ui_data_pending = true;
                k_mutex_unlock(&ui_data_mutex);
                publish_altitude_event();
                break;
            case ALTIMETER_WORK_PRESSURE:
                process_pressure_sample(&work_item);
                break;
        }
    }
    if (atomic_set(&floor_update_pending, 0)) {
        publish_altitude_event();
    }
}

static bool enqueue_altimeter_work(const altimeter_work_item_t *work_item)
{
    if (k_msgq_put(&altimeter_work_queue, work_item, K_NO_WAIT) != 0) {
        LOG_DBG("Altimeter queue full; event %d discarded", work_item->type);
        return false;
    }

    k_work_submit(&altimeter_work);
    return true;
}

static void on_floor_count_changed(void)
{
    atomic_set(&floor_update_pending, 1);
    k_work_submit(&altimeter_work);
}

static void process_pressure_sample(const altimeter_work_item_t *work_item)
{
    if (!isfinite(work_item->data.pressure_pa) || work_item->data.pressure_pa <= 0.0f) {
        return;
    }

    uint32_t timestamp_ms = work_item->timestamp_ms;
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

    float pressure_hpa = work_item->data.pressure_pa / 100.0f;
    float raw_altitude = get_current_altitude(pressure_hpa);
    if (!isfinite(raw_altitude)) {
        return;
    }
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

static void publish_altitude_event(void)
{
    struct altitude_event event;
    zsw_floor_counter_data_t counts = zsw_floor_counter_get_counts();

    k_mutex_lock(&ui_data_mutex, K_FOREVER);
    ui_data.floors_up = counts.floors_up;
    ui_data.floors_down = counts.floors_down;
    ui_data_pending = true;
    event = (struct altitude_event) {
        .altitude_m = ui_data.altitude,
        .is_calibrated = ui_data.is_calibrated,
        .floors_up = counts.floors_up,
        .floors_down = counts.floors_down,
    };
    k_mutex_unlock(&ui_data_mutex);

    int ret = zbus_chan_pub(&altitude_data_chan, &event, K_MSEC(250));
    if (ret != 0) {
        LOG_DBG("Altitude publication failed: %d", ret);
    }
}

static void on_zbus_pressure_callback(const struct zbus_channel *chan)
{
    const struct pressure_event *event = zbus_chan_const_msg(chan);
    altimeter_work_item_t work_item = {
        .type = ALTIMETER_WORK_PRESSURE,
        .data.pressure_pa = event->pressure,
        .timestamp_ms = (uint32_t)k_uptime_get_32(),
    };

    enqueue_altimeter_work(&work_item);
}

static void altimeter_app_start(lv_obj_t *root, lv_group_t *group)
{
    ARG_UNUSED(group);

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
    zsw_floor_counter_set_callback(on_floor_count_changed);
    zsw_app_manager_add_application(&app);
    return 0;
}

void zsw_altimeter_calibration_start(void)
{
    k_work_reschedule(&altimeter_calibration_work, K_NO_WAIT);
}

SYS_INIT(altimeter_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);