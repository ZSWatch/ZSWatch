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

#include "settings_ui.h"
#include "lvgl.h"
#include "assert.h"
#include <stdio.h>
#include "ui/zsw_ui_theme.h"

#define ROW_ICON_SIZE   30

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#endif

static on_close_cb_t close_callback;

static lv_obj_t *_menu = NULL;
static lv_obj_t *_mainPage;
static lv_obj_t *active_sub_page;

static lv_style_t style_row;
static lv_style_t style_row_pressed;
static lv_style_t style_row_focused;

static void init_styles(void)
{
    static bool inited;

    if (inited) {
        return;
    }
    inited = true;

    lv_style_init(&style_row);
    lv_style_set_bg_color(&style_row, ZSW_COLOR_SURFACE);
    lv_style_set_bg_opa(&style_row, LV_OPA_COVER);
    lv_style_set_radius(&style_row, ZSW_RADIUS_CARD);
    lv_style_set_pad_hor(&style_row, 10);
    lv_style_set_pad_ver(&style_row, 7);
    lv_style_set_pad_column(&style_row, 8);
    lv_style_set_min_height(&style_row, 44);

    lv_style_init(&style_row_pressed);
    lv_style_set_bg_color(&style_row_pressed, ZSW_COLOR_SURFACE_HIGH);
    lv_style_set_bg_opa(&style_row_pressed, LV_OPA_COVER);

    // Shown on the row selected with the buttons
    lv_style_init(&style_row_focused);
    lv_style_set_border_color(&style_row_focused, ZSW_COLOR_ACCENT);
    lv_style_set_border_width(&style_row_focused, 2);
}

static void close_button_pressed(lv_event_t *e)
{
    if (lv_menu_back_button_is_root(_menu, lv_event_get_target(e))) {
        lv_obj_del(_menu);
        _menu = NULL;
        close_callback();
    }
}

enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
};
typedef uint8_t lv_menu_builder_variant_t;

static lv_obj_t *create_text(lv_obj_t *parent, const char *icon, const char *txt,
                             lv_menu_builder_variant_t builder_variant);
static lv_obj_t *create_slider(lv_obj_t *parent,
                               const char *icon, const char *txt, int32_t min, int32_t max, int32_t val);
static lv_obj_t *create_switch(lv_obj_t *parent,
                               const char *icon, const char *txt, bool chk);

static void slider_event_cb(lv_event_t *e);

static lv_obj_t *create_text(lv_obj_t *parent, const char *icon, const char *txt,
                             lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t *obj = lv_menu_cont_create(parent);
    lv_obj_add_style(obj, &style_row, 0);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_t *img = NULL;
    lv_obj_t *label = NULL;

    if (icon) {
        img = lv_image_create(obj);
        lv_image_set_src(img, icon);
        lv_obj_set_style_text_color(img, ZSW_COLOR_TEXT_DIM, 0);
    }

    if (txt) {
        // Wraps instead of scrolling: a scrolling label keeps redrawing for as long as it is shown.
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_obj_set_flex_grow(label, 1);
    }

    if (builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t *create_switch(lv_obj_t *parent, const char *icon, const char *txt, bool chk)
{
    lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_obj_t *sw = lv_switch_create(obj);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : 0);

    return sw;
}

static lv_obj_t *create_button(lv_obj_t *parent, const char *icon, const char *txt, const char *btn_txt)
{
    lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_obj_t *btn = lv_btn_create(obj);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, btn_txt);
    lv_obj_set_style_pad_hor(btn, 14, LV_PART_MAIN);
    lv_obj_set_style_pad_ver(btn, 7, LV_PART_MAIN);
    lv_obj_center(label);

    return btn;
}

