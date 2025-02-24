

#include "../chronos_ui.h"
#include "ui/utils/zsw_ui_utils.h"


ZSW_LV_IMG_DECLARE(image_chronos_icon);


static lv_obj_t *ui_weeklypanel;
static lv_obj_t *ui_hourlypanel;

void weather_init(lv_obj_t *page)
{

    add_app_title(page, "Weather", &image_chronos_icon);

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

    lv_obj_t *ui_citylabel = lv_label_create(ui_weatherpanel);
    lv_obj_set_width(ui_citylabel, 240);
    lv_obj_set_height(ui_citylabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_citylabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_citylabel, "Nairobi");
    lv_obj_set_style_text_align(ui_citylabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_citylabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_weathericon = lv_img_create(ui_weatherpanel);
    lv_img_set_src(ui_weathericon, ZSW_LV_IMG_USE(image_chronos_icon));
    lv_obj_set_width(ui_weathericon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_weathericon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_weathericon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_weathericon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_weathericon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t *ui_templabel = lv_label_create(ui_weatherpanel);
    lv_obj_set_width(ui_templabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_templabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_templabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_templabel, "20C");
    lv_obj_set_style_text_font(ui_templabel, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_updatetime = lv_label_create(ui_weatherpanel);
    lv_obj_set_width(ui_updatetime, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_updatetime, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_updatetime, LV_ALIGN_CENTER);
    lv_label_set_text(ui_updatetime, "Update at\n12:45");
    lv_obj_set_style_text_align(ui_updatetime, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_updatetime, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_hourlypanel = lv_obj_create(page);
    lv_obj_set_width(ui_hourlypanel, lv_pct(100));
    lv_obj_set_height(ui_hourlypanel, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_hourlypanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_hourlypanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_hourlypanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_hourlypanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_hourlypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_hourlypanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_hourlypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_hourlypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_hourlypanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_hourlypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

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



}


void add_weekly(const char *day, const void *src, int temp)
{
    if (ui_weeklypanel == NULL) {
        return;
    }
    lv_obj_t *ui_weekitem = lv_obj_create(ui_weeklypanel);
    lv_obj_set_width(ui_weekitem, lv_pct(100));
    lv_obj_set_height(ui_weekitem, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_weekitem, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_weekitem, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_weekitem, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
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
    lv_obj_set_width(ui_daylabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_daylabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_daylabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_daylabel, day);
    lv_obj_set_style_text_font(ui_daylabel, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_dayicon = lv_img_create(ui_weekitem);
    lv_img_set_src(ui_dayicon, src);
    lv_obj_set_width(ui_dayicon, 32);
    lv_obj_set_height(ui_dayicon, 32);
    lv_obj_set_align(ui_dayicon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_dayicon, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_dayicon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ui_dayicon, 128);

    lv_obj_t *ui_daytemp = lv_label_create(ui_weekitem);
    lv_obj_set_width(ui_daytemp, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_daytemp, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_daytemp, LV_ALIGN_CENTER);
    lv_label_set_text_fmt(ui_daytemp, "%dC", temp);
    lv_obj_set_style_text_font(ui_daytemp, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
}