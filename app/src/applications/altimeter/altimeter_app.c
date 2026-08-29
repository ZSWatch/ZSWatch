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

// Functions needed for all applications
static void altimeter_app_start(lv_obj_t *root, lv_group_t *group);
static void altimeter_app_stop(void);
static void on_zbus_ble_data_callback(const struct zbus_channel *chan);
static void on_zbus_pressure_callback(const struct zbus_channel *chan);
static void periodic_calibration_handler(struct k_work *work);
static void http_rsp_cb(ble_http_status_code_t status, char *response);
static void update_altimeter_ui(struct k_work *work);
static float get_current_altitude(float pressure_sensor);

typedef struct {
    float ref_slp;            // Reference mean sea-level pressure (hPa)
    float filtered_pressure;  // EMA-smoothed pressure (hPa)
} altimeter_state_t;

// Static variables
static altimeter_state_t alt_state = { .ref_slp = 1013.25f, .filtered_pressure = 0.0f };
static float altitude_history[HISTORY_SAMPLES];
static altimeter_ui_data_t ui_data = {
    .altitude = 0.0f,
    .raw_pressure = 0.0f,
    .history_data = altitude_history,
    .start_idx = 0,
    .history_count = HISTORY_SAMPLES,
    .is_calibrated = false,
    .has_data = false
};

// ZBUS channel and listener for BLE communication
ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(altimeter_ble_comm_lis, on_zbus_ble_data_callback);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, altimeter_ble_comm_lis, 1);

// ZBUS channel and listener for Pressure sensor data
ZBUS_CHAN_DECLARE(pressure_data_chan);
ZBUS_LISTENER_DEFINE(altimeter_pressure_lis, on_zbus_pressure_callback);
ZBUS_CHAN_ADD_OBS(pressure_data_chan, altimeter_pressure_lis, 1);

K_WORK_DELAYABLE_DEFINE(altimeter_calibration_work, periodic_calibration_handler);

K_WORK_DEFINE(ui_update_work, update_altimeter_ui);

ZSW_LV_IMG_DECLARE(altimeter_icon);


static application_t app = {
    .name = "Altimeter",
    .icon = ZSW_LV_IMG_USE(altimeter_icon),
    .start_func = altimeter_app_start,
    .stop_func = altimeter_app_stop,
    .category = ZSW_APP_CATEGORY_SENSORS,
};

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

static int altimeter_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

static void altimeter_app_start(lv_obj_t *root, lv_group_t *group)
{
    LOG_INF("Altimeter app started!");
    altimeter_ui_show(root);

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



static void periodic_calibration_handler(struct k_work *work)
{
    int ret = ble_comm_request_gps_status(true);

    if (ret != 0) {
        LOG_ERR("Failed to request GPS for calibration: %d", ret);
    }

    // Always reschedule so recalibration keeps recurring even if this cycle failed
    k_work_reschedule(&altimeter_calibration_work, K_SECONDS(ALTIMETER_CALIBRATION_INTERVAL_S));
}

static void http_rsp_cb(ble_http_status_code_t status, char *response)
{
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
                alt_state.ref_slp = (float)pressure_msl_node->valuedouble;
                ui_data.is_calibrated = true;
                ui_data.has_data = false; // don't present this time, wait for next pressure reading to populate history
                float temp = 0.0f;
                if (temperature_2m_node && cJSON_IsNumber(temperature_2m_node)) {
                    temp = (float)temperature_2m_node->valuedouble;
                }
                LOG_INF("Calibrated! New MSL Reference: %.2f hPa (Temp 2m: %.1f C)",
                        alt_state.ref_slp, temp);

                // Trigger UI update directly so user sees the calibration instantly
                k_work_submit(&ui_update_work);
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

    if (event->data.type == BLE_COMM_DATA_TYPE_GPS) {
        ble_comm_request_gps_status(false);

        float lat = event->data.data.gps.lat;
        float lon = event->data.data.gps.lon;

        LOG_INF("GPS received: %.4f, %.4f. Requesting Altimeter data...", lat, lon);

        static char http_url[256];
        snprintf(http_url, sizeof(http_url), HTTP_REQUEST_URL_ALTIMETER_FMT, lat, lon);

        int ret = zsw_ble_http_get(http_url, http_rsp_cb);
        if (ret != 0 && ret != -EBUSY) {
            LOG_ERR("Failed to proxy HTTP request: %d", ret);
        }
    }
}

static void update_altimeter_ui(struct k_work *work)
{
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        zsw_power_manager_reset_idle_timout();
        altimeter_ui_update(&ui_data);
    }
}

static void on_zbus_pressure_callback(const struct zbus_channel *chan)
{
    const struct pressure_event *event = zbus_chan_const_msg(chan);

    float current_raw_pressure = event->pressure / 100.0f; // Convert Pa to hPa
    ui_data.altitude = get_current_altitude(current_raw_pressure);
    ui_data.raw_pressure = current_raw_pressure;
    // Append to altitude history
    if (current_raw_pressure > 0.0f) {
        altitude_history[ui_data.start_idx] = ui_data.altitude;
        ui_data.start_idx = (ui_data.start_idx + 1) % HISTORY_SAMPLES;
        ui_data.has_data = true;
    }

    k_work_submit(&ui_update_work); // Defer UI updates safely!
}

SYS_INIT(altimeter_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);