static lv_obj_t *create_slider(lv_obj_t *parent, const char *icon, const char *txt, int32_t min, int32_t max,
                               int32_t val)
{
    lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);
    lv_obj_add_flag(slider, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    if (icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    return slider;
}

static void slider_event_cb(lv_event_t *e)
{
    lv_settings_changed_cb_t callback;
    lv_setting_value_t settings_value;
    lv_event_code_t code = lv_event_get_code(e);

    settings_value.item.slider = lv_slider_get_value(lv_event_get_target(e));
    settings_value.type = LV_SETTINGS_TYPE_SLIDER;
    callback = (lv_settings_changed_cb_t)lv_event_get_user_data(e);

    if ((callback != NULL) && ((code == LV_EVENT_VALUE_CHANGED) || (code == LV_EVENT_RELEASED))) {
        callback(settings_value, code == LV_EVENT_RELEASED);
    }
}

static void switch_event_cb(lv_event_t *e)
{
    lv_settings_changed_cb_t callback;
    lv_setting_value_t settings_value;
    lv_settings_item_t *item = lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    bool val = lv_obj_has_state(lv_event_get_target(e), LV_STATE_CHECKED);

    settings_value.item.sw = val;
    settings_value.type = LV_SETTINGS_TYPE_SWITCH;
    callback = item->change_callback;

    if ((callback != NULL) && ((code == LV_EVENT_VALUE_CHANGED) || (code == LV_EVENT_RELEASED))) {
        callback(settings_value, code == LV_EVENT_RELEASED);
        /* Callbacks update the backing value to the accepted state; reflect it for rollback. */
        if (*item->item.sw.inital_val) {
            lv_obj_add_state(lv_event_get_target(e), LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(lv_event_get_target(e), LV_STATE_CHECKED);
        }
    }
}

static void btn_event_cb(lv_event_t *e)
{
    lv_settings_changed_cb_t callback;
    lv_setting_value_t settings_value;
    lv_event_code_t code = lv_event_get_code(e);

    settings_value.type = LV_SETTINGS_TYPE_BTN;
    callback = (lv_settings_changed_cb_t)lv_event_get_user_data(e);

    if ((callback != NULL) && (code == LV_EVENT_CLICKED)) {
        callback(settings_value, true);
    }
}

static void populate_page(lv_obj_t *page, lv_settings_page_t *settings_page)
{
    for (int i = 0; i < settings_page->num_items; i++) {
        lv_settings_item_t *item = &settings_page->items[i];
        lv_obj_t *obj;

        switch (item->type) {
            case LV_SETTINGS_TYPE_LABEL:
                obj = create_text(page, item->icon, item->item.label.name, LV_MENU_ITEM_BUILDER_VARIANT_1);
                lv_obj_t *label = lv_obj_get_child(obj, -1);
                lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
                lv_obj_set_style_text_font(label, ZSW_FONT_CAPTION, 0);
                lv_obj_set_style_text_line_space(label, 5, 0);
                break;
            case LV_SETTINGS_TYPE_SWITCH:
                obj = create_switch(page, item->icon, item->item.sw.name, *item->item.sw.inital_val);
                lv_obj_add_event_cb(obj, switch_event_cb, LV_EVENT_ALL, item);
                break;
            case LV_SETTINGS_TYPE_SLIDER:
                obj = create_slider(page, item->icon, item->item.slider.name,
                                    item->item.slider.min_val, item->item.slider.max_val,
                                    *item->item.slider.inital_val);
                lv_obj_add_event_cb(obj, slider_event_cb, LV_EVENT_ALL, item->change_callback);
                break;
            case LV_SETTINGS_TYPE_BTN:
                obj = create_button(page, item->icon, item->item.btn.name, item->item.btn.text);
                lv_obj_add_event_cb(obj, btn_event_cb, LV_EVENT_CLICKED, item->change_callback);
                break;
            default:
                printf("Unsupported settings type %d\n", item->type);
        }
    }
}

static void style_page(lv_obj_t *page)
{
    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_row(page, 6, 0);
    // Room to scroll the last row up from the narrow bottom of the round display
    lv_obj_set_style_pad_bottom(page, 50, 0);
}

static void create_icon_disc(lv_obj_t *parent, const char *symbol, lv_color_t color)
{
    lv_obj_t *disc = lv_obj_create(parent);
    lv_obj_remove_style_all(disc);
    lv_obj_set_size(disc, ROW_ICON_SIZE, ROW_ICON_SIZE);
    lv_obj_set_style_radius(disc, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(disc, color, 0);
    lv_obj_set_style_bg_opa(disc, LV_OPA_COVER, 0);
    lv_obj_remove_flag(disc, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(disc);
    lv_label_set_text(label, symbol);
    lv_obj_set_style_text_font(label, ZSW_FONT_TEXT, 0);
    lv_obj_set_style_text_color(label, ZSW_COLOR_TEXT, 0);
    lv_obj_center(label);
}

static void menu_page_changed(lv_event_t *e)
{
    lv_obj_t *page = lv_menu_get_cur_main_page(_menu);
    if (page == active_sub_page) {
        return;
    }

    if (active_sub_page != NULL) {
        lv_obj_clean(active_sub_page);
        active_sub_page = NULL;
    }
    if (page != NULL && page != _mainPage) {
        populate_page(page, lv_obj_get_user_data(page));
        active_sub_page = page;
    }
}

void lv_settings_create(lv_obj_t *root, lv_settings_page_t *pages, uint8_t num_pages, const char *title,
                        lv_group_t *input_group,
                        on_close_cb_t close_cb)
{
    lv_obj_t *label;
    lv_obj_t *cont;
    lv_obj_t *sub_page = NULL;

    init_styles();

    assert(_menu == NULL);

    close_callback = close_cb;

    // Draw menu screen
    active_sub_page = NULL;
    _menu = lv_menu_create(root);
    lv_obj_add_event_cb(_menu, close_button_pressed, LV_EVENT_CLICKED, _menu);
    lv_obj_add_event_cb(_menu, menu_page_changed, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_size(_menu, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(_menu, 0, 0);
    lv_obj_set_style_pad_top(_menu, 14, LV_PART_MAIN);
    lv_obj_set_style_pad_left(_menu, 26, LV_PART_MAIN);
    lv_obj_set_style_pad_right(_menu, 26, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(_menu, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(_menu, ZSW_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(_menu, LV_OPA_COVER, LV_PART_MAIN);

    // Disable the back button
    lv_menu_set_mode_root_back_button(_menu, LV_MENU_ROOT_BACK_BUTTON_DISABLED);
    lv_obj_t *header = lv_menu_get_main_header(_menu);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_text_font(header, ZSW_FONT_TEXT, 0);
    lv_obj_set_style_text_color(header, ZSW_COLOR_TEXT_DIM, 0);
    lv_obj_set_style_pad_bottom(header, 6, 0);
    lv_obj_t *header_title = lv_obj_get_child(header, 1);
    lv_obj_set_width(header_title, 110);
    lv_obj_set_style_text_align(header_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_t *back_btn = lv_menu_get_main_header_back_button(_menu);
    lv_obj_add_flag(back_btn, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align(back_btn, LV_ALIGN_LEFT_MID, 22, -3);

    // Main page
    _mainPage = lv_menu_page_create(_menu, "Settings");
    style_page(_mainPage);

    for (int i = 0; i < num_pages; i++) {
        sub_page = lv_menu_page_create(_menu, pages[i].name);
        lv_obj_set_user_data(sub_page, &pages[i]);
        style_page(sub_page);
        // Create a main page item
        cont = lv_menu_cont_create(_mainPage);
        lv_obj_add_style(cont, &style_row, 0);
        lv_obj_add_style(cont, &style_row_pressed, LV_STATE_PRESSED);
        lv_obj_add_style(cont, &style_row_focused, LV_STATE_FOCUS_KEY);
        if (pages[i].icon) {
            create_icon_disc(cont, pages[i].icon, lv_color_hex(pages[i].color));
        }
        label = lv_label_create(cont);
        lv_obj_set_style_text_font(label, ZSW_FONT_BODY, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(label, pages[i].name);
        lv_obj_set_flex_grow(label, 1);
        label = lv_label_create(cont);
        lv_obj_set_style_text_font(label, ZSW_FONT_CAPTION, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(label, ZSW_COLOR_TEXT_DISABLED, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(label, LV_SYMBOL_RIGHT);
        lv_menu_set_load_page_event(_menu, cont, sub_page);
        lv_group_add_obj(input_group, cont);
        lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    }

    lv_menu_set_page(_menu, _mainPage);
    if (sub_page) {
        lv_group_focus_obj(sub_page);
    }
}

void settings_ui_remove(void)
{
    if (_menu != NULL) {
        lv_obj_del(_menu);
        _menu = NULL;
        active_sub_page = NULL;
    }
}

bool settings_ui_back(void)
{
    lv_menu_get_cur_main_page(_menu);
    if ((_menu != NULL) && lv_menu_get_cur_main_page(_menu) != _mainPage) {
        lv_obj_send_event(lv_menu_get_main_header_back_button(_menu), LV_EVENT_CLICKED, NULL);
        return true;
    } else {
        return false;
    }
}
