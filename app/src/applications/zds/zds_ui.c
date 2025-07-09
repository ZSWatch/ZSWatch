#include <zds/zds_ui.h>
#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>
#include <assert.h>

static void on_button_pressed(lv_event_t *e);
static void set_img_rotation(void *obj, int32_t v);

static lv_obj_t *root_page = NULL;
static lv_obj_t *img;
static bool is_rotating = false;
static lv_anim_t rotate_anim;

ZSW_LV_IMG_DECLARE(zephyr_project);

void zds_ui_show(lv_obj_t *root)
{
    lv_obj_t *float_btn;

    assert(root_page == NULL);

    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    // Don't want it to be scollable. Putting anything close the edges
    // then LVGL automatically makes the page scrollable and shows a scroll bar.
    // Does not loog very good on the round display.
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    img = lv_img_create(root_page);
    lv_img_set_src(img, ZSW_LV_IMG_USE(zephyr_project));
    lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    // Add a button so we can easily close the application
    float_btn = lv_btn_create(root_page);
    lv_obj_set_size(float_btn, 50, 50);
    lv_obj_add_flag(float_btn, LV_OBJ_FLAG_FLOATING);
    lv_obj_align(float_btn, LV_ALIGN_BOTTOM_RIGHT, -20, -10);
    lv_obj_add_event_cb(float_btn, on_button_pressed, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(img, on_button_pressed, LV_EVENT_PRESSED, NULL);
    lv_obj_set_style_radius(float_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_img_src(float_btn, LV_SYMBOL_REFRESH, 0);
    lv_obj_set_style_text_font(float_btn, lv_theme_get_font_large(float_btn), 0);

    lv_group_focus_obj(float_btn);
}

void zds_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void zds_ui_reset_rotation(void)
{
    on_button_pressed(NULL);
}

static void on_button_pressed(lv_event_t *e)
{
    is_rotating = !is_rotating;

    if (is_rotating) {
        lv_anim_init(&rotate_anim);
        lv_anim_set_var(&rotate_anim, img);
        lv_anim_set_exec_cb(&rotate_anim, set_img_rotation);
        lv_anim_set_time(&rotate_anim, 2000);
        lv_anim_set_repeat_count(&rotate_anim, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_repeat_delay(&rotate_anim, 0);
        lv_anim_set_values(&rotate_anim, 0, 3600);
        lv_anim_start(&rotate_anim);
    } else {
        lv_anim_del(img, set_img_rotation);
    }
}

static void set_img_rotation(void *obj, int32_t v)
{
    lv_img_set_angle(obj, v);
}
