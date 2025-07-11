#include <battery/battery_ui.h>
#include "battery_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>
#include <assert.h>

// Common
static void create_page_indicator(lv_obj_t *container, uint8_t num_leds);
static void on_tileview_change(lv_event_t *e);
static lv_obj_t *tv;
static lv_obj_t *ui_page_indicator;
static lv_obj_t *led1;
static lv_obj_t *led2;
static lv_obj_t *led3;

static bool pmic_ui_enabled;

// SCREEN: ui_Screen1
static void ui_Screen1_screen_init(lv_obj_t *ui_Screen1);
static void chart_event(lv_event_t *e);
static void scale_event(lv_event_t *e);
static lv_obj_t *ui_root1;
static lv_obj_t *ui_Label2;
static lv_obj_t *ui_Label3;
static lv_obj_t *ui_charge_chart;
static lv_obj_t *ui_Label1;
static lv_obj_t *ui_nrf_icon;
static lv_obj_t *ui_percent_voltage_label;
static lv_obj_t *ui_charging_label;

// SCREEN: ui_Screen2
static void ui_Screen2_screen_init(lv_obj_t *ui_Screen2);
static lv_obj_t *ui_root2;
static lv_obj_t *ui_Label4;
static lv_obj_t *ui_nrf_icon1;
static lv_obj_t *ui_charging_label_state;
static lv_obj_t *ui_Container9;
static lv_obj_t *ui_Container5;
static lv_obj_t *ui_charging_label18;
static lv_obj_t *ui_charging_label10;
static lv_obj_t *ui_charging_label11;
static lv_obj_t *ui_charging_label12;
static lv_obj_t *ui_Container6;
static lv_obj_t *ui_Container7;
static lv_obj_t *ui_charging_label_ttf;
static lv_obj_t *ui_charging_label_tte;
static lv_obj_t *ui_charging_label_status;
static lv_obj_t *ui_charging_label_error;

// SCREEN: ui_Screen3
static void ui_Screen3_screen_init(lv_obj_t *ui_Screen3);
static lv_obj_t *ui_root3;
static lv_obj_t *ui_Label6;
static lv_obj_t *ui_nrf_icon2;
static lv_obj_t *ui_Container1;
static lv_obj_t *ui_Container2;
static lv_obj_t *ui_charging_label3;
static lv_obj_t *ui_charging_label2;
static lv_obj_t *ui_charging_label6;
static lv_obj_t *ui_charging_label7;
static lv_obj_t *ui_Container4;
static lv_obj_t *ui_Container3;
static lv_obj_t *ui_charging_label4;
static lv_obj_t *ui_charging_label5;
static lv_obj_t *ui_charging_label8;
static lv_obj_t *ui_charging_label9;

// Charts
static lv_chart_series_t *ui_charge_chart_series_1;
static lv_chart_series_t *ui_charge_chart_series_2;

static lv_obj_t *root_page = NULL;
static int max_battery_samples;
static on_clear_history on_clear_history_cb;

