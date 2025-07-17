#include <weather/weather_ui.h>
#include <lvgl.h>
#include <ui/zsw_ui.h>
#include <assert.h>

typedef struct {
    lv_obj_t *ui_day;
    lv_obj_t *ui_day_temp;
    lv_obj_t *ui_day_icon;
    lv_obj_t *ui_day_day;
} lv_obj_forecasts_t;

static void add_forecast_day(lv_obj_t *parent, lv_obj_forecasts_t *storage);

static lv_obj_t *root_page = NULL;

static lv_obj_t *ui_bg_img;
static lv_obj_t *ui_root_container;
static lv_obj_t *ui_status_label;
static lv_obj_t *ui_forecast_widget;
static lv_obj_t *ui_time;
static lv_obj_t *ui_today_container;
static lv_obj_t *ui_today_icon;
static lv_obj_t *ui_today_temp;
static lv_obj_t *ui_today_min_max_temp;
static lv_obj_t *ui_today_rain;
static lv_obj_t *ui_water_drop_img;
static lv_obj_t *ui_loading_spinner;

static lv_obj_forecasts_t ui_forecasts[WEATHER_UI_NUM_FORECASTS];

ZSW_LV_IMG_DECLARE(ui_img_weather_app_bg);
ZSW_LV_IMG_DECLARE(ui_img_water_16_png);

