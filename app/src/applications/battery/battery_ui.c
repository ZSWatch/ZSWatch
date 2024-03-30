#include <battery/battery_ui.h>
#include <lvgl.h>

static lv_obj_t *root_page = NULL;

static lv_obj_t *chart;
static lv_chart_series_t *ser;
static lv_obj_t *last_sample_label;

void battery_ui_show(lv_obj_t *root, int max_samples)
{
    assert(root_page == NULL);

    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    // Don't want it to be scollable. Putting anything close the edges
    // then LVGL automatically makes the page scrollable and shows a scroll bar.
    // Does not loog very good on the round display.
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    chart = lv_chart_create(root_page);
    lv_obj_set_size(chart, 150, 150);
    lv_obj_align(chart, LV_ALIGN_CENTER, 20, 0);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 4, 5, 6, 5, true, 40);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 3600, 4250);

    if (max_samples > 10) {
        lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
    }

    ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_point_count(chart, max_samples);
    lv_obj_t *label = lv_label_create(root_page);
    lv_label_set_text(label, "Battery hourly (mV)");
    lv_obj_align_to(label, chart, LV_ALIGN_OUT_TOP_MID, -20, -5);

    last_sample_label = lv_label_create(root_page);
    lv_label_set_text(last_sample_label, "---- mV");
    lv_obj_align_to(last_sample_label, chart, LV_ALIGN_OUT_BOTTOM_MID, -20, 10);
}

void battery_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void battery_ui_set_current_measurement(int value)
{
    lv_label_set_text_fmt(last_sample_label, "%d mV", value);
}

void battery_ui_add_measurement(int value)
{
    lv_chart_set_next_value(chart, ser, value);
}