void battery_ui_show(lv_obj_t *root, on_clear_history clear_hist_cb, int max_samples, bool include_pmic_ui)
{
    assert(root_page == NULL);
    on_clear_history_cb = clear_hist_cb;
    pmic_ui_enabled = include_pmic_ui;
    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    // Don't want it to be scollable. Putting anything close the edges
    // then LVGL automatically makes the page scrollable and shows a scroll bar.
    // Does not look very good on the round display.
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);

    if (include_pmic_ui) {
        // nPM1300 Nordic Style
        lv_obj_set_style_bg_color(root_page, lv_color_hex(0x94D1E3), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    max_battery_samples = max_samples;

    tv = lv_tileview_create(root_page);
    lv_obj_set_style_pad_all(tv, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(tv, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(tv, LV_OPA_TRANSP, 0);
    // Remove scroolbar on tv
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    ui_Screen1_screen_init(lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR));
    if (include_pmic_ui) {
        ui_Screen2_screen_init(lv_tileview_add_tile(tv, 1, 0, LV_DIR_HOR));
        ui_Screen3_screen_init(lv_tileview_add_tile(tv, 2, 0, LV_DIR_HOR));
        create_page_indicator(root_page, 3);
        // Add callback to tileview when new page changed, call set_indicator_page
        lv_obj_add_event_cb(tv, on_tileview_change, LV_EVENT_VALUE_CHANGED, NULL);
    }
}

void battery_ui_remove(void)
{
    assert(root_page != NULL);
    lv_obj_del(root_page);
    root_page = NULL;
}

void battery_ui_add_measurement(int percent, int voltage)
{
    if (root_page) {
        lv_chart_set_next_value(ui_charge_chart, ui_charge_chart_series_1, percent);
        lv_chart_set_next_value(ui_charge_chart, ui_charge_chart_series_2, voltage);
        lv_label_set_text_fmt(ui_percent_voltage_label, "%d%% / %.2fV", percent, voltage / 1000.0);
    }
}

void battery_ui_update(int ttf, int tte, int status, int error, int charging)
{
    int days, hours, minutes;

    if (root_page && pmic_ui_enabled) {
        zsw_ui_utils_seconds_to_day_hour_min(ttf, &days, &hours, &minutes);
        if (ttf == 0) {
            lv_label_set_text(ui_charging_label_ttf, "-");
        } else {
            lv_label_set_text_fmt(ui_charging_label_ttf, "%dd %dh %dm", days, hours, minutes);
        }
        zsw_ui_utils_seconds_to_day_hour_min(tte, &days, &hours, &minutes);
        if (tte == 0) {
            lv_label_set_text(ui_charging_label_tte, "-");
        } else {
            lv_label_set_text_fmt(ui_charging_label_tte, "%dd %dh %dm", days, hours, minutes);
        }

        lv_label_set_text_fmt(ui_charging_label_status, "%d", status);
        lv_label_set_text_fmt(ui_charging_label_error, "%d", error);
        lv_label_set_text(ui_charging_label_state, charging ? "Charging" : "-");
        lv_label_set_text_fmt(ui_charging_label, "Chg: %d", charging);
    }
}

static void create_page_indicator(lv_obj_t *container, uint8_t num_leds)
{
    ui_page_indicator = lv_obj_create(container);
    lv_obj_set_width(ui_page_indicator, 100);
    lv_obj_set_height(ui_page_indicator, 10);

    lv_obj_align(ui_page_indicator, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_clear_flag(ui_page_indicator, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_page_indicator, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_page_indicator, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_page_indicator, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_page_indicator, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    led1  = lv_led_create(ui_page_indicator);
    lv_obj_align(led1, LV_ALIGN_CENTER, -10, 0);
    lv_obj_set_size(led1, 7, 7);
    lv_led_off(led1);

    led2  = lv_led_create(ui_page_indicator);
    lv_obj_align(led2, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(led2, 7, 7);
    lv_led_off(led2);

    led3  = lv_led_create(ui_page_indicator);
    lv_obj_align(led3, LV_ALIGN_CENTER, 10, 0);
    lv_obj_set_size(led3, 7, 7);
    lv_led_off(led3);
}

static void set_indicator_page(int page)
{
    lv_color_t on_color = lv_color_hex(0xE6898B);
    lv_color_t off_color = lv_color_hex(0xFFFFFF);

    switch (page) {
        case 0:
            lv_led_set_color(led1, on_color);
            lv_led_set_color(led2, off_color);
            lv_led_set_color(led3, off_color);
            break;
        case 1:
            lv_led_set_color(led1, off_color);
            lv_led_set_color(led2, on_color);
            lv_led_set_color(led3, off_color);
            break;
        case 2:
            lv_led_set_color(led1, off_color);
            lv_led_set_color(led2, off_color);
            lv_led_set_color(led3, on_color);
            break;
        default:
            break;
    }
}

void ui_Screen1_screen_init(lv_obj_t *ui_Screen1)
{
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_width(ui_Screen1, 0, LV_PART_MAIN);

    ui_root1 = lv_obj_create(ui_Screen1);
    lv_obj_set_style_border_width(ui_root1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_root1, lv_pct(100));
    lv_obj_set_height(ui_root1, lv_pct(100));
    lv_obj_set_align(ui_root1, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    if (pmic_ui_enabled) {
        lv_obj_set_style_bg_color(ui_root1, lv_color_hex(0x94D1E3), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui_root1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        lv_obj_set_style_bg_opa(ui_root1, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    ui_Label2 = lv_label_create(ui_root1);
    lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label2, LV_ALIGN_TOP_MID);
    lv_obj_set_y(ui_Label2, 2);
    if (pmic_ui_enabled) {
        lv_obj_set_x(ui_Label2, 20);
        lv_label_set_text(ui_Label2, "nPM1300");
    } else {
        lv_label_set_text_fmt(ui_Label2, "Battery (%d min)", BATTERY_APP_SAMPLE_INTERVAL_MIN);
    }
    lv_obj_set_style_text_color(ui_Label2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label3 = lv_label_create(ui_root1);
    lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label3, -5);
    lv_obj_set_y(ui_Label3, 0);
    lv_obj_set_align(ui_Label3, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_Label3, "%");
    lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0x746DEC), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charge_chart = lv_chart_create(ui_root1);
    lv_obj_set_width(ui_charge_chart, 172);
    lv_obj_set_height(ui_charge_chart, 146);
    lv_obj_set_x(ui_charge_chart, -8);
    lv_obj_set_y(ui_charge_chart, 0);
    lv_obj_set_align(ui_charge_chart, LV_ALIGN_CENTER);
    lv_chart_set_type(ui_charge_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(ui_charge_chart, max_battery_samples);
    lv_chart_set_range(ui_charge_chart, LV_CHART_AXIS_SECONDARY_Y, 3000, 4300);
    lv_chart_set_div_line_count(ui_charge_chart, 5, 0);

    // Create a scale for the secondary Y axis (voltage)
    lv_obj_t *scale_y2 = lv_scale_create(ui_root1);
    lv_scale_set_mode(scale_y2, LV_SCALE_MODE_VERTICAL_RIGHT);
    lv_scale_set_range(scale_y2, 3000, 4300);
    lv_scale_set_total_tick_count(scale_y2, 5);
    lv_scale_set_major_tick_every(scale_y2, 1);
    lv_obj_set_style_line_width(scale_y2, 0, LV_PART_INDICATOR);
    lv_obj_set_style_line_width(scale_y2, 0, LV_PART_MAIN);
    lv_scale_set_label_show(scale_y2, true);
    lv_obj_set_height(scale_y2, 146);
    lv_obj_align_to(scale_y2, ui_charge_chart, LV_ALIGN_OUT_RIGHT_MID, 4, 0);

    // Add callback so we can format the ticks
    lv_obj_add_flag(scale_y2, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_obj_add_event_cb(scale_y2, scale_event, LV_EVENT_DRAW_TASK_ADDED, NULL);

    ui_charge_chart_series_1 = lv_chart_add_series(ui_charge_chart, lv_color_hex(0x746DEC),
                                                   LV_CHART_AXIS_PRIMARY_Y);
    ui_charge_chart_series_2 = lv_chart_add_series(ui_charge_chart, lv_color_hex(0x1EB931),
                                                   LV_CHART_AXIS_SECONDARY_Y);
    lv_obj_set_style_bg_color(ui_charge_chart, lv_color_hex(0x00A2C6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_charge_chart, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_charge_chart, lv_color_hex(0x00A2C6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_charge_chart, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_charge_chart, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui_charge_chart, lv_color_hex(0x00A2C6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_charge_chart, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(ui_charge_chart, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    if (max_battery_samples < 20) {
        lv_obj_set_style_size(ui_charge_chart, 6, 6, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    } else {
        lv_obj_set_style_size(ui_charge_chart, 0, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }

    lv_obj_set_style_line_color(ui_charge_chart, lv_color_hex(0x4040FF), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_charge_chart, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_charge_chart, lv_color_hex(0xFFFFFF), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charge_chart, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_charge_chart, chart_event, LV_EVENT_LONG_PRESSED, NULL);

    ui_Label1 = lv_label_create(ui_root1);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label1, -23);
    lv_obj_set_y(ui_Label1, -80);
    lv_obj_set_align(ui_Label1, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_Label1, "V");
    lv_obj_set_style_text_color(ui_Label1, lv_color_hex(0x1EB931), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    if (pmic_ui_enabled) {
        ui_nrf_icon = lv_img_create(ui_root1);
        lv_img_set_src(ui_nrf_icon, "S:ui_img_nrf_icon_png.bin");
        lv_obj_set_width(ui_nrf_icon, LV_SIZE_CONTENT);   /// 24
        lv_obj_set_height(ui_nrf_icon, LV_SIZE_CONTENT);    /// 21
        lv_obj_set_x(ui_nrf_icon, -30);
        lv_obj_set_y(ui_nrf_icon, 0);
        lv_obj_set_align(ui_nrf_icon, LV_ALIGN_TOP_MID);
        lv_obj_add_flag(ui_nrf_icon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
        lv_obj_clear_flag(ui_nrf_icon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    }

    ui_percent_voltage_label = lv_label_create(ui_root1);
    lv_obj_set_width(ui_percent_voltage_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_percent_voltage_label, LV_SIZE_CONTENT);    /// 1
    if (pmic_ui_enabled) {
        lv_obj_set_x(ui_percent_voltage_label, -38);
    }
    lv_obj_set_y(ui_percent_voltage_label, -10);
    lv_obj_set_align(ui_percent_voltage_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_percent_voltage_label, "68% / 4.2V ");
    lv_obj_set_style_text_color(ui_percent_voltage_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_percent_voltage_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    if (pmic_ui_enabled) {
        ui_charging_label = lv_label_create(ui_root1);
        lv_obj_set_width(ui_charging_label, LV_SIZE_CONTENT);   /// 1
        lv_obj_set_height(ui_charging_label, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_x(ui_charging_label, 38);
        lv_obj_set_y(ui_charging_label, -10);
        lv_obj_set_align(ui_charging_label, LV_ALIGN_BOTTOM_MID);
        lv_label_set_text(ui_charging_label, "-");
        lv_obj_set_style_text_color(ui_charging_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(ui_charging_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

void ui_Screen2_screen_init(lv_obj_t *ui_Screen2)
{
    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_width(ui_Screen2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_root2 = lv_obj_create(ui_Screen2);
    lv_obj_set_style_border_width(ui_root2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_root2, lv_pct(100));
    lv_obj_set_height(ui_root2, lv_pct(100));
    lv_obj_set_align(ui_root2, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_root2, lv_color_hex(0x94D1E3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_root2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label4 = lv_label_create(ui_root2);
    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label4, 20);
    lv_obj_set_y(ui_Label4, 2);
    lv_obj_set_align(ui_Label4, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Label4, "nPM1300");
    lv_obj_set_style_text_color(ui_Label4, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_nrf_icon1 = lv_img_create(ui_root2);
    lv_img_set_src(ui_nrf_icon1, "S:ui_img_nrf_icon_png.bin");
    lv_obj_set_width(ui_nrf_icon1, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_nrf_icon1, LV_SIZE_CONTENT);    /// 21
    lv_obj_set_x(ui_nrf_icon1, -30);
    lv_obj_set_y(ui_nrf_icon1, 0);
    lv_obj_set_align(ui_nrf_icon1, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_nrf_icon1, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_nrf_icon1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_charging_label_state = lv_label_create(ui_root2);
    lv_obj_set_width(ui_charging_label_state, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label_state, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_y(ui_charging_label_state, -10);
    lv_obj_set_align(ui_charging_label_state, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_charging_label_state, "N/A");
    lv_obj_set_style_text_color(ui_charging_label_state, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label_state, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Container9 = lv_obj_create(ui_root2);
    lv_obj_remove_style_all(ui_Container9);
    lv_obj_set_width(ui_Container9, lv_pct(90));
    lv_obj_set_height(ui_Container9, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_Container9, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_Container9, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_Container9, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Container9, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Container9, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Container9, lv_color_hex(0x495060), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container9, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Container9, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Container9, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Container9, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Container9, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Container5 = lv_obj_create(ui_Container9);
    lv_obj_remove_style_all(ui_Container5);
    lv_obj_set_width(ui_Container5, lv_pct(50));
    lv_obj_set_height(ui_Container5, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_Container5, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_Container5, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_Container5, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Container5, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Container5, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Container5, lv_color_hex(0xE78989), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container5, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Container5, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Container5, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Container5, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Container5, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_Container5, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_Container5, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_border_color(ui_Container5, lv_color_hex(0x000000), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Container5, 255, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

    ui_charging_label18 = lv_label_create(ui_Container5);
    lv_obj_set_width(ui_charging_label18, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label18, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label18, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label18, "Full in");
    lv_obj_set_style_text_color(ui_charging_label18, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label18, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label10 = lv_label_create(ui_Container5);
    lv_obj_set_width(ui_charging_label10, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label10, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label10, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label10, "Empty in");
    lv_obj_set_style_text_color(ui_charging_label10, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label10, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label11 = lv_label_create(ui_Container5);
    lv_obj_set_width(ui_charging_label11, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label11, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label11, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label11, "STATUS");
    lv_obj_set_style_text_color(ui_charging_label11, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label11, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label12 = lv_label_create(ui_Container5);
    lv_obj_set_width(ui_charging_label12, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label12, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label12, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label12, "ERROR");
    lv_obj_set_style_text_color(ui_charging_label12, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label12, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Container6 = lv_obj_create(ui_Container9);
    lv_obj_remove_style_all(ui_Container6);
    lv_obj_set_width(ui_Container6, 10);
    lv_obj_set_height(ui_Container6, 50);
    lv_obj_set_align(ui_Container6, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Container6, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Container7 = lv_obj_create(ui_Container9);
    lv_obj_remove_style_all(ui_Container7);
    lv_obj_set_width(ui_Container7, lv_pct(50));
    lv_obj_set_height(ui_Container7, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_Container7, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_Container7, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_Container7, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Container7, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Container7, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Container7, lv_color_hex(0xE6898B), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container7, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Container7, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Container7, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Container7, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Container7, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_Container7, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_Container7, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label_ttf = lv_label_create(ui_Container7);
    lv_obj_set_width(ui_charging_label_ttf, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label_ttf, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label_ttf, LV_ALIGN_CENTER);

    lv_obj_set_style_text_align(ui_charging_label_ttf, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(ui_charging_label_ttf, "12m");
    lv_obj_set_style_text_color(ui_charging_label_ttf, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label_ttf, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label_tte = lv_label_create(ui_Container7);
    lv_obj_set_width(ui_charging_label_tte, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label_tte, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label_tte, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(ui_charging_label_tte, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(ui_charging_label_tte, "2h");
    lv_obj_set_style_text_color(ui_charging_label_tte, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label_tte, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label_status = lv_label_create(ui_Container7);
    lv_obj_set_width(ui_charging_label_status, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label_status, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label_status, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(ui_charging_label_status, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(ui_charging_label_status, "8");
    lv_obj_set_style_text_color(ui_charging_label_status, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label_status, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label_error = lv_label_create(ui_Container7);
    lv_obj_set_width(ui_charging_label_error, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label_error, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label_error, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(ui_charging_label_error, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(ui_charging_label_error, "0");
    lv_obj_set_style_text_color(ui_charging_label_error, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label_error, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void ui_Screen3_screen_init(lv_obj_t *ui_Screen3)
{
    lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_width(ui_Screen3, 0, LV_PART_MAIN);

    ui_root3 = lv_obj_create(ui_Screen3);
    lv_obj_set_style_border_width(ui_root3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_root3, lv_pct(100));
    lv_obj_set_height(ui_root3, lv_pct(100));
    lv_obj_set_align(ui_root3, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_root3, lv_color_hex(0x94D1E3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_root3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label6 = lv_label_create(ui_root3);
    lv_obj_set_width(ui_Label6, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label6, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label6, 20);
    lv_obj_set_y(ui_Label6, 2);
    lv_obj_set_align(ui_Label6, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Label6, "nPM1300");
    lv_obj_set_style_text_color(ui_Label6, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label6, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_nrf_icon2 = lv_img_create(ui_root3);
    lv_img_set_src(ui_nrf_icon2, "S:ui_img_nrf_icon_png.bin");
    lv_obj_set_width(ui_nrf_icon2, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_nrf_icon2, LV_SIZE_CONTENT);    /// 21
    lv_obj_set_x(ui_nrf_icon2, -30);
    lv_obj_set_y(ui_nrf_icon2, 0);
    lv_obj_set_align(ui_nrf_icon2, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_nrf_icon2, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_nrf_icon2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Container1 = lv_obj_create(ui_root3);
    lv_obj_remove_style_all(ui_Container1);
    lv_obj_set_width(ui_Container1, lv_pct(80));
    lv_obj_set_height(ui_Container1, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_Container1, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_Container1, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_Container1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Container1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Container1, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Container1, lv_color_hex(0x495060), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Container1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Container1, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Container1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Container1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Container2 = lv_obj_create(ui_Container1);
    lv_obj_remove_style_all(ui_Container2);
    lv_obj_set_width(ui_Container2, lv_pct(50));
    lv_obj_set_height(ui_Container2, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_Container2, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_Container2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_Container2, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Container2, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Container2, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Container2, lv_color_hex(0xDE7B7B), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Container2, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Container2, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_Container2, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_Container2, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_border_color(ui_Container2, lv_color_hex(0x000000), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Container2, 255, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

    ui_charging_label3 = lv_label_create(ui_Container2);
    lv_obj_set_width(ui_charging_label3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label3, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label3, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label3, "BUCK1");
    lv_obj_set_style_text_color(ui_charging_label3, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label2 = lv_label_create(ui_Container2);
    lv_obj_set_width(ui_charging_label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label2, "BUCK2");
    lv_obj_set_style_text_color(ui_charging_label2, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label6 = lv_label_create(ui_Container2);
    lv_obj_set_width(ui_charging_label6, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label6, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label6, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label6, "LDO1");
    lv_obj_set_style_text_color(ui_charging_label6, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label6, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label7 = lv_label_create(ui_Container2);
    lv_obj_set_width(ui_charging_label7, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label7, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label7, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label7, "LDO2");
    lv_obj_set_style_text_color(ui_charging_label7, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label7, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Container4 = lv_obj_create(ui_Container1);
    lv_obj_remove_style_all(ui_Container4);
    lv_obj_set_width(ui_Container4, 10);
    lv_obj_set_height(ui_Container4, 50);
    lv_obj_set_align(ui_Container4, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Container4, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Container3 = lv_obj_create(ui_Container1);
    lv_obj_remove_style_all(ui_Container3);
    lv_obj_set_width(ui_Container3, lv_pct(50));
    lv_obj_set_height(ui_Container3, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_Container3, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_Container3, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_Container3, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Container3, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Container3, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Container3, lv_color_hex(0xDE797B), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Container3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Container3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Container3, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Container3, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_Container3, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_Container3, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label4 = lv_label_create(ui_Container3);
    lv_obj_set_width(ui_charging_label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label4, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label4, "1.8V");
    lv_obj_set_style_text_color(ui_charging_label4, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label5 = lv_label_create(ui_Container3);
    lv_obj_set_width(ui_charging_label5, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label5, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label5, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label5, "3.0V");
    lv_obj_set_style_text_color(ui_charging_label5, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label5, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label8 = lv_label_create(ui_Container3);
    lv_obj_set_width(ui_charging_label8, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label8, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label8, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label8, "3.3V");
    lv_obj_set_style_text_color(ui_charging_label8, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label8, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label9 = lv_label_create(ui_Container3);
    lv_obj_set_width(ui_charging_label9, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label9, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label9, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label9, "...");
    lv_obj_set_style_text_color(ui_charging_label9, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label9, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void chart_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_LONG_PRESSED && on_clear_history_cb) {
        on_clear_history_cb();
    }
}

static void scale_event(lv_event_t *e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);

    if(!draw_task) return;

    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    if(!base_dsc || base_dsc->part != LV_PART_INDICATOR) return;

    lv_draw_label_dsc_t * label_dsc = lv_draw_task_get_label_dsc(draw_task);
    if(!label_dsc || !label_dsc->text) return;

    if(label_dsc->text_local) lv_free((void *)label_dsc->text);

    char tmp_buffer[20] = {0};
    lv_snprintf(tmp_buffer, sizeof(tmp_buffer), "%.1f", (float) label_dsc->base.id2 / 1000.0);
    label_dsc->text = lv_strdup(tmp_buffer);
    label_dsc->text_local = 1;
}

static void on_tileview_change(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *curent = lv_tileview_get_tile_act(tv);
        if (lv_obj_get_parent(ui_root1) == curent) {
            set_indicator_page(0);
        } else if (lv_obj_get_parent(ui_root2) == curent) {
            set_indicator_page(1);
        } else if (lv_obj_get_parent(ui_root3) == curent) {
            set_indicator_page(2);
        } else {
            LV_LOG_ERROR("Failed finding parent!\n");
        }
    }
}