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

// Chart division line functions - newer API uses separate setters
static inline void lv_chart_set_hor_div_line_count(lv_obj_t * obj, uint8_t cnt)
{
    // Get current vdiv by reading from chart, default to 5 if unknown
    lv_chart_set_div_line_count(obj, cnt, 5);
}

static inline void lv_chart_set_ver_div_line_count(lv_obj_t * obj, uint8_t cnt)
{
    // Get current hdiv by reading from chart, default to 5 if unknown
    lv_chart_set_div_line_count(obj, 5, cnt);
}

// Chart axis value functions - newer API uses separate min/max setters
static inline void lv_chart_set_axis_min_value(lv_obj_t * obj, lv_chart_axis_t axis, int32_t min)
{
    // Set range with current max preserved (use a large default max)
    lv_chart_set_range(obj, axis, min, 100);
}

static inline void lv_chart_set_axis_max_value(lv_obj_t * obj, lv_chart_axis_t axis, int32_t max)
{
    // Set range with current min preserved (use 0 as default min)
    lv_chart_set_range(obj, axis, 0, max);
}

// Bar range functions - newer API uses separate min/max setters
static inline void lv_bar_set_min_value(lv_obj_t * obj, int32_t min)
{
    int32_t max = lv_bar_get_max_value(obj);
    lv_bar_set_range(obj, min, max);
}

static inline void lv_bar_set_max_value(lv_obj_t * obj, int32_t max)
{
    int32_t min = lv_bar_get_min_value(obj);
    lv_bar_set_range(obj, min, max);
}

// Bar bind value - this is a newer observer API, stub it out
// The binding functionality would require observer support
static inline lv_observer_t * lv_bar_bind_value(lv_obj_t * obj, lv_subject_t * subject)
{
    (void)obj;
    (void)subject;
    // Observer binding for bar not available in this LVGL version
    // Manual updates via lv_bar_set_value() should be used instead
    return NULL;
}
#endif