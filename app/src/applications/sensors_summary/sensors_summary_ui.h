#pragma once

#include <inttypes.h>
#include <lvgl.h>

#include <zephyr/devicetree.h>

typedef void(*on_ui_close_cb_t)(void);

typedef void(*on_reference_set_cb_t)(void);

void sensors_summary_ui_show(lv_obj_t *root, on_ui_close_cb_t close_cb, on_reference_set_cb_t ref_set_cb);

void sensors_summary_ui_remove(void);

void sensors_summary_ui_set_pressure(float pressure);

void sensors_summary_ui_set_humidity(float humidity);

void sensors_summary_ui_set_temp(float temp);

void sensors_summary_ui_set_rel_height(float rel_height);

void sensors_summary_ui_set_light(float light);

void sensors_summary_ui_set_iaq(float iaq);

void sensors_summary_ui_set_co2(float co2);

void sensors_summary_ui_set_heart_rate(int heart_rate);
