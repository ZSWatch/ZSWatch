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

#include "zsw_onboarding_ui.h"
#include "ui/zsw_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zsw_onboarding_ui, LOG_LEVEL_INF);

ZSW_LV_IMG_DECLARE(ZSWatch_logo_small);

#define NUM_PAGES       4
#define INDICATOR_SIZE  8

static lv_obj_t *root_page;
static lv_obj_t *tileview;
static lv_obj_t *page_indicator;
static lv_obj_t *indicators[NUM_PAGES];
static zsw_onboarding_done_cb_t on_done_cb;

static void on_tileview_changed(lv_event_t *e);
static void on_done_btn_clicked(lv_event_t *e);

static void create_page_indicator(lv_obj_t *parent)
{
    page_indicator = lv_obj_create(parent);
    lv_obj_remove_style_all(page_indicator);
    lv_obj_set_size(page_indicator, NUM_PAGES * 16, INDICATOR_SIZE);
    lv_obj_align(page_indicator, LV_ALIGN_BOTTOM_MID, 0, -14);
    lv_obj_clear_flag(page_indicator, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(page_indicator, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(page_indicator, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(page_indicator, 8, 0);

    for (int i = 0; i < NUM_PAGES; i++) {
        indicators[i] = lv_obj_create(page_indicator);
        lv_obj_set_size(indicators[i], INDICATOR_SIZE, INDICATOR_SIZE);
        lv_obj_set_style_radius(indicators[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(indicators[i], 0, 0);
        lv_obj_clear_flag(indicators[i], LV_OBJ_FLAG_SCROLLABLE);

        if (i == 0) {
            lv_obj_set_style_bg_color(indicators[i], lv_color_white(), 0);
        } else {
            lv_obj_set_style_bg_color(indicators[i], zsw_color_gray(), 0);
        }
        lv_obj_set_style_bg_opa(indicators[i], LV_OPA_COVER, 0);
    }
}

static void update_page_indicator(int active_page)
{
    for (int i = 0; i < NUM_PAGES; i++) {
        if (i == active_page) {
            lv_obj_set_style_bg_color(indicators[i], lv_color_white(), 0);
        } else {
            lv_obj_set_style_bg_color(indicators[i], zsw_color_gray(), 0);
        }
    }
}

static lv_obj_t *add_label(lv_obj_t *parent, const char *text, const lv_font_t *font,
                           lv_color_t color, lv_align_t align, int x_ofs, int y_ofs)
{
    lv_obj_t *lbl = lv_label_create(parent);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_font(lbl, font, 0);
    lv_obj_set_style_text_color(lbl, color, 0);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(lbl, 200);
    lv_obj_align(lbl, align, x_ofs, y_ofs);
    return lbl;
}

static void create_welcome_page(lv_obj_t *tile)
{
    lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLLABLE);

    /* Try to show the ZSWatch logo icon, fall back to text */
    lv_obj_t *logo = lv_image_create(tile);
#ifdef CONFIG_ARCH_POSIX
    lv_image_set_src(logo, ZSW_LV_IMG_USE(ZSWatch_logo_small));
#else
    lv_image_set_src(logo, "S:ZSWatch_logo_small.bin");
#endif
    lv_obj_set_align(logo, LV_ALIGN_CENTER);
    lv_obj_set_y(logo, -45);

    add_label(tile, "Welcome to", &lv_font_montserrat_14,
              zsw_color_red(), LV_ALIGN_CENTER, 0, -75);

    add_label(tile, "ZSWatch", &lv_font_montserrat_28,
              lv_color_white(), LV_ALIGN_CENTER, 0, -5);

    add_label(tile, LV_SYMBOL_LEFT " Swipe to continue",
              &lv_font_montserrat_14, zsw_color_blue(), LV_ALIGN_CENTER, 0, 45);
}

static void create_button_indicator(lv_obj_t *parent, int x, int y,
                                    const char *symbol, const char *text,
                                    lv_color_t color, bool label_above)
{
    /* Circle indicator at position matching physical button */
    lv_obj_t *circle = lv_obj_create(parent);
    lv_obj_set_size(circle, 36, 36);
    lv_obj_set_pos(circle, x - 18, y - 18);
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(circle, zsw_color_dark_gray(), 0);
    lv_obj_set_style_bg_opa(circle, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(circle, 2, 0);
    lv_obj_set_style_border_color(circle, color, 0);
    lv_obj_clear_flag(circle, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *sym = lv_label_create(circle);
    lv_label_set_text(sym, symbol);
    lv_obj_set_style_text_color(sym, color, 0);
    lv_obj_set_style_text_font(sym, &lv_font_montserrat_14, 0);
    lv_obj_center(sym);

    /* Label above or below the circle */
    lv_obj_t *lbl = lv_label_create(parent);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_color(lbl, color, 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(lbl, 60);
    if (label_above) {
        lv_obj_set_pos(lbl, x - 30, y - 34);
    } else {
        lv_obj_set_pos(lbl, x - 30, y + 22);
    }
}

static void create_buttons_page(lv_obj_t *tile)
{
    lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLLABLE);

    create_button_indicator(tile, 52, 52, LV_SYMBOL_RIGHT, "Next",
                            zsw_color_blue(), false);
    /* Top right */
    create_button_indicator(tile, 188, 52, LV_SYMBOL_OK, "Select",
                            lv_color_white(), false);
    /* Bottom left */
    create_button_indicator(tile, 52, 188, LV_SYMBOL_LEFT, "Prev",
                            zsw_color_blue(), true);
    /* Bottom right */
    create_button_indicator(tile, 188, 188, LV_SYMBOL_CLOSE, "Back",
                            zsw_color_red(), true);

    /* Center title */
    add_label(tile, "Buttons", &lv_font_montserrat_16,
              zsw_color_red(), LV_ALIGN_CENTER, 0, 0);
}

static void create_gestures_page(lv_obj_t *tile)
{
    lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLLABLE);

    add_label(tile, "Gestures", &lv_font_montserrat_18,
              lv_color_white(), LV_ALIGN_TOP_MID, 0, 30);

    lv_obj_t *container = lv_obj_create(tile);
    lv_obj_remove_style_all(container);
    lv_obj_set_size(container, 200, 110);
    lv_obj_align(container, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(container, 5, 0);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    static const char *gesture_texts[] = {
        LV_SYMBOL_UP "  Swipe up: Apps",
        LV_SYMBOL_DOWN "  Swipe down: Quick panel",
        LV_SYMBOL_LEFT "  Swipe left: Notifications",
        LV_SYMBOL_RIGHT "  Swipe right: Watch faces",
    };

    for (int i = 0; i < 4; i++) {
        lv_obj_t *lbl = lv_label_create(container);
        lv_label_set_text(lbl, gesture_texts[i]);
        lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    }

    add_label(tile, "Tip: App picker is swipeable",
              &lv_font_montserrat_10, zsw_color_gray(), LV_ALIGN_BOTTOM_MID, 0, -30);
}

static void create_getstarted_page(lv_obj_t *tile)
{
    lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLLABLE);

    add_label(tile, "Get Started", &lv_font_montserrat_18,
              lv_color_white(), LV_ALIGN_TOP_MID, 0, 25);

    add_label(tile,
              "Pair with GadgetBridge\n(Android) or connect\nvia iOS for the\nbest experience",
              &lv_font_montserrat_14, lv_color_white(), LV_ALIGN_CENTER, 0, -15);

    add_label(tile,
              "Upload filesystem for\nfull UI experience",
              &lv_font_montserrat_10, zsw_color_red(), LV_ALIGN_CENTER, 0, 40);

    lv_obj_t *btn = lv_btn_create(tile);
    lv_obj_set_size(btn, 120, 36);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -28);
    lv_obj_set_style_bg_color(btn, zsw_color_blue(), 0);
    lv_obj_set_style_radius(btn, 18, 0);
    lv_obj_add_event_cb(btn, on_done_btn_clicked, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Let's go!");
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_16, 0);
    lv_obj_center(btn_label);
}

static void on_tileview_changed(lv_event_t *e)
{
    LV_UNUSED(e);
    lv_obj_t *active_tile = lv_tileview_get_tile_active(tileview);
    int col = lv_obj_get_x(active_tile) / lv_obj_get_width(active_tile);
    update_page_indicator(col);
}

static void on_done_btn_clicked(lv_event_t *e)
{
    LV_UNUSED(e);
    if (on_done_cb) {
        on_done_cb();
    }
}

void zsw_onboarding_ui_show(lv_obj_t *root, zsw_onboarding_done_cb_t done_cb)
{
    on_done_cb = done_cb;

    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(root_page, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_COVER, 0);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    tileview = lv_tileview_create(root_page);
    lv_obj_set_size(tileview, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(tileview, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *tile0 = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_RIGHT);
    lv_obj_t *tile1 = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_HOR);
    lv_obj_t *tile2 = lv_tileview_add_tile(tileview, 2, 0, LV_DIR_HOR);
    lv_obj_t *tile3 = lv_tileview_add_tile(tileview, 3, 0, LV_DIR_LEFT);

    create_welcome_page(tile0);
    create_buttons_page(tile1);
    create_gestures_page(tile2);
    create_getstarted_page(tile3);

    create_page_indicator(root_page);

    lv_obj_add_event_cb(tileview, on_tileview_changed, LV_EVENT_VALUE_CHANGED, NULL);

    LOG_INF("Onboarding screen shown");
}

void zsw_onboarding_ui_remove(void)
{
    if (root_page) {
        lv_obj_del(root_page);
        root_page = NULL;
    }
}
