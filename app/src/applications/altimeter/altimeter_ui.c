#include "altimeter_ui.h"
#include "ui/zsw_ui.h" // <-- Change this line!
#include <stdio.h>

static lv_obj_t *root_page = NULL;
static lv_obj_t *alt_label = NULL;
static lv_obj_t *pressure_label = NULL;
static lv_obj_t *status_label = NULL;

void altimeter_ui_show(lv_obj_t *root)
{
    // Create the full-screen container
    root_page = lv_obj_create(root);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(root_page, zsw_color_bg(), LV_PART_MAIN); // System background color
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    
    // Altitude Number (Large Font)
    alt_label = lv_label_create(root_page);
    lv_obj_set_style_text_font(alt_label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_label_set_text(alt_label, "-- m");
    lv_obj_align(alt_label, LV_ALIGN_CENTER, 0, -10);

    // Raw Pressure text (Small, Gray)
    pressure_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(pressure_label, zsw_color_gray(), LV_PART_MAIN);
    lv_label_set_text(pressure_label, "-- hPa");
    lv_obj_align(pressure_label, LV_ALIGN_CENTER, 0, 25);

    // Network Calibration Status
    status_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(status_label, zsw_color_red(), LV_PART_MAIN);
    lv_label_set_text(status_label, "Uncalibrated");
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -25);
}

void altimeter_ui_remove(void)
{
    if (root_page != NULL) {
        lv_obj_del(root_page); // Deletes all children (labels) automatically
        root_page = NULL;
    }
}

void altimeter_ui_update(float altitude, float raw_pressure, bool is_calibrated)
{
    if (root_page == NULL) return; // Prevent crashes if screen is off

    lv_label_set_text_fmt(alt_label, "%.0f m", altitude);
    lv_label_set_text_fmt(pressure_label, "%.1f hPa", raw_pressure);

    if (is_calibrated) {
        lv_label_set_text(status_label, "Calibrated");
        // Update to system blue or generic green to show success
        lv_obj_set_style_text_color(status_label, zsw_color_blue(), LV_PART_MAIN); 
    }
}