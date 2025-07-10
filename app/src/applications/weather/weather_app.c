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

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(weather_ble_comm_lis, on_zbus_ble_data_callback);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, weather_ble_comm_lis, 1);

K_WORK_DELAYABLE_DEFINE(weather_app_fetch_work, periodic_fetch_weather_data);
K_WORK_DEFINE(weather_app_publish, publish_weather_data);

ZSW_LV_IMG_DECLARE(weather_app_icon);

static bool active;
static uint64_t last_update_gps_time;
static uint64_t last_update_weather_time;
static double last_lat;
static double last_lon;

static ble_comm_weather_t last_weather;

static application_t app = {
    .name = "Weather",
    .icon = ZSW_LV_IMG_USE(weather_app_icon),
    .start_func = weather_app_start,
    .stop_func = weather_app_stop
};

static void http_rsp_cb(ble_http_status_code_t status, char *response)
{
    zsw_timeval_t time_now;
    weather_ui_current_weather_data_t current_weather;
    weather_ui_forecast_data_t forecasts[WEATHER_UI_NUM_FORECASTS];
    char *days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

    if (status == BLE_HTTP_STATUS_OK) {
        zsw_clock_get_time(&time_now);
        cJSON *parsed_response = cJSON_Parse(response);
        cJSON *current = cJSON_GetObjectItem(parsed_response, "current");
        cJSON *current_temperature_2m = cJSON_GetObjectItem(current, "temperature_2m");
        current_weather.temperature = current_temperature_2m->valuedouble;
        cJSON *current_weather_code = cJSON_GetObjectItem(current, "weather_code");
        current_weather.icon = zsw_ui_utils_icon_from_wmo_weather_code(current_weather_code->valueint, &current_weather.color,
                                                                       &current_weather.text);
        cJSON *current_wind_speed = cJSON_GetObjectItem(current, "wind_speed_10m");
        current_weather.wind_speed = current_wind_speed->valuedouble;
        cJSON *apparent_temperature = cJSON_GetObjectItem(current, "apparent_temperature");
        current_weather.apparent_temperature = apparent_temperature->valuedouble;

        cJSON *daily_forecasts = cJSON_GetObjectItem(parsed_response, "daily");

        cJSON *weather_code_list = cJSON_GetObjectItem(daily_forecasts, "weather_code");
        cJSON *temperature_2m_max_list = cJSON_GetObjectItem(daily_forecasts, "temperature_2m_max");
        cJSON *temperature_2m_min_list = cJSON_GetObjectItem(daily_forecasts, "temperature_2m_min");
        cJSON *precipitation_probability_max_list = cJSON_GetObjectItem(daily_forecasts, "precipitation_probability_max");

        for (int i = 0; i < cJSON_GetArraySize(weather_code_list); i++) {
            forecasts[i].temperature = cJSON_GetArrayItem(temperature_2m_max_list, i)->valuedouble;
            forecasts[i].low_temp = cJSON_GetArrayItem(temperature_2m_min_list, i)->valuedouble;
            forecasts[i].high_temp = cJSON_GetArrayItem(temperature_2m_max_list, i)->valuedouble;
            forecasts[i].rain_percent = cJSON_GetArrayItem(precipitation_probability_max_list, i)->valueint;
            forecasts[i].icon = zsw_ui_utils_icon_from_wmo_weather_code(cJSON_GetArrayItem(weather_code_list, i)->valueint,
                                                                        &forecasts[i].color, &forecasts[i].text);
            sprintf(forecasts[i].day, "%s", days[(time_now.tm.tm_wday + i) % 7]);
        }

        weather_ui_set_weather_data(current_weather, forecasts, cJSON_GetArraySize(weather_code_list));

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
        weather_ui_set_error(status == BLE_HTTP_STATUS_TIMEOUT ? "Timeout" : "Failed");
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
        weather_ui_set_error("Failed fetching weather");
    }
}

static void periodic_fetch_weather_data(struct k_work *work)
{
    int ret = ble_comm_request_gps_status(true);
    if (ret != 0) {
        LOG_ERR("Failed to disable phone GPS: %d", ret);
    }
    k_work_reschedule(&weather_app_fetch_work, K_SECONDS(WEATHER_BACKGROUND_FETCH_INTERVAL_S));
}

static void on_zbus_ble_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);

    if (event->data.type == BLE_COMM_DATA_TYPE_GPS) {
        last_update_gps_time = k_uptime_get();
        LOG_DBG("Got GPS data, fetch weather\n");
        LOG_DBG("Latitude: %f\n", event->data.data.gps.lat);
        LOG_DBG("Longitude: %f\n", event->data.data.gps.lon);
        last_lat = event->data.data.gps.lat;
        last_lon = event->data.data.gps.lon;
        fetch_weather_data(event->data.data.gps.lat, event->data.data.gps.lon);
        int ret = ble_comm_request_gps_status(false);
        if (ret != 0) {
            LOG_ERR("Failed to request GPS data: %d", ret);
        }
    }
}

static void weather_app_start(lv_obj_t *root, lv_group_t *group)
{
    weather_ui_show(root);
    if (last_update_gps_time == 0 || k_uptime_delta(&last_update_gps_time) > MAX_GPS_AGED_TIME_MS) {
        LOG_DBG("GPS data is too old, request GPS\n");
        int res = ble_comm_request_gps_status(true);
        if (res != 0) {
            LOG_ERR("Failed to request GPS data: %d",res);
            weather_ui_set_error("Failed to get GPS data");
        }
        // TODO Show GPS fetching in progress in app
    } else {
        fetch_weather_data(last_lat, last_lon);
    }

    zsw_timeval_t time;
    zsw_clock_get_time(&time);
    weather_ui_set_time(time.tm.tm_hour, time.tm.tm_min, time.tm.tm_sec);
    active = true;
}

static void weather_app_stop(void)
{
    active = false;
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
