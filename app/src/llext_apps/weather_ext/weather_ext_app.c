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

/*
 * LLEXT version of the Weather app.
 * Combines weather_app.c and weather_ui.c into a single LLEXT module.
 *
 * Differences from built-in:
 *   - Background periodic fetch is started/stopped with app start/stop
 *     (no boot-time scheduling since LLEXT can't survive XIP disable for work handlers)
 *   - Zbus observer registered at runtime
 *   - K_WORK_DELAYABLE_DEFINE → k_work_init_delayable() at load time
 *   - K_WORK_DEFINE → k_work_init() at load time
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <zephyr/sys/printk.h>
#include <zephyr/zbus/zbus.h>
#include <lvgl.h>
#include <stdio.h>

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "events/ble_event.h"
#include "ble/ble_http.h"
#include "ble/ble_comm.h"
#include "zsw_clock.h"
#include "cJSON.h"
#include "ui/zsw_ui.h"

#include "managers/zsw_llext_iflash.h"

#define HTTP_REQUEST_URL_FMT "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f&current=wind_speed_10m,temperature_2m,apparent_temperature,weather_code&daily=weather_code,temperature_2m_max,temperature_2m_min,apparent_temperature_max,apparent_temperature_min,precipitation_sum,rain_sum,precipitation_probability_max&wind_speed_unit=ms&timezone=auto&forecast_days=%d"

#define MAX_GPS_AGED_TIME_MS (30 * 60 * 1000)
#define WEATHER_BACKGROUND_FETCH_INTERVAL_S (30 * 60)
#define WEATHER_DATA_TIMEOUT_S  20
#define WEATHER_UI_NUM_FORECASTS 4

/* ---- Icon image compiled into .rodata (XIP flash) ---- */
#include "weather_app_icon.c"

/* ---- Forward declarations ---- */
static void weather_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void weather_app_stop(void *user_data);
static void on_zbus_ble_data_callback(const struct zbus_channel *chan);
static void periodic_fetch_weather_data(struct k_work *work);
static void publish_weather_data(struct k_work *work);
static void weather_data_timeout(struct k_work *work);

/* ---- Zbus: runtime observer ---- */
ZBUS_CHAN_DECLARE(ble_comm_data_chan);

static struct zbus_observer_data weather_ext_obs_data = {
    .enabled = true,
};

static struct zbus_observer weather_ext_listener = {
#if defined(CONFIG_ZBUS_OBSERVER_NAME)
    .name = "wea_ext_lis",
#endif
    .type = ZBUS_OBSERVER_LISTENER_TYPE,
    .data = &weather_ext_obs_data,
    .callback = on_zbus_ble_data_callback,
};

/* ---- Work items (runtime initialized) ---- */
static struct k_work_delayable weather_app_fetch_work;
static struct k_work weather_app_publish;
static struct k_work_delayable weather_data_timeout_work;

/* ---- App registration ---- */
static application_t app = {
    .name = "Weather",
    .icon = &weather_app_icon,
    .start_func = weather_app_start,
    .stop_func = weather_app_stop,
    .category = ZSW_APP_CATEGORY_ROOT,
};

static uint64_t last_update_gps_time;
static uint64_t last_update_weather_time;
static double last_lat;
static double last_lon;
static ble_comm_weather_t last_weather;

/* ===========================================================================
 * Weather UI (from weather_ui.c)
 * =========================================================================== */

typedef struct {
    double temperature;
    double apparent_temperature;
    double wind_speed;
    const void *icon;
    char *text;
    lv_color_t color;
} weather_ui_current_weather_data_t;

typedef struct {
    double temperature;
    int rain_percent;
    const void *icon;
    double low_temp;
    double high_temp;
    char day[4];
    char *text;
    lv_color_t color;
} weather_ui_forecast_data_t;

typedef struct {
    lv_obj_t *ui_day;
    lv_obj_t *ui_day_temp;
    lv_obj_t *ui_day_icon;
    lv_obj_t *ui_day_day;
} lv_obj_forecasts_t;

