#pragma once

#include <inttypes.h>
#include <lvgl.h>

void update_ui_show(lv_obj_t *root, void (*start_fw_update_cb)(void));

void update_ui_remove(void);

void update_ui_set_status(const char *status);

void update_ui_set_progress(int progress);

void update_ui_set_fota_usb_callback(void (*fota_usb_cb)(bool enabled));
