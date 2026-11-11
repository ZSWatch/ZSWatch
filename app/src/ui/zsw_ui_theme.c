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
#include <themes/lv_theme_private.h>

#include "ui/zsw_ui_theme.h"

/* Styles added on top of the LVGL default theme, shared by all objects of a type. */
static lv_style_t style_screen;
static lv_style_t style_card;
static lv_style_t style_button;
static lv_style_t style_track;
static lv_style_t style_indicator;
static lv_style_t style_knob;
static lv_style_t style_arc_track;
static lv_style_t style_arc_indicator;
static lv_style_t style_scrollbar;

static lv_theme_t theme;

static void init_styles(void)
{
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, ZSW_COLOR_BG);
    lv_style_set_bg_opa(&style_screen, LV_OPA_COVER);
    lv_style_set_text_color(&style_screen, ZSW_COLOR_TEXT);
    lv_style_set_text_font(&style_screen, ZSW_FONT_TEXT);

    lv_style_init(&style_card);
    lv_style_set_bg_color(&style_card, ZSW_COLOR_SURFACE);
    lv_style_set_border_width(&style_card, 0);
    lv_style_set_radius(&style_card, ZSW_RADIUS_CARD);

    /* No shadow: it is expensive to draw and does not show on a black background. */
    lv_style_init(&style_button);
    lv_style_set_radius(&style_button, ZSW_RADIUS_PILL);
    lv_style_set_shadow_width(&style_button, 0);
    lv_style_set_text_color(&style_button, ZSW_COLOR_TEXT);

    lv_style_init(&style_track);
    lv_style_set_bg_color(&style_track, ZSW_COLOR_SURFACE_HIGH);
    lv_style_set_bg_opa(&style_track, LV_OPA_COVER);

    lv_style_init(&style_indicator);
    lv_style_set_bg_color(&style_indicator, ZSW_COLOR_ACCENT);

    lv_style_init(&style_knob);
    lv_style_set_bg_color(&style_knob, ZSW_COLOR_TEXT);
    lv_style_set_shadow_width(&style_knob, 0);

    lv_style_init(&style_arc_track);
    lv_style_set_arc_color(&style_arc_track, ZSW_COLOR_SURFACE_HIGH);
    lv_style_set_arc_rounded(&style_arc_track, true);

    lv_style_init(&style_arc_indicator);
    lv_style_set_arc_color(&style_arc_indicator, ZSW_COLOR_ACCENT);
    lv_style_set_arc_rounded(&style_arc_indicator, true);

    lv_style_init(&style_scrollbar);
    lv_style_set_bg_color(&style_scrollbar, ZSW_COLOR_OUTLINE);
    lv_style_set_width(&style_scrollbar, 3);
    lv_style_set_pad_right(&style_scrollbar, 4);
}

static void theme_apply(lv_theme_t *th, lv_obj_t *obj)
{
    LV_UNUSED(th);

    if (lv_obj_get_parent(obj) == NULL) {
        lv_obj_add_style(obj, &style_screen, 0);
        return;
    }

    if (lv_obj_check_type(obj, &lv_obj_class)) {
        lv_obj_add_style(obj, &style_card, 0);
        lv_obj_add_style(obj, &style_scrollbar, LV_PART_SCROLLBAR);
    } else if (lv_obj_check_type(obj, &lv_button_class)) {
        lv_obj_add_style(obj, &style_button, 0);
    } else if (lv_obj_check_type(obj, &lv_slider_class)) {
        lv_obj_add_style(obj, &style_track, 0);
        lv_obj_add_style(obj, &style_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &style_knob, LV_PART_KNOB);
    } else if (lv_obj_check_type(obj, &lv_bar_class)) {
        lv_obj_add_style(obj, &style_track, 0);
        lv_obj_add_style(obj, &style_indicator, LV_PART_INDICATOR);
    } else if (lv_obj_check_type(obj, &lv_switch_class)) {
        lv_obj_add_style(obj, &style_track, 0);
        lv_obj_add_style(obj, &style_indicator, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &style_knob, LV_PART_KNOB);
    } else if (lv_obj_check_type(obj, &lv_arc_class)) {
        lv_obj_add_style(obj, &style_arc_track, 0);
        lv_obj_add_style(obj, &style_arc_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &style_knob, LV_PART_KNOB);
    }
}

void zsw_ui_theme_init(void)
{
    lv_display_t *display = lv_display_get_default();
    lv_theme_t *base = lv_theme_default_init(display, ZSW_COLOR_ACCENT, ZSW_COLOR_TEAL, true, ZSW_FONT_TEXT);

    init_styles();

    theme = *base;
    lv_theme_set_parent(&theme, base);
    lv_theme_set_apply_cb(&theme, theme_apply);
    lv_display_set_theme(display, &theme);
}
