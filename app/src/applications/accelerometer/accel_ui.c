#include <accelerometer/accel_ui.h>
#include <lvgl.h>

static void close_button_pressed(lv_event_t *e);

static lv_obj_t *root_page = NULL;

static lv_obj_t *bar_acc_x;
static lv_obj_t *bar_acc_y;
static lv_obj_t *bar_acc_z;

static lv_obj_t *acc_x_label;
static lv_obj_t *acc_y_label;
static lv_obj_t *acc_z_label;

static on_close_cb_t close_callback;

void accel_ui_show(lv_obj_t *root, on_close_cb_t close_cb)
{
    static lv_style_t style_indic_red;
    static lv_style_t style_indic_green;
    static lv_style_t style_indic_blue;
    assert(root_page == NULL);

    close_callback = close_cb;

    root_page = lv_obj_create(root);
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, 240, 240);
    lv_obj_center(root_page);
    lv_style_init(&style_indic_red);
    lv_style_init(&style_indic_green);
    lv_style_init(&style_indic_blue);

    bar_acc_x = lv_bar_create(root_page);
    lv_style_set_bg_color(&style_indic_red, lv_palette_main(LV_PALETTE_RED));
    lv_obj_add_style(bar_acc_x, &style_indic_red, LV_PART_INDICATOR);
    lv_bar_set_range(bar_acc_x, -20000, 20000);
    lv_obj_set_size(bar_acc_x, 200, 8);
    lv_obj_align(bar_acc_x, LV_ALIGN_CENTER, 0, -40);

    acc_x_label = lv_label_create(root_page);
    lv_label_set_text(acc_x_label, "00000");
    lv_obj_align_to(acc_x_label, bar_acc_x, LV_ALIGN_CENTER, 0, -15);

    bar_acc_y = lv_bar_create(root_page);
    lv_style_set_bg_color(&style_indic_green, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
    lv_obj_add_style(bar_acc_y, &style_indic_green, LV_PART_INDICATOR);
    lv_bar_set_range(bar_acc_y, -20000, 20000);
    lv_obj_set_size(bar_acc_y, 200, 8);
    lv_obj_align(bar_acc_y, LV_ALIGN_CENTER, 0, 0);
    acc_y_label = lv_label_create(root_page);
    lv_label_set_text(acc_y_label, "00000");
    lv_obj_align_to(acc_y_label, bar_acc_y, LV_ALIGN_CENTER, 0, -15);

    bar_acc_z = lv_bar_create(root_page);
    lv_style_set_bg_color(&style_indic_blue, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
    lv_obj_add_style(bar_acc_z, &style_indic_blue, LV_PART_INDICATOR);
    lv_bar_set_range(bar_acc_z, -20000, 20000);
    lv_obj_set_size(bar_acc_z, 200, 8);
    lv_obj_align(bar_acc_z, LV_ALIGN_CENTER, 0, 40);
    acc_z_label = lv_label_create(root_page);
    lv_label_set_text(acc_z_label, "00000");
    lv_obj_align_to(acc_z_label, bar_acc_z, LV_ALIGN_CENTER, 0, -15);

    // TODO add a close button which have focus by default so we can exit back to app manager.

    lv_obj_t *float_btn = lv_btn_create(root_page);
    lv_obj_set_size(float_btn, 50, 50);
    lv_obj_add_flag(float_btn, LV_OBJ_FLAG_FLOATING);
    lv_obj_align(float_btn, LV_ALIGN_BOTTOM_RIGHT, 0, -lv_obj_get_style_pad_right(root_page, LV_PART_MAIN));
    lv_obj_add_event_cb(float_btn, close_button_pressed, LV_EVENT_PRESSED, root_page);
    lv_obj_set_style_radius(float_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_img_src(float_btn, LV_SYMBOL_CLOSE, 0);
    lv_obj_set_style_text_font(float_btn, lv_theme_get_font_large(float_btn), 0);

    lv_group_focus_obj(float_btn);
}

void accel_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void accel_ui_set_values(int32_t x, int32_t y, int32_t z)
{
    if (!root_page) {
        return;
    }
    lv_bar_set_value(bar_acc_x, x, LV_ANIM_ON);
    lv_bar_set_value(bar_acc_y, y, LV_ANIM_ON);
    lv_bar_set_value(bar_acc_z, z, LV_ANIM_ON);

    lv_label_set_text_fmt(acc_x_label, "X: %d", x);
    lv_label_set_text_fmt(acc_y_label, "Y: %d", y);
    lv_label_set_text_fmt(acc_z_label, "Z: %d", z);
}

static void close_button_pressed(lv_event_t *e)
{
    close_callback();
}
