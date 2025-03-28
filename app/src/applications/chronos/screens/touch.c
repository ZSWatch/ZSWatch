#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/logging/log.h>

#include "../chronos_ui.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(chronos_touch, CONFIG_ZSW_BLE_LOG_LEVEL);


static lv_obj_t *ui_touchpoint;
static lv_obj_t *ui_xpoint;
static lv_obj_t *ui_ypoint;

void chronos_ui_remote_touch_init(lv_obj_t *page)
{

    ui_touchpoint = lv_obj_create(page);
    lv_obj_set_width(ui_touchpoint, 20);
    lv_obj_set_height(ui_touchpoint, 20);
    lv_obj_set_x(ui_touchpoint, 110);
    lv_obj_set_y(ui_touchpoint, 100);
    lv_obj_clear_flag(ui_touchpoint, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_add_flag(ui_touchpoint, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_radius(ui_touchpoint, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_touchpoint, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_touchpoint, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_touchpoint, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_touchpoint, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_touchpoint, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_touchpoint, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_touchpoint, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_touchpoint, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_touchpoint, 1, LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_obj_t *ui_touchlabel = lv_label_create(page);
    lv_obj_set_width(ui_touchlabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_touchlabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_touchlabel, 0);
    lv_obj_set_y(ui_touchlabel, -35);
    lv_obj_set_align(ui_touchlabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_touchlabel, "Remote Touch");
    lv_obj_set_style_text_color(ui_touchlabel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_touchlabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_touchlabel, CHRONOS_FONT_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_touchinfo = lv_label_create(page);
    lv_obj_set_width(ui_touchinfo, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_touchinfo, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_touchinfo, 0);
    lv_obj_set_y(ui_touchinfo, 21);
    lv_obj_set_align(ui_touchinfo, LV_ALIGN_CENTER);
    lv_label_set_text(ui_touchinfo, "Try from the \nChronos app");
    lv_obj_set_style_text_color(ui_touchinfo, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_touchinfo, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_touchinfo, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_xpoint = lv_label_create(page);
    lv_obj_set_width(ui_xpoint, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_xpoint, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_xpoint, -130);
    lv_obj_set_y(ui_xpoint, -40);
    lv_obj_set_align(ui_xpoint, LV_ALIGN_BOTTOM_RIGHT);
    lv_label_set_text(ui_xpoint, "X: 240");
    lv_obj_add_flag(ui_xpoint, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_color(ui_xpoint, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_xpoint, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_xpoint, CHRONOS_FONT_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ypoint = lv_label_create(page);
    lv_obj_set_width(ui_ypoint, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_ypoint, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_ypoint, 130);
    lv_obj_set_y(ui_ypoint, -40);
    lv_obj_set_align(ui_ypoint, LV_ALIGN_BOTTOM_LEFT);
    lv_label_set_text(ui_ypoint, "Y: 240");
    lv_obj_add_flag(ui_ypoint, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_color(ui_ypoint, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ypoint, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_ypoint, CHRONOS_FONT_18, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void chronos_ui_set_remote_touch(chronos_remote_touch_t touch)
{

    lv_obj_set_x(ui_touchpoint, touch.x - 10);
    lv_obj_set_y(ui_touchpoint, touch.y - 10);

    lv_label_set_text_fmt(ui_xpoint, "X: %d", touch.x);
    lv_label_set_text_fmt(ui_ypoint, "Y: %d", touch.y);

    if (touch.state) {
        lv_obj_clear_flag(ui_touchpoint, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_xpoint, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_ypoint, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui_touchpoint, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_xpoint, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_ypoint, LV_OBJ_FLAG_HIDDEN);
    }

}