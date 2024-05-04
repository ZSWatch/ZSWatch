#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <lvgl.h>

#define BATTERY_APP_SAMPLE_INTERVAL_MIN         15

typedef void(*on_clear_history)(void);

void battery_ui_show(lv_obj_t *root, on_clear_history clear_hist_cb, int max_samples, bool include_pmic_ui);

void battery_ui_remove(void);

void battery_ui_add_measurement(int percent, int voltage);

void battery_ui_update(int ttf, int tte, int status, int error, int charging);