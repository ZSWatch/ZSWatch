#include "sensors_summary_ui.h"

void Sensors_Summary_UI_Init(lv_obj_t *root)
{
    SensorsSummary_HomeScreen_Show(root);
    SensorsSummary_TemperatureScreen_Show(root);
    SensorsSummary_HumidityScreen_Show(root);
    SensorsSummary_PressureScreen_Show(root);
    lv_disp_load_scr(sensors_summary_screenHome);
}

void Sensors_Summary_UI_ChangeScreen(void)
{
    SensorsSummary_HomeScreen_Show(NULL);
    lv_scr_load_anim(sensors_summary_screenHome, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
}