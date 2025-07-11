#include "zsw_ui.h"
#include "lvgl.h"
#include "applications/watchface/watchface_app.h"

static lv_obj_t *ui_down_bg_panel;
static lv_obj_t *ui_up_bg_Panel;
static lv_obj_t *ui_music_info_label;
static lv_obj_t *ui_music_button;
static lv_obj_t *ui_music_label;
static lv_obj_t *ui_flashlight_button;
static lv_obj_t *ui_flashlight_label;
static lv_obj_t *ui_shutdown_button;
static lv_obj_t *ui_shutdown_label;
static lv_obj_t *ui_settings_button;
static lv_obj_t *ui_settings_label;
static lv_obj_t *ui_bri_slider;
static lv_obj_t *ui_light_label;
static lv_obj_t *ui_dropdown_bg_panel;
static lv_obj_t *ui_battery_state;
static lv_obj_t *ui_battery_charge_state;
static lv_obj_t *ui_remaining_time_bat_label;

static lv_obj_t *dropdown_root;

static uint8_t ui_light_slider_value = 30;

static bool is_shown;
static watchface_app_evt_listener evt_cb;

LV_FONT_DECLARE(lv_font_montserrat_14_full);

ZSW_LV_IMG_DECLARE(light);
ZSW_LV_IMG_DECLARE(brightness_adjust_icon);

ZSW_LV_IMG_DECLARE(face_goog_20_61728_0);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_1);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_2);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_3);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_4);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_5);
ZSW_LV_IMG_DECLARE(face_goog_20_61728_6);

// Battery icons from the pixel watchface
static const void *face_goog_battery[] = {
    ZSW_LV_IMG_USE(face_goog_20_61728_0),
    ZSW_LV_IMG_USE(face_goog_20_61728_1),
    ZSW_LV_IMG_USE(face_goog_20_61728_2),
    ZSW_LV_IMG_USE(face_goog_20_61728_3),
    ZSW_LV_IMG_USE(face_goog_20_61728_4),
    ZSW_LV_IMG_USE(face_goog_20_61728_5),
    ZSW_LV_IMG_USE(face_goog_20_61728_6),
};

