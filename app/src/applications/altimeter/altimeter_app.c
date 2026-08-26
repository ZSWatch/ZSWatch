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
#include "events/ble_event.h"
#include "events/pressure_event.h"
#include "altimeter_ui.h"
#include <zsw_clock.h>
#include <stdio.h>
#include <math.h>

LOG_MODULE_REGISTER(altimeter_app, LOG_LEVEL_DBG);

#define HTTP_REQUEST_URL_ALTIMETER_FMT "https://api.open-meteo.com/v1/elevation?latitude=%f&longitude=%f"
#define HISTORY_SAMPLES 24

// Functions needed for all applications
static void altimeter_app_start(lv_obj_t *root, lv_group_t *group);
static void altimeter_app_stop(void);
static void on_zbus_ble_data_callback(const struct zbus_channel *chan);
static void on_zbus_pressure_callback(const struct zbus_channel *chan);
static void periodic_fetch_altimeter_data(struct k_work *work);
static void publish_altimeter_data(struct k_work *work);
static void altimeter_data_timeout(struct k_work *work);
static void http_rsp_cb(ble_http_status_code_t status, char *response);
static void update_altimeter_ui_safely(struct k_work *work);

// Static variables
static float current_sea_level_pressure = 1013.25; // Default standard atmosphere
static bool is_calibrated = false;
static float current_altitude = 0.0;
static float current_raw_pressure = 0.0;
static float altitude_history[HISTORY_SAMPLES];
static uint8_t history_index = 0;
static bool has_history = false;

// ZBUS channel and listener for BLE communication
ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(altimeter_ble_comm_lis, on_zbus_ble_data_callback);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, altimeter_ble_comm_lis, 1);

// ZBUS channel and listener for Pressure sensor data
ZBUS_CHAN_DECLARE(pressure_data_chan);
ZBUS_LISTENER_DEFINE(altimeter_pressure_lis, on_zbus_pressure_callback);
ZBUS_CHAN_ADD_OBS(pressure_data_chan, altimeter_pressure_lis, 1);

K_WORK_DELAYABLE_DEFINE(altimeter_app_fetch_work, periodic_fetch_altimeter_data);
K_WORK_DEFINE(altimeter_app_publish, publish_altimeter_data);
K_WORK_DELAYABLE_DEFINE(altimeter_data_timeout_work, altimeter_data_timeout);

K_WORK_DEFINE(ui_update_work, update_altimeter_ui_safely);

ZSW_LV_IMG_DECLARE(altimeter_icon); 


static application_t app = {
    .name = "Altimeter",
    .icon = ZSW_LV_IMG_USE(altimeter_icon),
    .start_func = altimeter_app_start,
    .stop_func = altimeter_app_stop,
    .category = ZSW_APP_CATEGORY_SENSORS,
};

static void http_rsp_cb(ble_http_status_code_t status, char *response)
{
    if (status == BLE_HTTP_STATUS_OK) {
        cJSON *parsed_response = cJSON_Parse(response);
        if (parsed_response == NULL) {
            LOG_ERR("Failed to parse JSON");
            return;
        }
        
        cJSON *elevation_array = cJSON_GetObjectItem(parsed_response, "elevation");
        if (elevation_array && cJSON_IsArray(elevation_array)) {
            cJSON *elevation_node = cJSON_GetArrayItem(elevation_array, 0);
            if (elevation_node && cJSON_IsNumber(elevation_node)) {
                
                float real_altitude = elevation_node->valuedouble;
                
                // Only calibrate if we have received at least one raw pressure reading
                if (current_raw_pressure > 0.0) {
                    // Reverse hypsometric formula to get the localized sea level pressure
                    // P_sea = P / ( (1 - (h / 44330.0)) ^ (1 / 0.1903) )
                    // (1 / 0.1903) is approximately 5.255
                    current_sea_level_pressure = current_raw_pressure / pow(1.0 - (real_altitude / 44330.0), 5.255);
                    
                    is_calibrated = true;
                    LOG_INF("Calibrated! True Alt: %.1fm -> New MSL Reference: %.2f hPa", 
                            real_altitude, current_sea_level_pressure);
                            
                    // Trigger UI update directly so user sees the calibration instantly
                    k_work_submit(&ui_update_work);
                } else {
                    LOG_WRN("Received elevation but raw pressure is 0. Waiting for sensor.");
                }
            }
        }
        
        cJSON_Delete(parsed_response); // CRITICAL: prevent memory leaks
    } else {
        LOG_ERR("HTTP request failed with status: %d", status);
    }
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
    ble_comm_request_gps_status(true);

    // Kickstart the background data sampling for the chart immediately
    k_work_reschedule(&altimeter_app_fetch_work, K_NO_WAIT);
}

static void altimeter_app_stop(void)
{
    LOG_INF("Altimeter app stopped!");
    altimeter_ui_remove(); 
}



static void periodic_fetch_altimeter_data(struct k_work *work)
{
    // Save current altitude into the ring buffer history
    if (current_raw_pressure > 0.0) {
        altitude_history[history_index] = current_altitude; // <-- Save Altitude!
        history_index = (history_index + 1) % HISTORY_SAMPLES;
        has_history = true;
    } else {
        LOG_WRN("Skipped history sample (pressure is 0).");
    }
    
    // Update UI if screen is on
    k_work_submit(&ui_update_work);

    // Keep it at 1 minute for testing, change to K_HOURS(1) for production!
    k_work_reschedule(&altimeter_app_fetch_work, K_SECONDS(10));
}

static void publish_altimeter_data(struct k_work *work)
{
    LOG_INF("Publishing altimeter data...");
}

static void altimeter_data_timeout(struct k_work *work)
{
    LOG_INF("Altimeter data timeout!");
}

static void on_zbus_ble_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);

    if (event->data.type == BLE_COMM_DATA_TYPE_GPS) {
        // GPS received! Stop requesting if you want, or just proceed.
        ble_comm_request_gps_status(false);
        
        float lat = event->data.data.gps.lat;
        float lon = event->data.data.gps.lon;
        
        LOG_INF("GPS received: %.4f, %.4f. Requesting Altimeter data...", lat, lon);
        
        // Ensure you declare a buffer large enough
        static char http_url[256];
        snprintf(http_url, sizeof(http_url), HTTP_REQUEST_URL_ALTIMETER_FMT, lat, lon);
        
        // Execute the proxy HTTP GET
        int ret = zsw_ble_http_get(http_url, http_rsp_cb);
        if (ret != 0 && ret != -EBUSY) {
            LOG_ERR("Failed to proxy HTTP request: %d", ret);
        }
    }
}

static void update_altimeter_ui_safely(struct k_work *work)
{
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        altimeter_ui_update(current_altitude, current_raw_pressure, is_calibrated, 
                            altitude_history, history_index, has_history); // <-- Pass new array
    }
}

static void on_zbus_pressure_callback(const struct zbus_channel *chan)
{
    const struct pressure_event *event = zbus_chan_const_msg(chan);
    
    current_raw_pressure = event->pressure / 100.0; 
    current_altitude = 44330.0 * (1.0 - pow((current_raw_pressure / current_sea_level_pressure), 0.1903));
    
    k_work_submit(&ui_update_work); // Defer UI updates safely!
}

SYS_INIT(altimeter_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);