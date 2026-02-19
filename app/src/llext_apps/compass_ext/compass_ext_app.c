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

/*
 * LLEXT version of the Compass app.
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <zephyr/sys/printk.h>
#include <lvgl.h>

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "ui/popup/zsw_popup_window.h"
#include "zsw_magnetometer.h"
#include "sensor_fusion/zsw_sensor_fusion.h"

/* Hardcoded Kconfig values */
#define COMPASS_REFRESH_INTERVAL_MS     50
#define COMPASS_CALIBRATION_TIME_S      30

/* ---- Icon images compiled into .rodata (XIP flash) ---- */
#include "move.c"
#include "cardinal_point.c"

/* ---- Forward declarations ---- */
static void compass_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void compass_app_stop(void *user_data);
static void timer_callback(lv_timer_t *timer);
static void on_start_calibration(void);

/* ---- App registration ---- */
static application_t app = {
    .name = "Compass",
    .icon = &move,
    .start_func = compass_app_start,
    .stop_func = compass_app_stop,
    .category = ZSW_APP_CATEGORY_ROOT,
};

static lv_timer_t *refresh_timer;
static bool is_calibrating;
static uint32_t cal_start_ms;

/* ---- Compass UI (inlined since it's small and uses local cardinal_point image) ---- */

static lv_obj_t *root_page;
static lv_obj_t *compass_img;
static lv_obj_t *compass_label;
static void (*start_cal_cb)(void);

static void calibrate_button_event_cb(lv_event_t *e)
{
    if (start_cal_cb) {
        start_cal_cb();
    }
}

static void compass_ui_show(lv_obj_t *root, void (*cal_cb)(void))
{
    lv_obj_t *cal_btn;
    lv_obj_t *cal_btn_label;

    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_remove_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    start_cal_cb = cal_cb;

    cal_btn = lv_button_create(root_page);
    lv_obj_set_style_pad_all(cal_btn, 3, LV_PART_MAIN);
    lv_obj_set_align(cal_btn, LV_ALIGN_CENTER);
    lv_obj_set_pos(cal_btn, 0, 80);
    lv_obj_set_size(cal_btn, 70, 25);
    lv_obj_set_style_bg_color(cal_btn, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN | LV_STATE_DEFAULT);
    cal_btn_label = lv_label_create(cal_btn);
    lv_label_set_text(cal_btn_label, "Calibrate");
    lv_obj_add_event_cb(cal_btn, calibrate_button_event_cb, LV_EVENT_CLICKED, NULL);

    compass_img = lv_image_create(root_page);
    lv_image_set_src(compass_img, &cardinal_point);
    lv_obj_set_width(compass_img, LV_SIZE_CONTENT);
    lv_obj_set_height(compass_img, LV_SIZE_CONTENT);
    lv_obj_set_align(compass_img, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(compass_img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(compass_img, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_pivot(compass_img, cardinal_point.header.w / 2, cardinal_point.header.h - 10);

    compass_label = lv_label_create(root_page);
    lv_obj_set_width(compass_label, LV_SIZE_CONTENT);
    lv_obj_set_height(compass_label, LV_SIZE_CONTENT);
    lv_obj_set_align(compass_label, LV_ALIGN_TOP_MID);
    lv_label_set_text(compass_label, "360");
    lv_obj_set_style_text_opa(compass_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void compass_ui_remove(void)
{
    lv_obj_delete(root_page);
    root_page = NULL;
}

static void compass_ui_set_heading(double heading)
{
    lv_label_set_text_fmt(compass_label, "%.0f°", heading);
    lv_image_set_rotation(compass_img, heading * 10);
}

/* ---- App lifecycle ---- */

static void compass_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    LV_UNUSED(group);
    compass_ui_show(root, on_start_calibration);
    refresh_timer = lv_timer_create(timer_callback, COMPASS_REFRESH_INTERVAL_MS, NULL);
    zsw_sensor_fusion_init();
}

static void compass_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    if (refresh_timer) {
        lv_timer_delete(refresh_timer);
        refresh_timer = NULL;
    }
    compass_ui_remove();
    zsw_magnetometer_stop_calibration();
    zsw_sensor_fusion_deinit();
    if (is_calibrating) {
        zsw_popup_remove();
        is_calibrating = false;
    }
}

static void on_start_calibration(void)
{
    zsw_magnetometer_start_calibration();
    is_calibrating = true;
    cal_start_ms = lv_tick_get();
    zsw_popup_show("Calibration",
                   "Rotate the watch 360 degrees\naround each x,y,z.\n a few times.", NULL,
                   COMPASS_CALIBRATION_TIME_S, false);
}

static void timer_callback(lv_timer_t *timer)
{
    float heading;
    if (is_calibrating &&
        (lv_tick_elaps(cal_start_ms) >= (COMPASS_CALIBRATION_TIME_S * 1000UL))) {
        zsw_magnetometer_stop_calibration();
        is_calibrating = false;
        zsw_popup_remove();
    }
    if (!is_calibrating) {
        zsw_sensor_fusion_get_heading(&heading);
        compass_ui_set_heading(heading);
    }
}

/* ---- Entry point ---- */
application_t *app_entry(void)
{
    printk("compass_ext: app_entry called\n");
    return &app;
}
EXPORT_SYMBOL(app_entry);
