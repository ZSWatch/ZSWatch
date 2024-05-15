#include "zsw_ui.h"
#include "lvgl.h"
#include "drivers/zsw_display_control.h"
#include "applications/watchface/watchface_app.h"

/*
Settings
Restart
Power off (nPM ship or hibernate) (BLE)
Music (NFC)
Current playing? (zbus in watchface_app listener and then call func here)
Brightness

*/

static lv_obj_t *ui_down_bg_panel;
static lv_obj_t *ui_up_bg_Panel;
static lv_obj_t *ui_music_info_label;
static lv_obj_t *ui_music_button;
static lv_obj_t *ui_music_label;
static lv_obj_t *ui_restart_button;
static lv_obj_t *ui_restart_label;
static lv_obj_t *ui_shutdown_button;
static lv_obj_t *ui_shutdown_label;
static lv_obj_t *ui_settings_button;
static lv_obj_t *ui_settings_label;
static lv_obj_t *ui_bri_slider;
static lv_obj_t *ui_LightLabel;
static lv_obj_t *ui_dropdown_bg_panel;

static lv_obj_t *dropdown_root;

static uint8_t ui_light_slider_value = 30;

static bool is_shown;
static watchface_app_evt_listener evt_cb;

LV_FONT_DECLARE(ui_font_iconfont34);
LV_FONT_DECLARE(lv_font_montserrat_14_full);

void ui_event_light_slider(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED) {
        ui_light_slider_value = lv_slider_get_value(ui_bri_slider);
        evt_cb((watchface_app_evt_t) {
            WATCHFACE_APP_EVENT_SET_BRIGHTNESS, .data.brightness = ui_light_slider_value
        });
    }
}

static void on_lvgl_screen_gesture_event_callback(lv_event_t *e)
{
    lv_dir_t  dir;
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_GESTURE) {
        dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir) {
            case LV_DIR_BOTTOM: {
                if (!is_shown) {
                    lv_obj_clear_flag(ui_down_bg_panel, LV_OBJ_FLAG_HIDDEN);
                    is_shown = true;
                }
                break;
            }
        }
    }
}

void ui_event_button(lv_event_t *e)
{
    lv_obj_t *button = e->current_target;
    if (e->code == LV_EVENT_CLICKED) {
        if (button == ui_restart_button)
            evt_cb((watchface_app_evt_t) {
            WATCHFACE_APP_EVENT_RESTART
        });
        else if (button == ui_music_button) {
            evt_cb((watchface_app_evt_t) {
                WATCHFACE_APP_EVENT_OPEN_APP, .data.app = WATCHFACE_APP_EVT_CLICK_MUSIC
            });
        } else if (button == ui_shutdown_button) {
            evt_cb((watchface_app_evt_t) {
                WATCHFACE_APP_EVENT_SHUTDOWN
            });
        } else if (button == ui_settings_button) {
            evt_cb((watchface_app_evt_t) {
                WATCHFACE_APP_EVENT_OPEN_APP, .data.app = WATCHFACE_APP_EVT_CLICK_SETTINGS
            });
        }
    }
}

static void on_lvgl_screen_gesture_event_callback_drop(lv_event_t *e)
{
    lv_dir_t  dir;
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_GESTURE) {
        dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir) {
            case LV_DIR_TOP: {
                if (is_shown) {
                    lv_obj_add_flag(ui_down_bg_panel, LV_OBJ_FLAG_HIDDEN);
                    is_shown = false;
                }
                break;
            }
        }
    }
}

