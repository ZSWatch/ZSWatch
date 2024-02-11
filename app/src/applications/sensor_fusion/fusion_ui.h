#pragma once

#include <inttypes.h>
#include <lvgl.h>

typedef void(*on_close_cb_t)(void);

void fusion_ui_show(lv_obj_t *root, on_close_cb_t close_cb);

void fusion_ui_remove(void);

void fusion_ui_set_values(int32_t x, int32_t y, int32_t z);