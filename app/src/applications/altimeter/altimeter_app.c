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
#include "managers/zsw_power_manager.h"
#include <ble/ble_http.h>
#include "events/ble_event.h"
#include "events/pressure_event.h"
#include "events/altitude_event.h"
#include "altimeter_ui.h"
#include <zsw_clock.h>
#include <stdio.h>
#include <math.h>

LOG_MODULE_REGISTER(altimeter_app, LOG_LEVEL_DBG);

#define HTTP_REQUEST_URL_ALTIMETER_FMT "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f&current=temperature_2m,pressure_msl&models=dmi_seamless"
#define ALTIMETER_CALIBRATION_INTERVAL_S (30 * 60)

// Standard Atmosphere model constants (float32 to leverage the FPU)
#define LAPSE_EXP   5.255f
#define CONST_44330 44330.0f
#define EMA_ALPHA   0.2f

// Floor climb/descend detection (Apple/Fitbit-style: 3m per floor, hysteresis rejects sensor noise)
#define FLOOR_HEIGHT_M      3.0f
#define FLOOR_HYSTERESIS_M  0.5f

// type definitions for altimeter state, floor count state, and work items
typedef struct {
    float ref_slp;            // Reference mean sea-level pressure (hPa)
    float filtered_pressure;  // EMA-smoothed pressure (hPa)
} altimeter_state_t;

typedef struct {
    float last_altitude;      // Previous sample, for delta computation
    bool has_last_altitude;
    float climb_accum;        // Accumulated gain (m) toward next floor up
    float descend_accum;      // Accumulated loss (m) toward next floor down
    uint16_t floors_up;       // Floors climbed today
    uint16_t floors_down;     // Floors descended today
    int last_reset_yday;      // tm_yday of last daily reset, -1 = not yet initialized
} floor_count_state_t;

typedef enum {
    ALTIMETER_WORK_PRESSURE,
    ALTIMETER_WORK_CALIBRATION,
} altimeter_work_type_t;

typedef struct {
    altimeter_work_type_t type;
    float value;
} altimeter_work_item_t;

// Functions needed for all applications
static void altimeter_app_start(lv_obj_t *root, lv_group_t *group);
static void altimeter_app_stop(void);
static void on_zbus_ble_data_callback(const struct zbus_channel *chan);
static void on_zbus_pressure_callback(const struct zbus_channel *chan);
static void periodic_calibration_handler(struct k_work *work);
static void http_rsp_cb(ble_http_status_code_t status, char *response);
static void update_altimeter_ui(struct k_work *work);
static void process_altimeter_work(struct k_work *work);
static float get_current_altitude(float pressure_sensor);
static void check_daily_floor_reset(void);
static void update_floor_count(float altitude);

// Static variables
static altimeter_state_t alt_state = { .ref_slp = 1013.25f, .filtered_pressure = 0.0f };
static floor_count_state_t floor_state = { .last_reset_yday = -1 };
static float altitude_history[HISTORY_SAMPLES];
static altimeter_ui_data_t ui_data = {
    .altitude = 0.0f,
    .raw_pressure = 0.0f,
    .history_data = altitude_history,
    .start_idx = 0,
    .valid_count = 0,
    .is_calibrated = false,
    .has_data = false
};

static application_t app = {
    .name = "Altimeter",
    .icon = ZSW_LV_IMG_USE(altimeter_icon),
    .start_func = altimeter_app_start,
    .stop_func = altimeter_app_stop,
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
K_WORK_DEFINE(ui_update_work, update_altimeter_ui);
K_WORK_DEFINE(altimeter_work, process_altimeter_work);
K_MSGQ_DEFINE(altimeter_work_queue, sizeof(altimeter_work_item_t), 8, 4);

ZSW_LV_IMG_DECLARE(altimeter_icon);

static float get_current_altitude(float pressure_sensor)
{
    if (alt_state.filtered_pressure <= 0.0f) {
        // for the first reading, just assigned
        alt_state.filtered_pressure = pressure_sensor;
    } else {
        alt_state.filtered_pressure = (EMA_ALPHA * pressure_sensor) +
                                      ((1.0f - EMA_ALPHA) * alt_state.filtered_pressure);
    }

    if (alt_state.ref_slp <= 0.0f) {
        return 0.0f;
    }

    float ratio = alt_state.filtered_pressure / alt_state.ref_slp;
    float power_term = powf(ratio, 1.0f / LAPSE_EXP);

    return CONST_44330 * (1.0f - power_term);
}

static void periodic_calibration_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    int ret = ble_comm_request_gps_status(true);

    if (ret != 0) {
        LOG_ERR("Failed to request GPS for calibration: %d", ret);
    }

    // Always reschedule so recalibration keeps recurring even if this cycle failed
    k_work_reschedule(&altimeter_calibration_work, K_SECONDS(ALTIMETER_CALIBRATION_INTERVAL_S));
}

