#include <lvgl.h>

#include "../sensors_summary_ui.h"

lv_obj_t *sensors_summary_screenTemperature = NULL;

void SensorsSummary_TemperatureScreen_Show(lv_obj_t *root)
{
    sensors_summary_screenTemperature = lv_obj_create(NULL);

    lv_obj_clear_flag(sensors_summary_screenTemperature, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(sensors_summary_screenTemperature, 0, LV_PART_MAIN);
    lv_obj_set_size(sensors_summary_screenTemperature, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(sensors_summary_screenTemperature, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void SensorsSummary_TemperatureScreen_Remove(void)
{
    lv_obj_del(sensors_summary_screenTemperature);
    sensors_summary_screenTemperature = NULL;
}