#pragma once

#include <lvgl.h>

extern lv_obj_t *sensors_summary_screenHome;
extern lv_obj_t *sensors_summary_screenTemperature;
extern lv_obj_t *sensors_summary_screenHumidity;
extern lv_obj_t *sensors_summary_screenPressure;

void Sensors_Summary_UI_Init(lv_obj_t *root);
void Sensors_Summary_UI_ChangeScreen(void);

void SensorsSummary_HomeScreen_Show(lv_obj_t *root);
void SensorsSummary_TemperatureScreen_Show(lv_obj_t *root);
void SensorsSummary_PressureScreen_Show(lv_obj_t *root);
void SensorsSummary_HumidityScreen_Show(lv_obj_t *root);

void SensorsSummary_HomeScreen_Remove(void);