/**
 * @file app_picker_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "app_picker_gen.h"
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

lv_obj_t * app_picker_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_root;
    static lv_style_t style_app_container;
    static lv_style_t style_app_slot;
    static lv_style_t style_app_icon_bg;
    static lv_style_t style_app_icon_img;
    static lv_style_t style_app_label;
    static lv_style_t style_folder_overlay;
    static lv_style_t style_folder_title;
    static lv_style_t style_folder_grid;
    static lv_style_t style_folder_app_slot;
    static lv_style_t style_folder_app_icon_bg;
    static lv_style_t style_folder_app_label;
    static lv_style_t style_folder_close;
    static lv_style_t style_nav_arrow;
    static lv_style_t style_nav_arrow_label;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&style_root);
        lv_style_set_width(&style_root, 240);
        lv_style_set_height(&style_root, 240);
        lv_style_set_bg_opa(&style_root, 0);
        lv_style_set_border_width(&style_root, 0);
        lv_style_set_pad_all(&style_root, 0);

        lv_style_init(&style_app_container);
        lv_style_set_width(&style_app_container, 240);
        lv_style_set_height(&style_app_container, 240);
        lv_style_set_bg_opa(&style_app_container, 0);
        lv_style_set_border_width(&style_app_container, 0);
        lv_style_set_pad_all(&style_app_container, 0);

        lv_style_init(&style_app_slot);
        lv_style_set_width(&style_app_slot, 57);
        lv_style_set_height(&style_app_slot, 70);
        lv_style_set_bg_opa(&style_app_slot, 0);
        lv_style_set_border_width(&style_app_slot, 0);
        lv_style_set_pad_all(&style_app_slot, 0);

        lv_style_init(&style_app_icon_bg);
        lv_style_set_width(&style_app_icon_bg, 55);
        lv_style_set_height(&style_app_icon_bg, 55);
        lv_style_set_radius(&style_app_icon_bg, 28);
        lv_style_set_bg_opa(&style_app_icon_bg, 255);
        lv_style_set_border_width(&style_app_icon_bg, 0);
        lv_style_set_pad_all(&style_app_icon_bg, 0);

        lv_style_init(&style_app_icon_img);
        lv_style_set_width(&style_app_icon_img, 40);
        lv_style_set_height(&style_app_icon_img, 40);

        lv_style_init(&style_app_label);
        lv_style_set_text_align(&style_app_label, LV_TEXT_ALIGN_CENTER);
        lv_style_set_width(&style_app_label, 60);
        lv_style_set_text_color(&style_app_label, lv_color_hex(0xFFBAAF));
        lv_style_set_text_font(&style_app_label, montserrat_12);

        lv_style_init(&style_folder_overlay);
        lv_style_set_width(&style_folder_overlay, 240);
        lv_style_set_height(&style_folder_overlay, 240);
        lv_style_set_bg_color(&style_folder_overlay, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_folder_overlay, 205);
        lv_style_set_border_width(&style_folder_overlay, 0);
        lv_style_set_pad_all(&style_folder_overlay, 0);

        lv_style_init(&style_folder_title);
        lv_style_set_text_align(&style_folder_title, LV_TEXT_ALIGN_CENTER);
        lv_style_set_text_color(&style_folder_title, lv_color_hex(0xffffff));

        lv_style_init(&style_folder_grid);
        lv_style_set_layout(&style_folder_grid, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_folder_grid, LV_FLEX_FLOW_ROW_WRAP);
        lv_style_set_flex_main_place(&style_folder_grid, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_folder_grid, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_all(&style_folder_grid, 8);
        lv_style_set_pad_row(&style_folder_grid, 8);
        lv_style_set_pad_column(&style_folder_grid, 8);
        lv_style_set_bg_opa(&style_folder_grid, 0);
        lv_style_set_border_width(&style_folder_grid, 0);
        lv_style_set_width(&style_folder_grid, 200);
        lv_style_set_height(&style_folder_grid, LV_SIZE_CONTENT);

        lv_style_init(&style_folder_app_slot);
        lv_style_set_width(&style_folder_app_slot, 55);
        lv_style_set_height(&style_folder_app_slot, 70);
        lv_style_set_bg_opa(&style_folder_app_slot, 0);
        lv_style_set_border_width(&style_folder_app_slot, 0);
        lv_style_set_pad_all(&style_folder_app_slot, 0);

        lv_style_init(&style_folder_app_icon_bg);
        lv_style_set_width(&style_folder_app_icon_bg, 55);
        lv_style_set_height(&style_folder_app_icon_bg, 55);
        lv_style_set_radius(&style_folder_app_icon_bg, 28);
        lv_style_set_bg_opa(&style_folder_app_icon_bg, 255);
        lv_style_set_border_width(&style_folder_app_icon_bg, 0);
        lv_style_set_pad_all(&style_folder_app_icon_bg, 0);

        lv_style_init(&style_folder_app_label);
        lv_style_set_text_align(&style_folder_app_label, LV_TEXT_ALIGN_CENTER);
        lv_style_set_width(&style_folder_app_label, 60);
        lv_style_set_text_color(&style_folder_app_label, lv_color_hex(0xFFBAAF));
        lv_style_set_text_font(&style_folder_app_label, montserrat_12);

        lv_style_init(&style_folder_close);
        lv_style_set_width(&style_folder_close, 55);
        lv_style_set_height(&style_folder_close, 55);
        lv_style_set_radius(&style_folder_close, 28);
        lv_style_set_bg_color(&style_folder_close, lv_color_hex(0x495060));
        lv_style_set_bg_opa(&style_folder_close, 255);
        lv_style_set_border_width(&style_folder_close, 0);
        lv_style_set_pad_all(&style_folder_close, 0);

        lv_style_init(&style_nav_arrow);
        lv_style_set_width(&style_nav_arrow, 28);
        lv_style_set_height(&style_nav_arrow, 90);
        lv_style_set_radius(&style_nav_arrow, 14);
        lv_style_set_bg_color(&style_nav_arrow, lv_color_hex(0x495060));
        lv_style_set_bg_opa(&style_nav_arrow, 255);
        lv_style_set_border_width(&style_nav_arrow, 0);
        lv_style_set_pad_all(&style_nav_arrow, 0);

        lv_style_init(&style_nav_arrow_label);
        lv_style_set_text_color(&style_nav_arrow_label, lv_color_hex(0xFFBAAF));
        lv_style_set_text_font(&style_nav_arrow_label, montserrat_14);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "app_picker_#");
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

    lv_obj_remove_style_all(lv_obj_0);
    lv_obj_add_style(lv_obj_0, &style_root, 0);
    lv_obj_t * app_container = lv_obj_create(lv_obj_0);
    lv_obj_set_name(app_container, "app_container");
    lv_obj_set_flag(app_container, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_container, &style_app_container, 0);
    lv_obj_t * app_slot_0 = lv_obj_create(app_container);
    lv_obj_set_name(app_slot_0, "app_slot_0");
    lv_obj_set_x(app_slot_0, 92);
    lv_obj_set_y(app_slot_0, 8);
    lv_obj_set_flag(app_slot_0, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_set_flag(app_slot_0, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_slot_0, &style_app_slot, 0);
    lv_obj_t * app_icon_bg_0 = lv_obj_create(app_slot_0);
    lv_obj_set_name(app_icon_bg_0, "app_icon_bg_0");
    lv_obj_set_align(app_icon_bg_0, LV_ALIGN_TOP_MID);
    lv_obj_set_flag(app_icon_bg_0, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(app_icon_bg_0, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_icon_bg_0, &style_app_icon_bg, 0);
    lv_obj_t * app_icon_0 = lv_image_create(app_icon_bg_0);
    lv_obj_set_name(app_icon_0, "app_icon_0");
    lv_obj_set_align(app_icon_0, LV_ALIGN_CENTER);
    lv_obj_set_flag(app_icon_0, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_add_style(app_icon_0, &style_app_icon_img, 0);
    
    lv_obj_t * app_label_0 = lv_label_create(app_slot_0);
    lv_obj_set_name(app_label_0, "app_label_0");
    lv_obj_set_align(app_label_0, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(app_label_0, 2);
    lv_label_set_text(app_label_0, "App 0");
    lv_obj_set_flag(app_label_0, LV_OBJ_FLAG_CLICKABLE, false);
    lv_label_set_long_mode(app_label_0, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_add_style(app_label_0, &style_app_label, 0);
    
    lv_obj_add_event_cb(app_slot_0, app_picker_on_app_clicked, LV_EVENT_SHORT_CLICKED, NULL);
    
    lv_obj_t * app_slot_1 = lv_obj_create(app_container);
    lv_obj_set_name(app_slot_1, "app_slot_1");
    lv_obj_set_x(app_slot_1, 152);
    lv_obj_set_y(app_slot_1, 42);
    lv_obj_set_flag(app_slot_1, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_set_flag(app_slot_1, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_slot_1, &style_app_slot, 0);
    lv_obj_t * app_icon_bg_1 = lv_obj_create(app_slot_1);
    lv_obj_set_name(app_icon_bg_1, "app_icon_bg_1");
    lv_obj_set_align(app_icon_bg_1, LV_ALIGN_TOP_MID);
    lv_obj_set_flag(app_icon_bg_1, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(app_icon_bg_1, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_icon_bg_1, &style_app_icon_bg, 0);
    lv_obj_t * app_icon_1 = lv_image_create(app_icon_bg_1);
    lv_obj_set_name(app_icon_1, "app_icon_1");
    lv_obj_set_align(app_icon_1, LV_ALIGN_CENTER);
    lv_obj_set_flag(app_icon_1, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_add_style(app_icon_1, &style_app_icon_img, 0);
    
    lv_obj_t * app_label_1 = lv_label_create(app_slot_1);
    lv_obj_set_name(app_label_1, "app_label_1");
    lv_obj_set_align(app_label_1, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(app_label_1, 2);
    lv_label_set_text(app_label_1, "App 1");
    lv_obj_set_flag(app_label_1, LV_OBJ_FLAG_CLICKABLE, false);
    lv_label_set_long_mode(app_label_1, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_add_style(app_label_1, &style_app_label, 0);
    
    lv_obj_add_event_cb(app_slot_1, app_picker_on_app_clicked, LV_EVENT_SHORT_CLICKED, NULL);
    
    lv_obj_t * app_slot_2 = lv_obj_create(app_container);
    lv_obj_set_name(app_slot_2, "app_slot_2");
    lv_obj_set_x(app_slot_2, 152);
    lv_obj_set_y(app_slot_2, 118);
    lv_obj_set_flag(app_slot_2, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_set_flag(app_slot_2, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_slot_2, &style_app_slot, 0);
    lv_obj_t * app_icon_bg_2 = lv_obj_create(app_slot_2);
    lv_obj_set_name(app_icon_bg_2, "app_icon_bg_2");
    lv_obj_set_align(app_icon_bg_2, LV_ALIGN_TOP_MID);
    lv_obj_set_flag(app_icon_bg_2, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(app_icon_bg_2, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_icon_bg_2, &style_app_icon_bg, 0);
    lv_obj_t * app_icon_2 = lv_image_create(app_icon_bg_2);
    lv_obj_set_name(app_icon_2, "app_icon_2");
    lv_obj_set_align(app_icon_2, LV_ALIGN_CENTER);
    lv_obj_set_flag(app_icon_2, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_add_style(app_icon_2, &style_app_icon_img, 0);
    
    lv_obj_t * app_label_2 = lv_label_create(app_slot_2);
    lv_obj_set_name(app_label_2, "app_label_2");
    lv_obj_set_align(app_label_2, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(app_label_2, 2);
    lv_label_set_text(app_label_2, "App 2");
    lv_obj_set_flag(app_label_2, LV_OBJ_FLAG_CLICKABLE, false);
    lv_label_set_long_mode(app_label_2, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_add_style(app_label_2, &style_app_label, 0);
    
    lv_obj_add_event_cb(app_slot_2, app_picker_on_app_clicked, LV_EVENT_SHORT_CLICKED, NULL);
    
    lv_obj_t * app_slot_3 = lv_obj_create(app_container);
    lv_obj_set_name(app_slot_3, "app_slot_3");
    lv_obj_set_x(app_slot_3, 92);
    lv_obj_set_y(app_slot_3, 160);
    lv_obj_set_flag(app_slot_3, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_set_flag(app_slot_3, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_slot_3, &style_app_slot, 0);
    lv_obj_t * app_icon_bg_3 = lv_obj_create(app_slot_3);
    lv_obj_set_name(app_icon_bg_3, "app_icon_bg_3");
    lv_obj_set_align(app_icon_bg_3, LV_ALIGN_TOP_MID);
    lv_obj_set_flag(app_icon_bg_3, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(app_icon_bg_3, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_icon_bg_3, &style_app_icon_bg, 0);
    lv_obj_t * app_icon_3 = lv_image_create(app_icon_bg_3);
    lv_obj_set_name(app_icon_3, "app_icon_3");
    lv_obj_set_align(app_icon_3, LV_ALIGN_CENTER);
    lv_obj_set_flag(app_icon_3, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_add_style(app_icon_3, &style_app_icon_img, 0);
    
    lv_obj_t * app_label_3 = lv_label_create(app_slot_3);
    lv_obj_set_name(app_label_3, "app_label_3");
    lv_obj_set_align(app_label_3, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(app_label_3, 2);
    lv_label_set_text(app_label_3, "App 3");
    lv_obj_set_flag(app_label_3, LV_OBJ_FLAG_CLICKABLE, false);
    lv_label_set_long_mode(app_label_3, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_add_style(app_label_3, &style_app_label, 0);
    
    lv_obj_add_event_cb(app_slot_3, app_picker_on_app_clicked, LV_EVENT_SHORT_CLICKED, NULL);
    
    lv_obj_t * app_slot_4 = lv_obj_create(app_container);
    lv_obj_set_name(app_slot_4, "app_slot_4");
    lv_obj_set_x(app_slot_4, 32);
    lv_obj_set_y(app_slot_4, 118);
    lv_obj_set_flag(app_slot_4, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_set_flag(app_slot_4, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_slot_4, &style_app_slot, 0);
    lv_obj_t * app_icon_bg_4 = lv_obj_create(app_slot_4);
    lv_obj_set_name(app_icon_bg_4, "app_icon_bg_4");
    lv_obj_set_align(app_icon_bg_4, LV_ALIGN_TOP_MID);
    lv_obj_set_flag(app_icon_bg_4, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(app_icon_bg_4, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_icon_bg_4, &style_app_icon_bg, 0);
    lv_obj_t * app_icon_4 = lv_image_create(app_icon_bg_4);
    lv_obj_set_name(app_icon_4, "app_icon_4");
    lv_obj_set_align(app_icon_4, LV_ALIGN_CENTER);
    lv_obj_set_flag(app_icon_4, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_add_style(app_icon_4, &style_app_icon_img, 0);
    
    lv_obj_t * app_label_4 = lv_label_create(app_slot_4);
    lv_obj_set_name(app_label_4, "app_label_4");
    lv_obj_set_align(app_label_4, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(app_label_4, 2);
    lv_label_set_text(app_label_4, "App 4");
    lv_obj_set_flag(app_label_4, LV_OBJ_FLAG_CLICKABLE, false);
    lv_label_set_long_mode(app_label_4, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_add_style(app_label_4, &style_app_label, 0);
    
    lv_obj_add_event_cb(app_slot_4, app_picker_on_app_clicked, LV_EVENT_SHORT_CLICKED, NULL);
    
    lv_obj_t * app_slot_5 = lv_obj_create(app_container);
    lv_obj_set_name(app_slot_5, "app_slot_5");
    lv_obj_set_x(app_slot_5, 32);
    lv_obj_set_y(app_slot_5, 42);
    lv_obj_set_flag(app_slot_5, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_set_flag(app_slot_5, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_slot_5, &style_app_slot, 0);
    lv_obj_t * app_icon_bg_5 = lv_obj_create(app_slot_5);
    lv_obj_set_name(app_icon_bg_5, "app_icon_bg_5");
    lv_obj_set_align(app_icon_bg_5, LV_ALIGN_TOP_MID);
    lv_obj_set_flag(app_icon_bg_5, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(app_icon_bg_5, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_icon_bg_5, &style_app_icon_bg, 0);
    lv_obj_t * app_icon_5 = lv_image_create(app_icon_bg_5);
    lv_obj_set_name(app_icon_5, "app_icon_5");
    lv_obj_set_align(app_icon_5, LV_ALIGN_CENTER);
    lv_obj_set_flag(app_icon_5, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_add_style(app_icon_5, &style_app_icon_img, 0);
    
    lv_obj_t * app_label_5 = lv_label_create(app_slot_5);
    lv_obj_set_name(app_label_5, "app_label_5");
    lv_obj_set_align(app_label_5, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(app_label_5, 2);
    lv_label_set_text(app_label_5, "App 5");
    lv_obj_set_flag(app_label_5, LV_OBJ_FLAG_CLICKABLE, false);
    lv_label_set_long_mode(app_label_5, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_add_style(app_label_5, &style_app_label, 0);
    
    lv_obj_add_event_cb(app_slot_5, app_picker_on_app_clicked, LV_EVENT_SHORT_CLICKED, NULL);
    
    lv_obj_t * app_slot_6 = lv_obj_create(app_container);
    lv_obj_set_name(app_slot_6, "app_slot_6");
    lv_obj_set_x(app_slot_6, 92);
    lv_obj_set_y(app_slot_6, 82);
    lv_obj_set_flag(app_slot_6, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_set_flag(app_slot_6, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_slot_6, &style_app_slot, 0);
    lv_obj_t * app_icon_bg_6 = lv_obj_create(app_slot_6);
    lv_obj_set_name(app_icon_bg_6, "app_icon_bg_6");
    lv_obj_set_align(app_icon_bg_6, LV_ALIGN_TOP_MID);
    lv_obj_set_flag(app_icon_bg_6, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(app_icon_bg_6, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(app_icon_bg_6, &style_app_icon_bg, 0);
    lv_obj_t * app_icon_6 = lv_image_create(app_icon_bg_6);
    lv_obj_set_name(app_icon_6, "app_icon_6");
    lv_obj_set_align(app_icon_6, LV_ALIGN_CENTER);
    lv_obj_set_flag(app_icon_6, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_add_style(app_icon_6, &style_app_icon_img, 0);
    
    lv_obj_t * app_label_6 = lv_label_create(app_slot_6);
    lv_obj_set_name(app_label_6, "app_label_6");
    lv_obj_set_align(app_label_6, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(app_label_6, 2);
    lv_label_set_text(app_label_6, "Center");
    lv_obj_set_flag(app_label_6, LV_OBJ_FLAG_CLICKABLE, false);
    lv_label_set_long_mode(app_label_6, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_add_style(app_label_6, &style_app_label, 0);
    
    lv_obj_add_event_cb(app_slot_6, app_picker_on_app_clicked, LV_EVENT_SHORT_CLICKED, NULL);
    
    lv_obj_t * nav_arrow_left = lv_button_create(lv_obj_0);
    lv_obj_set_name(nav_arrow_left, "nav_arrow_left");
    lv_obj_set_align(nav_arrow_left, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(nav_arrow_left, 0);
    lv_obj_set_y(nav_arrow_left, 0);
    lv_obj_set_flag(nav_arrow_left, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_add_style(nav_arrow_left, &style_nav_arrow, 0);
    lv_obj_t * nav_arrow_left_icon = lv_label_create(nav_arrow_left);
    lv_obj_set_name(nav_arrow_left_icon, "nav_arrow_left_icon");
    lv_label_set_text(nav_arrow_left_icon, "<");
    lv_obj_set_align(nav_arrow_left_icon, LV_ALIGN_CENTER);
    lv_obj_add_style(nav_arrow_left_icon, &style_nav_arrow_label, 0);
    
    lv_obj_add_event_cb(nav_arrow_left, app_picker_on_nav_left_clicked, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * nav_arrow_right = lv_button_create(lv_obj_0);
    lv_obj_set_name(nav_arrow_right, "nav_arrow_right");
    lv_obj_set_align(nav_arrow_right, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(nav_arrow_right, 0);
    lv_obj_set_y(nav_arrow_right, 0);
    lv_obj_set_flag(nav_arrow_right, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_add_style(nav_arrow_right, &style_nav_arrow, 0);
    lv_obj_t * nav_arrow_right_icon = lv_label_create(nav_arrow_right);
    lv_obj_set_name(nav_arrow_right_icon, "nav_arrow_right_icon");
    lv_label_set_text(nav_arrow_right_icon, ">");
    lv_obj_set_align(nav_arrow_right_icon, LV_ALIGN_CENTER);
    lv_obj_add_style(nav_arrow_right_icon, &style_nav_arrow_label, 0);
    
    lv_obj_add_event_cb(nav_arrow_right, app_picker_on_nav_right_clicked, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * folder_overlay = lv_obj_create(lv_obj_0);
    lv_obj_set_name(folder_overlay, "folder_overlay");
    lv_obj_set_flag(folder_overlay, LV_OBJ_FLAG_HIDDEN, true);
    lv_obj_set_flag(folder_overlay, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(folder_overlay, &style_folder_overlay, 0);
    lv_obj_t * folder_title = lv_label_create(folder_overlay);
    lv_obj_set_name(folder_title, "folder_title");
    lv_obj_set_align(folder_title, LV_ALIGN_TOP_MID);
    lv_obj_set_y(folder_title, 20);
    lv_label_bind_text(folder_title, &app_picker_folder_title, NULL);
    lv_obj_add_style(folder_title, &style_folder_title, 0);
    
    lv_obj_t * folder_grid = lv_obj_create(folder_overlay);
    lv_obj_set_name(folder_grid, "folder_grid");
    lv_obj_set_align(folder_grid, LV_ALIGN_CENTER);
    lv_obj_set_y(folder_grid, -10);
    lv_obj_set_flag(folder_grid, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(folder_grid, &style_folder_grid, 0);
    
    lv_obj_t * folder_close_btn = lv_button_create(folder_overlay);
    lv_obj_set_name(folder_close_btn, "folder_close_btn");
    lv_obj_set_align(folder_close_btn, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(folder_close_btn, -15);
    lv_obj_set_flag(folder_close_btn, LV_OBJ_FLAG_CLICKABLE, true);
    lv_obj_add_style(folder_close_btn, &style_folder_close, 0);
    lv_obj_t * folder_close_icon = lv_image_create(folder_close_btn);
    lv_obj_set_name(folder_close_icon, "folder_close_icon");
    lv_obj_set_align(folder_close_icon, LV_ALIGN_CENTER);
    
    lv_obj_add_event_cb(folder_close_btn, app_picker_on_folder_close_clicked, LV_EVENT_CLICKED, NULL);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

