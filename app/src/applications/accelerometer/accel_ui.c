#include <accelerometer/accel_ui.h>
#include <lvgl.h>

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
    lv_bar_set_range(bar_acc_x, -20, 20);
    lv_obj_set_size(bar_acc_x, 200, 8);
    lv_obj_align(bar_acc_x, LV_ALIGN_CENTER, 0, -40);

    acc_x_label = lv_label_create(root_page);
    lv_label_set_text(acc_x_label, "0 g");
    lv_obj_align_to(acc_x_label, bar_acc_x, LV_ALIGN_CENTER, 0, -15);

    bar_acc_y = lv_bar_create(root_page);
    lv_style_set_bg_color(&style_indic_green, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
    lv_obj_add_style(bar_acc_y, &style_indic_green, LV_PART_INDICATOR);
    lv_bar_set_range(bar_acc_y, -20, 20);
    lv_obj_set_size(bar_acc_y, 200, 8);
    lv_obj_align(bar_acc_y, LV_ALIGN_CENTER, 0, 0);
    acc_y_label = lv_label_create(root_page);
    lv_label_set_text(acc_y_label, "0 g");
    lv_obj_align_to(acc_y_label, bar_acc_y, LV_ALIGN_CENTER, 0, -15);

    bar_acc_z = lv_bar_create(root_page);
    lv_style_set_bg_color(&style_indic_blue, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
    lv_obj_add_style(bar_acc_z, &style_indic_blue, LV_PART_INDICATOR);
    lv_bar_set_range(bar_acc_z, -20, 20);
    lv_obj_set_size(bar_acc_z, 200, 8);
    lv_obj_align(bar_acc_z, LV_ALIGN_CENTER, 0, 40);
    acc_z_label = lv_label_create(root_page);
    lv_label_set_text(acc_z_label, "0 g");
    lv_obj_align_to(acc_z_label, bar_acc_z, LV_ALIGN_CENTER, 0, -15);
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

    lv_label_set_text_fmt(acc_x_label, "X: %d m/s2", x);
    lv_label_set_text_fmt(acc_y_label, "Y: %d m/s2", y);
    lv_label_set_text_fmt(acc_z_label, "Z: %d m/s2", z);
}
