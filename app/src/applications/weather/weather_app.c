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

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include "cJSON.h"

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "events/ble_event.h"
#include <ble/ble_http.h>
#include "weather_ui.h"
#include <zsw_clock.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

LOG_MODULE_REGISTER(weather_app, LOG_LEVEL_DBG);

#define HTTP_REQUEST_URL_FMT "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f&current=wind_speed_10m,temperature_2m,apparent_temperature,weather_code&daily=weather_code,temperature_2m_max,temperature_2m_min,apparent_temperature_max,apparent_temperature_min,precipitation_sum,rain_sum,precipitation_probability_max&wind_speed_unit=ms&timezone=auto&forecast_days=%d"

#define MAX_GPS_AGED_TIME_MS 30 * 60 * 1000
#define WEATHER_BACKGROUND_FETCH_INTERVAL_S (30 * 60)

// Functions needed for all applications
static void weather_app_start(lv_obj_t *root, lv_group_t *group);
static void weather_app_stop(void);
static void on_zbus_ble_data_callback(const struct zbus_channel *chan);
static void periodic_fetch_weather_data(struct k_work *work);
static void publish_weather_data(struct k_work *work);
static void weather_data_timeout(struct k_work *work);
static void fetch_cached_weather_data(struct k_work *work);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(weather_ble_comm_lis, on_zbus_ble_data_callback);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, weather_ble_comm_lis, 1);

K_WORK_DELAYABLE_DEFINE(weather_app_fetch_work, periodic_fetch_weather_data);
K_WORK_DEFINE(weather_app_publish, publish_weather_data);
K_WORK_DELAYABLE_DEFINE(weather_data_timeout_work, weather_data_timeout);
K_WORK_DEFINE(weather_cached_fetch_work, fetch_cached_weather_data);

#define WEATHER_DATA_TIMEOUT_S  20

ZSW_LV_IMG_DECLARE(weather_app_icon);

static uint64_t last_update_gps_time;
static uint64_t last_update_weather_time;
static double last_lat;
static double last_lon;

static ble_comm_weather_t last_weather;

static application_t app = {
    .name = "Weather",
    .icon = ZSW_LV_IMG_USE(weather_app_icon),
    .start_func = weather_app_start,
    .stop_func = weather_app_stop,
    .category = ZSW_APP_CATEGORY_ROOT
};

static void http_rsp_cb(ble_http_status_code_t status, char *response)
{
    zsw_timeval_t time_now;
    weather_ui_current_weather_data_t current_weather = {0};
    weather_ui_forecast_data_t forecasts[WEATHER_UI_NUM_FORECASTS] = {0};
    // TODO: We should cache the last successful weather data and populate it when app opens.

    if (status == BLE_HTTP_STATUS_OK) {
        zsw_clock_get_time(&time_now);
        cJSON *parsed_response = cJSON_Parse(response);
        if (parsed_response == NULL) {
            LOG_ERR("Failed to parse weather JSON response");
            goto invalid_response;
        }
        cJSON *current = cJSON_GetObjectItem(parsed_response, "current");
        cJSON *current_temperature_2m = cJSON_GetObjectItem(current, "temperature_2m");
        cJSON *current_weather_code = cJSON_GetObjectItem(current, "weather_code");
        cJSON *current_wind_speed = cJSON_GetObjectItem(current, "wind_speed_10m");
        cJSON *apparent_temperature = cJSON_GetObjectItem(current, "apparent_temperature");

        cJSON *daily_forecasts = cJSON_GetObjectItem(parsed_response, "daily");

        cJSON *weather_code_list = cJSON_GetObjectItem(daily_forecasts, "weather_code");
        cJSON *temperature_2m_max_list = cJSON_GetObjectItem(daily_forecasts, "temperature_2m_max");
        cJSON *temperature_2m_min_list = cJSON_GetObjectItem(daily_forecasts, "temperature_2m_min");
        cJSON *precipitation_probability_max_list = cJSON_GetObjectItem(daily_forecasts, "precipitation_probability_max");

        int num_forecasts = cJSON_GetArraySize(weather_code_list);
        if (!cJSON_IsNumber(current_temperature_2m) || !cJSON_IsNumber(current_weather_code) ||
            !cJSON_IsNumber(current_wind_speed) || !cJSON_IsNumber(apparent_temperature) ||
            num_forecasts < 1 || num_forecasts > WEATHER_UI_NUM_FORECASTS ||
            !cJSON_IsArray(weather_code_list) || !cJSON_IsArray(temperature_2m_max_list) ||
            !cJSON_IsArray(temperature_2m_min_list) || !cJSON_IsArray(precipitation_probability_max_list) ||
            cJSON_GetArraySize(temperature_2m_max_list) != num_forecasts ||
            cJSON_GetArraySize(temperature_2m_min_list) != num_forecasts ||
            cJSON_GetArraySize(precipitation_probability_max_list) != num_forecasts) {
            cJSON_Delete(parsed_response);
            goto invalid_response;
        }
        current_weather.temperature = current_temperature_2m->valuedouble;
        current_weather.wind_speed = current_wind_speed->valuedouble;
        current_weather.apparent_temperature = apparent_temperature->valuedouble;
        current_weather.icon = zsw_ui_utils_icon_from_wmo_weather_code(current_weather_code->valueint,
                                                                       &current_weather.color, &current_weather.text);
        for (int i = 0; i < num_forecasts; i++) {
            cJSON *high = cJSON_GetArrayItem(temperature_2m_max_list, i);
            cJSON *low = cJSON_GetArrayItem(temperature_2m_min_list, i);
            cJSON *rain = cJSON_GetArrayItem(precipitation_probability_max_list, i);
            cJSON *code = cJSON_GetArrayItem(weather_code_list, i);
            if (!cJSON_IsNumber(high) || !cJSON_IsNumber(low) ||
                !cJSON_IsNumber(rain) || !cJSON_IsNumber(code)) {
                cJSON_Delete(parsed_response);
                goto invalid_response;
            }
            forecasts[i].temperature = high->valuedouble;
            forecasts[i].low_temp = low->valuedouble;
            forecasts[i].high_temp = high->valuedouble;
            forecasts[i].rain_percent = rain->valueint;
            forecasts[i].icon = zsw_ui_utils_icon_from_wmo_weather_code(code->valueint,
                                                                        &forecasts[i].color, &forecasts[i].text);
            snprintf(forecasts[i].day, sizeof(forecasts[i].day), "%s",
                     zsw_ui_utils_day_name((time_now.tm.tm_wday + i) % 7));
        }
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            weather_ui_set_weather_data(current_weather, forecasts, num_forecasts);
            LOG_INF("Weather UI updated (%d forecasts)", num_forecasts);
        }

        ble_comm_request_gps_status(false);
        last_weather.temperature_c = current_weather.temperature;
        last_weather.humidity = 0;
        last_weather.wind = current_weather.wind_speed;
        last_weather.wind_direction = 0;
        last_weather.weather_code = wmo_code_to_weather_code(current_weather_code->valueint);
        strncpy(last_weather.report_text, current_weather.text, sizeof(last_weather.report_text));

        cJSON_Delete(parsed_response);
        last_update_weather_time = k_uptime_get();

        k_work_submit(&weather_app_publish);
    } else {
        LOG_ERR("HTTP request failed\n");
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            weather_ui_set_error(status == BLE_HTTP_STATUS_TIMEOUT ? "Timeout" : "Failed");
        }
    }
    return;

