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
 * LLEXT version of the About app.
 * This app is loaded dynamically at runtime from LittleFS.
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <zephyr/sys/printk.h>
#include <lvgl.h>
#include "managers/zsw_app_manager.h"

static void about_ext_start(lv_obj_t *root, lv_group_t *group);
static void about_ext_stop(void);

static lv_obj_t *root_page;

static application_t app = {
    .name = "About LLEXT",
    .start_func = about_ext_start,
    .stop_func = about_ext_stop,
    .category = ZSW_APP_CATEGORY_SYSTEM,
};

static void about_ext_start(lv_obj_t *root, lv_group_t *group)
{
    printk("about_ext: start\n");

    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Vertical layout */
    lv_obj_set_flex_flow(root_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root_page, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(root_page, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_top(root_page, 60, LV_PART_MAIN);

    /* Title */
    lv_obj_t *title = lv_label_create(root_page);
    lv_label_set_text(title, "ZSWatch");
    lv_obj_set_style_text_color(title, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    /* Subtitle */
    lv_obj_t *subtitle = lv_label_create(root_page);
    lv_label_set_text(subtitle, "LLEXT App");
    lv_obj_set_style_text_color(subtitle, lv_color_make(0x00, 0xBC, 0xD4), LV_PART_MAIN);
    lv_obj_set_style_text_align(subtitle, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    /* Description */
    lv_obj_t *desc = lv_label_create(root_page);
    lv_label_set_text(desc, "Dynamically loaded\nextension app!");
    lv_obj_set_style_text_align(desc, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    /* Info */
    lv_obj_t *info = lv_label_create(root_page);
    lv_label_set_text_fmt(info, "%d apps loaded", zsw_app_manager_get_num_apps());
    lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
}

static void about_ext_stop(void)
{
    printk("about_ext: stop\n");
    lv_obj_delete(root_page);
    root_page = NULL;
}

application_t *app_entry(void)
{
    printk("about_ext: app_entry called\n");
    return &app;
}
EXPORT_SYMBOL(app_entry);