void zsw_watchface_dropdown_ui_add(lv_obj_t *root_page,
                                   watchface_app_evt_listener callback /*, TODO input starting state of buttons and sliders */)
{
    __ASSERT(ui_down_bg_panel == NULL, "ui_down_bg_panel is not NULL");

    evt_cb = callback;
    dropdown_root = root_page;

    ui_down_bg_panel = lv_obj_create(root_page);
    lv_obj_set_width(ui_down_bg_panel, 240);
    lv_obj_set_height(ui_down_bg_panel, 240);
    lv_obj_set_x(ui_down_bg_panel, 0);
    lv_obj_set_y(ui_down_bg_panel, -10);
    lv_obj_set_align(ui_down_bg_panel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_down_bg_panel, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_CHAIN |
                      LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_scroll_dir(ui_down_bg_panel, LV_DIR_VER);
    lv_obj_set_style_bg_color(ui_down_bg_panel, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_down_bg_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_down_bg_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_down_bg_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_down_bg_panel, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_down_bg_panel, 0, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    lv_obj_add_flag(ui_down_bg_panel, LV_OBJ_FLAG_HIDDEN);

    ui_up_bg_Panel = lv_obj_create(ui_down_bg_panel);

    lv_obj_set_width(ui_up_bg_Panel, 240);
    lv_obj_set_height(ui_up_bg_Panel, 170);
    lv_obj_set_x(ui_up_bg_Panel, 0);
    lv_obj_set_y(ui_up_bg_Panel, -10);
    lv_obj_set_align(ui_up_bg_Panel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_up_bg_Panel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_up_bg_Panel, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_up_bg_Panel, 200, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_music_info_label = lv_label_create(ui_down_bg_panel);
    lv_label_set_text(ui_music_info_label, "");
    lv_obj_set_style_text_align(ui_music_info_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_music_info_label, LV_ALIGN_TOP_MID);
    lv_obj_set_y(ui_music_info_label, 10);
    lv_obj_set_width(ui_music_info_label, 150);
    lv_obj_set_style_text_font(ui_music_info_label, &lv_font_montserrat_14_full, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(ui_music_info_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_anim_speed(ui_music_info_label, 10, 0);

    ui_music_button = lv_btn_create(ui_down_bg_panel);
    lv_obj_set_width(ui_music_button, 50);
    lv_obj_set_height(ui_music_button, 50);
    lv_obj_set_x(ui_music_button, 20);
    lv_obj_set_y(ui_music_button, 35);
    lv_obj_clear_flag(ui_music_button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_music_label = lv_label_create(ui_music_button);
    lv_obj_set_width(ui_music_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_music_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_music_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_music_label, LV_SYMBOL_AUDIO);
    lv_obj_set_style_text_color(ui_music_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_music_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_music_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_restart_button = lv_btn_create(ui_down_bg_panel);
    lv_obj_set_width(ui_restart_button, 50);
    lv_obj_set_height(ui_restart_button, 50);
    lv_obj_set_x(ui_restart_button, 20);
    lv_obj_set_y(ui_restart_button, 95);
    lv_obj_clear_flag(ui_restart_button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_restart_label = lv_label_create(ui_restart_button);
    lv_obj_set_width(ui_restart_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_restart_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_restart_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_restart_label, LV_SYMBOL_REFRESH);
    lv_obj_set_style_text_color(ui_restart_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_restart_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_restart_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_shutdown_button = lv_btn_create(ui_down_bg_panel);
    lv_obj_set_width(ui_shutdown_button, 50);
    lv_obj_set_height(ui_shutdown_button, 50);
    lv_obj_set_x(ui_shutdown_button, 5);
    lv_obj_set_y(ui_shutdown_button, 95);
    lv_obj_set_align(ui_shutdown_button, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_shutdown_button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_shutdown_label = lv_label_create(ui_shutdown_button);
    lv_obj_set_width(ui_shutdown_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_shutdown_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_shutdown_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_shutdown_label, LV_SYMBOL_POWER);
    lv_obj_set_style_text_color(ui_shutdown_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_shutdown_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_shutdown_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settings_button = lv_btn_create(ui_down_bg_panel);
    lv_obj_set_width(ui_settings_button, 50);
    lv_obj_set_height(ui_settings_button, 50);
    lv_obj_set_x(ui_settings_button, 5);
    lv_obj_set_y(ui_settings_button, 35);
    lv_obj_set_align(ui_settings_button, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_settings_button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_settings_label = lv_label_create(ui_settings_button);
    lv_obj_set_width(ui_settings_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_settings_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_settings_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settings_label, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_color(ui_settings_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_settings_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_settings_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_bri_slider = lv_slider_create(ui_down_bg_panel);
    lv_slider_set_value(ui_bri_slider, ui_light_slider_value, LV_ANIM_OFF);
    if (lv_slider_get_mode(ui_bri_slider) == LV_SLIDER_MODE_RANGE) {
        lv_slider_set_left_value(ui_bri_slider, 0, LV_ANIM_OFF);
    }
    lv_obj_set_width(ui_bri_slider, 50);
    lv_obj_set_height(ui_bri_slider, 110);
    lv_obj_set_x(ui_bri_slider, -10);
    lv_obj_set_y(ui_bri_slider, 35);
    lv_obj_set_align(ui_bri_slider, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_radius(ui_bri_slider, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_bri_slider, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_bri_slider, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_bri_slider, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_bri_slider, lv_color_hex(0x3264C8), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_bri_slider, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_bri_slider, lv_color_hex(0x000000), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_bri_slider, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_bri_slider, LV_OBJ_FLAG_GESTURE_BUBBLE);      /// Flags

    ui_LightLabel = lv_label_create(ui_bri_slider);
    lv_obj_set_width(ui_LightLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LightLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_LightLabel, 0);
    lv_obj_set_y(ui_LightLabel, 30);
    lv_obj_set_align(ui_LightLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LightLabel, "");
    lv_obj_set_style_text_color(ui_LightLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LightLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LightLabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_dropdown_bg_panel = lv_obj_create(ui_down_bg_panel);
    lv_obj_set_width(ui_dropdown_bg_panel, 240);
    lv_obj_set_height(ui_dropdown_bg_panel, 160);
    lv_obj_set_x(ui_dropdown_bg_panel, 0);
    lv_obj_set_y(ui_dropdown_bg_panel, 150);
    lv_obj_set_align(ui_dropdown_bg_panel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_dropdown_bg_panel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_dropdown_bg_panel, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_dropdown_bg_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_dropdown_bg_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_dropdown_bg_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_music_button, ui_event_button, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_restart_button, ui_event_button, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_shutdown_button, ui_event_button, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settings_button, ui_event_button, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_bri_slider, ui_event_light_slider, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(dropdown_root, on_lvgl_screen_gesture_event_callback, LV_EVENT_GESTURE, NULL);
    lv_obj_add_event_cb(ui_down_bg_panel, on_lvgl_screen_gesture_event_callback_drop, LV_EVENT_GESTURE, NULL);
}

void zsw_watchface_dropdown_ui_set_music_info(char *track_name, char *artist)
{
    lv_label_set_text_fmt(ui_music_info_label, "%s - %s", artist, track_name);
}

void zsw_watchface_dropdown_ui_remove(void)
{
    lv_obj_remove_event_cb(dropdown_root, on_lvgl_screen_gesture_event_callback);
    lv_obj_del(ui_down_bg_panel);
    ui_down_bg_panel = NULL;
    is_shown = false;
}