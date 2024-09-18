#include <lvgl.h>

#include "../sensors_summary_ui.h"

LV_IMG_DECLARE(ui_img_925774327);
LV_IMG_DECLARE(ui_img_1463213690);
LV_IMG_DECLARE(ui_img_1479496048);

static lv_obj_t *ui_ButtonTemperature;
static lv_obj_t *ui_ButtonPressure;
static lv_obj_t *ui_ButtonHumidity;

lv_obj_t *sensors_summary_screenHome = NULL;

static void on_ButtonTemperature_Clicked(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED) {
        //SensorsSummary_HomeScreen_Remove();
        //SensorsSummary_TemperatureScreen_Show(e->user_data);
        lv_scr_load_anim(sensors_summary_screenTemperature, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
    }
}

static void on_ButtonPressure_Clicked(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED) {
        //_ui_screen_change(&ui_ScreenPressure, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_ScreenPressure_screen_init);
    }
}

static void on_ButtonHumidity_Clicked(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED) {
        //_ui_screen_change(&ui_ScreenHumidity, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_ScreenHumidity_screen_init);
    }
}

void SensorsSummary_HomeScreen_Show(lv_obj_t *root)
{
    //assert(sensors_summary_screenHome == NULL);

    sensors_summary_screenHome = lv_obj_create(NULL);

    lv_obj_clear_flag(sensors_summary_screenHome, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(sensors_summary_screenHome, 0, LV_PART_MAIN);
    lv_obj_set_size(sensors_summary_screenHome, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(sensors_summary_screenHome, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ButtonTemperature = lv_btn_create(sensors_summary_screenHome);
    lv_obj_set_width(ui_ButtonTemperature, 50);
    lv_obj_set_height(ui_ButtonTemperature, 50);
    lv_obj_set_x(ui_ButtonTemperature, -41);
    lv_obj_set_y(ui_ButtonTemperature, -44);
    lv_obj_set_align(ui_ButtonTemperature, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_ButtonTemperature, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_ButtonTemperature, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_img_src(ui_ButtonTemperature, &ui_img_925774327, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ButtonPressure = lv_btn_create(sensors_summary_screenHome);
    lv_obj_set_width(ui_ButtonPressure, 50);
    lv_obj_set_height(ui_ButtonPressure, 50);
    lv_obj_set_x(ui_ButtonPressure, 49);
    lv_obj_set_y(ui_ButtonPressure, -44);
    lv_obj_set_align(ui_ButtonPressure, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_ButtonPressure, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_ButtonPressure, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_img_src(ui_ButtonPressure, &ui_img_1463213690, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ButtonHumidity = lv_btn_create(sensors_summary_screenHome);
    lv_obj_set_width(ui_ButtonHumidity, 50);
    lv_obj_set_height(ui_ButtonHumidity, 50);
    lv_obj_set_x(ui_ButtonHumidity, -42);
    lv_obj_set_y(ui_ButtonHumidity, 35);
    lv_obj_set_align(ui_ButtonHumidity, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_ButtonHumidity, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_ButtonHumidity, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_img_src(ui_ButtonHumidity, &ui_img_1479496048, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_ButtonTemperature, on_ButtonTemperature_Clicked, LV_EVENT_ALL, root);
    lv_obj_add_event_cb(ui_ButtonPressure, on_ButtonPressure_Clicked, LV_EVENT_ALL, root);
    lv_obj_add_event_cb(ui_ButtonHumidity, on_ButtonHumidity_Clicked, LV_EVENT_ALL, root);
}

void SensorsSummary_HomeScreen_Remove(void)
{
    lv_obj_del(sensors_summary_screenHome);
    sensors_summary_screenHome = NULL;
}