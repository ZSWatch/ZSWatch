/**
 * @file music_app_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "music_app_gen.h"
#include "lvgl_editor.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * music_app_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_root;
    static lv_style_t style_arc_bg;
    static lv_style_t style_arc_indicator;
    static lv_style_t style_button_base;
    static lv_style_t style_button_focused;
    static lv_style_t style_track_label;
    static lv_style_t style_artist_label;
    static lv_style_t style_time_label;
    static lv_style_t style_button_container;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&style_root);
        lv_style_set_width(&style_root, lv_pct(100));
        lv_style_set_height(&style_root, lv_pct(100));
        lv_style_set_border_width(&style_root, 0);
        lv_style_set_bg_opa(&style_root, 0);

        lv_style_init(&style_arc_bg);
        lv_style_set_width(&style_arc_bg, 240);
        lv_style_set_height(&style_arc_bg, 240);
        lv_style_set_arc_width(&style_arc_bg, 3);
        lv_style_set_arc_color(&style_arc_bg, lv_color_hex(0x444444));
        lv_style_set_bg_opa(&style_arc_bg, 0);

        lv_style_init(&style_arc_indicator);
        lv_style_set_arc_width(&style_arc_indicator, 3);
        lv_style_set_arc_color(&style_arc_indicator, lv_color_hex(0xFF6600));

        lv_style_init(&style_button_base);
        lv_style_set_bg_opa(&style_button_base, 0);
        lv_style_set_border_width(&style_button_base, 0);
        lv_style_set_pad_all(&style_button_base, 0);
        lv_style_set_width(&style_button_base, LV_SIZE_CONTENT);
        lv_style_set_height(&style_button_base, LV_SIZE_CONTENT);

        lv_style_init(&style_button_focused);
        lv_style_set_border_side(&style_button_focused, LV_BORDER_SIDE_BOTTOM);
        lv_style_set_border_width(&style_button_focused, 1);
        lv_style_set_border_color(&style_button_focused, lv_color_hex(0xFF6600));

        lv_style_init(&style_track_label);
        lv_style_set_text_align(&style_track_label, LV_TEXT_ALIGN_CENTER);
        lv_style_set_width(&style_track_label, lv_pct(50));

        lv_style_init(&style_artist_label);
        lv_style_set_text_align(&style_artist_label, LV_TEXT_ALIGN_CENTER);
        lv_style_set_width(&style_artist_label, lv_pct(50));

        lv_style_init(&style_time_label);
        lv_style_set_text_align(&style_time_label, LV_TEXT_ALIGN_CENTER);
        lv_style_set_width(&style_time_label, LV_SIZE_CONTENT);

        lv_style_init(&style_button_container);
        lv_style_set_layout(&style_button_container, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_button_container, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_button_container, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_button_container, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_column(&style_button_container, 10);
        lv_style_set_bg_opa(&style_button_container, 0);
        lv_style_set_border_width(&style_button_container, 0);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);

    lv_obj_remove_style_all(lv_obj_0);
    lv_obj_add_style(lv_obj_0, &style_root, 0);
    lv_obj_t * lv_arc_0 = lv_arc_create(lv_obj_0);
    lv_obj_set_align(lv_arc_0, LV_ALIGN_CENTER);
    lv_arc_set_rotation(lv_arc_0, 270);
    lv_arc_set_bg_start_angle(lv_arc_0, 0);
    lv_arc_set_bg_end_angle(lv_arc_0, 360);
    lv_arc_set_min_value(lv_arc_0, 0);
    lv_arc_set_max_value(lv_arc_0, 100);
    lv_arc_bind_value(lv_arc_0, &music_progress);
    lv_arc_set_mode(lv_arc_0, LV_ARC_MODE_NORMAL);
    lv_obj_set_flag(lv_arc_0, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_add_style(lv_arc_0, &style_arc_bg, 0);
    lv_obj_add_style(lv_arc_0, &style_arc_indicator, LV_PART_INDICATOR);
    lv_obj_remove_style(lv_arc_0, NULL, LV_PART_KNOB);
    
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
    lv_obj_set_style_text_font(lv_label_0, montserrat_14_full, 0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_TOP_MID);
    lv_obj_set_x(lv_label_0, 0);
    lv_obj_set_y(lv_label_0, 25);
    lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_label_bind_text(lv_label_0, &music_track_name, NULL);
    lv_obj_add_style(lv_label_0, &style_track_label, 0);
    
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
    lv_obj_set_style_text_font(lv_label_1, montserrat_14_full, 0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_TOP_MID);
    lv_obj_set_x(lv_label_1, 0);
    lv_obj_set_y(lv_label_1, 45);
    lv_label_set_long_mode(lv_label_1, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_label_bind_text(lv_label_1, &music_artist_name, NULL);
    lv_obj_add_style(lv_label_1, &style_artist_label, 0);
    
    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_0);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_obj_set_x(lv_label_2, 0);
    lv_obj_set_y(lv_label_2, -105);
    lv_label_bind_text(lv_label_2, &music_time, NULL);
    lv_obj_add_style(lv_label_2, &style_time_label, 0);
    
    lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_align(lv_obj_1, LV_ALIGN_CENTER);
    lv_obj_set_x(lv_obj_1, 0);
    lv_obj_set_y(lv_obj_1, 0);
    lv_obj_set_width(lv_obj_1, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_1, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_1, &style_button_container, 0);
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_set_flag(lv_button_0, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_set_flag(lv_button_0, LV_OBJ_FLAG_CLICK_FOCUSABLE, true);
    lv_obj_add_style(lv_button_0, &style_button_base, 0);
    lv_obj_t * lv_image_0 = lv_image_create(lv_button_0);
    lv_image_set_src(lv_image_0, icon_skip_back);
    lv_obj_set_align(lv_image_0, LV_ALIGN_CENTER);
    
    lv_obj_add_event_cb(lv_button_0, music_on_prev_clicked, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_1);
    lv_obj_set_flag(lv_button_1, LV_OBJ_FLAG_CLICK_FOCUSABLE, true);
    lv_obj_remove_style_all(lv_button_1);
    lv_obj_add_style(lv_button_1, &style_button_base, 0);
    lv_obj_t * lv_image_1 = lv_image_create(lv_button_1);
    lv_image_set_src(lv_image_1, icon_play);
    lv_obj_set_align(lv_image_1, LV_ALIGN_CENTER);
    lv_obj_bind_flag_if_eq(lv_image_1, &music_playing, LV_OBJ_FLAG_HIDDEN, 1);
    
    lv_obj_t * lv_image_2 = lv_image_create(lv_button_1);
    lv_image_set_src(lv_image_2, icon_pause);
    lv_obj_set_align(lv_image_2, LV_ALIGN_CENTER);
    lv_obj_bind_flag_if_eq(lv_image_2, &music_playing, LV_OBJ_FLAG_HIDDEN, 0);
    
    lv_obj_add_event_cb(lv_button_1, music_on_play_pause_clicked, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_2 = lv_button_create(lv_obj_1);
    lv_obj_set_flag(lv_button_2, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_set_flag(lv_button_2, LV_OBJ_FLAG_CLICK_FOCUSABLE, true);
    lv_obj_add_style(lv_button_2, &style_button_base, 0);
    lv_obj_t * lv_image_3 = lv_image_create(lv_button_2);
    lv_image_set_src(lv_image_3, icon_skip_forward);
    lv_obj_set_align(lv_image_3, LV_ALIGN_CENTER);
    
    lv_obj_add_event_cb(lv_button_2, music_on_next_clicked, LV_EVENT_CLICKED, NULL);

    LV_TRACE_OBJ_CREATE("finished");

    lv_obj_set_name(lv_obj_0, "music_app_#");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

