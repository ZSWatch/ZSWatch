#include <sensor_fusion/fusion_ui.h>
#include <lvgl.h>

static lv_obj_t *root_page = NULL;

static lv_obj_t *bar_roll;
static lv_obj_t *bar_pitch;
static lv_obj_t *bar_yaw;

static lv_obj_t *roll_label;
static lv_obj_t *pitch_label;
static lv_obj_t *yaw_label;

static on_close_cb_t close_callback;

void fusion_ui_show(lv_obj_t *root, on_close_cb_t close_cb)
{
    static lv_style_t style_indic_red;
    static lv_style_t style_indic_green;
    static lv_style_t style_indic_blue;
    assert(root_page == NULL);

    close_callback = close_cb;

    root_page = lv_obj_create(root);
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(root_page, 240, 240);
    lv_obj_center(root_page);
    lv_style_init(&style_indic_red);
    lv_style_init(&style_indic_green);
    lv_style_init(&style_indic_blue);

    bar_roll = lv_bar_create(root_page);
    lv_style_set_bg_color(&style_indic_red, lv_palette_main(LV_PALETTE_RED));
    lv_obj_add_style(bar_roll, &style_indic_red, LV_PART_INDICATOR);
    lv_bar_set_range(bar_roll, -180, 180);
    lv_obj_set_size(bar_roll, 200, 8);
    lv_obj_align(bar_roll, LV_ALIGN_CENTER, 0, -40);

    roll_label = lv_label_create(root_page);
    lv_label_set_text(roll_label, "0 °");
    lv_obj_align_to(roll_label, bar_roll, LV_ALIGN_CENTER, 0, -15);

    bar_pitch = lv_bar_create(root_page);
    lv_style_set_bg_color(&style_indic_green, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
    lv_obj_add_style(bar_pitch, &style_indic_green, LV_PART_INDICATOR);
    lv_bar_set_range(bar_pitch, -180, 180);
    lv_obj_set_size(bar_pitch, 200, 8);
    lv_obj_align(bar_pitch, LV_ALIGN_CENTER, 0, 0);
    pitch_label = lv_label_create(root_page);
    lv_label_set_text(pitch_label, "0 °");
    lv_obj_align_to(pitch_label, bar_pitch, LV_ALIGN_CENTER, 0, -15);

    bar_yaw = lv_bar_create(root_page);
    lv_style_set_bg_color(&style_indic_blue, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
    lv_obj_add_style(bar_yaw, &style_indic_blue, LV_PART_INDICATOR);
    lv_bar_set_range(bar_yaw, -180, 180);
    lv_obj_set_size(bar_yaw, 200, 8);
    lv_obj_align(bar_yaw, LV_ALIGN_CENTER, 0, 40);
    yaw_label = lv_label_create(root_page);
    lv_label_set_text(yaw_label, "0 °");
    lv_obj_align_to(yaw_label, bar_yaw, LV_ALIGN_CENTER, 0, -15);
}

void fusion_ui_remove(void)
{
    if (!root_page) {
        return;
    }
    lv_obj_del(root_page);
    root_page = NULL;
}

void fusion_ui_set_values(int32_t x, int32_t y, int32_t z)
{
    if (!root_page) {
        return;
    }
    lv_bar_set_value(bar_roll, x, LV_ANIM_ON);
    lv_bar_set_value(bar_pitch, y, LV_ANIM_ON);
    lv_bar_set_value(bar_yaw, z, LV_ANIM_ON);

    lv_label_set_text_fmt(roll_label, "Roll: %d °", x);
    lv_label_set_text_fmt(pitch_label, "Pitch: %d °", y);
    lv_label_set_text_fmt(yaw_label, "Yaw: %d °", z);
}
