#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/logging/log.h>

#include "chronos_ui.h"

#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>

#include "ble/chronos/ble_chronos.h"


LOG_MODULE_REGISTER(chronos_ui, CONFIG_ZSW_BLE_LOG_LEVEL);

ZSW_LV_IMG_DECLARE(image_chronos_icon);

ZSW_LV_IMG_DECLARE(ui_img_phone_png);
ZSW_LV_IMG_DECLARE(ui_img_camera_png);

static lv_obj_t *root_page = NULL;
static uint32_t navIcCRC = 0xFFFFFFFF;

static lv_obj_t *ui_camerapanel;
static lv_obj_t *ui_callpanel;
static lv_obj_t *ui_callername;

void on_ringer_cb(bool state, char *name)
{
    if (state) {
        lv_label_set_text(ui_callername, name);
        lv_obj_clear_flag(ui_callpanel, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui_callpanel, LV_OBJ_FLAG_HIDDEN);
    }
}

void on_notification_received_cb(const chronos_notification_t *notification)
{
    chronos_ui_hide_notification_empty();
    chronos_ui_add_notification(*notification);
}

void on_remote_touch_cb(chronos_remote_touch_t *touch)
{
    chronos_ui_set_remote_touch(*touch);
}

void on_configuration_received_cb(chronos_config_t config, uint32_t a, uint32_t b)
{
    switch (config) {
        case CH_CONFIG_CAMERA:
            if (b) {
                lv_obj_clear_flag(ui_camerapanel, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(ui_camerapanel, LV_OBJ_FLAG_HIDDEN);
            }
            break;
        case CH_CONFIG_APP:
            chronos_app_info_t *app_info = ble_chronos_get_app_info();
            chronos_ui_set_app_info(*app_info);
            break;
        case CH_CONFIG_PBAT:
            chronos_phone_info_t *phone_info = ble_chronos_get_phone_info();
            chronos_ui_set_phone_info(*phone_info);
            break;
        case CH_CONFIG_NAV_DATA:
            chronos_navigation_t *nav = ble_chronos_get_navigation();

            chronos_ui_set_nav_icon_state(nav->active && nav->has_icon);

            char *navtext = NULL;
            size_t len = 0;
            len = strlen(nav->eta) + strlen(nav->duration) + strlen(nav->distance) + 3;

            navtext = (char *)malloc(len);
            if (navtext) {
                snprintf(navtext, len, "%s\n%s %s", nav->eta, nav->duration, nav->distance);
            }

            chronos_ui_set_nav_info(navtext, nav->title, nav->directions);

            free(navtext);

            break;
        case CH_CONFIG_NAV_ICON:

            if (a == 2) {
                chronos_navigation_t *nav = ble_chronos_get_navigation();
                if (nav->icon_crc != navIcCRC) {
                    navIcCRC = nav->icon_crc;
                    chronos_ui_set_nav_icon_state(nav->active && nav->has_icon);
                    for (int y = 0; y < 48; y++) {
                        for (int x = 0; x < 48; x++) {
                            int byte_index = (y * 48 + x) / 8;
                            int bit_pos = 7 - (x % 8);
                            bool px_on = (nav->icon[byte_index] >> bit_pos) & 0x01;
                            chronos_ui_set_nav_icon_px(x, y, px_on);
                        }
                    }
                }
            }
            break;
        case CH_CONFIG_WEATHER:
            if (a == 2) {
                chronos_weather_info_t *weather_info = ble_chronos_get_weather_info();
                if (weather_info->size > 0) {
                    chronos_weather_t *weather = ble_chronos_get_weather(0);

                    chronos_ui_set_weather_city(weather_info->city);
                    chronos_ui_set_weather_info(weather->temp, weather->icon, weather_info->time);

                    chronos_ui_clear_weather_weekly(false);

                    for (int i = 0; i < weather_info->size; i++) {
                        chronos_weather_t *wday = ble_chronos_get_weather(i);
                        chronos_ui_add_weather_weekly(*wday);
                    }

                    chronos_ui_clear_weather_hourly(false);

                    struct tm tm_info = ble_chronos_get_time_struct();
                    for (int i = tm_info.tm_hour; i < CH_FORECAST_SIZE; i++) {
                        chronos_hourly_forecast_t *forecast = ble_chronos_get_forecast_hour(i);
                        chronos_ui_add_weather_hourly(*forecast);
                    }
                }

            }
            if (b) {
                chronos_weather_info_t *weather_info = ble_chronos_get_weather_info();
                chronos_ui_set_weather_city(weather_info->city);
            }
            break;
        default:
            break;
    }
}


void chronos_ui_init(lv_obj_t *root)
{
    LOG_INF("Starting Chronos app");

    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_style_bg_color(root_page, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(root_page, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(root_page, lv_pct(100));
    lv_obj_set_height(root_page, lv_pct(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(root_page, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(root_page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_add_flag(root_page, LV_OBJ_FLAG_SCROLL_ONE); /// Flags
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_snap_x(root_page, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scroll_dir(root_page, LV_DIR_HOR);
    lv_obj_set_style_radius(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(root_page, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(root_page, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *page = chronos_ui_add_page(root_page);
    chronos_ui_about_init(page);

    chronos_app_info_t *app_info = ble_chronos_get_app_info();
    chronos_ui_set_app_info(*app_info);
    chronos_phone_info_t *phone_info = ble_chronos_get_phone_info();
    chronos_ui_set_phone_info(*phone_info);


    page = chronos_ui_add_page(root_page);
    chronos_ui_notifications_init(page);

    int count = ble_chronos_get_notification_count();
    chronos_ui_clear_notifications(!count);

    for (int i = 0; i < count; i++) {
        chronos_notification_t *notif = ble_chronos_get_notification(i);
        if (notif->available) {
            chronos_ui_add_notification(*notif);
        }

    }

    page = chronos_ui_add_page(root_page);
    chronos_ui_weather_init(page);

    chronos_weather_info_t *weather_info = ble_chronos_get_weather_info();
    if (weather_info->size > 0) {
        chronos_weather_t *weather = ble_chronos_get_weather(0);

        chronos_ui_set_weather_city(weather_info->city);
        chronos_ui_set_weather_info(weather->temp, weather->icon, weather_info->time);

        chronos_ui_clear_weather_weekly(false);

        for (int i = 0; i < weather_info->size; i++) {
            chronos_weather_t *wday = ble_chronos_get_weather(i);
            chronos_ui_add_weather_weekly(*wday);
        }

        chronos_ui_clear_weather_hourly(false);

        struct tm tm_info = ble_chronos_get_time_struct();
        for (int i = tm_info.tm_hour; i < CH_FORECAST_SIZE; i++) {
            chronos_hourly_forecast_t *forecast = ble_chronos_get_forecast_hour(i);
            chronos_ui_add_weather_hourly(*forecast);
        }
    }


    page = chronos_ui_add_page(root_page);
    chronos_ui_navigation_init(page);

    page = chronos_ui_add_page_static(root_page);
    chronos_ui_remote_touch_init(page);

    chronos_ui_set_nav_icon_state(false);
    chronos_ui_set_nav_info("Navigation\nInactive", "Chronos", "Start navigation on Google maps");

    chronos_ui_camera_panel_init(root);
    chronos_ui_call_panel_init(root);


    ble_chronos_add_notification_cb(on_notification_received_cb);
    ble_chronos_add_configuration_cb(on_configuration_received_cb);
    ble_chronos_add_touch_cb(on_remote_touch_cb);
    ble_chronos_add_ringer_cb(on_ringer_cb);

    LOG_INF("Started Chronos app");
}

void chronos_ui_deinit()
{
    LOG_INF("Closing Chronos app");

    ble_chronos_add_notification_cb(NULL);
    ble_chronos_add_configuration_cb(NULL);
    ble_chronos_add_touch_cb(NULL);
    ble_chronos_add_ringer_cb(NULL);

    lv_obj_del(root_page);
    root_page = NULL;

    LOG_INF("Closed Chronos app");
}

//////////////////////////////////////////////////////////////////
/////////////////// UI COMMON FUNCTIONS //////////////////////////
//////////////////////////////////////////////////////////////////

lv_obj_t *chronos_ui_add_page(lv_obj_t *parent)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_set_width(page, lv_pct(100));
    lv_obj_set_height(page, lv_pct(100));
    lv_obj_set_align(page, LV_ALIGN_CENTER);
    lv_obj_add_flag(page, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_add_flag(page, LV_OBJ_FLAG_SNAPPABLE);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_style_bg_color(page, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(page, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(page, LV_DIR_VER);
    lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // lv_obj_set_style_bg_color(page, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(page, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(page, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(page, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(page, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    return page;
}

lv_obj_t *chronos_ui_add_page_static(lv_obj_t *parent)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_remove_style_all(page);
    lv_obj_set_width(page, lv_pct(100));
    lv_obj_set_height(page, lv_pct(100));
    lv_obj_set_align(page, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(page, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(page, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    return page;
}

void chronos_ui_add_app_title(lv_obj_t *parent, const char *title, const void *src)
{
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_set_width(panel, lv_pct(100));
    lv_obj_set_height(panel, LV_SIZE_CONTENT); /// 70
    lv_obj_set_align(panel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(panel, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *app_icon = lv_img_create(panel);
    lv_img_set_src(app_icon, src);
    lv_obj_set_width(app_icon, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(app_icon, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(app_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(app_icon, LV_OBJ_FLAG_ADV_HITTEST);  /// Flags
    lv_obj_clear_flag(app_icon, LV_OBJ_FLAG_SCROLLABLE); /// Flags

    lv_obj_t *app_label = lv_label_create(panel);
    lv_obj_set_width(app_label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(app_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(app_label, LV_ALIGN_CENTER);
    lv_label_set_text(app_label, title);
    lv_obj_set_style_text_font(app_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
}


void chronos_ui_camera_panel_init(lv_obj_t *parent)
{

    ui_camerapanel = lv_obj_create(parent);
    lv_obj_set_width(ui_camerapanel, 240);
    lv_obj_set_height(ui_camerapanel, 240);
    lv_obj_set_align(ui_camerapanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_camerapanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_camerapanel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(ui_camerapanel, LV_OBJ_FLAG_HIDDEN);     /// Flags
    lv_obj_clear_flag(ui_camerapanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_camerapanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_camerapanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_camerapanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_camerapanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_camerapanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_cameralabel = lv_label_create(ui_camerapanel);
    lv_obj_set_width(ui_cameralabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_cameralabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_cameralabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_cameralabel, "Camera");
    lv_obj_set_style_text_font(ui_cameralabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_cameraimage = lv_img_create(ui_camerapanel);
    lv_img_set_src(ui_cameraimage, &ui_img_camera_png);
    lv_obj_set_width(ui_cameraimage, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_cameraimage, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_cameraimage, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_cameraimage, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_cameraimage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t *ui_camerainfo = lv_label_create(ui_camerapanel);
    lv_obj_set_width(ui_camerainfo, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_camerainfo, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_camerainfo, LV_ALIGN_CENTER);
    lv_label_set_text(ui_camerainfo, "Click to take a photo");
    lv_obj_set_style_text_font(ui_camerainfo, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_capturebutton = lv_obj_create(ui_camerapanel);
    lv_obj_remove_style_all(ui_capturebutton);
    lv_obj_set_width(ui_capturebutton, 150);
    lv_obj_set_height(ui_capturebutton, 40);
    lv_obj_set_align(ui_capturebutton, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_capturebutton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_capturebutton, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_capturebutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_capturebutton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_capturebutton, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_capturebutton, 20, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_capturebutton, lv_color_hex(0xD0D0D0), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_capturebutton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_t *ui_capturetext = lv_label_create(ui_capturebutton);
    lv_obj_set_width(ui_capturetext, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_capturetext, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_capturetext, LV_ALIGN_CENTER);
    lv_label_set_text(ui_capturetext, "Capture");
    lv_obj_set_style_text_font(ui_capturetext, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void chronos_ui_call_panel_init(lv_obj_t *parent)
{

    ui_callpanel = lv_obj_create(parent);
    lv_obj_set_width(ui_callpanel, 240);
    lv_obj_set_height(ui_callpanel, 240);
    lv_obj_set_align(ui_callpanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_callpanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_callpanel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(ui_callpanel, LV_OBJ_FLAG_HIDDEN);     /// Flags
    lv_obj_clear_flag(ui_callpanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_callpanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_callpanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_callpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_callpanel, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_callpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_calltext = lv_label_create(ui_callpanel);
    lv_obj_set_width(ui_calltext, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_calltext, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_calltext, LV_ALIGN_CENTER);
    lv_label_set_text(ui_calltext, "Incoming Call");
    lv_obj_set_style_text_font(ui_calltext, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_callimage = lv_img_create(ui_callpanel);
    lv_img_set_src(ui_callimage, &ui_img_phone_png);
    lv_obj_set_width(ui_callimage, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_callimage, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_callimage, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_callimage, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_callimage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_callername = lv_label_create(ui_callpanel);
    lv_obj_set_width(ui_callername, 200);
    lv_obj_set_height(ui_callername, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_callername, LV_ALIGN_CENTER);
    lv_label_set_text(ui_callername, "Caller name");
    lv_obj_set_style_text_align(ui_callername, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_callername, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

}