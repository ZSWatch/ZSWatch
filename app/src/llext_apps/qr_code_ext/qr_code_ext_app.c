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
 * LLEXT version of the QR Code app.
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <zephyr/sys/printk.h>
#include <lvgl.h>

#include "drivers/zsw_display_control.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

/* ---- Icon image compiled into .rodata (XIP flash) ---- */
#include "qr_code_icon.c"

/* ---- Forward declarations ---- */
static void qr_code_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void qr_code_app_stop(void *user_data);

/* ---- App registration ---- */
static application_t app = {
    .name = "QR",
    .icon = &qr_code_icon,
    .start_func = qr_code_app_start,
    .stop_func = qr_code_app_stop,
    .category = ZSW_APP_CATEGORY_RANDOM,
};

static lv_obj_t *root_page;
static uint8_t original_brightness;

static void qr_code_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    LV_UNUSED(group);

    original_brightness = zsw_display_control_get_brightness();
    zsw_display_control_set_brightness(100);

    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *img = lv_image_create(root_page);
    lv_image_set_src(img, "S:qr_code.bin");
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(img, 240, 240);
}

static void qr_code_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    zsw_display_control_set_brightness(original_brightness);
    lv_obj_delete(root_page);
    root_page = NULL;
}

/* ---- Entry point ---- */
application_t *app_entry(void)
{
    printk("qr_code_ext: app_entry called\n");
    return &app;
}
EXPORT_SYMBOL(app_entry);
