#pragma once

#include <inttypes.h>
#include <lvgl.h>

typedef void(*on_ui_increment_cb_t)(void);

void pmic_ui_show(lv_obj_t *root);

void pmic_ui_remove(void);

void pmic_ui_set_current_measurement(int value);

void pmic_ui_add_measurement(int value);