invalid_response:
    LOG_ERR("Invalid weather response");
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        weather_ui_set_error("Invalid weather data");
    }
}

static void publish_weather_data(struct k_work *work)
{
    ble_comm_cb_data_t data;
    data.type = BLE_COMM_DATA_TYPE_WEATHER;
    data.data.weather = last_weather;
    zbus_chan_pub(&ble_comm_data_chan, &data, K_MSEC(250));
}

static void fetch_weather_data(double lat, double lon)
{
    char weather_url[512];
    snprintf(weather_url, sizeof(weather_url), HTTP_REQUEST_URL_FMT, lat, lon, WEATHER_UI_NUM_FORECASTS);
    int ret = zsw_ble_http_get(weather_url, http_rsp_cb);
    if (ret != 0 && ret != -EBUSY) {
        LOG_ERR("Failed to send HTTP request: %d", ret);
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            weather_ui_set_error("Failed fetching weather");
        }
    }
}

static void fetch_cached_weather_data(struct k_work *work)
{
    ARG_UNUSED(work);
    fetch_weather_data(last_lat, last_lon);
}

static void periodic_fetch_weather_data(struct k_work *work)
{
    int ret = ble_comm_request_gps_status(true);
    if (ret != 0) {
        LOG_ERR("Failed to disable phone GPS: %d", ret);
    }
    k_work_reschedule(&weather_app_fetch_work, K_SECONDS(WEATHER_BACKGROUND_FETCH_INTERVAL_S));
}

static void weather_data_timeout(struct k_work *work)
{
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        weather_ui_set_error("No data received\nMake sure phone is connected");
    }
}

static void on_zbus_ble_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);

    if (event->data.type == BLE_COMM_DATA_TYPE_GPS) {
        double lat = event->data.data.gps.lat;
        double lon = event->data.data.gps.lon;
        if (!isfinite(lat) || !isfinite(lon) || lat < -90 || lat > 90 || lon < -180 || lon > 180) {
            return;
        }
        k_work_cancel_delayable(&weather_data_timeout_work);
        last_update_gps_time = k_uptime_get();
        LOG_DBG("Got GPS data, fetch weather");
        last_lat = lat;
        last_lon = lon;
        fetch_weather_data(lat, lon);
        int ret = ble_comm_request_gps_status(false);
        if (ret != 0) {
            LOG_ERR("Failed to stop GPS data: %d", ret);
        }
    }
}

static void weather_app_start(lv_obj_t *root, lv_group_t *group)
{
    weather_ui_show(root);
    if (last_update_gps_time == 0 || k_uptime_get() - last_update_gps_time > MAX_GPS_AGED_TIME_MS) {
        LOG_DBG("GPS data is too old, request GPS\n");
        int res = ble_comm_request_gps_status(true);
        if (res != 0) {
            LOG_ERR("Failed to request GPS data: %d", res);
            weather_ui_set_error("Failed to get GPS data");
        } else {
            k_work_reschedule(&weather_data_timeout_work, K_SECONDS(WEATHER_DATA_TIMEOUT_S));
        }
        // TODO Show GPS fetching in progress in app
    } else {
        /* App startup runs inside the LVGL timer handler. Do not put the URL
         * buffer and Bluetooth send on top of that rendering stack.
         */
        k_work_submit(&weather_cached_fetch_work);
    }

    zsw_timeval_t time;
    zsw_clock_get_time(&time);
    weather_ui_set_time(time.tm.tm_hour, time.tm.tm_min, time.tm.tm_sec);
}

static void weather_app_stop(void)
{
    k_work_cancel_delayable(&weather_data_timeout_work);
    weather_ui_remove();
    ble_comm_request_gps_status(false);
}

static int weather_app_add(void)
{
    zsw_app_manager_add_application(&app);

    k_work_reschedule(&weather_app_fetch_work, K_SECONDS(30));

    return 0;
}

SYS_INIT(weather_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
