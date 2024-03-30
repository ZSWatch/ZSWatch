#include "ppt_remote_ui.h"

static void next_event_cb(lv_event_t *e);
static void prev_event_cb(lv_event_t *e);

static lv_obj_t *root_page = NULL;
static lv_obj_t *counter_label;
static on_button_press_cb_t next_callback;
static on_button_press_cb_t prev_callback;

void ppt_remote_ui_show(lv_obj_t *root, on_button_press_cb_t next_cb, on_button_press_cb_t prev_cb)
{
    assert(root_page == NULL);
    prev_callback = prev_cb;
    next_callback = next_cb;

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

    // Add the timer counter label
    counter_label = lv_label_create(root_page);
    lv_obj_align(counter_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_label_set_text(counter_label, "-");

    /*Properties to transition*/
    static lv_style_prop_t props[] = {
        LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_TEXT_LETTER_SPACE, 0
    };

    /*Transition descriptor when going back to the default state.
     *Add some delay to be sure the press transition is visible even if the press was very short*/
    static lv_style_transition_dsc_t transition_dsc_def;
    lv_style_transition_dsc_init(&transition_dsc_def, props, lv_anim_path_overshoot, 200, 0, NULL);

    /*Transition descriptor when going to pressed state.
     *No delay, go to presses state immediately*/
    static lv_style_transition_dsc_t transition_dsc_pr;
    lv_style_transition_dsc_init(&transition_dsc_pr, props, lv_anim_path_ease_in_out, 200, 0, NULL);

    /*Add only the new transition to he default state*/
    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_radius(&style_def, 80);
    lv_style_set_size(&style_def, 80);
    lv_style_set_transition(&style_def, &transition_dsc_def);

    /*Add the transition and some transformation to the presses state.*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_transform_width(&style_pr, 10);
    lv_style_set_transform_height(&style_pr, -10);
    lv_style_set_text_letter_space(&style_pr, 10);
    lv_style_set_transition(&style_pr, &transition_dsc_pr);

    /*Add a button the current screen*/
    lv_obj_t *next = lv_btn_create(root_page);
    lv_obj_align(next, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_style(next, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(next, &style_def, 0);

    /*Assign a callback to the button*/
    lv_obj_add_event_cb(next, next_event_cb, LV_EVENT_CLICKED, NULL);

    /*Add a label to the button*/
    lv_obj_t *label_next = lv_label_create(next);
    lv_label_set_text(label_next, "Next");

    lv_obj_center(label_next);

    /*Add a button the current screen*/
    lv_obj_t *prev = lv_btn_create(root_page);
    lv_obj_align(prev, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_style(prev, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(prev, &style_def, 0);

    /*Assign a callback to the button*/
    lv_obj_add_event_cb(prev, prev_event_cb, LV_EVENT_CLICKED, NULL);

    /*Add a label to the button*/
    lv_obj_t *label_prev = lv_label_create(prev);
    lv_label_set_text(label_prev, "Prev");

    lv_obj_center(label_prev);
}

void ppt_remote_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void ppt_remote_ui_set_timer_counter_value(int value)
{
    uint8_t min = value / 60;
    uint8_t sec = value % 60;
    lv_label_set_text_fmt(counter_label, "%02d:%02d", min, sec);
}

static void prev_event_cb(lv_event_t *e)
{
    prev_callback();
}

static void next_event_cb(lv_event_t *e)
{
    next_callback();
}
