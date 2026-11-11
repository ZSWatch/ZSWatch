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
 * The look of the watch UI in one place: colors, fonts, sizes and the LVGL theme
 * that applies them to the standard widgets. Apps should use these instead of
 * own hex colors and font sizes.
 */

/* Surfaces, from the screen background and up. */
#define ZSW_COLOR_BG            lv_color_hex(0x000000)
#define ZSW_COLOR_SURFACE       lv_color_hex(0x16181D)
#define ZSW_COLOR_SURFACE_HIGH  lv_color_hex(0x262A33)
#define ZSW_COLOR_OUTLINE       lv_color_hex(0x3A3F4B)

#define ZSW_COLOR_TEXT          lv_color_hex(0xFFFFFF)
#define ZSW_COLOR_TEXT_DIM      lv_color_hex(0xA0A6B4)
#define ZSW_COLOR_TEXT_DISABLED lv_color_hex(0x5A606C)

/* The color of the things the user can act on: buttons, sliders, switches. */
#define ZSW_COLOR_ACCENT        lv_color_hex(0x3B82F6)

#define ZSW_COLOR_GREEN         lv_color_hex(0x30D158)
#define ZSW_COLOR_YELLOW        lv_color_hex(0xFFD60A)
#define ZSW_COLOR_ORANGE        lv_color_hex(0xFF9500)
#define ZSW_COLOR_RED           lv_color_hex(0xFF453A)
#define ZSW_COLOR_PINK          lv_color_hex(0xFF2D6B)
#define ZSW_COLOR_PURPLE        lv_color_hex(0xA855F7)
#define ZSW_COLOR_TEAL          lv_color_hex(0x2DD4BF)
#define ZSW_COLOR_SKY           lv_color_hex(0x2AA7E8)

#define ZSW_COLOR_OK            ZSW_COLOR_GREEN
#define ZSW_COLOR_WARNING       ZSW_COLOR_ORANGE
#define ZSW_COLOR_ERROR         ZSW_COLOR_RED

/*
 * Fonts. ZSW_FONT_TEXT is the only one with Latin-1, Hebrew and emoji: use it
 * for all text that comes from the phone.
 */
LV_FONT_DECLARE(lv_font_montserrat_14_full);
LV_FONT_DECLARE(zsw_font_48);

#define ZSW_FONT_CAPTION        (&lv_font_montserrat_12)
#define ZSW_FONT_TEXT           (&lv_font_montserrat_14_full)
#define ZSW_FONT_BODY           (&lv_font_montserrat_16)
#define ZSW_FONT_TITLE          (&lv_font_montserrat_20)
#define ZSW_FONT_HEADLINE       (&lv_font_montserrat_28)
/* Digits, ':', '.', '%', '+', '-', '/' and '°' only, see scripts/generate_fonts.sh. */
#define ZSW_FONT_NUMBER         (&zsw_font_48)

#define ZSW_RADIUS_CARD         16
#define ZSW_RADIUS_PILL         LV_RADIUS_CIRCLE

/* Keep animations short and on small areas, the display is slow to redraw. */
#define ZSW_ANIM_TIME_FAST      120
#define ZSW_ANIM_TIME           200

/** Installs the theme on the default display. Call once, before any UI is created. */
void zsw_ui_theme_init(void);
