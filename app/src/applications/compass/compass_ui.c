/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <compass/compass_ui.h>
#include <lvgl.h>
#include "assert.h"

static lv_obj_t *root_page = NULL;

static lv_obj_t *compass_img;
static lv_obj_t *compass_label;

static on_start_calibraion_cb_t start_cal;

static void calibrate_button_event_cb(lv_event_t *e)
{
    if (start_cal) {
        start_cal();
    }
}

static void create_ui(lv_obj_t *compass_panel)
{
    lv_obj_t *cal_btn;
    lv_obj_t *cal_btn_label;

    LV_IMG_DECLARE(cardinal_point)

    cal_btn = lv_btn_create(compass_panel);
    lv_obj_set_style_pad_all(cal_btn, 3, LV_PART_MAIN);
    lv_obj_set_align(cal_btn, LV_ALIGN_CENTER);
    lv_obj_set_pos(cal_btn, 0, 80);
    lv_obj_set_size(cal_btn, 70, 25);
    lv_obj_set_style_bg_color(cal_btn, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN | LV_STATE_DEFAULT);
    cal_btn_label = lv_label_create(cal_btn);
    lv_label_set_text(cal_btn_label, "Calibrate");
    lv_obj_add_event_cb(cal_btn, calibrate_button_event_cb, LV_EVENT_CLICKED, NULL);

    compass_img = lv_img_create(compass_panel);
    lv_img_set_src(compass_img, &cardinal_point);
    lv_obj_set_width(compass_img, LV_SIZE_CONTENT);
    lv_obj_set_height(compass_img, LV_SIZE_CONTENT);
    lv_obj_set_align(compass_img, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(compass_img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(compass_img, LV_OBJ_FLAG_SCROLLABLE);
    lv_img_set_pivot(compass_img, cardinal_point.header.w / 2, cardinal_point.header.h - 10);

    compass_label = lv_label_create(compass_panel);
    lv_obj_set_width(compass_label, LV_SIZE_CONTENT);
    lv_obj_set_height(compass_label, LV_SIZE_CONTENT);
    lv_obj_set_align(compass_label, LV_ALIGN_TOP_MID);
    lv_label_set_text(compass_label, "360");
    lv_obj_set_style_text_opa(compass_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void compass_ui_show(lv_obj_t *root, on_start_calibraion_cb_t start_cal_cb)
{
    assert(root_page == NULL);

    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    start_cal = start_cal_cb;

    create_ui(root_page);
}

void compass_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void compass_ui_set_heading(double heading)
{
    lv_label_set_text_fmt(compass_label, "%.0f°", heading);
    lv_img_set_angle(compass_img, heading * 10);
}
