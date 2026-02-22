#pragma once

#include <lvgl.h>

void calculator_ui_show(lv_obj_t *root);
void calculator_ui_remove(void);
void calculator_ui_update_display(const char *text);
void calculator_ui_init(void);
