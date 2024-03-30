#include "../template_ui.h"

static void increment_button_pressed(lv_event_t *e);

static lv_obj_t *root_page = NULL;
static lv_obj_t *counter_label;
static lv_obj_t *btn_counter_label;
static on_ui_increment_cb_t close_callback;

void template_ui_show(lv_obj_t *root, on_ui_increment_cb_t close_cb)
{
    lv_obj_t *float_btn;

    assert(root_page == NULL);
    close_callback = close_cb;

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
    // Optional, keep default background from theme
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Add a button so we can easily close the application
    float_btn = lv_btn_create(root_page);
    lv_obj_set_size(float_btn, 120, 120);
    lv_obj_add_flag(float_btn, LV_OBJ_FLAG_FLOATING);
    lv_obj_center(float_btn);
    lv_obj_add_event_cb(float_btn, increment_button_pressed, LV_EVENT_PRESSED, root_page);
    lv_obj_set_style_radius(float_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_img_src(float_btn, LV_SYMBOL_PLUS, 0);
    lv_obj_set_style_text_font(float_btn, lv_theme_get_font_large(float_btn), 0);

    // Add the timer counter label
    counter_label = lv_label_create(root_page);
    lv_obj_align(counter_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_label_set_text(counter_label, "-");

    // Add the manual counter label
    btn_counter_label = lv_label_create(root_page);
    lv_obj_align(btn_counter_label, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_label_set_text(btn_counter_label, "0");
    lv_obj_set_style_text_color(btn_counter_label, lv_palette_main(LV_PALETTE_PURPLE), LV_PART_MAIN);

    lv_group_focus_obj(float_btn);
}

void template_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void template_ui_set_timer_counter_value(int value)
{
    lv_label_set_text_fmt(counter_label, "%d", value);
}

void template_ui_set_button_counter_value(int value)
{
    lv_label_set_text_fmt(btn_counter_label, "%d", value);
}

static void increment_button_pressed(lv_event_t *e)
{
    close_callback();
}