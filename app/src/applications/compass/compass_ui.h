#pragma once

#include <inttypes.h>
#include <lvgl.h>

void compass_ui_show(lv_obj_t *root);

void compass_ui_remove(void);

void compass_ui_set_heading(double heading);
