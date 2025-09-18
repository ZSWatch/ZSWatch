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

#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include "watchface_picker_ui.h"
#include "ui/utils/zsw_ui_utils.h"

static lv_obj_t *ui_faceSelect;
static on_watchface_selected_cb_t watchface_selected_cb;

void on_watchface_selected(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    int index = (int)lv_event_get_user_data(e);

    if (event_code == LV_EVENT_CLICKED) {
        watchface_selected_cb(index);
    }
}

void watchface_picker_ui_add_watchface(const lv_img_dsc_t *src, const char *name, int index)
{
    lv_obj_t *ui_faceItem = lv_obj_create(ui_faceSelect);
    lv_obj_set_width( ui_faceItem, 160);
    lv_obj_set_height( ui_faceItem, 180);
    lv_obj_set_align( ui_faceItem, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_faceItem, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_bg_opa(ui_faceItem, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_facePreview = lv_img_create(ui_faceItem);
    lv_img_set_src(ui_facePreview, src);
    lv_obj_set_width( ui_facePreview, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_facePreview, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_align( ui_facePreview, LV_ALIGN_TOP_MID );
    lv_obj_add_flag( ui_facePreview, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_facePreview, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

    lv_obj_t *ui_faceLabel = lv_label_create(ui_faceItem);
    lv_obj_set_width( ui_faceLabel, 160);
    lv_obj_set_height( ui_faceLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_align( ui_faceLabel, LV_ALIGN_BOTTOM_MID );
    lv_label_set_long_mode(ui_faceLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_faceLabel, name);
    lv_obj_set_style_text_align(ui_faceLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_faceLabel, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_faceItem, on_watchface_selected, LV_EVENT_ALL, (void *)index);

    lv_obj_t *ui_face_outline = lv_obj_create(ui_faceItem);
    lv_obj_set_width(ui_face_outline, 160);
    lv_obj_set_height(ui_face_outline, 160);
    lv_obj_clear_flag(ui_face_outline, LV_OBJ_FLAG_CLICKABLE);    /// Flags
    lv_obj_set_align( ui_faceItem, LV_ALIGN_CENTER );
    lv_obj_set_style_bg_opa(ui_face_outline, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_face_outline, LV_RADIUS_CIRCLE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_face_outline, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_face_outline, lv_color_hex(0xFFBAAF), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_outline_opa(ui_face_outline, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_face_outline, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_face_outline, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void watchface_picker_ui_show(lv_obj_t *root, on_watchface_selected_cb_t select_cb)
{
    watchface_selected_cb = select_cb;
    ui_faceSelect = lv_obj_create(root);
    lv_obj_set_width( ui_faceSelect, 240);
    lv_obj_set_height( ui_faceSelect, 240);
    lv_obj_set_align( ui_faceSelect, LV_ALIGN_CENTER );
    lv_obj_set_flex_flow(ui_faceSelect, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_faceSelect, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag( ui_faceSelect, LV_OBJ_FLAG_SNAPPABLE );    /// Flags
    lv_obj_set_scrollbar_mode(ui_faceSelect, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(ui_faceSelect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_faceSelect, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_faceSelect, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_faceSelect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_faceSelect, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_faceSelect, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_faceSelect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_faceSelect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_faceSelect, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_faceSelect, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void watchface_picker_ui_set_selected(int index)
{
    if (index < lv_obj_get_child_cnt(ui_faceSelect)) {
        lv_obj_scroll_to_view(lv_obj_get_child(ui_faceSelect, index), LV_ANIM_ON);
    }
}

void watchface_picker_ui_remove(void)
{
    lv_obj_del(ui_faceSelect);
}
