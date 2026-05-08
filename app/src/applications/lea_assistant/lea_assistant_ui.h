#pragma once

#include <inttypes.h>
#include <lvgl.h>
#include "lea_assistant_app.h"

typedef void (*on_button_press_cb_t)(lea_assistant_device_t *device);
typedef void(*on_close_cb_t)(void);

void lea_assistant_ui_show(lv_obj_t *root, on_button_press_cb_t on_button_click_cb, on_close_cb_t close_cb);

void lea_assistant_ui_show_source(lv_obj_t *root, on_button_press_cb_t on_button_click_cb);

void lea_assistant_ui_add_sink_list_entry(lea_assistant_device_t *device);

void lea_assistant_ui_add_source_list_entry(lea_assistant_device_t *device);

void lea_assistant_ui_remove(void);
