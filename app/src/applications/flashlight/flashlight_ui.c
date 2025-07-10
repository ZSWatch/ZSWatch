#include "flashlight_ui.h"
#include "ui/zsw_ui.h"
#include "assert.h"

static lv_obj_t *root_page = NULL;
static bool on;

static void on_click(lv_event_t *e)
{
    on = !on;
    if (on) {
        lv_obj_set_style_bg_color(root_page, lv_color_white(), LV_PART_MAIN);
    } else {
        lv_obj_set_style_bg_color(root_page, lv_color_black(), LV_PART_MAIN);
    }
}

void flashlight_ui_show(lv_obj_t *root)
{
    assert(root_page == NULL);

    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));

    lv_obj_set_width(root_page, lv_pct(100));
    lv_obj_set_height(root_page, lv_pct(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(root_page, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(root_page, on_click, LV_EVENT_CLICKED, NULL);

    on = true;
}

void flashlight_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}