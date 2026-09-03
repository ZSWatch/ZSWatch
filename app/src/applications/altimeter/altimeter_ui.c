#include "altimeter_ui.h"
#include "ui/zsw_ui.h" 
#include <stdio.h>

static lv_obj_t *root_page = NULL;
static lv_obj_t *alt_label = NULL;
static lv_obj_t *pressure_label = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *chart = NULL;
static lv_chart_series_t *chart_series = NULL;
static lv_obj_t *floors_up_label = NULL;
static lv_obj_t *floors_down_label = NULL;

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

    // Floors climbed/descended today, flanking the pressure label
    floors_up_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(floors_up_label, zsw_color_gray(), LV_PART_MAIN);
    lv_label_set_text(floors_up_label, LV_SYMBOL_UP " 0");
    lv_obj_align(floors_up_label, LV_ALIGN_TOP_LEFT, 15, 58);

    floors_down_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(floors_down_label, zsw_color_gray(), LV_PART_MAIN);
    lv_label_set_text(floors_down_label, LV_SYMBOL_DOWN " 0");
    lv_obj_align(floors_down_label, LV_ALIGN_TOP_RIGHT, -15, 58);

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
        floors_up_label = NULL;
        floors_down_label = NULL;
    }
}

void altimeter_ui_update(altimeter_ui_data_t *data)
{
    if (root_page == NULL || data == NULL) {
        return;
    }

    lv_label_set_text_fmt(alt_label, "%.2f m", data->altitude);
    lv_label_set_text_fmt(pressure_label, "%.2f hPa", data->raw_pressure);
    lv_label_set_text_fmt(floors_up_label, LV_SYMBOL_UP " %u", data->floors_up);
    lv_label_set_text_fmt(floors_down_label, LV_SYMBOL_DOWN " %u", data->floors_down);

    if (data->is_calibrated) {
        lv_label_set_text(status_label, "Calibrated");
        lv_obj_set_style_text_color(status_label, zsw_color_blue(), LV_PART_MAIN);
    }

    // Update Chart
    if (data->has_data && chart != NULL && chart_series != NULL) {
        float min = 99999.0;
        float max = -99999.0;

        for (int i = 0; i < HISTORY_SAMPLES; i++) {
            if (data->history_data[i] == 0.0) {
                continue;
            }
            if (data->history_data[i] < min) {
                min = data->history_data[i];
            }
            if (data->history_data[i] > max) {
                max = data->history_data[i];
            }
        }

        // Give chart tightly zoomed +/- 5 meters breathing room
        if (min < 99999.0 && max > -99999.0) {
            lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, (int)min - 5, (int)max + 5);
        }

        // Feed chronologically
        for (int i = 0; i < HISTORY_SAMPLES; i++) {
            int actual_idx = (data->start_idx + i) % HISTORY_SAMPLES;
            if (data->history_data[actual_idx] == 0.0) {
                // Skips drawing a huge plummeting line for empty array slots
                lv_chart_set_next_value(chart, chart_series, LV_CHART_POINT_NONE);
            } else {
                lv_chart_set_next_value(chart, chart_series, (int)data->history_data[actual_idx]);
            }
        }
        lv_chart_refresh(chart);
    }
}