void weather_ui_show(lv_obj_t *root)
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

    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    ui_bg_img = lv_img_create(root_page);
    lv_img_set_src(ui_bg_img, ZSW_LV_IMG_USE(ui_img_weather_app_bg));
    lv_obj_set_width(ui_bg_img, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_bg_img, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_bg_img, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_bg_img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_bg_img, LV_OBJ_FLAG_SCROLLABLE);

    ui_loading_spinner = lv_spinner_create(root_page);
    lv_spinner_set_anim_params(ui_loading_spinner, 5000, 400);
    lv_obj_set_width(ui_loading_spinner, 60);
    lv_obj_set_height(ui_loading_spinner, 60);
    lv_obj_set_align(ui_loading_spinner, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_loading_spinner, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(ui_loading_spinner, zsw_color_dark_gray(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_loading_spinner, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_loading_spinner, zsw_color_blue(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_loading_spinner, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_root_container = lv_obj_create(root_page);
    lv_obj_remove_style_all(ui_root_container);
    lv_obj_set_width(ui_root_container, lv_pct(100));
    lv_obj_set_height(ui_root_container, lv_pct(100));
    lv_obj_set_align(ui_root_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_root_container, LV_OBJ_FLAG_HIDDEN);

    ui_status_label = lv_label_create(root_page);
    lv_obj_set_width(ui_status_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_label, 0);
    lv_obj_set_y(ui_status_label, 25);
    lv_obj_set_align(ui_status_label, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_status_label, "");
    lv_obj_set_style_text_font(ui_status_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_forecast_widget = lv_obj_create(ui_root_container);
    lv_obj_remove_style_all(ui_forecast_widget);
    lv_obj_set_width(ui_forecast_widget, lv_pct(100));
    lv_obj_set_height(ui_forecast_widget, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_forecast_widget, 3);
    lv_obj_set_y(ui_forecast_widget, 55);
    lv_obj_set_align(ui_forecast_widget, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_forecast_widget, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_forecast_widget, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_forecast_widget, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_row(ui_forecast_widget, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_forecast_widget, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    for (int i = 0; i < WEATHER_UI_NUM_FORECASTS; i++) {
        add_forecast_day(ui_forecast_widget, &ui_forecasts[i]);
    }

    ui_time = lv_label_create(root_page);
    lv_obj_set_width(ui_time, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_time, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_time, 0);
    lv_obj_set_y(ui_time, 10);
    lv_obj_set_align(ui_time, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_time, LV_OBJ_FLAG_HIDDEN);

    ui_today_container = lv_obj_create(root_page);
    lv_obj_remove_style_all(ui_today_container);
    lv_obj_set_pos(ui_today_container, 0, -10);
    lv_obj_set_height(ui_today_container, 89);
    lv_obj_set_width(ui_today_container, lv_pct(100));
    lv_obj_set_align(ui_today_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_today_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_today_container, LV_OBJ_FLAG_HIDDEN);

    ui_today_icon = lv_img_create(ui_today_container);
    lv_obj_set_width(ui_today_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_today_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_today_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_today_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_today_icon, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *ui_Label8 = lv_label_create(ui_today_container);
    lv_obj_set_width(ui_Label8, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label8, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Label8, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Label8, "NOW");
    lv_obj_set_style_text_font(ui_Label8, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_today_temp = lv_label_create(ui_today_container);
    lv_obj_set_width(ui_today_temp, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_today_temp, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_today_temp, -40);
    lv_obj_set_y(ui_today_temp, -10);
    lv_obj_set_align(ui_today_temp, LV_ALIGN_CENTER);
    lv_obj_set_style_text_font(ui_today_temp, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_today_min_max_temp = lv_label_create(ui_today_container);
    lv_obj_set_width(ui_today_min_max_temp, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_today_min_max_temp, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_today_min_max_temp, 60);
    lv_obj_set_y(ui_today_min_max_temp, 0);
    lv_obj_set_align(ui_today_min_max_temp, LV_ALIGN_CENTER);
    //lv_obj_set_style_text_color(ui_today_min_max_temp, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_today_min_max_temp, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_today_min_max_temp, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_today_rain = lv_label_create(ui_today_container);
    lv_obj_set_width(ui_today_rain, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_today_rain, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_today_rain, -40);
    lv_obj_set_y(ui_today_rain, 10);
    lv_obj_set_align(ui_today_rain, LV_ALIGN_CENTER);
    lv_obj_set_style_text_font(ui_today_rain, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_water_drop_img = lv_img_create(ui_today_container);
    lv_img_set_src(ui_water_drop_img, ZSW_LV_IMG_USE(ui_img_water_16_png));
    lv_obj_set_width(ui_water_drop_img, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_water_drop_img, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_water_drop_img, -68);
    lv_obj_set_y(ui_water_drop_img, 11);
    lv_obj_set_align(ui_water_drop_img, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_water_drop_img, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_water_drop_img, LV_OBJ_FLAG_SCROLLABLE);
}

static void add_forecast_day(lv_obj_t *parent, lv_obj_forecasts_t *storage)
{
    storage->ui_day = lv_obj_create(parent);
    lv_obj_remove_style_all(storage->ui_day);
    lv_obj_set_width(storage->ui_day, LV_SIZE_CONTENT);
    lv_obj_set_height(storage->ui_day, LV_SIZE_CONTENT);
    lv_obj_set_align(storage->ui_day, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(storage->ui_day, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(storage->ui_day, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(storage->ui_day, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    storage->ui_day_temp = lv_label_create(storage->ui_day);
    lv_obj_set_width(storage->ui_day_temp, LV_SIZE_CONTENT);
    lv_obj_set_height(storage->ui_day_temp, LV_SIZE_CONTENT);
    lv_obj_set_align(storage->ui_day_temp, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(storage->ui_day_temp, lv_color_hex(0x5AA1EE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(storage->ui_day_temp, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    storage->ui_day_icon = lv_img_create(storage->ui_day);
    lv_obj_set_width(storage->ui_day_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(storage->ui_day_icon, LV_SIZE_CONTENT);
    lv_obj_set_x(storage->ui_day_icon, 48);
    lv_obj_set_y(storage->ui_day_icon, 53);
    lv_obj_set_align(storage->ui_day_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(storage->ui_day_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(storage->ui_day_icon, LV_OBJ_FLAG_SCROLLABLE);

    storage->ui_day_day = lv_label_create(storage->ui_day);
    lv_obj_set_width(storage->ui_day_day, LV_SIZE_CONTENT);
    lv_obj_set_height(storage->ui_day_day, LV_SIZE_CONTENT);
    lv_obj_set_align(storage->ui_day_day, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(storage->ui_day_day, lv_color_hex(0x5AA1EE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(storage->ui_day_day, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(storage->ui_day_day, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void weather_ui_set_weather_data(weather_ui_current_weather_data_t current_weather,
                                 weather_ui_forecast_data_t weather_ui_forecast_data_t[WEATHER_UI_NUM_FORECASTS],
                                 int num_forecasts)
{
    if (root_page == NULL || num_forecasts == 0) {
        return;
    }

    if (lv_obj_has_flag(ui_root_container, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_today_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_time, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_loading_spinner, LV_OBJ_FLAG_HIDDEN);
    }

    lv_label_set_text_fmt(ui_today_temp, "%.1f°", current_weather.temperature);
    lv_label_set_text_fmt(ui_today_min_max_temp, "%.1f° / %.1f°", weather_ui_forecast_data_t[0].low_temp,
                          weather_ui_forecast_data_t[0].high_temp);
    lv_label_set_text_fmt(ui_today_rain, "%d%%", weather_ui_forecast_data_t[0].rain_percent);
    lv_img_set_src(ui_today_icon, current_weather.icon);

    for (int i = 0; i < num_forecasts; i++) {
        lv_label_set_text_fmt(ui_forecasts[i].ui_day_temp, "%.1f°", weather_ui_forecast_data_t[i].temperature);
        lv_label_set_text(ui_forecasts[i].ui_day_day, weather_ui_forecast_data_t[i].day);
        lv_img_set_src(ui_forecasts[i].ui_day_icon, weather_ui_forecast_data_t[i].icon);
    }
}

void weather_ui_set_error(char *error)
{
    if (root_page == NULL) {
        return;
    }

    lv_obj_add_flag(ui_loading_spinner, LV_OBJ_FLAG_HIDDEN);

    lv_label_set_text(ui_status_label, error);
}

void weather_ui_set_time(int hour, int min, int second)
{
    lv_label_set_text_fmt(ui_time, "%02d:%02d", hour, min);
}

void weather_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}