static lv_obj_t *root_page;
static lv_obj_t *ui_bg_img;
static lv_obj_t *ui_root_container;
static lv_obj_t *ui_status_label;
static lv_obj_t *ui_forecast_widget;
static lv_obj_t *ui_time;
static lv_obj_t *ui_today_container;
static lv_obj_t *ui_today_icon;
static lv_obj_t *ui_today_temp;
static lv_obj_t *ui_today_min_max_temp;
static lv_obj_t *ui_today_rain;
static lv_obj_t *ui_water_drop_img;
static lv_obj_t *ui_loading_spinner;

static lv_obj_forecasts_t ui_forecasts[WEATHER_UI_NUM_FORECASTS];

static void add_forecast_day(lv_obj_t *parent, lv_obj_forecasts_t *storage)
{
    storage->ui_day = lv_obj_create(parent);
    lv_obj_remove_style_all(storage->ui_day);
    lv_obj_set_width(storage->ui_day, LV_SIZE_CONTENT);
    lv_obj_set_height(storage->ui_day, LV_SIZE_CONTENT);
    lv_obj_set_align(storage->ui_day, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(storage->ui_day, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(storage->ui_day, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(storage->ui_day, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    storage->ui_day_temp = lv_label_create(storage->ui_day);
    lv_obj_set_width(storage->ui_day_temp, LV_SIZE_CONTENT);
    lv_obj_set_height(storage->ui_day_temp, LV_SIZE_CONTENT);
    lv_obj_set_align(storage->ui_day_temp, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(storage->ui_day_temp, lv_color_hex(0x5AA1EE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(storage->ui_day_temp, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    storage->ui_day_icon = lv_image_create(storage->ui_day);
    lv_obj_set_width(storage->ui_day_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(storage->ui_day_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(storage->ui_day_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(storage->ui_day_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(storage->ui_day_icon, LV_OBJ_FLAG_SCROLLABLE);

    storage->ui_day_day = lv_label_create(storage->ui_day);
    lv_obj_set_width(storage->ui_day_day, LV_SIZE_CONTENT);
    lv_obj_set_height(storage->ui_day_day, LV_SIZE_CONTENT);
    lv_obj_set_align(storage->ui_day_day, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(storage->ui_day_day, lv_color_hex(0x5AA1EE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(storage->ui_day_day, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(storage->ui_day_day, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void weather_ui_show(lv_obj_t *root)
{
    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    ui_bg_img = lv_image_create(root_page);
    lv_image_set_src(ui_bg_img, "S:ui_img_weather_app_bg.bin");
    lv_obj_set_width(ui_bg_img, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_bg_img, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_bg_img, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_bg_img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(ui_bg_img, LV_OBJ_FLAG_SCROLLABLE);

    ui_loading_spinner = lv_spinner_create(root_page);
    lv_spinner_set_anim_params(ui_loading_spinner, 5000, 400);
    lv_obj_set_width(ui_loading_spinner, 60);
    lv_obj_set_height(ui_loading_spinner, 60);
    lv_obj_set_align(ui_loading_spinner, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_loading_spinner, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(ui_loading_spinner, zsw_color_dark_gray(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_loading_spinner, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_loading_spinner, zsw_color_blue(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_loading_spinner, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_root_container = lv_obj_create(root_page);
    lv_obj_remove_style_all(ui_root_container);
    lv_obj_set_width(ui_root_container, lv_pct(100));
    lv_obj_set_height(ui_root_container, lv_pct(100));
    lv_obj_set_align(ui_root_container, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_root_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_root_container, LV_OBJ_FLAG_HIDDEN);

    ui_status_label = lv_label_create(root_page);
    lv_obj_set_width(ui_status_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_label, 0);
    lv_obj_set_y(ui_status_label, 25);
    lv_obj_set_align(ui_status_label, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_status_label, "");
    lv_obj_set_style_text_font(ui_status_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_forecast_widget = lv_obj_create(ui_root_container);
    lv_obj_remove_style_all(ui_forecast_widget);
    lv_obj_set_width(ui_forecast_widget, lv_pct(100));
    lv_obj_set_height(ui_forecast_widget, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_forecast_widget, 3);
    lv_obj_set_y(ui_forecast_widget, 55);
    lv_obj_set_align(ui_forecast_widget, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_forecast_widget, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_forecast_widget, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_forecast_widget, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_row(ui_forecast_widget, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_forecast_widget, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    for (int i = 0; i < WEATHER_UI_NUM_FORECASTS; i++) {
        add_forecast_day(ui_forecast_widget, &ui_forecasts[i]);
    }

    ui_time = lv_label_create(root_page);
    lv_obj_set_width(ui_time, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_time, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_time, 0);
    lv_obj_set_y(ui_time, 10);
    lv_obj_set_align(ui_time, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_time, LV_OBJ_FLAG_HIDDEN);

    ui_today_container = lv_obj_create(root_page);
    lv_obj_remove_style_all(ui_today_container);
    lv_obj_set_pos(ui_today_container, 0, -10);
    lv_obj_set_height(ui_today_container, 89);
    lv_obj_set_width(ui_today_container, lv_pct(100));
    lv_obj_set_align(ui_today_container, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_today_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_today_container, LV_OBJ_FLAG_HIDDEN);

    ui_today_icon = lv_image_create(ui_today_container);
    lv_obj_set_width(ui_today_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_today_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_today_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_today_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(ui_today_icon, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *ui_Label8 = lv_label_create(ui_today_container);
    lv_obj_set_width(ui_Label8, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label8, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Label8, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Label8, "NOW");
    lv_obj_set_style_text_font(ui_Label8, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_today_temp = lv_label_create(ui_today_container);
    lv_obj_set_width(ui_today_temp, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_today_temp, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_today_temp, -40);
    lv_obj_set_y(ui_today_temp, -10);
    lv_obj_set_align(ui_today_temp, LV_ALIGN_CENTER);
    lv_obj_set_style_text_font(ui_today_temp, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_today_min_max_temp = lv_label_create(ui_today_container);
    lv_obj_set_width(ui_today_min_max_temp, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_today_min_max_temp, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_today_min_max_temp, 60);
    lv_obj_set_y(ui_today_min_max_temp, 0);
    lv_obj_set_align(ui_today_min_max_temp, LV_ALIGN_CENTER);
    lv_obj_set_style_text_opa(ui_today_min_max_temp, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_today_min_max_temp, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_today_rain = lv_label_create(ui_today_container);
    lv_obj_set_width(ui_today_rain, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_today_rain, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_today_rain, -40);
    lv_obj_set_y(ui_today_rain, 10);
    lv_obj_set_align(ui_today_rain, LV_ALIGN_CENTER);
    lv_obj_set_style_text_font(ui_today_rain, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_water_drop_img = lv_image_create(ui_today_container);
    lv_image_set_src(ui_water_drop_img, "S:ui_img_water_16_png.bin");
    lv_obj_set_width(ui_water_drop_img, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_water_drop_img, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_water_drop_img, -68);
    lv_obj_set_y(ui_water_drop_img, 11);
    lv_obj_set_align(ui_water_drop_img, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_water_drop_img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(ui_water_drop_img, LV_OBJ_FLAG_SCROLLABLE);
}

static void weather_ui_set_weather_data(weather_ui_current_weather_data_t current_weather,
                                         weather_ui_forecast_data_t forecasts[WEATHER_UI_NUM_FORECASTS],
                                         int num_forecasts)
{
    if (root_page == NULL || num_forecasts == 0) {
        return;
    }

    if (lv_obj_has_flag(ui_root_container, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_remove_flag(ui_root_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(ui_today_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(ui_time, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_loading_spinner, LV_OBJ_FLAG_HIDDEN);
    }

    lv_label_set_text_fmt(ui_today_temp, "%.1f°", current_weather.temperature);
    lv_label_set_text_fmt(ui_today_min_max_temp, "%.1f° / %.1f°", forecasts[0].low_temp,
                          forecasts[0].high_temp);
    lv_label_set_text_fmt(ui_today_rain, "%d%%", forecasts[0].rain_percent);
    lv_image_set_src(ui_today_icon, current_weather.icon);

    for (int i = 0; i < num_forecasts; i++) {
        lv_label_set_text_fmt(ui_forecasts[i].ui_day_temp, "%.1f°", forecasts[i].temperature);
        lv_label_set_text(ui_forecasts[i].ui_day_day, forecasts[i].day);
        lv_image_set_src(ui_forecasts[i].ui_day_icon, forecasts[i].icon);
    }
}

static void weather_ui_set_error(char *error)
{
    if (root_page == NULL) {
        return;
    }
    lv_obj_add_flag(ui_loading_spinner, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui_status_label, error);
}

static void weather_ui_set_time(int hour, int min, int second)
{
    lv_label_set_text_fmt(ui_time, "%02d:%02d", hour, min);
}

static void weather_ui_remove(void)
{
    lv_obj_delete(root_page);
    root_page = NULL;
}

/* ===========================================================================
 * Weather App Logic (from weather_app.c)
 * =========================================================================== */

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
            snprintf(forecasts[i].day, sizeof(forecasts[i].day), "%s", days[(time_now.tm.tm_wday + i) % 7]);
        }
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            weather_ui_set_weather_data(current_weather, forecasts, cJSON_GetArraySize(weather_code_list));
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
        printk("weather_ext: HTTP request failed\n");
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            weather_ui_set_error(status == BLE_HTTP_STATUS_TIMEOUT ? "Timeout" : "Failed");
        }
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
        printk("weather_ext: Failed to send HTTP request: %d\n", ret);
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            weather_ui_set_error("Failed fetching weather");
        }
    }
}

static void periodic_fetch_weather_data(struct k_work *work)
{
    int ret = ble_comm_request_gps_status(true);
    if (ret != 0) {
        printk("weather_ext: Failed to request GPS: %d\n", ret);
    }
    k_work_reschedule(&weather_app_fetch_work, K_SECONDS(WEATHER_BACKGROUND_FETCH_INTERVAL_S));
}

static void weather_data_timeout(struct k_work *work)
{
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        weather_ui_set_error("No data received\nMake sure phone is connected");
    }
}

LLEXT_IFLASH
static void on_zbus_ble_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event =
        (const struct ble_data_event *)chan->message;

    if (event->data.type == BLE_COMM_DATA_TYPE_GPS) {
        k_work_cancel_delayable(&weather_data_timeout_work);
        last_update_gps_time = k_uptime_get();
        last_lat = event->data.data.gps.lat;
        last_lon = event->data.data.gps.lon;
        fetch_weather_data(event->data.data.gps.lat, event->data.data.gps.lon);
        ble_comm_request_gps_status(false);
    }
}

/* ---- App lifecycle ---- */

static void weather_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    LV_UNUSED(group);
    weather_ui_show(root);
    if (last_update_gps_time == 0 || (k_uptime_get() - last_update_gps_time) > MAX_GPS_AGED_TIME_MS) {
        int res = ble_comm_request_gps_status(true);
        if (res != 0) {
            printk("weather_ext: Failed to request GPS data: %d\n", res);
            weather_ui_set_error("Failed to get GPS data");
        } else {
            k_work_reschedule(&weather_data_timeout_work, K_SECONDS(WEATHER_DATA_TIMEOUT_S));
        }
    } else {
        fetch_weather_data(last_lat, last_lon);
    }

    zsw_timeval_t time;
    zsw_clock_get_time(&time);
    weather_ui_set_time(time.tm.tm_hour, time.tm.tm_min, time.tm.tm_sec);
}

static void weather_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    k_work_cancel_delayable(&weather_data_timeout_work);
    weather_ui_remove();
    ble_comm_request_gps_status(false);
}

/* ---- Entry point ---- */
application_t *app_entry(void)
{
    printk("weather_ext: app_entry called\n");

    /* Initialize work items */
    k_work_init_delayable(&weather_app_fetch_work, periodic_fetch_weather_data);
    k_work_init(&weather_app_publish, publish_weather_data);
    k_work_init_delayable(&weather_data_timeout_work, weather_data_timeout);

    /* Register zbus observer for BLE data */
    int ret = zbus_chan_add_obs(&ble_comm_data_chan,
                                &weather_ext_listener, K_MSEC(100));
    if (ret != 0) {
        printk("weather_ext: failed to add zbus observer: %d\n", ret);
    }

    /* Start periodic weather fetch */
    k_work_reschedule(&weather_app_fetch_work, K_SECONDS(30));

    return &app;
}
EXPORT_SYMBOL(app_entry);
