#pragma once

#include "lvgl.h"

typedef void(*on_ui_increment_cb_t)(void);

void template_ui_show(lv_obj_t *root, on_ui_increment_cb_t close_cb);

void template_ui_remove(void);

void template_ui_set_timer_counter_value(int value);

void template_ui_set_button_counter_value(int value);