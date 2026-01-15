/**
 * @file hr_app_overview_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hr_app_overview_gen.h"
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

lv_obj_t * hr_app_overview_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_root;
    static lv_style_t style_header;
    static lv_style_t style_hr_label;
    static lv_style_t style_hr_display;
    static lv_style_t style_hr_value;
    static lv_style_t style_hr_unit;
    static lv_style_t style_conf_container;
    static lv_style_t style_conf_label_row;
    static lv_style_t style_conf_label;
    static lv_style_t style_conf_bar;
    static lv_style_t style_conf_bar_indicator;
    static lv_style_t style_data_grid;
    static lv_style_t style_data_item;
    static lv_style_t style_data_label;
    static lv_style_t style_data_value;
    static lv_style_t style_data_value_good;
    static lv_style_t style_bottom_row;
    static lv_style_t style_skin_container;
    static lv_style_t style_skin_text;
    static lv_style_t style_activity_pill;
    static lv_style_t style_activity_text;

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
        lv_style_set_pad_top(&style_root, 15);
        lv_style_set_pad_row(&style_root, 2);
        lv_style_set_bg_color(&style_root, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_root, 255);
        lv_style_set_border_width(&style_root, 0);

        lv_style_init(&style_header);
        lv_style_set_layout(&style_header, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_header, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_header, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_header, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_all(&style_header, 0);
        lv_style_set_margin_all(&style_header, 0);
        lv_style_set_bg_opa(&style_header, 0);
        lv_style_set_border_width(&style_header, 0);

        lv_style_init(&style_hr_label);
        lv_style_set_text_color(&style_hr_label, lv_color_hex(0x666666));
        lv_style_set_text_font(&style_hr_label, montserrat_12);

        lv_style_init(&style_hr_display);
        lv_style_set_layout(&style_hr_display, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_hr_display, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_hr_display, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_hr_display, LV_FLEX_ALIGN_END);
        lv_style_set_margin_all(&style_hr_display, 0);
        lv_style_set_pad_all(&style_hr_display, 0);
        lv_style_set_bg_opa(&style_hr_display, 0);
        lv_style_set_border_width(&style_hr_display, 0);

        lv_style_init(&style_hr_value);
        lv_style_set_text_color(&style_hr_value, lv_color_hex(0xffffff));
        lv_style_set_text_font(&style_hr_value, montserrat_48);

        lv_style_init(&style_hr_unit);
        lv_style_set_text_color(&style_hr_unit, lv_color_hex(0x666666));
        lv_style_set_text_font(&style_hr_unit, montserrat_14);

        lv_style_init(&style_conf_container);
        lv_style_set_width(&style_conf_container, 140);
        lv_style_set_layout(&style_conf_container, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_conf_container, LV_FLEX_FLOW_COLUMN);
        lv_style_set_pad_row(&style_conf_container, 3);
        lv_style_set_margin_all(&style_conf_container, 0);
        lv_style_set_pad_all(&style_conf_container, 0);
        lv_style_set_bg_opa(&style_conf_container, 0);
        lv_style_set_border_width(&style_conf_container, 0);

        lv_style_init(&style_conf_label_row);
        lv_style_set_layout(&style_conf_label_row, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_conf_label_row, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_conf_label_row, LV_FLEX_ALIGN_SPACE_BETWEEN);
        lv_style_set_width(&style_conf_label_row, lv_pct(100));
        lv_style_set_margin_all(&style_conf_label_row, 0);
        lv_style_set_pad_all(&style_conf_label_row, 0);
        lv_style_set_bg_opa(&style_conf_label_row, 0);
        lv_style_set_border_width(&style_conf_label_row, 0);

        lv_style_init(&style_conf_label);
        lv_style_set_text_color(&style_conf_label, lv_color_hex(0x666666));
        lv_style_set_text_font(&style_conf_label, montserrat_12);

        lv_style_init(&style_conf_bar);
        lv_style_set_pad_all(&style_conf_bar, 0);
        lv_style_set_margin_all(&style_conf_bar, 0);
        lv_style_set_width(&style_conf_bar, lv_pct(100));
        lv_style_set_height(&style_conf_bar, 10);
        lv_style_set_bg_color(&style_conf_bar, lv_color_hex(0x222222));
        lv_style_set_radius(&style_conf_bar, 5);

        lv_style_init(&style_conf_bar_indicator);
        lv_style_set_bg_color(&style_conf_bar_indicator, lv_color_hex(0x3388ff));
        lv_style_set_radius(&style_conf_bar_indicator, 5);

        lv_style_init(&style_data_grid);
        lv_style_set_layout(&style_data_grid, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_data_grid, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_data_grid, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_column(&style_data_grid, 25);
        lv_style_set_pad_top(&style_data_grid, 0);
        lv_style_set_bg_opa(&style_data_grid, 0);
        lv_style_set_border_width(&style_data_grid, 0);

        lv_style_init(&style_data_item);
        lv_style_set_layout(&style_data_item, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_data_item, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_cross_place(&style_data_item, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_top(&style_data_item, 6);
        lv_style_set_bg_opa(&style_data_item, 0);
        lv_style_set_border_width(&style_data_item, 0);

        lv_style_init(&style_data_label);
        lv_style_set_text_color(&style_data_label, lv_color_hex(0x666666));
        lv_style_set_text_font(&style_data_label, montserrat_12);

        lv_style_init(&style_data_value);
        lv_style_set_text_color(&style_data_value, lv_color_hex(0xffffff));
        lv_style_set_text_font(&style_data_value, montserrat_18);

        lv_style_init(&style_data_value_good);
        lv_style_set_text_color(&style_data_value_good, lv_color_hex(0x00ff88));
        lv_style_set_text_font(&style_data_value_good, montserrat_18);

        lv_style_init(&style_bottom_row);
        lv_style_set_layout(&style_bottom_row, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_bottom_row, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_bottom_row, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_bottom_row, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_column(&style_bottom_row, 15);
        lv_style_set_bg_opa(&style_bottom_row, 0);
        lv_style_set_border_width(&style_bottom_row, 0);

        lv_style_init(&style_skin_container);
        lv_style_set_layout(&style_skin_container, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_skin_container, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_cross_place(&style_skin_container, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_column(&style_skin_container, 5);
        lv_style_set_bg_opa(&style_skin_container, 0);
        lv_style_set_border_width(&style_skin_container, 0);

        lv_style_init(&style_skin_text);
        lv_style_set_text_color(&style_skin_text, lv_color_hex(0x00ff88));
        lv_style_set_text_font(&style_skin_text, montserrat_12);

        lv_style_init(&style_activity_pill);
        lv_style_set_layout(&style_activity_pill, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_activity_pill, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_cross_place(&style_activity_pill, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_column(&style_activity_pill, 4);
        lv_style_set_pad_left(&style_activity_pill, 10);
        lv_style_set_pad_right(&style_activity_pill, 10);
        lv_style_set_pad_top(&style_activity_pill, 4);
        lv_style_set_pad_bottom(&style_activity_pill, 4);
        lv_style_set_bg_color(&style_activity_pill, lv_color_hex(0x222222));
        lv_style_set_bg_opa(&style_activity_pill, 255);
        lv_style_set_radius(&style_activity_pill, 12);
        lv_style_set_border_width(&style_activity_pill, 0);

        lv_style_init(&style_activity_text);
        lv_style_set_text_color(&style_activity_text, lv_color_hex(0xffffff));
        lv_style_set_text_font(&style_activity_text, montserrat_12);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "hr_app_overview_#");

    lv_obj_remove_style_all(lv_obj_0);
    lv_obj_add_style(lv_obj_0, &style_root, 0);
    lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_1, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_1, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_1, &style_header, 0);
    lv_obj_t * lv_image_0 = lv_image_create(lv_obj_1);
    lv_image_set_src(lv_image_0, heart);
    
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
    lv_label_set_text(lv_label_0, "HEART RATE");
    lv_obj_add_style(lv_label_0, &style_hr_label, 0);
    
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_2, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_2, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_2, &style_hr_display, 0);
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_2);
    lv_label_bind_text(lv_label_1, &hr_bpm_text, NULL);
    lv_obj_add_style(lv_label_1, &style_hr_value, 0);
    
    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_2);
    lv_label_set_text(lv_label_2, "bpm");
    lv_obj_add_style(lv_label_2, &style_hr_unit, 0);
    
    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_3, 140);
    lv_obj_set_height(lv_obj_3, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_3, &style_conf_container, 0);
    lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_3);
    lv_obj_set_width(lv_obj_4, 140);
    lv_obj_set_height(lv_obj_4, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_4, &style_conf_label_row, 0);
    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_4);
    lv_label_set_text(lv_label_3, "Confidence");
    lv_obj_add_style(lv_label_3, &style_conf_label, 0);
    
    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_4);
    lv_label_bind_text(lv_label_4, &hr_confidence_text, NULL);
    lv_obj_add_style(lv_label_4, &style_conf_label, 0);
    
    lv_obj_t * lv_bar_0 = lv_bar_create(lv_obj_3);
    lv_obj_set_width(lv_bar_0, 140);
    lv_obj_set_height(lv_bar_0, 8);
    lv_bar_set_value(lv_bar_0, 50, false);
    lv_bar_set_min_value(lv_bar_0, 0);
    lv_bar_set_max_value(lv_bar_0, 100);
    lv_bar_bind_value(lv_bar_0, &hr_confidence);
    lv_obj_add_style(lv_bar_0, &style_conf_bar, 0);
    lv_obj_add_style(lv_bar_0, &style_conf_bar_indicator, LV_PART_INDICATOR);
    
    lv_obj_t * lv_obj_5 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_5, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_5, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_5, &style_data_grid, 0);
    lv_obj_t * lv_obj_6 = lv_obj_create(lv_obj_5);
    lv_obj_set_width(lv_obj_6, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_6, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_6, &style_data_item, 0);
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_6);
    lv_label_set_text(lv_label_5, "RR INT");
    lv_obj_add_style(lv_label_5, &style_data_label, 0);
    
    lv_obj_t * lv_label_6 = lv_label_create(lv_obj_6);
    lv_label_bind_text(lv_label_6, &hr_rr_text, NULL);
    lv_obj_add_style(lv_label_6, &style_data_value, 0);
    
    lv_obj_t * lv_label_7 = lv_label_create(lv_obj_6);
    lv_label_bind_text(lv_label_7, &hr_skin_text, NULL);
    lv_obj_add_style(lv_label_7, &style_skin_text, 0);
    
    lv_obj_t * lv_obj_7 = lv_obj_create(lv_obj_5);
    lv_obj_set_width(lv_obj_7, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_7, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_7, &style_data_item, 0);
    lv_obj_t * lv_label_8 = lv_label_create(lv_obj_7);
    lv_label_set_text(lv_label_8, "SPO2");
    lv_obj_add_style(lv_label_8, &style_data_label, 0);
    
    lv_obj_t * lv_label_9 = lv_label_create(lv_obj_7);
    lv_label_bind_text(lv_label_9, &hr_spo2_text, NULL);
    lv_obj_add_style(lv_label_9, &style_data_value_good, 0);
    
    lv_obj_t * lv_obj_8 = lv_obj_create(lv_obj_7);
    lv_obj_set_width(lv_obj_8, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_8, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_8, &style_activity_pill, 0);
    lv_obj_t * lv_label_10 = lv_label_create(lv_obj_8);
    lv_label_bind_text(lv_label_10, &hr_activity_text, NULL);
    lv_obj_add_style(lv_label_10, &style_activity_text, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

