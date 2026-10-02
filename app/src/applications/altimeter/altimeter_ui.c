#include "altimeter_ui.h"
#include "ui/zsw_ui.h"

#define RUNOUT_DURATION_S 10

static lv_obj_t *root_page = NULL;
static lv_obj_t *alt_label = NULL;
static lv_obj_t *pressure_label = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *chart = NULL;
static lv_chart_series_t *chart_series = NULL;
static lv_obj_t *floors_title_label = NULL;
static lv_obj_t *floors_up_label = NULL;
static lv_obj_t *floors_down_label = NULL;
static lv_obj_t *runout_arc = NULL;
static lv_timer_t *runout_timer = NULL;
static int runout_seconds_left;

static void runout_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    runout_seconds_left--;
    if (runout_seconds_left <= 0) {
        altimeter_ui_notify_data_ready();
        return;
    }
    lv_arc_set_value(runout_arc, (runout_seconds_left * 100) / RUNOUT_DURATION_S);
}

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
    lv_obj_align(alt_label, LV_ALIGN_TOP_MID, 0, 15);

    // Floors title, above the up/down counts
    floors_title_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(floors_title_label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(floors_title_label, "Floors");
    lv_obj_align(floors_title_label, LV_ALIGN_TOP_MID, 0, 66);

    // Floors climbed/descended today
    floors_up_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(floors_up_label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(floors_up_label, LV_SYMBOL_UP " 0");
    lv_obj_align(floors_up_label, LV_ALIGN_TOP_LEFT, 20, 66);

    floors_down_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(floors_down_label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(floors_down_label, LV_SYMBOL_DOWN " 0");
    lv_obj_align(floors_down_label, LV_ALIGN_TOP_RIGHT, -20, 66);

    // Raw Pressure text (Small, Gray), tucked above the calibration status
    pressure_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(pressure_label, zsw_color_gray(), LV_PART_MAIN);
    lv_label_set_text(pressure_label, "-- hPa");
    lv_obj_align(pressure_label, LV_ALIGN_BOTTOM_MID, 0, -35);

    // Network Calibration Status
    status_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(status_label, zsw_color_red(), LV_PART_MAIN);
    lv_label_set_text(status_label, "Uncalibrated");
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -15); // Pushed to BOTTOM

    // History Chart (Center bounds)
    chart = lv_chart_create(root_page);
    lv_obj_set_size(chart, 180, 70);
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 15);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_obj_set_style_line_width(chart, 3, LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(chart, 0, LV_PART_MAIN);
    lv_chart_set_point_count(chart, 24);
    lv_chart_set_div_line_count(chart, 0, 0);
    lv_obj_set_style_line_color(chart, zsw_color_red(), LV_PART_ITEMS);
    chart_series = lv_chart_add_series(chart, zsw_color_blue(), LV_CHART_AXIS_PRIMARY_Y);

    // Runout ring: shrinks over RUNOUT_DURATION_S while waiting for the first sample/calibration
    runout_arc = lv_arc_create(root_page);
    lv_obj_set_size(runout_arc, LV_PCT(100), LV_PCT(100));
    lv_obj_center(runout_arc);
    lv_arc_set_rotation(runout_arc, 270);
    lv_arc_set_bg_angles(runout_arc, 0, 360);
    lv_arc_set_range(runout_arc, 0, 100);
    lv_arc_set_value(runout_arc, 100);
    lv_obj_set_style_arc_width(runout_arc, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_color(runout_arc, zsw_color_gray(), LV_PART_MAIN);
    lv_obj_set_style_arc_width(runout_arc, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(runout_arc, zsw_color_blue(), LV_PART_INDICATOR);
    lv_obj_remove_style(runout_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(runout_arc, LV_OBJ_FLAG_CLICKABLE);

    runout_seconds_left = RUNOUT_DURATION_S;
    runout_timer = lv_timer_create(runout_timer_cb, 1000, NULL);
}

void altimeter_ui_remove(void)
{
    if (root_page != NULL) {
        if (runout_timer != NULL) {
            lv_timer_del(runout_timer);
            runout_timer = NULL;
        }
        lv_obj_del(root_page); // Deletes all children (labels) automatically
        root_page = NULL;
        floors_title_label = NULL;
        floors_up_label = NULL;
        floors_down_label = NULL;
        runout_arc = NULL;
    }
}

void altimeter_ui_notify_data_ready(void)
{
    if (runout_timer != NULL) {
        lv_timer_del(runout_timer);
        runout_timer = NULL;
    }
    if (runout_arc != NULL) {
        lv_obj_add_flag(runout_arc, LV_OBJ_FLAG_HIDDEN);
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
    if (data->has_data && data->valid_count > 0 && chart != NULL && chart_series != NULL) {
        float min = 99999.0;
        float max = -99999.0;
        // Valid samples occupy the valid_count slots immediately preceding start_idx
        uint8_t first_valid = (data->start_idx + HISTORY_SAMPLES - data->valid_count) % HISTORY_SAMPLES;

        for (int i = 0; i < data->valid_count; i++) {
            int idx = (first_valid + i) % HISTORY_SAMPLES;
            if (data->history_data[idx] < min) {
                min = data->history_data[idx];
            }
            if (data->history_data[idx] > max) {
                max = data->history_data[idx];
            }
        }

        // Give chart tightly zoomed +/- 5 meters breathing room
        if (min < 99999.0 && max > -99999.0) {
            lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, (int)min - 5, (int)max + 5);
        }

        // Feed chronologically, oldest valid sample first
        for (int i = 0; i < HISTORY_SAMPLES; i++) {
            if (i < HISTORY_SAMPLES - data->valid_count) {
                // Slot not yet written since last calibration/reset
                lv_chart_set_next_value(chart, chart_series, LV_CHART_POINT_NONE);
            } else {
                int idx = (first_valid + i - (HISTORY_SAMPLES - data->valid_count)) % HISTORY_SAMPLES;
                lv_chart_set_next_value(chart, chart_series, (int)data->history_data[idx]);
            }
        }
        lv_chart_refresh(chart);
    }
}