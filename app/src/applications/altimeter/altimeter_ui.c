#include "altimeter_ui.h"
#include "ui/zsw_ui.h" // <-- Change this line!
#include <stdio.h>

static lv_obj_t *root_page = NULL;
static lv_obj_t *alt_label = NULL;
static lv_obj_t *pressure_label = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *chart = NULL;
static lv_chart_series_t *chart_series = NULL;

void altimeter_ui_show(lv_obj_t *root)
{
    root_page = lv_obj_create(root);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(root_page, zsw_color_bg(), LV_PART_MAIN); 
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    
    // Altitude Number (Large Font)
    alt_label = lv_label_create(root_page);
    lv_obj_set_style_text_font(alt_label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_label_set_text(alt_label, "-- m");
    lv_obj_align(alt_label, LV_ALIGN_TOP_MID, 0, 20); // Moved to very TOP

    // Raw Pressure text (Small, Gray)
    pressure_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(pressure_label, zsw_color_gray(), LV_PART_MAIN);
    lv_label_set_text(pressure_label, "-- hPa");
    lv_obj_align(pressure_label, LV_ALIGN_TOP_MID, 0, 55); // Tucked right under altitude

    // Network Calibration Status
    status_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(status_label, zsw_color_red(), LV_PART_MAIN);
    lv_label_set_text(status_label, "Uncalibrated");
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -15); // Pushed to BOTTOM

    // History Chart (Center bounds)
    chart = lv_chart_create(root_page);
    lv_obj_set_size(chart, 180, 90);
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 15); // Placed securely in the CENTER
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_obj_set_style_line_width(chart, 3, LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(chart, 0, LV_PART_MAIN); 
    lv_chart_set_point_count(chart, 24); 
    lv_chart_set_div_line_count(chart, 0, 0);       
    lv_obj_set_style_line_color(chart, zsw_color_red(), LV_PART_ITEMS);
    chart_series = lv_chart_add_series(chart, zsw_color_blue(), LV_CHART_AXIS_PRIMARY_Y);
}

void altimeter_ui_remove(void)
{
    if (root_page != NULL) {
        lv_obj_del(root_page); // Deletes all children (labels) automatically
        root_page = NULL;
    }
}

void altimeter_ui_update(float altitude, float raw_pressure, bool is_calibrated, float *history_data, uint8_t start_idx, bool has_data)
{
    if (root_page == NULL) return; 

    lv_label_set_text_fmt(alt_label, "%.0f m", altitude);
    lv_label_set_text_fmt(pressure_label, "%.1f hPa", raw_pressure);

    if (is_calibrated) {
        lv_label_set_text(status_label, "Calibrated");
        lv_obj_set_style_text_color(status_label, zsw_color_blue(), LV_PART_MAIN); 
    }

    // Update Chart
    if (has_data && chart != NULL && chart_series != NULL) {
        float min = 99999.0;
        float max = -99999.0;
        
        for (int i = 0; i < 24; i++) {
            if (history_data[i] == 0.0) continue; 
            if (history_data[i] < min) min = history_data[i];
            if (history_data[i] > max) max = history_data[i];
        }
        
        // Give chart tightly zoomed +/- 5 meters breathing room 
        if (min < 99999.0 && max > -99999.0) {
            lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, (int)min - 5, (int)max + 5);
        }

        // Feed chronologically
        for (int i = 0; i < 24; i++) {
            int actual_idx = (start_idx + i) % 24; 
            if (history_data[actual_idx] == 0.0) {
                // Skips drawing a huge plummeting line for empty array slots
                lv_chart_set_next_value(chart, chart_series, LV_CHART_POINT_NONE); 
            } else {
                lv_chart_set_next_value(chart, chart_series, (int)history_data[actual_idx]);
            }
        }
        lv_chart_refresh(chart);
    }
}