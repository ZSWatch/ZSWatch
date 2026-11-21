#include "altimeter_ui.h"
#include "ui/zsw_ui.h"

#include <math.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(altimeter_ui, CONFIG_ZSW_APP_LOG_LEVEL);

#define RUNOUT_DURATION_S 10

static lv_obj_t *root_page;
static lv_obj_t *overview;
static lv_obj_t *history_page;
static lv_obj_t *history_button;
static lv_obj_t *alt_label;
static lv_obj_t *pressure_label;
static lv_obj_t *status_label;
static lv_obj_t *floors_up_label;
static lv_obj_t *floors_down_label;
static lv_obj_t *chart;
static lv_obj_t *history_range_label;
static lv_obj_t *history_scale_label;
static lv_obj_t *history_empty_label;
static lv_chart_series_t *chart_series;
static lv_obj_t *runout_arc;
static lv_timer_t *runout_timer;
static lv_group_t *input_group;
static void (*request_refresh)(void);
static int runout_seconds_left;

static const lv_point_precise_t stairs_points[] = {
    {0, 15}, {5, 15}, {5, 10}, {10, 10}, {10, 5}, {15, 5}, {15, 0}, {20, 0},
};

static lv_obj_t *make_label(lv_obj_t *parent, const char *text, const lv_font_t *font,
                            lv_color_t color, lv_align_t align, int x, int y)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_align(label, align, x, y);
    return label;
}

static lv_obj_t *make_page(lv_obj_t *parent)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_set_size(page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(page, zsw_color_bg(), 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(page, 0, 0);
    lv_obj_set_style_pad_all(page, 0, 0);
    lv_obj_set_style_radius(page, 0, 0);
    lv_obj_remove_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    return page;
}

static void show_overview(void)
{
    if (history_page == NULL) {
        return;
    }

    lv_obj_delete(history_page);
    history_page = NULL;
    chart = NULL;
    chart_series = NULL;
    history_range_label = NULL;
    history_scale_label = NULL;
    history_empty_label = NULL;
    lv_obj_remove_flag(overview, LV_OBJ_FLAG_HIDDEN);
    if (input_group != NULL) {
        lv_group_focus_obj(history_button);
    }
    LOG_INF("Altimeter overview shown");
}

bool altimeter_ui_back(void)
{
    if (history_page == NULL) {
        return false;
    }
    show_overview();
    return true;
}

static void back_button_cb(lv_event_t *event)
{
    LV_UNUSED(event);
    show_overview();
}

static lv_obj_t *make_button(lv_obj_t *parent, const char *text, lv_event_cb_t callback)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, 112, 30);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x182330), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(button, 15, 0);
    lv_obj_set_style_pad_all(button, 0, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_align(button, LV_ALIGN_BOTTOM_MID, 0, -7);
    lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, NULL);
    if (input_group != NULL) {
        lv_group_add_obj(input_group, button);
    }
    make_label(button, text, &lv_font_montserrat_14, lv_color_white(), LV_ALIGN_CENTER, 0, 0);
    return button;
}

static void history_button_cb(lv_event_t *event)
{
    LV_UNUSED(event);

    if (history_page != NULL) {
        return;
    }

    lv_obj_add_flag(overview, LV_OBJ_FLAG_HIDDEN);
    history_page = make_page(root_page);
    make_label(history_page, "ALTITUDE HISTORY", &lv_font_montserrat_14,
               zsw_color_blue(), LV_ALIGN_TOP_MID, 0, 28);
    history_range_label = make_label(history_page, "", &lv_font_montserrat_12,
                                     lv_color_hex(0xCBD5E1), LV_ALIGN_TOP_MID, 0, 50);

    chart = lv_chart_create(history_page);
    lv_obj_set_size(chart, 160, 90);
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, HISTORY_SAMPLES);
    lv_chart_set_div_line_count(chart, 2, 0);
    lv_obj_set_style_bg_color(chart, lv_color_hex(0x182330), LV_PART_MAIN);
    lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(chart, 10, LV_PART_MAIN);
    lv_obj_set_style_line_width(chart, 3, LV_PART_ITEMS);
    lv_obj_remove_flag(chart, LV_OBJ_FLAG_CLICKABLE);
    chart_series = lv_chart_add_series(chart, zsw_color_blue(), LV_CHART_AXIS_PRIMARY_Y);
    history_empty_label = make_label(history_page, "Waiting for history", &lv_font_montserrat_14,
                                     lv_color_white(), LV_ALIGN_CENTER, 0, 0);
    history_scale_label = make_label(history_page, "", &lv_font_montserrat_12,
                                     lv_color_hex(0xCBD5E1), LV_ALIGN_TOP_MID, 0, 170);
    make_button(history_page, LV_SYMBOL_LEFT " Overview", back_button_cb);

    LOG_INF("Altimeter history shown");
    if (request_refresh != NULL) {
        request_refresh();
    }
}

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

