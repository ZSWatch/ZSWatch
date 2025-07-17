#include "trivia_ui.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <lvgl.h>

#define CLOSE_TXT "Close"

static lv_obj_t *root_page = NULL;
static lv_obj_t *question_lb;
static lv_obj_t *mbox;
static on_button_press_cb_t click_callback;
static lv_obj_t *more_btn;
static lv_obj_t *close_btn;

static void click_event_cb(lv_event_t *e);
static void click_popup_event_cb(lv_event_t *e);

void trivia_ui_show(lv_obj_t *root, on_button_press_cb_t on_button_click_cb)
{
    assert(root_page == NULL);
    click_callback = on_button_click_cb;

    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    // Don't want it to be scollable. Putting anything close the edges
    // then LVGL automatically makes the page scrollable and shows a scroll bar.
    // Does not loog very good on the round display.
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Create field for the question */
    question_lb = lv_label_create(root_page);
    lv_obj_set_width(question_lb, LV_PCT(100));
    lv_label_set_long_mode(question_lb, LV_LABEL_LONG_WRAP);            /*Break the long lines*/
    lv_obj_align(question_lb, LV_ALIGN_TOP_MID, 0, 35);                 /*Aling to the center of screen*/
    lv_obj_set_style_text_align(question_lb, LV_TEXT_ALIGN_CENTER, 0);  /*Allign text center*/
    lv_obj_set_style_text_font(question_lb, &lv_font_montserrat_16, 0); /*Change font size*/
    lv_label_set_text(question_lb, "-");

    /*Create Buttons*/
    lv_obj_t *btn_true = lv_btn_create(root_page);
    static trivia_button_t trivia_button_true = TRUE_BUTTON;
    lv_obj_add_event_cb(btn_true, click_event_cb, LV_EVENT_CLICKED, &trivia_button_true);
    lv_obj_align(btn_true, LV_ALIGN_CENTER, -45, 45);

    lv_obj_t *label_true = lv_label_create(btn_true);
    lv_label_set_text(label_true, "True");
    lv_obj_center(label_true);

    lv_obj_t *btn_false = lv_btn_create(root_page);
    static trivia_button_t trivia_button_false = FALSE_BUTTON;
    lv_obj_add_event_cb(btn_false, click_event_cb, LV_EVENT_CLICKED, &trivia_button_false);
    lv_obj_align(btn_false, LV_ALIGN_CENTER, 45, 45);

    lv_obj_t *label_false = lv_label_create(btn_false);
    lv_label_set_text(label_false, "False");
    lv_obj_center(label_false);
}

void trivia_ui_remove(void)
{
    trivia_ui_close_popup();

    lv_obj_del(root_page);
    root_page = NULL;
}

void trivia_ui_close_popup(void)
{
    if (mbox != NULL) {
        lv_msgbox_close(mbox);
    }
}

void trivia_ui_guess_feedback(bool correct)
{
    char msg[sizeof("Your answer is correct!")];

    sprintf(msg, "Your answer is %s!", correct ? "Correct" : "Wrong");

    mbox = lv_msgbox_create(NULL);
    lv_msgbox_add_text(mbox, msg);
    more_btn = lv_msgbox_add_footer_button(mbox, "More");
    close_btn = lv_msgbox_add_footer_button(mbox, CLOSE_TXT);
    lv_obj_add_event_cb(more_btn, click_popup_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(close_btn, click_popup_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_center(mbox);
}

void trivia_ui_not_supported()
{
    mbox = lv_msgbox_create(NULL);
    lv_msgbox_add_text(mbox, "Your phone does not support this app");
    close_btn = lv_msgbox_add_footer_button(mbox, CLOSE_TXT);
    lv_obj_add_event_cb(close_btn, click_popup_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_center(mbox);
}

static void click_popup_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target_obj(e);
    trivia_button_t trivia_button;

    if (obj == close_btn) {
        trivia_button = CLOSE_BUTTON;
    } else if (obj == more_btn) {
        trivia_button = PLAY_MORE_BUTTON;
    } else {
        return;
    }

    click_callback(trivia_button);
}

static void click_event_cb(lv_event_t *e)
{
    trivia_button_t trivia_button = *(trivia_button_t *)lv_event_get_user_data(e);
    click_callback(trivia_button);
}

void trivia_ui_update_question(uint8_t *buff)
{
    lv_label_set_text_fmt(question_lb, "%s", buff);
}
