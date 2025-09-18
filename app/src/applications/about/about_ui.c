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

#include "about_ui.h"
#include "ui/zsw_ui.h"
#include "assert.h"

static lv_obj_t *root_page = NULL;

static lv_obj_t *ui_fw_version;
static lv_obj_t *ui_compile_time;
static lv_obj_t *ui_filesystem_stats;
static lv_obj_t *ui_num_applications;
static lv_obj_t *ui_hw_revision;
static lv_obj_t *ui_sdk_versions;

ZSW_LV_IMG_DECLARE(ZSWatch_logo_small);
ZSW_LV_IMG_DECLARE(zswatch_text);

LV_FONT_DECLARE(lv_font_montserrat_10);
LV_FONT_DECLARE(lv_font_montserrat_12);

void about_ui_show(lv_obj_t *root, char *board, char *fw_version, char *fw_version_info, char *sdk_version,
                   char *fs_stats, int num_apps)
{
    assert(root_page == NULL);

    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_width(root_page, lv_pct(100));
    lv_obj_set_height(root_page, lv_pct(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Set up flex layout for vertical arrangement
    lv_obj_set_flex_flow(root_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root_page, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(root_page, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_top(root_page, 25, LV_PART_MAIN);

    lv_obj_t *ui_logo_img = lv_img_create(root_page);
#ifdef CONFIG_ARCH_POSIX
    lv_img_set_src(ui_logo_img, ZSW_LV_IMG_USE(zswatch_text));
#else
    lv_img_set_src(ui_logo_img, "S:zswatch_text.bin");
#endif
    lv_obj_set_width(ui_logo_img, LV_SIZE_CONTENT);   /// 180
    lv_obj_set_height(ui_logo_img, LV_SIZE_CONTENT);    /// 26
    lv_obj_set_style_pad_bottom(ui_logo_img, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_logo_img, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_logo_img, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // Create a container for hardware info with icon and text
    lv_obj_t *hw_container = lv_obj_create(root_page);
    lv_obj_remove_style_all(hw_container);
    lv_obj_set_size(hw_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(hw_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(hw_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(hw_container, 10, LV_PART_MAIN);

    lv_obj_t *icon_img = lv_img_create(hw_container);
#ifdef CONFIG_ARCH_POSIX
    lv_img_set_src(icon_img, ZSW_LV_IMG_USE(ZSWatch_logo_small));
#else
    lv_img_set_src(icon_img, "S:ZSWatch_logo_small.bin");
#endif
    lv_obj_set_width(icon_img, LV_SIZE_CONTENT);   /// 25
    lv_obj_set_height(icon_img, LV_SIZE_CONTENT);    /// 40
    lv_obj_add_flag(icon_img, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(icon_img, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_hw_revision = lv_label_create(hw_container);
    lv_obj_set_width(ui_hw_revision, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_hw_revision, LV_SIZE_CONTENT);
    lv_label_set_text_fmt(ui_hw_revision, "%s", board);
    lv_obj_set_style_text_align(ui_hw_revision, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_hw_revision, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_filesystem_stats = lv_label_create(root_page);
    lv_obj_set_width(ui_filesystem_stats, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_filesystem_stats, LV_SIZE_CONTENT);
    lv_label_set_text(ui_filesystem_stats, fs_stats);
    lv_obj_set_style_text_align(ui_filesystem_stats, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_filesystem_stats, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_num_applications = lv_label_create(root_page);
    lv_obj_set_width(ui_num_applications, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_num_applications, LV_SIZE_CONTENT);
    lv_label_set_text_fmt(ui_num_applications, "%d Applications", num_apps);
    lv_obj_set_style_text_align(ui_num_applications, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_num_applications, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_sdk_versions = lv_label_create(root_page);
    lv_obj_set_width(ui_sdk_versions, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_sdk_versions, LV_SIZE_CONTENT);
    lv_label_set_text(ui_sdk_versions, sdk_version);
    lv_obj_set_style_text_align(ui_sdk_versions, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_fw_version = lv_label_create(root_page);
    lv_obj_set_width(ui_fw_version, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_fw_version, LV_SIZE_CONTENT);
    lv_label_set_text(ui_fw_version, fw_version);
    lv_obj_set_style_text_align(ui_fw_version, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_compile_time = lv_label_create(root_page);
    lv_obj_set_width(ui_compile_time, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_compile_time, LV_SIZE_CONTENT);
    lv_label_set_text_fmt(ui_compile_time, "Compiled at\n%s", fw_version_info);
    lv_obj_set_style_text_align(ui_compile_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_compile_time, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void about_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}