static void http_rsp_cb(ble_http_status_code_t status, char *response)
{
    if (app.current_state != ZSW_APP_STATE_UI_VISIBLE) {
        // App was closed before this late response arrived, avoid mutating stopped app's state
        return;
    }

    if (status == BLE_HTTP_STATUS_OK) {
        cJSON *parsed_response = cJSON_Parse(response);
        if (parsed_response == NULL) {
            LOG_ERR("Failed to parse JSON");
            return;
        }

        cJSON *current = cJSON_GetObjectItem(parsed_response, "current");
        if (current) {
            cJSON *pressure_msl_node = cJSON_GetObjectItem(current, "pressure_msl");
            cJSON *temperature_2m_node = cJSON_GetObjectItem(current, "temperature_2m");

            if (pressure_msl_node && cJSON_IsNumber(pressure_msl_node)) {
                altimeter_work_item_t work_item = {
                    .type = ALTIMETER_WORK_CALIBRATION,
                    .value = (float)pressure_msl_node->valuedouble,
                };

                if (k_msgq_put(&altimeter_work_queue, &work_item, K_NO_WAIT) != 0) {
                    LOG_WRN("Altimeter work queue full; calibration discarded");
                } else {
                    k_work_submit(&altimeter_work);
                }

                float temp = 0.0f;
                if (temperature_2m_node && cJSON_IsNumber(temperature_2m_node)) {
                    temp = (float)temperature_2m_node->valuedouble;
                }
                LOG_INF("Calibrated! New MSL Reference: %.2f hPa (Temp 2m: %.1f C)",
                        work_item.value, temp);
            } else {
                LOG_WRN("Missing or invalid pressure_msl in JSON response.");
            }
        } else {
            LOG_WRN("Missing current object in JSON response.");
        }

        cJSON_Delete(parsed_response); // CRITICAL: prevent memory leaks
    } else {
        LOG_ERR("HTTP request failed with status: %d", status);
    }
}

static void on_zbus_ble_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);

    if (event->data.type != BLE_COMM_DATA_TYPE_GPS) {
        return;
    }

    if (app.current_state != ZSW_APP_STATE_UI_VISIBLE) {
        // App was closed before this late GPS event arrived, ignore to avoid starting calibration work
        return;
    }

    ble_comm_request_gps_status(false);

    float lat = event->data.data.gps.lat;
    float lon = event->data.data.gps.lon;

    LOG_INF("GPS received: %.4f, %.4f. Requesting Altimeter data...", lat, lon);

    char http_url[256];
    snprintf(http_url, sizeof(http_url), HTTP_REQUEST_URL_ALTIMETER_FMT, lat, lon);

    int ret = zsw_ble_http_get(http_url, http_rsp_cb);
    if (ret != 0 && ret != -EBUSY) {
        LOG_ERR("Failed to proxy HTTP request: %d", ret);
    }
}

static void update_altimeter_ui(struct k_work *work)
{
    ARG_UNUSED(work);

    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        zsw_power_manager_reset_idle_timout();
        altimeter_ui_update(&ui_data);
        altimeter_ui_notify_data_ready();
    }
}

