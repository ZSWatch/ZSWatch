#include <compass/compass_ui.h>
#include <lvgl.h>

static lv_obj_t *root_page = NULL;

static lv_obj_t *compass_panel;
static lv_obj_t *compass_img;
static lv_obj_t *compass_label;

static void create_ui(lv_obj_t *parent)
{
    LV_IMG_DECLARE(compass)
    compass_panel = lv_obj_create(parent);
    lv_obj_set_width(compass_panel, 240);
    lv_obj_set_height(compass_panel, 240);
    lv_obj_set_align(compass_panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(compass_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(compass_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(compass_panel, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(compass_panel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

    compass_img = lv_img_create(compass_panel);
    lv_img_set_src(compass_img, &compass);
    lv_obj_set_width(compass_img, LV_SIZE_CONTENT);
    lv_obj_set_height(compass_img, LV_SIZE_CONTENT);
    lv_obj_set_align(compass_img, LV_ALIGN_CENTER);
    lv_obj_add_flag(compass_img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(compass_img, LV_OBJ_FLAG_SCROLLABLE);

    compass_label = lv_label_create(compass_panel);
    lv_obj_set_width(compass_label, LV_SIZE_CONTENT);
    lv_obj_set_height(compass_label, LV_SIZE_CONTENT);
    lv_obj_set_align(compass_label, LV_ALIGN_CENTER);
    lv_label_set_text(compass_label, "360");
    lv_obj_set_style_text_color(compass_label, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(compass_label, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
}

void compass_ui_show(lv_obj_t *root)
{
    assert(root_page == NULL);

    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    create_ui(root_page);
}

void compass_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void compass_ui_set_heading(double heading)
{
    char buf[100] = {};
    snprintf(buf, sizeof(buf), "%.0f°", heading);
    lv_label_set_text(compass_label, buf);
    lv_img_set_angle(compass_img, heading * 10);
}
