/*
 * ZSWatch EDK — minimal zsw_ui_utils.h for LLEXT apps.
 * Only the image macros needed by apps are included here.
 */
#pragma once

#include <lvgl.h>

/* LLEXT apps always run on hardware with external flash images */
#define ZSW_LV_IMG_DECLARE(var_name)
#define ZSW_LV_IMG_USE(var_name)                            "S:"#var_name".bin"
#define ZSW_LV_IMG_USE_WITH_MOUNT(var_name, mount_letter)   mount_letter "/" #var_name ".bin"
