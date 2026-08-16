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

#define HTTP_REQUEST_URL_ALTIMETER_FMT "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f&current=pressure_msl&timezone=auto"

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
        cJSON *parsed_response = cJSON_Parse((char *)response);
        if (parsed_response == NULL) {
            LOG_ERR("Failed to parse JSON");
            return;
        }
        
        cJSON *current = cJSON_GetObjectItem(parsed_response, "current");
        if (current) {
            cJSON *pressure_node = cJSON_GetObjectItem(current, "pressure_msl");
            if (pressure_node && cJSON_IsNumber(pressure_node)) {
                // Update our global state
                current_sea_level_pressure = pressure_node->valuedouble;
                is_calibrated = true;
                
                LOG_INF("Successfully Calibrated MSL Pressure: %.2f hPa", current_sea_level_pressure);
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
}

static void altimeter_app_stop(void)
{
    LOG_INF("Altimeter app stopped!");
    altimeter_ui_remove(); 
}



static void periodic_fetch_altimeter_data(struct k_work *work)
{
    // Fetch GPS data and then make HTTP request to get altimeter data
    LOG_INF("Fetching altimeter data...");
    ble_comm_request_gps_status(true);
    k_work_submit(&altimeter_app_publish);
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
        altimeter_ui_update(current_altitude, current_raw_pressure, is_calibrated);
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