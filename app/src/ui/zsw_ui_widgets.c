/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

#include <lvgl.h>

#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"
#include "ui/utils/zsw_ui_utils.h"

#define SLIDE_IN_OFFSET_PX      12
#define RING_ANIM_TIME_MS       500
// Round buttons from this size get the larger symbol
#define ROUND_BUTTON_LARGE_SIZE  48

ZSW_LV_IMG_DECLARE(ui_glow_top);

lv_obj_t *zsw_ui_screen_create(lv_obj_t *parent, lv_color_t color)
{
    lv_obj_t *screen = lv_obj_create(parent);
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(screen, ZSW_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    // An alpha only image: one file gives a glow in any color.
    lv_obj_t *glow = lv_image_create(screen);
    lv_image_set_src(glow, ZSW_LV_IMG_USE(ui_glow_top));
    lv_obj_align(glow, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_image_recolor(glow, color, LV_PART_MAIN);
    lv_obj_set_style_image_recolor_opa(glow, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_remove_flag(glow, LV_OBJ_FLAG_CLICKABLE);

    return screen;
}

lv_obj_t *zsw_ui_label_create(lv_obj_t *parent, const char *text, const lv_font_t *font, lv_color_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    return label;
}

lv_obj_t *zsw_ui_title_create(lv_obj_t *parent, const char *text, lv_color_t color)
{
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text_static(title, text);
    lv_obj_set_style_text_font(title, ZSW_FONT_CAPTION, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, color, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(title, 2, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 22);

    return title;
}

lv_obj_t *zsw_ui_ring_create(lv_obj_t *parent, lv_color_t color)
{
    lv_obj_t *ring = lv_arc_create(parent);
    lv_obj_set_size(ring, LV_PCT(100), LV_PCT(100));
    lv_obj_center(ring);
    lv_arc_set_rotation(ring, 270);
    lv_arc_set_bg_angles(ring, 0, 360);
    lv_arc_set_range(ring, 0, ZSW_UI_RING_MAX);
    lv_arc_set_value(ring, 0);
    lv_obj_set_style_arc_width(ring, ZSW_UI_RING_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ring, ZSW_UI_RING_WIDTH, LV_PART_INDICATOR);
    lv_obj_remove_style(ring, NULL, LV_PART_KNOB);
    lv_obj_remove_flag(ring, LV_OBJ_FLAG_CLICKABLE);
    zsw_ui_ring_set_color(ring, color);

    return ring;
}

void zsw_ui_ring_set_color(lv_obj_t *ring, lv_color_t color)
{
    // The track is the same color, dimmed. A solid color: blending it would cost on every redraw.
    lv_obj_set_style_arc_color(ring, lv_color_mix(color, ZSW_COLOR_BG, LV_OPA_20), LV_PART_MAIN);
    lv_obj_set_style_arc_color(ring, color, LV_PART_INDICATOR);
}

static void ring_anim_exec(void *ring, int32_t value)
{
    lv_arc_set_value(ring, value);
}

void zsw_ui_ring_set_value(lv_obj_t *ring, int32_t value, bool animate)
{
    value = LV_CLAMP(0, value, ZSW_UI_RING_MAX);

    lv_anim_delete(ring, ring_anim_exec);
    if (!animate) {
        lv_arc_set_value(ring, value);
        return;
    }

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, ring);
    lv_anim_set_exec_cb(&anim, ring_anim_exec);
    lv_anim_set_values(&anim, lv_arc_get_value(ring), value);
    lv_anim_set_duration(&anim, RING_ANIM_TIME_MS);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_start(&anim);
}

lv_obj_t *zsw_ui_round_button_create(lv_obj_t *parent, const char *symbol, int32_t size, lv_color_t color)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, size, size);
    lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(button, color, LV_PART_MAIN);
    lv_obj_set_style_pad_all(button, 0, LV_PART_MAIN);

    lv_obj_t *label = zsw_ui_label_create(button, symbol,
                                          size >= ROUND_BUTTON_LARGE_SIZE ? ZSW_FONT_TITLE : ZSW_FONT_BODY, ZSW_COLOR_TEXT);
    lv_obj_center(label);

    return button;
}

void zsw_ui_round_button_set_symbol(lv_obj_t *button, const char *symbol)
{
    lv_label_set_text(lv_obj_get_child(button, 0), symbol);
}

static void slide_in_exec(void *obj, int32_t value)
{
    lv_obj_set_style_translate_y(obj, value, LV_PART_MAIN);
}

void zsw_ui_slide_in(lv_obj_t *obj, uint32_t delay_ms)
{
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, obj);
    lv_anim_set_exec_cb(&anim, slide_in_exec);
    lv_anim_set_values(&anim, SLIDE_IN_OFFSET_PX, 0);
    lv_anim_set_duration(&anim, ZSW_ANIM_TIME);
    lv_anim_set_delay(&anim, delay_ms);
    lv_anim_set_early_apply(&anim, true);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_start(&anim);
}

