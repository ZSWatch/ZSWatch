#pragma once

#include <inttypes.h>
#include <lvgl.h>

#define WEATHER_UI_NUM_FORECASTS 4

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
    char day[4]; // 3 Letter day name
    char *text;
    lv_color_t color;
} weather_ui_forecast_data_t;

void weather_ui_show(lv_obj_t *root);
void weather_ui_remove(void);

/*
* @brief Set the weather data to be displayed on the UI

* @param weather_ui_forecast_data_t The weather data to be displayed. First one is the current weather.
* @param num_forecasts The number of forecasts in the array
*
* @return void
*/
void weather_ui_set_weather_data(weather_ui_current_weather_data_t current_weather,
                                 weather_ui_forecast_data_t weather_ui_forecast_data_t[WEATHER_UI_NUM_FORECASTS],
                                 int num_forecasts);

void weather_ui_set_time(int hour, int min, int second);