void ui_event_light_slider(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if ((event_code == LV_EVENT_VALUE_CHANGED) || (event_code == LV_EVENT_RELEASED)) {
        ui_light_slider_value = lv_slider_get_value(ui_bri_slider);
        evt_cb((watchface_app_evt_t) {
            WATCHFACE_APP_EVENT_SET_BRIGHTNESS, .data.brightness = ui_light_slider_value,
                                                .data.store_brightness = event_code == LV_EVENT_RELEASED
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
            default:
                break;
        }
    }
}

void ui_event_button(lv_event_t *e)
{
    lv_obj_t *button = lv_event_get_target_obj(e);
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (button == ui_flashlight_button)
            evt_cb((watchface_app_evt_t) {
            WATCHFACE_APP_EVENT_OPEN_APP, .data.app = WATCHFACE_APP_EVT_CLICK_FLASHLIGHT
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
            default:
                break;
        }
    }
}

void zsw_watchface_dropdown_ui_add(lv_obj_t *root_page,
                                   watchface_app_evt_listener callback, int brightness)
{
    __ASSERT(dropdown_root == NULL, "dropdown_root is not NULL");

    evt_cb = callback;
    dropdown_root = root_page;
    ui_light_slider_value = brightness;

    ui_down_bg_panel = lv_obj_create(root_page);
    lv_obj_set_width(ui_down_bg_panel, 240);
    lv_obj_set_height(ui_down_bg_panel, 240);
    lv_obj_set_x(ui_down_bg_panel, 0);
    lv_obj_set_y(ui_down_bg_panel, -10);
    lv_obj_set_align(ui_down_bg_panel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_down_bg_panel, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_CHAIN |
                      LV_OBJ_FLAG_SCROLLABLE);
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
    lv_obj_clear_flag(ui_up_bg_Panel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_up_bg_Panel, lv_color_hex(0x30343F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_up_bg_Panel, 200, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_music_info_label = lv_label_create(ui_down_bg_panel);
    lv_label_set_text(ui_music_info_label, "");
    lv_obj_set_style_text_align(ui_music_info_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_music_info_label, LV_ALIGN_TOP_MID);
    lv_obj_set_y(ui_music_info_label, 14);
    lv_obj_set_width(ui_music_info_label, 150);
    lv_obj_set_style_text_font(ui_music_info_label, &lv_font_montserrat_14_full, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(ui_music_info_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    //lv_obj_set_style_anim_speed(ui_music_info_label, 15, 0);

    ui_music_button = lv_btn_create(ui_down_bg_panel);
    lv_obj_set_width(ui_music_button, 50);
    lv_obj_set_height(ui_music_button, 50);
    lv_obj_set_x(ui_music_button, 20);
    lv_obj_set_y(ui_music_button, 35);
    lv_obj_clear_flag(ui_music_button, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_radius(ui_music_button, LV_RADIUS_CIRCLE, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_music_label = lv_label_create(ui_music_button);
    lv_obj_set_width(ui_music_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_music_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_music_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_music_label, LV_SYMBOL_AUDIO);
    lv_obj_set_style_text_color(ui_music_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_music_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_music_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_flashlight_button = lv_btn_create(ui_down_bg_panel);
    lv_obj_set_width(ui_flashlight_button, 50);
    lv_obj_set_height(ui_flashlight_button, 50);
    lv_obj_set_x(ui_flashlight_button, 20);
    lv_obj_set_y(ui_flashlight_button, 95);
    lv_obj_clear_flag(ui_flashlight_button, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_radius(ui_flashlight_button, LV_RADIUS_CIRCLE, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_flashlight_label = lv_label_create(ui_flashlight_button);
    lv_obj_set_width(ui_flashlight_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_flashlight_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_flashlight_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_flashlight_label, LV_SYMBOL_CHARGE);
    lv_obj_set_style_text_color(ui_flashlight_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_flashlight_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_flashlight_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_shutdown_button = lv_btn_create(ui_down_bg_panel);
    lv_obj_set_width(ui_shutdown_button, 50);
    lv_obj_set_height(ui_shutdown_button, 50);
    lv_obj_set_x(ui_shutdown_button, 5);
    lv_obj_set_y(ui_shutdown_button, 95);
    lv_obj_set_align(ui_shutdown_button, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_shutdown_button, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_radius(ui_shutdown_button, LV_RADIUS_CIRCLE, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_shutdown_label = lv_label_create(ui_shutdown_button);
    lv_obj_set_width(ui_shutdown_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_shutdown_label, LV_SIZE_CONTENT);
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
    lv_obj_clear_flag(ui_settings_button, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_radius(ui_settings_button, LV_RADIUS_CIRCLE, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settings_label = lv_label_create(ui_settings_button);
    lv_obj_set_width(ui_settings_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_settings_label, LV_SIZE_CONTENT);
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
    lv_slider_set_range(ui_bri_slider, 1, 100);
    lv_obj_set_width(ui_bri_slider, 50);
    lv_obj_set_height(ui_bri_slider, 110);
    lv_obj_set_x(ui_bri_slider, -10);
    lv_obj_set_y(ui_bri_slider, 35);
    lv_obj_set_align(ui_bri_slider, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_radius(ui_bri_slider, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_bri_slider, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_bri_slider, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_bri_slider, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_bri_slider, lv_color_hex(0x000000), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_bri_slider, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_bri_slider, LV_OBJ_FLAG_GESTURE_BUBBLE);

    ui_light_label = lv_img_create(ui_bri_slider);
    lv_obj_set_width(ui_light_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_light_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_light_label, 0);
    lv_obj_set_y(ui_light_label, 30);
    lv_obj_set_align(ui_light_label, LV_ALIGN_CENTER);
    lv_img_set_src(ui_light_label, ZSW_LV_IMG_USE(brightness_adjust_icon));

    ui_dropdown_bg_panel = lv_obj_create(ui_down_bg_panel);
    lv_obj_set_width(ui_dropdown_bg_panel, 240);
    lv_obj_set_height(ui_dropdown_bg_panel, 160);
    lv_obj_set_x(ui_dropdown_bg_panel, 0);
    lv_obj_set_y(ui_dropdown_bg_panel, 150);
    lv_obj_set_align(ui_dropdown_bg_panel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_dropdown_bg_panel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_dropdown_bg_panel, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_dropdown_bg_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_dropdown_bg_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_dropdown_bg_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battery_state = lv_img_create(ui_down_bg_panel);
    lv_img_set_src(ui_battery_state, ZSW_LV_IMG_USE(face_goog_20_61728_5));
    lv_obj_set_width(ui_battery_state, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_battery_state, LV_SIZE_CONTENT);
    lv_obj_align(ui_battery_state, LV_ALIGN_TOP_MID, -22, 0);

    ui_battery_charge_state = lv_img_create(ui_battery_state);
    lv_img_set_src(ui_battery_charge_state, ZSW_LV_IMG_USE(light));
    lv_obj_set_width(ui_battery_charge_state, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_battery_charge_state, LV_SIZE_CONTENT);
    lv_obj_align(ui_battery_charge_state, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(ui_battery_charge_state, LV_OBJ_FLAG_HIDDEN);

    ui_remaining_time_bat_label = lv_label_create(ui_down_bg_panel);
    lv_obj_set_width(ui_remaining_time_bat_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_remaining_time_bat_label, LV_SIZE_CONTENT);
    lv_obj_align(ui_remaining_time_bat_label, LV_ALIGN_TOP_MID, 22, 0);
    lv_label_set_text(ui_remaining_time_bat_label, "");
    lv_obj_set_style_text_font(ui_remaining_time_bat_label, &lv_font_montserrat_14_full, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_music_button, ui_event_button, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_flashlight_button, ui_event_button, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_shutdown_button, ui_event_button, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settings_button, ui_event_button, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_bri_slider, ui_event_light_slider, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(dropdown_root, on_lvgl_screen_gesture_event_callback, LV_EVENT_GESTURE, NULL);
    lv_obj_add_event_cb(ui_down_bg_panel, on_lvgl_screen_gesture_event_callback_drop, LV_EVENT_GESTURE, NULL);
}

void zsw_watchface_dropdown_ui_set_music_info(char *track_name, char *artist)
{
    if (!dropdown_root) {
        return;
    }
    lv_label_set_text_fmt(ui_music_info_label, "%s - %s", artist, track_name);
}

void zsw_watchface_dropdown_ui_set_battery_info(uint8_t battery_percent, bool is_charging, int tte, int ttf)
{
    int days;
    int hours;
    int minutes;

    if (!dropdown_root) {
        return;
    }
    int battery_index;

    // Can't just use lv_map as I want to have a different range for each battery icon to make it look better
    if (battery_percent <= 10) {
        battery_index = 0;
    } else if (battery_percent <= 20) {
        battery_index = 1;
    } else if (battery_percent <= 40) {
        battery_index = 2;
    } else if (battery_percent <= 60) {
        battery_index = 3;
    } else if (battery_percent <= 80) {
        battery_index = 4;
    } else if (battery_percent <= 90) {
        battery_index = 5;
    } else {
        battery_index = 6;
    }

    lv_img_set_src(ui_battery_state, face_goog_battery[battery_index]);

    if (is_charging) {
        lv_obj_clear_flag(ui_battery_charge_state, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui_battery_charge_state, LV_OBJ_FLAG_HIDDEN);
    }

    if (tte > 0) {
        zsw_ui_utils_seconds_to_day_hour_min(tte, &days, &hours, &minutes);
        if (days == 0 && hours == 0) {
            lv_label_set_text_fmt(ui_remaining_time_bat_label, "%dm", minutes);
        } else {
            lv_label_set_text_fmt(ui_remaining_time_bat_label, "%dd %dh", days, hours);
        }
    } else if (ttf > 0) {
        zsw_ui_utils_seconds_to_day_hour_min(ttf, &days, &hours, &minutes);
        if (days == 0 && hours == 0) {
            lv_label_set_text_fmt(ui_remaining_time_bat_label, "%dm", minutes);
        } else {
            // Charging should never take more than 24 hours, hence hour min is enough
            lv_label_set_text_fmt(ui_remaining_time_bat_label, "%dh %dm", hours, minutes);
        }
    } else {
        lv_label_set_text_fmt(ui_remaining_time_bat_label, "%d%%", battery_percent);
    }
}

void zsw_watchface_dropdown_ui_remove(void)
{
    lv_obj_remove_event_cb(dropdown_root, on_lvgl_screen_gesture_event_callback);
    lv_obj_del(ui_down_bg_panel);
    dropdown_root = NULL;
    is_shown = false;
}