#pragma once

#include <inttypes.h>
#include <lvgl.h>

void fitness_ui_show(lv_obj_t *root, uint16_t max_samples);

void fitness_ui_set_weekly_steps(uint16_t *samples, char **weekday_names, uint16_t num_samples);

void fitness_ui_remove(void);
