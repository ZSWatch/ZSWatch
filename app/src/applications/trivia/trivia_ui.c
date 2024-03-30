#include "trivia_ui.h"

static lv_obj_t *root_page = NULL;
static lv_obj_t *question_lb;
static lv_obj_t *mbox;
static on_button_press_cb_t click_callback;

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
    static const char *btns[] = {"More", "Close", ""};

    sprintf(msg, "Your answer is %s!", correct ? "Correct" : "Wrong");

    mbox = lv_msgbox_create(NULL, NULL, msg, btns, false);
    lv_obj_add_event_cb(mbox, click_popup_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(mbox);
}

static void click_popup_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_current_target(e);
    trivia_button_t trivia_button;

    if (lv_msgbox_get_active_btn(obj) == 0) {
        trivia_button = PLAY_MORE_BUTTON;
    } else {
        trivia_button = CLOSE_BUTTON;
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
