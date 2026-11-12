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

#pragma once

#include <lvgl.h>

/*
 * Building blocks of the app screens. An app made of these looks like the others:
 * a glow in the app's color at the top, a ring along the edge of the round display
 * for its main value, and round buttons.
 */

/** Range of a ring, see zsw_ui_ring_set_value(). */
#define ZSW_UI_RING_MAX         1000
#define ZSW_UI_RING_WIDTH       8

/**
 * Creates the root object of an app screen: full size, not scrollable, black with
 * a soft glow in color at the top.
 */
lv_obj_t *zsw_ui_screen_create(lv_obj_t *parent, lv_color_t color);

/**
 * Creates a page for a screen with several pages, such as a tile of a tileview: full size,
 * not scrollable and transparent. Put the pages on a zsw_ui_screen_create() so that the
 * glow stays in place while the pages are swiped.
 */
lv_obj_t *zsw_ui_page_create(lv_obj_t *parent);

/** Creates the soft glow in color at the top of the parent. Drawn dithered, not as an image. */
lv_obj_t *zsw_ui_glow_create(lv_obj_t *parent, lv_color_t color);

/**
 * A label with text, font and color set. Text is centered when it wraps or the label is wider than it.
 */
lv_obj_t *zsw_ui_label_create(lv_obj_t *parent, const char *text, const lv_font_t *font, lv_color_t color);

/** Creates a small title at the top of the screen. The text is not copied. */
lv_obj_t *zsw_ui_title_create(lv_obj_t *parent, const char *text, lv_color_t color);

/** Creates a ring along the edge of the display. It does not take input. */
lv_obj_t *zsw_ui_ring_create(lv_obj_t *parent, lv_color_t color);

/** Sets the color of a ring, for a value that changes color with its level. */
void zsw_ui_ring_set_color(lv_obj_t *ring, lv_color_t color);

/**
 * Sets how much of the ring is filled, 0 to ZSW_UI_RING_MAX.
 * Only the part of the ring that changes is redrawn, also when animated.
 */
void zsw_ui_ring_set_value(lv_obj_t *ring, int32_t value, bool animate);

/** Creates a round button with a LV_SYMBOL_* in it. */
lv_obj_t *zsw_ui_round_button_create(lv_obj_t *parent, const char *symbol, int32_t size, lv_color_t color);

/** Changes the symbol of a button made by zsw_ui_round_button_create(). */
void zsw_ui_round_button_set_symbol(lv_obj_t *button, const char *symbol);

/**
 * Lets an object appear by sliding up a few pixels, after delay_ms. Use it on
 * small objects only, the object is redrawn on every frame of the animation.
 */
void zsw_ui_slide_in(lv_obj_t *obj, uint32_t delay_ms);

/**
 * Creates the row of dots at the bottom of a screen with several pages.
 * The first page is selected.
 */
lv_obj_t *zsw_ui_page_dots_create(lv_obj_t *parent, uint8_t num_pages);

/** Shows which page is the current one. */
void zsw_ui_page_dots_set_page(lv_obj_t *dots, uint8_t page);

/**
 * A column of key/value rows, centered on the parent. Add rows with zsw_ui_info_row_create().
 */
lv_obj_t *zsw_ui_info_list_create(lv_obj_t *parent, int32_t width);

/**
 * A row with a dim key to the left and a value to the right, which ends in dots when it is too long.
 * With pill the row is a rounded card, without it a tight line of small text for pages with many values.
 *
 * @return The value label. Its parent is the row.
 */
lv_obj_t *zsw_ui_info_row_create(lv_obj_t *list, const char *key, bool pill);
