

#include "../chronos_ui.h"
#include "ui/utils/zsw_ui_utils.h"


ZSW_LV_IMG_DECLARE(chronos_logo_icon);

ZSW_LV_IMG_DECLARE(chronos_day_0_icon);
ZSW_LV_IMG_DECLARE(chronos_day_1_icon);
ZSW_LV_IMG_DECLARE(chronos_day_2_icon);
ZSW_LV_IMG_DECLARE(chronos_day_3_icon);
ZSW_LV_IMG_DECLARE(chronos_day_4_icon);
ZSW_LV_IMG_DECLARE(chronos_day_5_icon);
ZSW_LV_IMG_DECLARE(chronos_day_6_icon);
ZSW_LV_IMG_DECLARE(chronos_day_7_icon);


static const void *weatherIcons[] = {
    ZSW_LV_IMG_USE(chronos_day_0_icon),
    ZSW_LV_IMG_USE(chronos_day_1_icon),
    ZSW_LV_IMG_USE(chronos_day_2_icon),
    ZSW_LV_IMG_USE(chronos_day_3_icon),
    ZSW_LV_IMG_USE(chronos_day_4_icon),
    ZSW_LV_IMG_USE(chronos_day_5_icon),
    ZSW_LV_IMG_USE(chronos_day_6_icon),
    ZSW_LV_IMG_USE(chronos_day_7_icon)
};



const char *days[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};


static lv_obj_t *ui_weeklypanel;
static lv_obj_t *ui_hourlypanel;

static lv_obj_t *ui_citylabel;
static lv_obj_t *ui_updatetime;
static lv_obj_t *ui_templabel;
static lv_obj_t *ui_weathericon;


