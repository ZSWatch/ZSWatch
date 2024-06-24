#pragma once

#include <inttypes.h>
#include <lvgl.h>

typedef enum trivia_button {
    TRUE_BUTTON = 0,
    FALSE_BUTTON,
    PLAY_MORE_BUTTON,
    CLOSE_BUTTON,
} trivia_button_t;

typedef void (*on_button_press_cb_t)(trivia_button_t trivia_button);

void trivia_ui_show(lv_obj_t *root, on_button_press_cb_t on_button_click_cb);

void trivia_ui_remove(void);

void trivia_ui_update_question(uint8_t *buff);

void trivia_ui_guess_feedback(bool correct);

void trivia_ui_close_popup(void);

void trivia_ui_not_supported(void);