static void process_altimeter_work(struct k_work *work)
{
    altimeter_work_item_t work_item;

    ARG_UNUSED(work);

    while (k_msgq_get(&altimeter_work_queue, &work_item, K_NO_WAIT) == 0) {
        if (work_item.type == ALTIMETER_WORK_CALIBRATION) {
            alt_state.ref_slp = work_item.value;
            ui_data.is_calibrated = true;
            ui_data.has_data = false;
            ui_data.start_idx = 0;
            ui_data.valid_count = 0;
            floor_state.has_last_altitude = false;
            k_work_submit(&ui_update_work);
            continue;
        }

        check_daily_floor_reset();

        float current_raw_pressure = work_item.value / 100.0f;
        ui_data.altitude = get_current_altitude(current_raw_pressure);
        ui_data.raw_pressure = current_raw_pressure;

        if (current_raw_pressure > 0.0f) {
            altitude_history[ui_data.start_idx] = ui_data.altitude;
            ui_data.start_idx = (ui_data.start_idx + 1) % HISTORY_SAMPLES;
            if (ui_data.valid_count < HISTORY_SAMPLES) {
                ui_data.valid_count++;
            }
            ui_data.has_data = true;
        }

        update_floor_count(ui_data.altitude);
        ui_data.floors_up = floor_state.floors_up;
        ui_data.floors_down = floor_state.floors_down;

        struct altitude_event alt_evt = {
            .altitude_m = ui_data.altitude,
            .is_calibrated = ui_data.is_calibrated,
            .floors_up = floor_state.floors_up,
            .floors_down = floor_state.floors_down,
        };
        zbus_chan_pub(&altitude_data_chan, &alt_evt, K_MSEC(250));
        k_work_submit(&ui_update_work);
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

static void update_floor_count(float altitude)
{
    if (!floor_state.has_last_altitude) {
        floor_state.last_altitude = altitude;
        floor_state.has_last_altitude = true;
        return;
    }

    float delta = altitude - floor_state.last_altitude;

    if (delta > 0.0f) {
        // Reversal beyond hysteresis discards opposite progress, otherwise noise is ignored
        floor_state.descend_accum = (delta > FLOOR_HYSTERESIS_M) ? 0.0f :
                                    MAX(0.0f, floor_state.descend_accum - delta);
        floor_state.climb_accum += delta;
    } else if (delta < 0.0f) {
        floor_state.climb_accum = (-delta > FLOOR_HYSTERESIS_M) ? 0.0f :
                                  MAX(0.0f, floor_state.climb_accum + delta);
        floor_state.descend_accum -= delta;
    }

    while (floor_state.climb_accum >= FLOOR_HEIGHT_M) {
        floor_state.floors_up++;
        floor_state.climb_accum -= FLOOR_HEIGHT_M;
    }
    while (floor_state.descend_accum >= FLOOR_HEIGHT_M) {
        floor_state.floors_down++;
        floor_state.descend_accum -= FLOOR_HEIGHT_M;
    }

    floor_state.last_altitude = altitude;
}

static void on_zbus_pressure_callback(const struct zbus_channel *chan)
{
    const struct pressure_event *event = zbus_chan_const_msg(chan);
    altimeter_work_item_t work_item = {
        .type = ALTIMETER_WORK_PRESSURE,
        .value = event->pressure,
    };

    if (k_msgq_put(&altimeter_work_queue, &work_item, K_NO_WAIT) != 0) {
        LOG_WRN("Altimeter work queue full; pressure sample discarded");
        return;
    }

    k_work_submit(&altimeter_work);
}

static void altimeter_app_start(lv_obj_t *root, lv_group_t *group)
{
    ARG_UNUSED(group);

    LOG_INF("Altimeter app started!");
    altimeter_ui_show(root);
    altimeter_ui_update(&ui_data); // Show any stale data from a previous session immediately

    // Run the first calibration now, periodic_calibration_handler takes it from here
    k_work_reschedule(&altimeter_calibration_work, K_NO_WAIT);
}

static void altimeter_app_stop(void)
{
    LOG_INF("Altimeter app stopped!");
    altimeter_ui_remove();
    k_work_cancel_delayable(&altimeter_calibration_work);
    k_work_cancel(&ui_update_work);
    ble_comm_request_gps_status(false);
}

static int altimeter_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(altimeter_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);