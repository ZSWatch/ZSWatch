/**
 * @file hr_app_graph_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hr_app_graph_gen.h"
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

lv_obj_t * hr_app_graph_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_root;
    static lv_style_t style_top_bar;
    static lv_style_t style_hr_display;
    static lv_style_t style_hr_value;
    static lv_style_t style_hr_unit;
    static lv_style_t style_conf_display;
    static lv_style_t style_conf_value;
    static lv_style_t style_conf_label;
    static lv_style_t style_chart_container;
    static lv_style_t style_chart;
    static lv_style_t style_scale;
    static lv_style_t style_legend;
    static lv_style_t style_legend_item;
    static lv_style_t style_legend_line_hr;
    static lv_style_t style_legend_line_conf;
    static lv_style_t style_legend_text;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&style_root);
        lv_style_set_width(&style_root, lv_pct(100));
        lv_style_set_height(&style_root, lv_pct(100));
        lv_style_set_layout(&style_root, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_root, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_main_place(&style_root, LV_FLEX_ALIGN_START);
        lv_style_set_flex_cross_place(&style_root, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_top(&style_root, 5);
        lv_style_set_margin_all(&style_root, 0);
        lv_style_set_pad_all(&style_root, 0);
        lv_style_set_bg_color(&style_root, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_root, 255);
        lv_style_set_border_width(&style_root, 0);

        lv_style_init(&style_top_bar);
        lv_style_set_width(&style_top_bar, lv_pct(100));
        lv_style_set_layout(&style_top_bar, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_top_bar, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_top_bar, LV_FLEX_ALIGN_SPACE_BETWEEN);
        lv_style_set_flex_cross_place(&style_top_bar, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_left(&style_top_bar, 45);
        lv_style_set_pad_right(&style_top_bar, 45);
        lv_style_set_pad_bottom(&style_top_bar, 0);
        lv_style_set_bg_opa(&style_top_bar, 0);
        lv_style_set_border_width(&style_top_bar, 0);

        lv_style_init(&style_hr_display);
        lv_style_set_layout(&style_hr_display, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_hr_display, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_cross_place(&style_hr_display, LV_FLEX_ALIGN_END);
        lv_style_set_pad_column(&style_hr_display, 4);
        lv_style_set_pad_bottom(&style_hr_display, 0);
        lv_style_set_bg_opa(&style_hr_display, 0);
        lv_style_set_border_width(&style_hr_display, 0);

        lv_style_init(&style_hr_value);
        lv_style_set_text_color(&style_hr_value, lv_color_hex(0xff4444));
        lv_style_set_text_font(&style_hr_value, montserrat_24);

        lv_style_init(&style_hr_unit);
        lv_style_set_text_color(&style_hr_unit, lv_color_hex(0x666666));
        lv_style_set_text_font(&style_hr_unit, montserrat_10);

        lv_style_init(&style_conf_display);
        lv_style_set_layout(&style_conf_display, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_conf_display, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_cross_place(&style_conf_display, LV_FLEX_ALIGN_END);
        lv_style_set_pad_row(&style_conf_display, 0);
        lv_style_set_pad_bottom(&style_conf_display, 0);
        lv_style_set_bg_opa(&style_conf_display, 0);
        lv_style_set_border_width(&style_conf_display, 0);

        lv_style_init(&style_conf_value);
        lv_style_set_pad_bottom(&style_conf_value, 0);
        lv_style_set_text_color(&style_conf_value, lv_color_hex(0x3388ff));
        lv_style_set_text_font(&style_conf_value, montserrat_18);

        lv_style_init(&style_conf_label);
        lv_style_set_pad_top(&style_conf_label, 0);
        lv_style_set_text_color(&style_conf_label, lv_color_hex(0x666666));
        lv_style_set_text_font(&style_conf_label, montserrat_10);

        lv_style_init(&style_chart_container);
        lv_style_set_width(&style_chart_container, 230);
        lv_style_set_height(&style_chart_container, 130);
        lv_style_set_bg_color(&style_chart_container, lv_color_hex(0x0a0a0a));
        lv_style_set_bg_opa(&style_chart_container, 255);
        lv_style_set_radius(&style_chart_container, 8);
        lv_style_set_border_width(&style_chart_container, 0);
        lv_style_set_margin_all(&style_chart_container, 0);
        lv_style_set_pad_left(&style_chart_container, 20);
        lv_style_set_pad_right(&style_chart_container, 5);
        lv_style_set_pad_top(&style_chart_container, 5);
        lv_style_set_pad_bottom(&style_chart_container, 5);

        lv_style_init(&style_chart);
        lv_style_set_width(&style_chart, lv_pct(100));
        lv_style_set_height(&style_chart, lv_pct(100));
        lv_style_set_bg_opa(&style_chart, 0);
        lv_style_set_border_width(&style_chart, 0);
        lv_style_set_line_color(&style_chart, lv_color_hex(0x333333));
        lv_style_set_line_width(&style_chart, 2);
        lv_style_set_pad_all(&style_chart, 5);

        lv_style_init(&style_scale);
        lv_style_set_text_color(&style_scale, lv_color_hex(0xff4444));
        lv_style_set_text_font(&style_scale, montserrat_10);
        lv_style_set_line_width(&style_scale, 0);

        lv_style_init(&style_legend);
        lv_style_set_layout(&style_legend, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_legend, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_legend, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_column(&style_legend, 0);
        lv_style_set_pad_top(&style_legend, 0);
        lv_style_set_pad_bottom(&style_legend, 0);
        lv_style_set_bg_opa(&style_legend, 0);
        lv_style_set_border_width(&style_legend, 0);

        lv_style_init(&style_legend_item);
        lv_style_set_layout(&style_legend_item, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_legend_item, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_cross_place(&style_legend_item, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_column(&style_legend_item, 4);
        lv_style_set_bg_opa(&style_legend_item, 0);
        lv_style_set_border_width(&style_legend_item, 0);

        lv_style_init(&style_legend_line_hr);
        lv_style_set_width(&style_legend_line_hr, 18);
        lv_style_set_height(&style_legend_line_hr, 4);
        lv_style_set_bg_color(&style_legend_line_hr, lv_color_hex(0xff4444));
        lv_style_set_radius(&style_legend_line_hr, 2);
        lv_style_set_border_width(&style_legend_line_hr, 0);

        lv_style_init(&style_legend_line_conf);
        lv_style_set_width(&style_legend_line_conf, 18);
        lv_style_set_height(&style_legend_line_conf, 4);
        lv_style_set_bg_color(&style_legend_line_conf, lv_color_hex(0x3388ff));
        lv_style_set_radius(&style_legend_line_conf, 2);
        lv_style_set_border_width(&style_legend_line_conf, 0);

        lv_style_init(&style_legend_text);
        lv_style_set_text_color(&style_legend_text, lv_color_hex(0xaaaaaa));
        lv_style_set_text_font(&style_legend_text, montserrat_10);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "hr_app_graph_#");

    lv_obj_remove_style_all(lv_obj_0);
    lv_obj_add_style(lv_obj_0, &style_root, 0);
    lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_1, lv_pct(100));
    lv_obj_set_height(lv_obj_1, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_1, &style_top_bar, 0);
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
    lv_obj_set_width(lv_obj_2, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_2, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_2, &style_hr_display, 0);
    lv_obj_t * lv_image_0 = lv_image_create(lv_obj_2);
    lv_image_set_src(lv_image_0, heart);
    
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_2);
    lv_label_bind_text(lv_label_0, &hr_bpm_text, NULL);
    lv_obj_add_style(lv_label_0, &style_hr_value, 0);
    
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_2);
    lv_label_set_text(lv_label_1, "bpm");
    lv_obj_add_style(lv_label_1, &style_hr_unit, 0);
    
    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_1);
    lv_obj_set_width(lv_obj_3, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_3, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_3, &style_conf_display, 0);
    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_3);
    lv_label_bind_text(lv_label_2, &hr_confidence_text, NULL);
    lv_obj_add_style(lv_label_2, &style_conf_value, 0);
    
    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_3);
    lv_label_set_text(lv_label_3, "conf");
    lv_obj_add_style(lv_label_3, &style_conf_label, 0);
    
    lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_4, 230);
    lv_obj_set_height(lv_obj_4, 140);
    lv_obj_add_style(lv_obj_4, &style_chart_container, 0);
    lv_obj_t * lv_chart_0 = lv_chart_create(lv_obj_4);
    lv_obj_set_align(lv_chart_0, LV_ALIGN_CENTER);
    lv_chart_set_point_count(lv_chart_0, 60);
    lv_chart_set_update_mode(lv_chart_0, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_hor_div_line_count(lv_chart_0, 4);
    lv_chart_set_ver_div_line_count(lv_chart_0, 0);
    lv_obj_add_style(lv_chart_0, &style_chart, 0);
    lv_obj_t * hr_scale = lv_scale_create(lv_chart_0);
    lv_obj_set_name(hr_scale, "hr_scale");
    lv_scale_set_mode(hr_scale, LV_SCALE_MODE_VERTICAL_LEFT);
    lv_obj_set_height(hr_scale, lv_pct(100));
    lv_obj_set_align(hr_scale, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(hr_scale, -110);
    lv_obj_add_style(hr_scale, &style_scale, 0);
    
    lv_chart_add_series(lv_chart_0, lv_color_hex(0xff4444), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_add_series(lv_chart_0, lv_color_hex(0x3388ff), LV_CHART_AXIS_SECONDARY_Y);
    lv_chart_set_axis_min_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 40);
    lv_chart_set_axis_max_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 130);
    lv_chart_set_axis_min_value(lv_chart_0, LV_CHART_AXIS_SECONDARY_Y, 0);
    lv_chart_set_axis_max_value(lv_chart_0, LV_CHART_AXIS_SECONDARY_Y, 100);
    
    lv_obj_t * lv_obj_5 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_5, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_5, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_5, &style_legend, 0);
    lv_obj_t * lv_obj_6 = lv_obj_create(lv_obj_5);
    lv_obj_set_width(lv_obj_6, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_6, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_6, &style_legend_item, 0);
    lv_obj_t * lv_obj_7 = lv_obj_create(lv_obj_6);
    lv_obj_add_style(lv_obj_7, &style_legend_line_hr, 0);
    
    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_6);
    lv_label_set_text(lv_label_4, "HR");
    lv_obj_add_style(lv_label_4, &style_legend_text, 0);
    
    lv_obj_t * lv_obj_8 = lv_obj_create(lv_obj_5);
    lv_obj_set_width(lv_obj_8, LV_SIZE_CONTENT);
    lv_obj_set_height(lv_obj_8, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_8, &style_legend_item, 0);
    lv_obj_t * lv_obj_9 = lv_obj_create(lv_obj_8);
    lv_obj_add_style(lv_obj_9, &style_legend_line_conf, 0);
    
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_8);
    lv_label_set_text(lv_label_5, "Conf");
    lv_obj_add_style(lv_label_5, &style_legend_text, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

