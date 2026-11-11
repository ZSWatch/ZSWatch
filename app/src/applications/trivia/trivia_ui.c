/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project, Leonardo Bispo.
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

#include "trivia_ui.h"
#include <assert.h>
#include <lvgl.h>
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"

#define TRIVIA_COLOR        lv_color_hex(0xC026D3)

#define ANSWER_BTN_WIDTH    84
#define ANSWER_BTN_HEIGHT   38
#define SHEET_BTN_SIZE      46

static lv_obj_t *root_page = NULL;
static lv_obj_t *question_lb;
static lv_obj_t *sheet;
static on_button_press_cb_t click_callback;

static void click_event_cb(lv_event_t *e)
{
    click_callback((trivia_button_t)(uintptr_t)lv_event_get_user_data(e));
}

static void create_answer_button(lv_obj_t *parent, const char *text, lv_color_t color, int32_t x,
                                 trivia_button_t button)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_size(btn, ANSWER_BTN_WIDTH, ANSWER_BTN_HEIGHT);
    lv_obj_set_style_bg_color(btn, color, LV_PART_MAIN);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, x, -42);
    lv_obj_add_event_cb(btn, click_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)button);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, ZSW_FONT_BODY, LV_PART_MAIN);
    lv_obj_center(label);
}

void trivia_ui_show(lv_obj_t *root, on_button_press_cb_t on_button_click_cb)
{
    assert(root_page == NULL);
    click_callback = on_button_click_cb;

    root_page = zsw_ui_screen_create(root, TRIVIA_COLOR);
    zsw_ui_title_create(root_page, "TRIVIA", ZSW_COLOR_TEXT_DIM);

    // The question comes from the phone, so it needs the font with all characters
    question_lb = lv_label_create(root_page);
    lv_obj_set_width(question_lb, 184);
    lv_label_set_long_mode(question_lb, LV_LABEL_LONG_WRAP);
    lv_obj_align(question_lb, LV_ALIGN_TOP_MID, 0, 48);
    lv_obj_set_style_text_align(question_lb, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(question_lb, ZSW_FONT_TEXT, LV_PART_MAIN);
    lv_obj_set_style_text_color(question_lb, ZSW_COLOR_TEXT, LV_PART_MAIN);
    lv_label_set_text(question_lb, "-");

    create_answer_button(root_page, "True", ZSW_COLOR_GREEN, -(ANSWER_BTN_WIDTH / 2 + 5), TRUE_BUTTON);
    create_answer_button(root_page, "False", ZSW_COLOR_RED, ANSWER_BTN_WIDTH / 2 + 5, FALSE_BUTTON);
}

void trivia_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
    sheet = NULL;
}

// Called from the buttons on the sheet, so the sheet is hidden here and deleted when the next one is shown.
void trivia_ui_close_popup(void)
{
    if (sheet != NULL) {
        lv_obj_add_flag(sheet, LV_OBJ_FLAG_HIDDEN);
    }
}

// A sheet over the question: a symbol, a line of text and Close, plus Next when there is more to play.
static void show_sheet(lv_color_t color, const char *symbol, const char *text, bool can_continue)
{
    if (sheet != NULL) {
        lv_obj_del(sheet);
    }

    sheet = zsw_ui_screen_create(root_page, color);

    lv_obj_t *symbol_label = lv_label_create(sheet);
    lv_label_set_text(symbol_label, symbol);
    lv_obj_set_style_text_font(symbol_label, ZSW_FONT_HEADLINE, LV_PART_MAIN);
    lv_obj_set_style_text_color(symbol_label, color, LV_PART_MAIN);
    lv_obj_align(symbol_label, LV_ALIGN_TOP_MID, 0, 44);
    zsw_ui_slide_in(symbol_label, 0);

    lv_obj_t *text_label = lv_label_create(sheet);
    lv_obj_set_width(text_label, 180);
    lv_label_set_text(text_label, text);
    lv_obj_set_style_text_align(text_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(text_label, ZSW_FONT_TITLE, LV_PART_MAIN);
    lv_obj_set_style_text_color(text_label, ZSW_COLOR_TEXT, LV_PART_MAIN);
    lv_obj_align(text_label, LV_ALIGN_CENTER, 0, -8);

    lv_obj_t *close_btn = zsw_ui_round_button_create(sheet, LV_SYMBOL_CLOSE, SHEET_BTN_SIZE, ZSW_COLOR_SURFACE_HIGH);
    lv_obj_add_event_cb(close_btn, click_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)CLOSE_BUTTON);

    if (can_continue) {
        lv_obj_t *more_btn = zsw_ui_round_button_create(sheet, LV_SYMBOL_RIGHT, SHEET_BTN_SIZE, TRIVIA_COLOR);
        lv_obj_add_event_cb(more_btn, click_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)PLAY_MORE_BUTTON);
        lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, -34, -32);
        lv_obj_align(more_btn, LV_ALIGN_BOTTOM_MID, 34, -32);
    } else {
        lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, -32);
    }
}

void trivia_ui_guess_feedback(bool correct)
{
    if (correct) {
        show_sheet(ZSW_COLOR_GREEN, LV_SYMBOL_OK, "Correct!", true);
    } else {
        show_sheet(ZSW_COLOR_RED, LV_SYMBOL_CLOSE, "Wrong!", true);
    }
}

void trivia_ui_not_supported()
{
    show_sheet(ZSW_COLOR_ORANGE, LV_SYMBOL_WARNING, "Your phone does not\nsupport this app", false);
}

void trivia_ui_update_question(const char *buff)
{
    lv_label_set_text(question_lb, buff);
}
