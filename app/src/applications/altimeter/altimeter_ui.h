#pragma once

#include <lvgl.h>
#include <stdbool.h>

void altimeter_ui_show(lv_obj_t *root);
void altimeter_ui_remove(void);
void altimeter_ui_update(float altitude, float raw_pressure, bool is_calibrated, float *history_data, uint8_t start_idx, bool has_data);
