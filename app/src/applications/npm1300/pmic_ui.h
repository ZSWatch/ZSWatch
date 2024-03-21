#pragma once

#include <inttypes.h>
#include <lvgl.h>

typedef void(*on_clear_history)(void);

void pmic_ui_show(lv_obj_t *root, on_clear_history clear_hist_cb, int max_samples);

void pmic_ui_remove(void);

void pmic_ui_set_current_measurement(int value);

void pmic_ui_add_measurement(int percent, int voltage);

void pmic_ui_update(int ttf, int tte, int status, int error, int charging);