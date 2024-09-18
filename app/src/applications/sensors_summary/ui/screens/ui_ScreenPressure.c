#include <lvgl.h>

#include "../sensors_summary_ui.h"

static lv_obj_t *root_page = NULL;

static void on_ScreenPressure_Event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_KEY &&  lv_event_get_key(e) == LV_KEY_LEFT) {
        //_ui_screen_change(&ui_ScreenHome, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_ScreenHome_screen_init);
    }
}

void SensorsSummary_PressureScreen_Show(lv_obj_t *root)
{
    assert(root_page == NULL);

    root_page = lv_obj_create(root);

    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(root_page, on_ScreenPressure_Event, LV_EVENT_ALL, NULL);
}

void SensorsSummary_PressureScreen_Remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}