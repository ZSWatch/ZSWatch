#pragma once

#include <inttypes.h>
#include <lvgl.h>

typedef void (*on_button_press_cb_t)(void);

void ppt_remote_ui_show(lv_obj_t *root, on_button_press_cb_t next_cb, on_button_press_cb_t prev_cb);

void ppt_remote_ui_remove(void);

void ppt_remote_ui_set_timer_counter_value(int value);
