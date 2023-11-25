#pragma once

#include <inttypes.h>
#include <lvgl.h>

void speedometer_set_value(uint32_t speed);

void speedometer_ui_show(lv_obj_t *root);

void speedometer_ui_remove(void);