void altimeter_ui_show(lv_obj_t *root, lv_group_t *group, void (*refresh_cb)(void))
{
    input_group = group;
    request_refresh = refresh_cb;
    root_page = make_page(root);
    overview = make_page(root_page);

    make_label(overview, "ALTITUDE", &lv_font_montserrat_14,
               zsw_color_blue(), LV_ALIGN_TOP_MID, 0, 25);
    alt_label = make_label(overview, "-- m", &lv_font_montserrat_28,
                           lv_color_white(), LV_ALIGN_TOP_MID, 0, 46);

    lv_obj_t *panel = lv_obj_create(overview);
    lv_obj_set_size(panel, 168, 62);
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 99);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x182330), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_radius(panel, 18, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *stairs = lv_line_create(panel);
    lv_line_set_points(stairs, stairs_points, sizeof(stairs_points) / sizeof(stairs_points[0]));
    lv_obj_set_style_line_color(stairs, lv_color_white(), 0);
    lv_obj_set_style_line_width(stairs, 2, 0);
    lv_obj_align(stairs, LV_ALIGN_TOP_MID, 0, 5);
    floors_up_label = make_label(panel, LV_SYMBOL_UP " 0", &lv_font_montserrat_12,
                                 lv_color_white(), LV_ALIGN_TOP_LEFT, 8, 24);
    floors_down_label = make_label(panel, LV_SYMBOL_DOWN " 0", &lv_font_montserrat_12,
                                   lv_color_white(), LV_ALIGN_TOP_RIGHT, -8, 24);
    make_label(panel, "FLOORS TODAY", &lv_font_montserrat_12,
               lv_color_hex(0xCBD5E1), LV_ALIGN_BOTTOM_MID, 0, -7);

    pressure_label = make_label(overview, "-- hPa", &lv_font_montserrat_14,
                                lv_color_hex(0xCBD5E1), LV_ALIGN_TOP_MID, 0, 164);
    status_label = make_label(overview, "Waiting for sensor", &lv_font_montserrat_12,
                              zsw_color_red(), LV_ALIGN_TOP_MID, 0, 182);
    history_button = make_button(overview, "History " LV_SYMBOL_RIGHT, history_button_cb);

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
    lv_obj_remove_flag(runout_arc, LV_OBJ_FLAG_CLICKABLE);

    runout_seconds_left = RUNOUT_DURATION_S;
    runout_timer = lv_timer_create(runout_timer_cb, 1000, NULL);
}

void altimeter_ui_remove(void)
{
    if (runout_timer != NULL) {
        lv_timer_delete(runout_timer);
        runout_timer = NULL;
    }
    if (root_page != NULL) {
        lv_obj_delete(root_page);
    }
    root_page = NULL;
    overview = NULL;
    history_page = NULL;
    history_button = NULL;
    chart = NULL;
    chart_series = NULL;
    history_range_label = NULL;
    history_scale_label = NULL;
    history_empty_label = NULL;
    runout_arc = NULL;
    input_group = NULL;
    request_refresh = NULL;
}

void altimeter_ui_notify_data_ready(void)
{
    if (runout_timer != NULL) {
        lv_timer_delete(runout_timer);
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

    if (data->has_data) {
        lv_label_set_text_fmt(alt_label, "%.0f m", (double)data->altitude);
        lv_label_set_text_fmt(pressure_label, "%.1f hPa", (double)data->raw_pressure);
        lv_label_set_text(status_label, data->is_calibrated ? "Calibrated" : "Uncalibrated");
        lv_obj_set_style_text_color(status_label, data->is_calibrated ? zsw_color_blue() : zsw_color_red(), 0);
    } else {
        lv_label_set_text(alt_label, "-- m");
        lv_label_set_text(pressure_label, "-- hPa");
        lv_label_set_text(status_label, "Waiting for sensor");
        lv_obj_set_style_text_color(status_label, zsw_color_red(), 0);
    }
    lv_label_set_text_fmt(floors_up_label, LV_SYMBOL_UP " %u", data->floors_up);
    lv_label_set_text_fmt(floors_down_label, LV_SYMBOL_DOWN " %u", data->floors_down);

    if (chart == NULL || chart_series == NULL) {
        return;
    }

    if (!data->has_data || data->valid_count < 2) {
        lv_obj_add_flag(chart, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(history_empty_label, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(history_range_label, "");
        lv_label_set_text(history_scale_label, "");
        return;
    }

    if (lv_obj_has_flag(chart, LV_OBJ_FLAG_HIDDEN)) {
        LOG_INF("Altimeter history has samples");
        lv_obj_remove_flag(chart, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_add_flag(history_empty_label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text_fmt(history_range_label, "%u samples / %u s interval",
                          data->valid_count, ALTIMETER_CHART_INTERVAL_S);
    float min = data->history_data[(data->start_idx + HISTORY_SAMPLES - data->valid_count) % HISTORY_SAMPLES];
    float max = min;
    uint8_t first_valid = (data->start_idx + HISTORY_SAMPLES - data->valid_count) % HISTORY_SAMPLES;
    for (int i = 0; i < data->valid_count; i++) {
        float sample = data->history_data[(first_valid + i) % HISTORY_SAMPLES];
        min = MIN(min, sample);
        max = MAX(max, sample);
    }
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, (int)floorf(min) - 5, (int)ceilf(max) + 5);
    lv_label_set_text_fmt(history_scale_label, "%.0f to %.0f m",
                          (double)(floorf(min) - 5), (double)(ceilf(max) + 5));
    for (int i = 0; i < HISTORY_SAMPLES; i++) {
        if (i < HISTORY_SAMPLES - data->valid_count) {
            lv_chart_set_next_value(chart, chart_series, LV_CHART_POINT_NONE);
        } else {
            int idx = (first_valid + i - (HISTORY_SAMPLES - data->valid_count)) % HISTORY_SAMPLES;
            lv_chart_set_next_value(chart, chart_series, (int)data->history_data[idx]);
        }
    }
    lv_chart_refresh(chart);
}
