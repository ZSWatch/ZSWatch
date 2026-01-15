/**
 * @file hr_app_debug_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hr_app_debug_gen.h"
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

lv_obj_t * hr_app_debug_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_root;
    static lv_style_t style_header;
    static lv_style_t style_header_text;
    static lv_style_t style_hr_section;
    static lv_style_t style_hr_value;
    static lv_style_t style_hr_unit;
    static lv_style_t style_hr_conf;
    static lv_style_t style_data_section;
    static lv_style_t style_data_row;
    static lv_style_t style_data_row_last;
    static lv_style_t style_data_label;
    static lv_style_t style_data_value;
    static lv_style_t style_data_value_good;
    static lv_style_t style_data_value_info;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&style_root);
        lv_style_set_width(&style_root, lv_pct(100));
        lv_style_set_height(&style_root, lv_pct(100));
        lv_style_set_layout(&style_root, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_root, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_main_place(&style_root, LV_FLEX_ALIGN_START);
        lv_style_set_flex_cross_place(&style_root, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_track_place(&style_root, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_top(&style_root, 5);
        lv_style_set_pad_row(&style_root, 2);
        lv_style_set_pad_bottom(&style_root, 25);
        lv_style_set_bg_color(&style_root, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_root, 255);
        lv_style_set_border_width(&style_root, 0);

        lv_style_init(&style_header);
        lv_style_set_width(&style_header, lv_pct(100));
        lv_style_set_text_align(&style_header, LV_TEXT_ALIGN_CENTER);
        lv_style_set_pad_bottom(&style_header, 5);
        lv_style_set_border_side(&style_header, LV_BORDER_SIDE_BOTTOM);
        lv_style_set_border_width(&style_header, 1);
        lv_style_set_border_color(&style_header, lv_color_hex(0x222222));
        lv_style_set_bg_opa(&style_header, 0);

        lv_style_init(&style_header_text);
        lv_style_set_text_color(&style_header_text, lv_color_hex(0x666666));
        lv_style_set_text_font(&style_header_text, montserrat_12);

        lv_style_init(&style_hr_section);
        lv_style_set_layout(&style_hr_section, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_hr_section, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_hr_section, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_hr_section, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_column(&style_hr_section, 4);
        lv_style_set_pad_top(&style_hr_section, 0);
        lv_style_set_pad_bottom(&style_hr_section, 0);
        lv_style_set_bg_opa(&style_hr_section, 0);
        lv_style_set_border_width(&style_hr_section, 0);

        lv_style_init(&style_hr_value);
        lv_style_set_text_color(&style_hr_value, lv_color_hex(0xff4444));
        lv_style_set_text_font(&style_hr_value, montserrat_28);

        lv_style_init(&style_hr_unit);
        lv_style_set_text_color(&style_hr_unit, lv_color_hex(0x666666));
        lv_style_set_text_font(&style_hr_unit, montserrat_12);

        lv_style_init(&style_hr_conf);
        lv_style_set_text_color(&style_hr_conf, lv_color_hex(0x00ff88));
        lv_style_set_text_font(&style_hr_conf, montserrat_12);
        lv_style_set_text_align(&style_hr_conf, LV_TEXT_ALIGN_CENTER);

        lv_style_init(&style_data_section);
        lv_style_set_width(&style_data_section, lv_pct(100));
        lv_style_set_layout(&style_data_section, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_data_section, LV_FLEX_FLOW_COLUMN);
        lv_style_set_pad_left(&style_data_section, 15);
        lv_style_set_pad_right(&style_data_section, 15);
        lv_style_set_bg_opa(&style_data_section, 0);
        lv_style_set_border_width(&style_data_section, 0);

        lv_style_init(&style_data_row);
        lv_style_set_width(&style_data_row, lv_pct(100));
        lv_style_set_layout(&style_data_row, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_data_row, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_data_row, LV_FLEX_ALIGN_SPACE_BETWEEN);
        lv_style_set_flex_cross_place(&style_data_row, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_top(&style_data_row, 3);
        lv_style_set_pad_bottom(&style_data_row, 3);
        lv_style_set_border_side(&style_data_row, LV_BORDER_SIDE_BOTTOM);
        lv_style_set_border_width(&style_data_row, 1);
        lv_style_set_border_color(&style_data_row, lv_color_hex(0x1a1a1a));
        lv_style_set_bg_opa(&style_data_row, 0);

        lv_style_init(&style_data_row_last);
        lv_style_set_width(&style_data_row_last, lv_pct(100));
        lv_style_set_layout(&style_data_row_last, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_data_row_last, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_data_row_last, LV_FLEX_ALIGN_SPACE_BETWEEN);
        lv_style_set_flex_cross_place(&style_data_row_last, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_top(&style_data_row_last, 3);
        lv_style_set_pad_bottom(&style_data_row_last, 3);
        lv_style_set_border_width(&style_data_row_last, 0);
        lv_style_set_bg_opa(&style_data_row_last, 0);

        lv_style_init(&style_data_label);
        lv_style_set_text_color(&style_data_label, lv_color_hex(0x666666));
        lv_style_set_text_font(&style_data_label, montserrat_12);

        lv_style_init(&style_data_value);
        lv_style_set_text_color(&style_data_value, lv_color_hex(0xffffff));
        lv_style_set_text_font(&style_data_value, montserrat_12);

        lv_style_init(&style_data_value_good);
        lv_style_set_text_color(&style_data_value_good, lv_color_hex(0x00ff88));
        lv_style_set_text_font(&style_data_value_good, montserrat_12);

        lv_style_init(&style_data_value_info);
        lv_style_set_text_color(&style_data_value_info, lv_color_hex(0x3388ff));
        lv_style_set_text_font(&style_data_value_info, montserrat_12);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "hr_app_debug_#");

    lv_obj_remove_style_all(lv_obj_0);
    lv_obj_add_style(lv_obj_0, &style_root, 0);
    lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_1, 200);
    lv_obj_set_height(lv_obj_1, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_1, &style_header, 0);
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
    lv_label_set_text(lv_label_0, "DEBUG VIEW");
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_obj_add_style(lv_label_0, &style_header_text, 0);
    
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_2, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_2, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_2, &style_hr_section, 0);
    lv_obj_t * lv_image_0 = lv_image_create(lv_obj_2);
    lv_image_set_src(lv_image_0, heart);
    
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_2);
    lv_label_bind_text(lv_label_1, &hr_bpm_text, NULL);
    lv_obj_add_style(lv_label_1, &style_hr_value, 0);
    
    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_2);
    lv_label_set_text(lv_label_2, "bpm");
    lv_obj_add_style(lv_label_2, &style_hr_unit, 0);
    
    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_3, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_3, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_3, &style_hr_section, 0);
    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_3);
    lv_label_set_text(lv_label_3, "Conf");
    lv_obj_add_style(lv_label_3, &style_hr_conf, 0);
    
    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_3);
    lv_label_bind_text(lv_label_4, &hr_confidence_text, NULL);
    lv_obj_add_style(lv_label_4, &style_hr_conf, 0);
    
    lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_4, 200);
    lv_obj_set_height(lv_obj_4, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_4, &style_data_section, 0);
    lv_obj_t * lv_obj_5 = lv_obj_create(lv_obj_4);
    lv_obj_set_width(lv_obj_5, lv_pct(100));
    lv_obj_set_height(lv_obj_5, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_5, &style_data_row, 0);
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_5);
    lv_label_set_text(lv_label_5, "RR Interval");
    lv_obj_add_style(lv_label_5, &style_data_label, 0);
    
    lv_obj_t * lv_label_6 = lv_label_create(lv_obj_5);
    lv_label_bind_text(lv_label_6, &hr_rr_text, NULL);
    lv_obj_add_style(lv_label_6, &style_data_value, 0);
    
    lv_obj_t * lv_obj_6 = lv_obj_create(lv_obj_4);
    lv_obj_set_width(lv_obj_6, lv_pct(100));
    lv_obj_set_height(lv_obj_6, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_6, &style_data_row, 0);
    lv_obj_t * lv_label_7 = lv_label_create(lv_obj_6);
    lv_label_set_text(lv_label_7, "RR Conf");
    lv_obj_add_style(lv_label_7, &style_data_label, 0);
    
    lv_obj_t * lv_label_8 = lv_label_create(lv_obj_6);
    lv_label_bind_text(lv_label_8, &hr_confidence_text, NULL);
    lv_obj_add_style(lv_label_8, &style_data_value_info, 0);
    
    lv_obj_t * lv_obj_7 = lv_obj_create(lv_obj_4);
    lv_obj_set_width(lv_obj_7, lv_pct(100));
    lv_obj_set_height(lv_obj_7, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_7, &style_data_row, 0);
    lv_obj_t * lv_label_9 = lv_label_create(lv_obj_7);
    lv_label_set_text(lv_label_9, "SpO2");
    lv_obj_add_style(lv_label_9, &style_data_label, 0);
    
    lv_obj_t * lv_label_10 = lv_label_create(lv_obj_7);
    lv_label_bind_text(lv_label_10, &hr_spo2_text, NULL);
    lv_obj_add_style(lv_label_10, &style_data_value_good, 0);
    
    lv_obj_t * lv_obj_8 = lv_obj_create(lv_obj_4);
    lv_obj_set_width(lv_obj_8, lv_pct(100));
    lv_obj_set_height(lv_obj_8, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_8, &style_data_row, 0);
    lv_obj_t * lv_label_11 = lv_label_create(lv_obj_8);
    lv_label_set_text(lv_label_11, "SpO2 Conf");
    lv_obj_add_style(lv_label_11, &style_data_label, 0);
    
    lv_obj_t * lv_label_12 = lv_label_create(lv_obj_8);
    lv_label_bind_text(lv_label_12, &hr_confidence_text, NULL);
    lv_obj_add_style(lv_label_12, &style_data_value_info, 0);
    
    lv_obj_t * lv_obj_9 = lv_obj_create(lv_obj_4);
    lv_obj_set_width(lv_obj_9, lv_pct(100));
    lv_obj_set_height(lv_obj_9, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_9, &style_data_row, 0);
    lv_obj_t * lv_label_13 = lv_label_create(lv_obj_9);
    lv_label_set_text(lv_label_13, "Skin Contact");
    lv_obj_add_style(lv_label_13, &style_data_label, 0);
    
    lv_obj_t * lv_label_14 = lv_label_create(lv_obj_9);
    lv_label_bind_text(lv_label_14, &hr_skin_text, NULL);
    lv_obj_add_style(lv_label_14, &style_data_value_good, 0);
    
    lv_obj_t * lv_obj_10 = lv_obj_create(lv_obj_4);
    lv_obj_set_width(lv_obj_10, lv_pct(100));
    lv_obj_set_height(lv_obj_10, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_10, &style_data_row_last, 0);
    lv_obj_t * lv_label_15 = lv_label_create(lv_obj_10);
    lv_label_set_text(lv_label_15, "Activity");
    lv_obj_add_style(lv_label_15, &style_data_label, 0);
    
    lv_obj_t * lv_label_16 = lv_label_create(lv_obj_10);
    lv_label_bind_text(lv_label_16, &hr_activity_text, NULL);
    lv_obj_add_style(lv_label_16, &style_data_value, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

