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

// Arc range functions - newer API uses separate min/max setters
static inline void lv_arc_set_min_value(lv_obj_t * obj, int32_t min)
{
    int32_t max = lv_arc_get_max_value(obj);
    lv_arc_set_range(obj, min, max);
}

static inline void lv_arc_set_max_value(lv_obj_t * obj, int32_t max)
{
    int32_t min = lv_arc_get_min_value(obj);
    lv_arc_set_range(obj, min, max);
}

// Flag functions - newer API uses lv_obj_set_flag with bool parameter
static inline void lv_obj_set_flag(lv_obj_t * obj, lv_obj_flag_t flag, bool enable)
{
    if (enable) {
        lv_obj_add_flag(obj, flag);
    } else {
        lv_obj_clear_flag(obj, flag);
    }
}
#endif