void chronos_ui_weather_init(lv_obj_t *page)
{

    chronos_ui_add_app_title(page, "Weather", ZSW_LV_IMG_USE(chronos_logo_icon));

    lv_obj_t *ui_weatherpanel = lv_obj_create(page);
    lv_obj_set_width(ui_weatherpanel, lv_pct(100));
    lv_obj_set_height(ui_weatherpanel, LV_SIZE_CONTENT);    /// 240
    lv_obj_set_align(ui_weatherpanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_weatherpanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_weatherpanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_weatherpanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_weatherpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_weatherpanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_weatherpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_weatherpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_weatherpanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_weatherpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_citylabel = lv_label_create(ui_weatherpanel);
    lv_obj_set_width(ui_citylabel, 240);
    lv_obj_set_height(ui_citylabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_citylabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_citylabel, "");
    lv_obj_set_style_text_align(ui_citylabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_citylabel, CHRONOS_FONT_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_weathericon = lv_img_create(ui_weatherpanel);
    lv_img_set_src(ui_weathericon, ZSW_LV_IMG_USE(chronos_day_0_icon));
    lv_obj_set_width(ui_weathericon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_weathericon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_weathericon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_weathericon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_weathericon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_templabel = lv_label_create(ui_weatherpanel);
    lv_obj_set_width(ui_templabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_templabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_templabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_templabel, "--°C");
    lv_obj_set_style_text_font(ui_templabel, CHRONOS_FONT_40, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_updatetime = lv_label_create(ui_weatherpanel);
    lv_obj_set_width(ui_updatetime, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_updatetime, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_updatetime, LV_ALIGN_CENTER);
    lv_label_set_text(ui_updatetime, "No weather\ndata");
    lv_obj_set_style_text_align(ui_updatetime, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_updatetime, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_hourlypanel = lv_obj_create(page);
    lv_obj_set_width(ui_hourlypanel, lv_pct(100));
    lv_obj_set_height(ui_hourlypanel, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_hourlypanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_hourlypanel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_hourlypanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(ui_hourlypanel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_hourlypanel, LV_DIR_HOR);
    lv_obj_set_style_radius(ui_hourlypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_hourlypanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_hourlypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_hourlypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_hourlypanel, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_hourlypanel, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_hourlypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_hourlypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_hourlypanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_hourlypanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_weeklypanel = lv_obj_create(page);
    lv_obj_set_width(ui_weeklypanel, lv_pct(100));
    lv_obj_set_height(ui_weeklypanel, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_weeklypanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_weeklypanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_weeklypanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_weeklypanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_weeklypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_weeklypanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_weeklypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_weeklypanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_weeklypanel, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_weeklypanel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_weeklypanel, LV_BORDER_SIDE_TOP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_weeklypanel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_weeklypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    chronos_ui_clear_weather_weekly(true);

    // chronos_ui_clear_weather_hourly(true);


}

void chronos_ui_set_weather_info(int temp, uint8_t icon, chronos_time_t updated)
{
    lv_label_set_text_fmt(ui_updatetime, "update at\n%02d:%02d", updated.hour, updated.minute);
    lv_label_set_text_fmt(ui_templabel, "%d°C", temp);
    lv_img_set_src(ui_weathericon, weatherIcons[icon % 7]);
}

void chronos_ui_set_weather_city(const char *city)
{
    lv_label_set_text(ui_citylabel, city);
}

void chronos_ui_clear_weather_weekly(bool info)
{

    if (ui_weeklypanel == NULL) {
        return;
    }

    lv_obj_clean(ui_weeklypanel);

    if (!info) {
        return;
    }

    lv_obj_t *ui_noweather = lv_label_create(ui_weeklypanel);
    lv_obj_set_width(ui_noweather, lv_pct(70));
    lv_obj_set_height(ui_noweather, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_noweather, LV_ALIGN_CENTER);
    lv_label_set_text(ui_noweather, "No weather data.\nSync with Chronos app");
    lv_obj_set_style_text_color(ui_noweather, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_noweather, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_noweather, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_noweather, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void chronos_ui_clear_weather_hourly(bool info)
{

    if (ui_hourlypanel == NULL) {
        return;
    }

    lv_obj_clean(ui_hourlypanel);

    if (!info) {
        return;
    }

    lv_obj_t *ui_noweather = lv_label_create(ui_hourlypanel);
    lv_obj_set_width(ui_noweather, lv_pct(70));
    lv_obj_set_height(ui_noweather, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_noweather, LV_ALIGN_CENTER);
    lv_label_set_text(ui_noweather, "No forecast data.\nSync with Chronos app");
    lv_obj_set_style_text_color(ui_noweather, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_noweather, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_noweather, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_noweather, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);


}

void chronos_ui_add_weather_hourly(chronos_hourly_forecast_t forecast)
{
    if (ui_hourlypanel == NULL) {
        return;
    }
    lv_obj_t *ui_hourlyitem = lv_obj_create(ui_hourlypanel);
    lv_obj_set_width(ui_hourlyitem, 60);
    lv_obj_set_height(ui_hourlyitem, 100);
    lv_obj_set_align(ui_hourlyitem, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_hourlyitem, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_hourlyitem, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_hourlyitem, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_hourlyitem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_hourlyitem, lv_color_hex(0xFAFAFA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_hourlyitem, 155, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_hourlyitem, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_hourlyitem, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_hourlyitem, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_hourlyitem, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_hourlyitem, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_hourlyitem, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_hourlyitem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_hourlytime = lv_label_create(ui_hourlyitem);
    lv_obj_set_width(ui_hourlytime, lv_pct(100));
    lv_obj_set_height(ui_hourlytime, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_hourlytime, LV_ALIGN_TOP_MID);
    lv_label_set_text_fmt(ui_hourlytime, "%02d:00", forecast.hour);
    lv_obj_set_style_text_align(ui_hourlytime, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_decor(ui_hourlytime, LV_TEXT_DECOR_UNDERLINE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_hourlytime, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_hourlyicon = lv_img_create(ui_hourlyitem);
    lv_img_set_src(ui_hourlyicon, weatherIcons[forecast.icon % 7]);
    lv_obj_set_width(ui_hourlyicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_hourlyicon, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_hourlyicon, 0);
    lv_obj_set_y(ui_hourlyicon, 1);
    lv_obj_set_align(ui_hourlyicon, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_hourlyicon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_hourlyicon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ui_hourlyicon, 100);

    lv_obj_t *ui_hourlytemp = lv_label_create(ui_hourlyitem);
    lv_obj_set_width(ui_hourlytemp, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_hourlytemp, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_hourlytemp, 0);
    lv_obj_set_y(ui_hourlytemp, -20);
    lv_obj_set_align(ui_hourlytemp, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text_fmt(ui_hourlytemp, "%d°", forecast.temp);
    lv_obj_set_style_text_font(ui_hourlytemp, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_hourlyhumidity = lv_label_create(ui_hourlyitem);
    lv_obj_set_width(ui_hourlyhumidity, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_hourlyhumidity, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_hourlyhumidity, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text_fmt(ui_hourlyhumidity, "%d%%", forecast.humidity);
    lv_obj_set_style_text_font(ui_hourlyhumidity, CHRONOS_FONT_14, LV_PART_MAIN | LV_STATE_DEFAULT);

}
void chronos_ui_add_weather_weekly(chronos_weather_t weather)
{
    if (ui_weeklypanel == NULL) {
        return;
    }
    lv_obj_t *ui_weekitem = lv_obj_create(ui_weeklypanel);
    lv_obj_set_height(ui_weekitem, 40);
    lv_obj_set_width(ui_weekitem, lv_pct(100));
    lv_obj_set_align(ui_weekitem, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_weekitem, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_weekitem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_weekitem, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_weekitem, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_weekitem, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_weekitem, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_weekitem, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_weekitem, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_weekitem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_weekitem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_weekitem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_weekitem, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_daylabel = lv_label_create(ui_weekitem);
    lv_obj_set_width(ui_daylabel, 70);
    lv_obj_set_height(ui_daylabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_daylabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_daylabel, days[weather.day % 7]);
    lv_obj_set_style_text_align(ui_daylabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_daylabel, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_dayicon = lv_img_create(ui_weekitem);
    lv_img_set_src(ui_dayicon, weatherIcons[weather.icon % 7]);
    lv_obj_set_width(ui_dayicon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_dayicon, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_dayicon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_dayicon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_dayicon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ui_dayicon, 128);

    lv_obj_t *ui_daytemp = lv_label_create(ui_weekitem);
    lv_obj_set_width(ui_daytemp, 70);
    lv_obj_set_height(ui_daytemp, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_daytemp, LV_ALIGN_RIGHT_MID);
    lv_label_set_text_fmt(ui_daytemp, "%d°C", weather.temp);
    lv_obj_set_style_text_align(ui_daytemp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_daytemp, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);

}