#pragma once

#include <inttypes.h>
#include <lvgl.h>

void about_ui_show(lv_obj_t *root, int hw_version, char *fw_version, char *fw_version_info, char *sdk_version,
                   char *fs_stats, int num_apps);

void about_ui_remove(void);
