#pragma once

#include <inttypes.h>
#include <lvgl.h>

typedef void(*on_ui_close_cb_t)(void);

typedef void(*on_reference_set_cb_t)(void);

void sensors_summary_ui_show(lv_obj_t *root, on_ui_close_cb_t close_cb, on_reference_set_cb_t ref_set_cb);

void sensors_summary_ui_remove(void);

void sensors_summary_ui_set_pressure(double pressure);

void sensors_summary_ui_set_humidity(double humidity);

void sensors_summary_ui_set_temp(double temp);

void sensors_summary_ui_set_rel_height(double rel_height);

void sensors_summary_ui_set_gas(double gas);