#define PAGE_DOT_SIZE           6
#define PAGE_DOT_SELECTED_WIDTH 14

lv_obj_t *zsw_ui_page_dots_create(lv_obj_t *parent, uint8_t num_pages)
{
    lv_obj_t *dots = lv_obj_create(parent);
    lv_obj_remove_style_all(dots);
    lv_obj_set_size(dots, LV_SIZE_CONTENT, PAGE_DOT_SIZE);
    lv_obj_align(dots, LV_ALIGN_BOTTOM_MID, 0, -14);
    lv_obj_set_flex_flow(dots, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(dots, 4, LV_PART_MAIN);
    lv_obj_remove_flag(dots, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    for (uint8_t i = 0; i < num_pages; i++) {
        lv_obj_t *dot = lv_obj_create(dots);
        lv_obj_remove_style_all(dot);
        lv_obj_set_size(dot, PAGE_DOT_SIZE, PAGE_DOT_SIZE);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_remove_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    }
    zsw_ui_page_dots_set_page(dots, 0);

    return dots;
}

void zsw_ui_page_dots_set_page(lv_obj_t *dots, uint8_t page)
{
    for (uint32_t i = 0; i < lv_obj_get_child_count(dots); i++) {
        lv_obj_t *dot = lv_obj_get_child(dots, i);
        bool selected = i == page;

        lv_obj_set_width(dot, selected ? PAGE_DOT_SELECTED_WIDTH : PAGE_DOT_SIZE);
        lv_obj_set_style_bg_color(dot, selected ? ZSW_COLOR_TEXT : ZSW_COLOR_OUTLINE, LV_PART_MAIN);
    }
}

lv_obj_t *zsw_ui_info_list_create(lv_obj_t *parent, int32_t width)
{
    lv_obj_t *list = lv_obj_create(parent);

    lv_obj_remove_style_all(list);
    lv_obj_set_size(list, width, LV_SIZE_CONTENT);
    lv_obj_align(list, LV_ALIGN_CENTER, 0, 2);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 5, LV_PART_MAIN);
    lv_obj_remove_flag(list, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    return list;
}

lv_obj_t *zsw_ui_info_row_create(lv_obj_t *list, const char *key, bool pill)
{
    const lv_font_t *font = pill ? ZSW_FONT_TEXT : ZSW_FONT_CAPTION;
    lv_obj_t *row = lv_obj_create(list);

    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, LV_PCT(100), pill ? 30 : LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row, 6, LV_PART_MAIN);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    if (pill) {
        lv_obj_set_style_radius(row, ZSW_RADIUS_PILL, LV_PART_MAIN);
        lv_obj_set_style_bg_color(row, ZSW_COLOR_SURFACE, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(row, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_pad_hor(row, 14, LV_PART_MAIN);
    }

    lv_obj_t *key_label = lv_label_create(row);
    lv_label_set_text(key_label, key);
    lv_obj_set_style_text_font(key_label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(key_label, ZSW_COLOR_TEXT_DIM, LV_PART_MAIN);

    lv_obj_t *value_label = lv_label_create(row);
    lv_label_set_text(value_label, "-");
    lv_label_set_long_mode(value_label, LV_LABEL_LONG_DOT);
    lv_obj_set_flex_grow(value_label, 1);
    lv_obj_set_height(value_label, lv_font_get_line_height(font));
    lv_obj_set_style_text_align(value_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
    lv_obj_set_style_text_font(value_label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(value_label, ZSW_COLOR_TEXT, LV_PART_MAIN);

    return value_label;
}
