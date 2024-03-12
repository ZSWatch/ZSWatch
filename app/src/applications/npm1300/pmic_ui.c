#include <battery/battery_ui.h>
#include <lvgl.h>

LV_IMG_DECLARE(ui_img_nrf_icon_png);

void _ui_screen_change(lv_obj_t ** target, lv_scr_load_anim_t fademode, int spd, int delay, void (*target_init)(void));

// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
void ui_event_Screen1(lv_event_t * e);
lv_obj_t * ui_Screen1;
lv_obj_t * ui_root;
lv_obj_t * ui_Label2;
lv_obj_t * ui_Label3;
lv_obj_t * ui_charge_chart;
lv_obj_t * ui_Label1;
lv_obj_t * ui_nrf_icon;
lv_obj_t * ui_percent_voltage_label;
lv_obj_t * ui_charging_label;
lv_obj_t * ui_page_indicator;


// SCREEN: ui_Screen2
void ui_Screen2_screen_init(void);
void ui_event_Screen2(lv_event_t * e);
lv_obj_t * ui_Screen2;
lv_obj_t * ui_root1;
lv_obj_t * ui_Label4;
lv_obj_t * ui_state_chart;
lv_obj_t * ui_nrf_icon1;
lv_obj_t * ui_percent_voltage_label1;
lv_obj_t * ui_charging_label1;
lv_obj_t * ui_page_indicator1;


// SCREEN: ui_Screen3
void ui_Screen3_screen_init(void);
void ui_event_Screen3(lv_event_t * e);
lv_obj_t * ui_Screen3;
void ui_event_root2(lv_event_t * e);
lv_obj_t * ui_root2;
lv_obj_t * ui_Label6;
lv_obj_t * ui_nrf_icon2;
lv_obj_t * ui_Container1;
lv_obj_t * ui_Container2;
lv_obj_t * ui_charging_label3;
lv_obj_t * ui_charging_label2;
lv_obj_t * ui_charging_label6;
lv_obj_t * ui_charging_label7;
lv_obj_t * ui_Container4;
lv_obj_t * ui_Container3;
lv_obj_t * ui_charging_label4;
lv_obj_t * ui_charging_label5;
lv_obj_t * ui_charging_label8;
lv_obj_t * ui_charging_label9;
lv_obj_t * ui_page_indicator2;
lv_obj_t * ui____initial_actions0;

static lv_obj_t *root_page = NULL;


void pmic_ui_show(lv_obj_t *root)
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

    ui_Screen1_screen_init();
    ui_Screen2_screen_init();
    ui_Screen3_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr(ui_Screen1);

}

void pmic_ui_remove(void)
{
}

static void create_page_indicator(lv_obj_t* container, uint8_t index)
{
    lv_obj_t * label = lv_label_create(container);
    lv_label_set_text_fmt(label, "%d/3", index);
    lv_obj_center(label);
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
}

