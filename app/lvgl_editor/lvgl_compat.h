/*
 * Added for ZSWatch project
 * LVGL API compatibility layer for LVGL Editor generated code
 * Maps newer LVGL v9 API to the version used in Zephyr NCS
 */

#pragma once

#if !LV_EDITOR_PREVIEW

#include <lvgl.h>

// We don't use object names in this project, so map to no-op
static inline void lv_obj_set_name(lv_obj_t * obj, const char * name) 
{
    (void)obj;
    (void)name;
}

static inline void lv_obj_set_name_static(lv_obj_t * obj, const char * name)
{
    (void)obj;
    (void)name;
}

#endif