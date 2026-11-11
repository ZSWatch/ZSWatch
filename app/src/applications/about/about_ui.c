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
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"

#define ABOUT_COLOR         ZSW_COLOR_ACCENT
#define INFO_LIST_WIDTH     188

static lv_obj_t *root_page = NULL;

ZSW_LV_IMG_DECLARE(zswatch_text);

static void add_row(lv_obj_t *list, const char *key, const char *value)
{
    lv_label_set_text(zsw_ui_info_row_create(list, key, false), value);
}

void about_ui_show(lv_obj_t *root, const about_ui_info_t *info)
{
    char num_apps[8];

    assert(root_page == NULL);

    root_page = zsw_ui_screen_create(root, ABOUT_COLOR);

    lv_obj_t *logo = lv_image_create(root_page);
    lv_image_set_src(logo, ZSW_LV_IMG_USE(zswatch_text));
    lv_obj_align(logo, LV_ALIGN_TOP_MID, 0, 28);

    // Too long to share a row with a key
    lv_obj_t *board = lv_label_create(root_page);
    lv_label_set_text(board, info->board);
    lv_obj_set_style_text_font(board, ZSW_FONT_CAPTION, LV_PART_MAIN);
    lv_obj_set_style_text_color(board, ZSW_COLOR_TEXT_DIM, LV_PART_MAIN);
    lv_obj_align(board, LV_ALIGN_TOP_MID, 0, 60);

    lv_obj_t *list = zsw_ui_info_list_create(root_page, INFO_LIST_WIDTH);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 84);
    lv_obj_set_style_pad_row(list, 3, LV_PART_MAIN);

    lv_snprintf(num_apps, sizeof(num_apps), "%d", info->num_apps);

    add_row(list, "Firmware", info->fw_version);
    add_row(list, "Built", info->build_time);
    add_row(list, "NCS", info->ncs_version);
    add_row(list, "Zephyr", info->zephyr_version);
    add_row(list, "Files", info->fs_stats);
    add_row(list, "Apps", num_apps);
}

void about_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}