void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_root = lv_obj_create(ui_Screen1);
    lv_obj_set_width(ui_root, lv_pct(100));
    lv_obj_set_height(ui_root, lv_pct(100));
    lv_obj_set_align(ui_root, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_root, lv_color_hex(0x94D1E3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_root, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label2 = lv_label_create(ui_root);
    lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label2, 20);
    lv_obj_set_y(ui_Label2, 2);
    lv_obj_set_align(ui_Label2, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Label2, "nPM1300");
    lv_obj_set_style_text_color(ui_Label2, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label3 = lv_label_create(ui_root);
    lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label3, -5);
    lv_obj_set_y(ui_Label3, 0);
    lv_obj_set_align(ui_Label3, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_Label3, "%");
    lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charge_chart = lv_chart_create(ui_root);
    lv_obj_set_width(ui_charge_chart, 190);
    lv_obj_set_height(ui_charge_chart, 146);
    lv_obj_set_align(ui_charge_chart, LV_ALIGN_CENTER);
    lv_chart_set_type(ui_charge_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(ui_charge_chart, 20);
    lv_chart_set_range(ui_charge_chart, LV_CHART_AXIS_SECONDARY_Y, 3000, 4300);
    lv_chart_set_div_line_count(ui_charge_chart, 5, 0);
    lv_chart_set_axis_tick(ui_charge_chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 5, 2, false, 50);
    lv_chart_set_axis_tick(ui_charge_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 2, false, 50);
    lv_chart_set_axis_tick(ui_charge_chart, LV_CHART_AXIS_SECONDARY_Y, 0, 1, 4, 1, false, 12);
    lv_chart_series_t * ui_charge_chart_series_1 = lv_chart_add_series(ui_charge_chart, lv_color_hex(0x746DEC),
                                                                       LV_CHART_AXIS_PRIMARY_Y);
    static lv_coord_t ui_charge_chart_series_1_array[] = { 10, 20, 30, 25, 60, 80, 90, 100 };
    lv_chart_set_ext_y_array(ui_charge_chart, ui_charge_chart_series_1, ui_charge_chart_series_1_array);
    lv_chart_series_t * ui_charge_chart_series_2 = lv_chart_add_series(ui_charge_chart, lv_color_hex(0x1EB931),
                                                                       LV_CHART_AXIS_SECONDARY_Y);
    static lv_coord_t ui_charge_chart_series_2_array[] = { 4000, 3900, 3800, 4000, 4100, 3400, 4300 };
    lv_chart_set_ext_y_array(ui_charge_chart, ui_charge_chart_series_2, ui_charge_chart_series_2_array);
    lv_obj_set_style_bg_color(ui_charge_chart, lv_color_hex(0x00A2C6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_charge_chart, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_charge_chart, lv_color_hex(0x00A2C6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_charge_chart, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_charge_chart, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui_charge_chart, lv_color_hex(0x00A2C6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_charge_chart, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(ui_charge_chart, 1, LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_obj_set_style_size(ui_charge_chart, 6, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_line_color(ui_charge_chart, lv_color_hex(0x4040FF), LV_PART_TICKS | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_charge_chart, 0, LV_PART_TICKS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_charge_chart, lv_color_hex(0x493838), LV_PART_TICKS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charge_chart, 255, LV_PART_TICKS | LV_STATE_DEFAULT);

    ui_Label1 = lv_label_create(ui_root);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label1, 5);
    lv_obj_set_y(ui_Label1, 0);
    lv_obj_set_align(ui_Label1, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_Label1, "V");
    lv_obj_set_style_text_color(ui_Label1, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_nrf_icon = lv_img_create(ui_root);
    lv_img_set_src(ui_nrf_icon, &ui_img_nrf_icon_png);
    lv_obj_set_width(ui_nrf_icon, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_nrf_icon, LV_SIZE_CONTENT);    /// 21
    lv_obj_set_x(ui_nrf_icon, -30);
    lv_obj_set_y(ui_nrf_icon, 0);
    lv_obj_set_align(ui_nrf_icon, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_nrf_icon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_nrf_icon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_percent_voltage_label = lv_label_create(ui_root);
    lv_obj_set_width(ui_percent_voltage_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_percent_voltage_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_percent_voltage_label, -40);
    lv_obj_set_y(ui_percent_voltage_label, -10);
    lv_obj_set_align(ui_percent_voltage_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_percent_voltage_label, "68% / 4.2V ");
    lv_obj_set_style_text_color(ui_percent_voltage_label, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_percent_voltage_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label = lv_label_create(ui_root);
    lv_obj_set_width(ui_charging_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_charging_label, 40);
    lv_obj_set_y(ui_charging_label, -10);
    lv_obj_set_align(ui_charging_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_charging_label, "Charging");
    lv_obj_set_style_text_color(ui_charging_label, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_page_indicator = lv_obj_create(ui_root);
    lv_obj_set_width(ui_page_indicator, 100);
    lv_obj_set_height(ui_page_indicator, 10);
    lv_obj_set_x(ui_page_indicator, 0);
    lv_obj_set_y(ui_page_indicator, 5);
    lv_obj_set_align(ui_page_indicator, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_page_indicator, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_page_indicator, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_page_indicator, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_page_indicator, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_page_indicator, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    create_page_indicator(ui_page_indicator, 1);

    lv_obj_add_event_cb(ui_Screen1, ui_event_Screen1, LV_EVENT_ALL, NULL);
}

void ui_Screen2_screen_init(void)
{
    ui_Screen2 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_root1 = lv_obj_create(ui_Screen2);
    lv_obj_set_width(ui_root1, lv_pct(100));
    lv_obj_set_height(ui_root1, lv_pct(100));
    lv_obj_set_align(ui_root1, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_root1, lv_color_hex(0x94D1E3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_root1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label4 = lv_label_create(ui_root1);
    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label4, 20);
    lv_obj_set_y(ui_Label4, 2);
    lv_obj_set_align(ui_Label4, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Label4, "nPM1300");
    lv_obj_set_style_text_color(ui_Label4, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_state_chart = lv_chart_create(ui_root1);
    lv_obj_set_width(ui_state_chart, 135);
    lv_obj_set_height(ui_state_chart, 146);
    lv_obj_set_x(ui_state_chart, 13);
    lv_obj_set_y(ui_state_chart, 0);
    lv_obj_set_align(ui_state_chart, LV_ALIGN_CENTER);
    lv_chart_set_type(ui_state_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(ui_state_chart, 20);
    lv_chart_set_range(ui_state_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 7);
    lv_chart_set_range(ui_state_chart, LV_CHART_AXIS_SECONDARY_Y, 10, 35);
    lv_chart_set_div_line_count(ui_state_chart, 8, 0);
    lv_chart_set_axis_tick(ui_state_chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 5, 2, false, 50);
    lv_chart_set_axis_tick(ui_state_chart, LV_CHART_AXIS_PRIMARY_Y, 5, 1, 8, 1, true, 50);
    lv_chart_set_axis_tick(ui_state_chart, LV_CHART_AXIS_SECONDARY_Y, 0, 1, 4, 1, true, 12);
    lv_chart_series_t * ui_state_chart_series_1 = lv_chart_add_series(ui_state_chart, lv_color_hex(0x746DEC),
                                                                      LV_CHART_AXIS_PRIMARY_Y);
    static lv_coord_t ui_state_chart_series_1_array[] = { 10, 20, 30, 25, 60, 80, 90, 100 };
    lv_chart_set_ext_y_array(ui_state_chart, ui_state_chart_series_1, ui_state_chart_series_1_array);
    lv_chart_series_t * ui_state_chart_series_2 = lv_chart_add_series(ui_state_chart, lv_color_hex(0x1EB931),
                                                                      LV_CHART_AXIS_SECONDARY_Y);
    static lv_coord_t ui_state_chart_series_2_array[] = { 4000, 3900, 3800, 4000, 4100, 3400, 4300 };
    lv_chart_set_ext_y_array(ui_state_chart, ui_state_chart_series_2, ui_state_chart_series_2_array);
    lv_obj_set_style_bg_color(ui_state_chart, lv_color_hex(0x00A2C6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_state_chart, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_state_chart, lv_color_hex(0x00A2C6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_state_chart, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_state_chart, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui_state_chart, lv_color_hex(0x00A2C6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_state_chart, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(ui_state_chart, 1, LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_obj_set_style_size(ui_state_chart, 6, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_line_color(ui_state_chart, lv_color_hex(0x4040FF), LV_PART_TICKS | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_state_chart, 0, LV_PART_TICKS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_state_chart, lv_color_hex(0x493838), LV_PART_TICKS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_state_chart, 255, LV_PART_TICKS | LV_STATE_DEFAULT);

    ui_nrf_icon1 = lv_img_create(ui_root1);
    lv_img_set_src(ui_nrf_icon1, &ui_img_nrf_icon_png);
    lv_obj_set_width(ui_nrf_icon1, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_nrf_icon1, LV_SIZE_CONTENT);    /// 21
    lv_obj_set_x(ui_nrf_icon1, -30);
    lv_obj_set_y(ui_nrf_icon1, 0);
    lv_obj_set_align(ui_nrf_icon1, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_nrf_icon1, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_nrf_icon1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_percent_voltage_label1 = lv_label_create(ui_root1);
    lv_obj_set_width(ui_percent_voltage_label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_percent_voltage_label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_percent_voltage_label1, 40);
    lv_obj_set_y(ui_percent_voltage_label1, -10);
    lv_obj_set_align(ui_percent_voltage_label1, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_percent_voltage_label1, "26 degree");
    lv_obj_set_style_text_color(ui_percent_voltage_label1, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_percent_voltage_label1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label1 = lv_label_create(ui_root1);
    lv_obj_set_width(ui_charging_label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_charging_label1, -40);
    lv_obj_set_y(ui_charging_label1, -10);
    lv_obj_set_align(ui_charging_label1, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_charging_label1, "Chg State");
    lv_obj_set_style_text_color(ui_charging_label1, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_page_indicator1 = lv_obj_create(ui_root1);
    lv_obj_set_width(ui_page_indicator1, 100);
    lv_obj_set_height(ui_page_indicator1, 10);
    lv_obj_set_x(ui_page_indicator1, 0);
    lv_obj_set_y(ui_page_indicator1, 5);
    lv_obj_set_align(ui_page_indicator1, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_page_indicator1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_page_indicator1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_page_indicator1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_page_indicator1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_page_indicator1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    create_page_indicator(ui_page_indicator1, 2);

    lv_obj_add_event_cb(ui_Screen2, ui_event_Screen2, LV_EVENT_ALL, NULL);
}


void ui_Screen3_screen_init(void)
{
    ui_Screen3 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_root2 = lv_obj_create(ui_Screen3);
    lv_obj_set_width(ui_root2, lv_pct(100));
    lv_obj_set_height(ui_root2, lv_pct(100));
    lv_obj_set_align(ui_root2, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_root2, lv_color_hex(0x94D1E3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_root2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label6 = lv_label_create(ui_root2);
    lv_obj_set_width(ui_Label6, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label6, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label6, 20);
    lv_obj_set_y(ui_Label6, 2);
    lv_obj_set_align(ui_Label6, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Label6, "nPM1300");
    lv_obj_set_style_text_color(ui_Label6, lv_color_hex(0x493838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label6, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_nrf_icon2 = lv_img_create(ui_root2);
    lv_img_set_src(ui_nrf_icon2, &ui_img_nrf_icon_png);
    lv_obj_set_width(ui_nrf_icon2, LV_SIZE_CONTENT);   /// 24
    lv_obj_set_height(ui_nrf_icon2, LV_SIZE_CONTENT);    /// 21
    lv_obj_set_x(ui_nrf_icon2, -30);
    lv_obj_set_y(ui_nrf_icon2, 0);
    lv_obj_set_align(ui_nrf_icon2, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_nrf_icon2, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_nrf_icon2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Container1 = lv_obj_create(ui_root2);
    lv_obj_remove_style_all(ui_Container1);
    lv_obj_set_width(ui_Container1, lv_pct(80));
    lv_obj_set_height(ui_Container1, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_Container1, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_Container1, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_Container1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Container1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Container1, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Container1, lv_color_hex(0xD35D5D), LV_PART_MAIN | LV_STATE_DEFAULT);
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
    lv_obj_set_style_text_color(ui_charging_label4, lv_color_hex(0xFFD100), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label5 = lv_label_create(ui_Container3);
    lv_obj_set_width(ui_charging_label5, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label5, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label5, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label5, "3.0V");
    lv_obj_set_style_text_color(ui_charging_label5, lv_color_hex(0xFFD100), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label5, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label8 = lv_label_create(ui_Container3);
    lv_obj_set_width(ui_charging_label8, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label8, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label8, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label8, "3.3V");
    lv_obj_set_style_text_color(ui_charging_label8, lv_color_hex(0xFFD100), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label8, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_charging_label9 = lv_label_create(ui_Container3);
    lv_obj_set_width(ui_charging_label9, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_charging_label9, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_charging_label9, LV_ALIGN_CENTER);
    lv_label_set_text(ui_charging_label9, "...");
    lv_obj_set_style_text_color(ui_charging_label9, lv_color_hex(0xFFD100), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_charging_label9, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_page_indicator2 = lv_obj_create(ui_root2);
    lv_obj_set_width(ui_page_indicator2, 100);
    lv_obj_set_height(ui_page_indicator2, 10);
    lv_obj_set_x(ui_page_indicator2, 0);
    lv_obj_set_y(ui_page_indicator2, 5);
    lv_obj_set_align(ui_page_indicator2, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_page_indicator2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_page_indicator2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_page_indicator2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_page_indicator2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_page_indicator2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    create_page_indicator(ui_page_indicator2, 3);

    lv_obj_add_event_cb(ui_root2, ui_event_root2, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Screen3, ui_event_Screen3, LV_EVENT_ALL, NULL);
}

void ui_event_Screen1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, &ui_Screen2_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Screen3, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, &ui_Screen3_screen_init);
    }
}

void ui_event_Screen2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, &ui_Screen1_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Screen3, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, &ui_Screen3_screen_init);
    }
}

void ui_event_Screen3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, &ui_Screen2_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, &ui_Screen1_screen_init);
    }
}

void ui_event_root2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, &ui_Screen2_screen_init);
    }
}


void _ui_screen_change(lv_obj_t ** target, lv_scr_load_anim_t fademode, int spd, int delay, void (*target_init)(void))
{
    if(*target == NULL)
        target_init();
    lv_scr_load_anim(*target, fademode, spd, delay, false);
}
