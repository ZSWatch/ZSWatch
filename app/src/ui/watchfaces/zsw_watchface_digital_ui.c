/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <lvgl.h>

#include "ui/zsw_ui.h"
#include "applications/watchface/watchface_app.h"

#ifdef __ZEPHYR__
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(watchface_digital, LOG_LEVEL_WRN);
#endif

static void watchface_ui_invalidate_cached(void);
static void arc_event_pressed(lv_event_t *e);

static lv_obj_t *root_page = NULL;
static lv_obj_t *ui_digital_watchface;
static lv_obj_t *ui_iaq_or_pressure_arc;
#ifdef CONFIG_EXTERNAL_USE_BOSCH_BSEC
static lv_obj_t *ui_co2_arc;
static lv_obj_t *ui_iaq_co2_text_image;
#else
static lv_obj_t *ui_pressure_image;
#endif
static lv_obj_t *ui_humidity_arc;
static lv_obj_t *ui_humidity_icon;
static lv_obj_t *ui_watch_temperature_label;
static lv_obj_t *ui_time;
static lv_obj_t *ui_min_label;
static lv_obj_t *ui_colon_label;
static lv_obj_t *ui_hour_label;
static lv_obj_t *ui_sec_label;
static lv_obj_t *ui_battery_arc;
static lv_obj_t *ui_battery_arc_icon;
static lv_obj_t *ui_battery_percent_label;
static lv_obj_t *ui_step_arc;
static lv_obj_t *ui_step_arc_icon;
static lv_obj_t *ui_step_arc_label;
static lv_obj_t *ui_top_panel;
static lv_obj_t *ui_day_label;
static lv_obj_t *ui_date_label;
static lv_obj_t *ui_notifications;
static lv_obj_t *ui_notification_icon;
static lv_obj_t *ui_notification_count_label;
static lv_obj_t *ui_bt_icon;
static lv_obj_t *ui_weather_temperature_label;
static lv_obj_t *ui_weather_icon;

LV_IMG_DECLARE(ui_img_iaq_co2_text);    // assets/air_quality.png
ZSW_LV_IMG_DECLARE(ui_img_pressure_png);    // assets/pressure.png
ZSW_LV_IMG_DECLARE(ui_img_temperatures_png);    // assets/temperatures.png
ZSW_LV_IMG_DECLARE(ui_img_charging_png);    // assets/charging.png
ZSW_LV_IMG_DECLARE(ui_img_running_png);    // assets/running.png
ZSW_LV_IMG_DECLARE(ui_img_chat_png);    // assets/chat.png
ZSW_LV_IMG_DECLARE(ui_img_bluetooth_png);    // assets/bluetooth.png

LV_FONT_DECLARE(ui_font_aliean_47);
LV_FONT_DECLARE(ui_font_aliean_25);

// Remember last values as if no change then
// no reason to waste resourses and redraw
static int last_hour = -1;
static int last_minute = -1;
static int last_second = -1;
static int last_num_not = -1;

static watchface_app_evt_listener ui_evt_cb;

static void watchface_show(watchface_app_evt_listener evt_cb, zsw_settings_watchface_t *settings)
{
    ui_evt_cb = evt_cb;
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
    root_page = lv_obj_create(lv_scr_act());
    watchface_ui_invalidate_cached();

    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, 240, 240);
    lv_obj_align(root_page, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_digital_watchface = root_page;
    lv_obj_set_style_bg_img_src(ui_digital_watchface, global_watchface_bg_img, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_clear_flag(ui_digital_watchface, LV_OBJ_FLAG_SCROLLABLE);

    ui_iaq_or_pressure_arc = lv_arc_create(ui_digital_watchface);
    lv_obj_set_width(ui_iaq_or_pressure_arc, 240);
    lv_obj_set_height(ui_iaq_or_pressure_arc, 240);
    lv_obj_set_align(ui_iaq_or_pressure_arc, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_iaq_or_pressure_arc, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(ui_iaq_or_pressure_arc, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                      LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_arc_set_value(ui_iaq_or_pressure_arc, 70);
    lv_arc_set_bg_angles(ui_iaq_or_pressure_arc, 195, 245);
    lv_arc_set_rotation(ui_iaq_or_pressure_arc, 1);
    lv_obj_set_style_arc_width(ui_iaq_or_pressure_arc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
#ifdef CONFIG_EXTERNAL_USE_BOSCH_BSEC
    lv_arc_set_range(ui_iaq_or_pressure_arc, 0, 351);
#else
    lv_arc_set_range(ui_iaq_or_pressure_arc, 950, 1050);
#endif
    lv_obj_set_style_arc_color(ui_iaq_or_pressure_arc, lv_color_hex(0x4AC73F), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_iaq_or_pressure_arc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_iaq_or_pressure_arc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_iaq_or_pressure_arc, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_iaq_or_pressure_arc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

#ifdef CONFIG_EXTERNAL_USE_BOSCH_BSEC
    ui_co2_arc = lv_arc_create(ui_digital_watchface);
    lv_obj_set_width(ui_co2_arc, 240);
    lv_obj_set_height(ui_co2_arc, 240);
    lv_obj_set_align(ui_co2_arc, LV_ALIGN_CENTER);
    lv_obj_set_pos(ui_co2_arc, 5, 5);
    lv_obj_add_flag(ui_co2_arc, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(ui_co2_arc, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                      LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_arc_set_value(ui_co2_arc, 70);
    lv_arc_set_bg_angles(ui_co2_arc, 197, 243);
    lv_arc_set_rotation(ui_co2_arc, 1);
    lv_arc_set_range(ui_co2_arc, 0, 2000);
    lv_obj_set_style_arc_width(ui_co2_arc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_co2_arc, lv_color_hex(0x4AC73F), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_co2_arc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_co2_arc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_co2_arc, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_co2_arc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_iaq_co2_text_image = lv_img_create(ui_co2_arc);
    lv_img_set_src(ui_iaq_co2_text_image, ZSW_LV_IMG_USE(ui_img_pressure_png));
    lv_obj_set_width(ui_iaq_co2_text_image, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_iaq_co2_text_image, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_iaq_co2_text_image, -80, -70);
    lv_obj_set_align(ui_iaq_co2_text_image, LV_ALIGN_CENTER);
    lv_img_set_src(ui_iaq_co2_text_image, &ui_img_iaq_co2_text);
    lv_img_set_angle(ui_iaq_co2_text_image, 300);
    lv_obj_add_flag(ui_iaq_co2_text_image, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_iaq_co2_text_image,
                      LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                      LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_img_recolor(ui_iaq_co2_text_image, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_iaq_co2_text_image, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
#else
    ui_pressure_image = lv_img_create(ui_iaq_or_pressure_arc);
    lv_img_set_src(ui_pressure_image, ZSW_LV_IMG_USE(ui_img_pressure_png));
    lv_obj_set_width(ui_pressure_image, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_pressure_image, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_pressure_image, -70);
    lv_obj_set_y(ui_pressure_image, -68);
    lv_obj_set_align(ui_pressure_image, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_pressure_image, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_pressure_image,
                      LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                      LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_img_recolor(ui_pressure_image, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_pressure_image, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
#endif

    ui_humidity_arc = lv_arc_create(ui_digital_watchface);
    lv_obj_set_width(ui_humidity_arc, 240);
    lv_obj_set_height(ui_humidity_arc, 240);
    lv_obj_set_align(ui_humidity_arc, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_humidity_arc, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(ui_humidity_arc, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                      LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_arc_set_value(ui_humidity_arc, 30);
    lv_arc_set_bg_angles(ui_humidity_arc, 290, 345);
    lv_arc_set_mode(ui_humidity_arc, LV_ARC_MODE_REVERSE);
    lv_arc_set_range(ui_humidity_arc, 0, 100);
    lv_arc_set_rotation(ui_humidity_arc, 1);
    lv_obj_set_style_arc_width(ui_humidity_arc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_humidity_arc, lv_color_hex(0x60AEF7), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_humidity_arc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_humidity_arc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_humidity_arc, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_humidity_arc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_humidity_icon = lv_img_create(ui_humidity_arc);
    lv_img_set_src(ui_humidity_icon, ZSW_LV_IMG_USE(ui_img_temperatures_png));
    lv_obj_set_width(ui_humidity_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_humidity_icon, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_humidity_icon, 70);
    lv_obj_set_y(ui_humidity_icon, -68);
    lv_obj_set_align(ui_humidity_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_humidity_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_humidity_icon, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_img_recolor(ui_humidity_icon, lv_color_hex(0xDADADA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_humidity_icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_watch_temperature_label = lv_label_create(ui_humidity_arc);
    lv_obj_set_width(ui_watch_temperature_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_watch_temperature_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_watch_temperature_label, 86);
    lv_obj_set_y(ui_watch_temperature_label, -51);
    lv_obj_set_align(ui_watch_temperature_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_watch_temperature_label, "-°");
    lv_obj_clear_flag(ui_watch_temperature_label,
                      LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                      LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_font(ui_watch_temperature_label, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_time = lv_obj_create(ui_digital_watchface);
    lv_obj_set_width(ui_time, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_time, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_time, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_time, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_time, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_time, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_bg_color(ui_time, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_time, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_min_label = lv_label_create(ui_time);
    lv_obj_set_width(ui_min_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_min_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_min_label, 31);
    lv_obj_set_y(ui_min_label, -1);
    lv_obj_set_align(ui_min_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_min_label, "");
    lv_label_set_recolor(ui_min_label, true);
    lv_obj_clear_flag(ui_min_label, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_font(ui_min_label, &ui_font_aliean_47, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_colon_label = lv_label_create(ui_time);
    lv_obj_set_width(ui_colon_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_colon_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_colon_label, 13);
    lv_obj_set_y(ui_colon_label, -32);
    lv_obj_set_align(ui_colon_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_colon_label, ":");
    lv_label_set_recolor(ui_colon_label, true);
    lv_obj_clear_flag(ui_colon_label, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_color(ui_colon_label, lv_color_hex(0xFF8600), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_colon_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_colon_label, &ui_font_aliean_47, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_hour_label = lv_label_create(ui_time);
    lv_obj_set_width(ui_hour_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_hour_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_hour_label, -60);
    lv_obj_set_y(ui_hour_label, 0);
    lv_obj_set_align(ui_hour_label, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_hour_label, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_hour_label, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_label_set_text(ui_hour_label, "");
    lv_label_set_recolor(ui_hour_label, true);
    lv_obj_add_flag(ui_hour_label, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(ui_hour_label, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SNAPPABLE |
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_font(ui_hour_label, &ui_font_aliean_47, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_sec_label = lv_label_create(ui_time);
    lv_obj_set_width(ui_sec_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_sec_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_sec_label, 31);
    lv_obj_set_y(ui_sec_label, -1);
    lv_obj_set_align(ui_sec_label, LV_ALIGN_BOTTOM_RIGHT);
    lv_label_set_text(ui_sec_label, "");
    lv_label_set_recolor(ui_sec_label, true);
    lv_obj_clear_flag(ui_sec_label, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_color(ui_sec_label, lv_color_hex(0xFF8600), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_sec_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_sec_label, &ui_font_aliean_25, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battery_arc = lv_arc_create(ui_digital_watchface);
    lv_obj_set_width(ui_battery_arc, 50);
    lv_obj_set_height(ui_battery_arc, 50);
    lv_obj_set_x(ui_battery_arc, 52);
    lv_obj_set_y(ui_battery_arc, 67);
    lv_obj_set_align(ui_battery_arc, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_battery_arc, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(ui_battery_arc, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SNAPPABLE |
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_arc_width(ui_battery_arc, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_battery_arc, lv_color_hex(0xFFB140), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_battery_arc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_battery_arc, 3, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_battery_arc, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_battery_arc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_battery_arc_icon = lv_img_create(ui_battery_arc);
    lv_img_set_src(ui_battery_arc_icon, ZSW_LV_IMG_USE(ui_img_charging_png));
    lv_obj_set_width(ui_battery_arc_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_battery_arc_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_battery_arc_icon, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_battery_arc_icon,
                      LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                      LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_img_recolor(ui_battery_arc_icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_battery_arc_icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_battery_percent_label = lv_label_create(ui_battery_arc);
    lv_obj_set_width(ui_battery_percent_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_battery_percent_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_battery_percent_label, 0);
    lv_obj_set_y(ui_battery_percent_label, 20);
    lv_obj_set_align(ui_battery_percent_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_battery_percent_label, "100");
    lv_obj_clear_flag(ui_battery_percent_label,
                      LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                      LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_font(ui_battery_percent_label, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_step_arc = lv_arc_create(ui_digital_watchface);
    lv_obj_set_width(ui_step_arc, 50);
    lv_obj_set_height(ui_step_arc, 50);
    lv_obj_set_x(ui_step_arc, -52);
    lv_obj_set_y(ui_step_arc, 67);
    lv_obj_set_align(ui_step_arc, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_step_arc, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(ui_step_arc, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SNAPPABLE |
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_arc_width(ui_step_arc, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_step_arc, lv_color_hex(0x9D3BE0), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_step_arc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_step_arc, 3, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_arc_set_range(ui_step_arc, 0, 10000);

    lv_obj_set_style_bg_color(ui_step_arc, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_step_arc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_step_arc_icon = lv_img_create(ui_step_arc);
    lv_img_set_src(ui_step_arc_icon, ZSW_LV_IMG_USE(ui_img_running_png));
    lv_obj_set_width(ui_step_arc_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_step_arc_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_step_arc_icon, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_step_arc_icon, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_img_recolor(ui_step_arc_icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_step_arc_icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_step_arc_label = lv_label_create(ui_step_arc);
    lv_obj_set_width(ui_step_arc_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_step_arc_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_step_arc_label, 0);
    lv_obj_set_y(ui_step_arc_label, 20);
    lv_obj_set_align(ui_step_arc_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_step_arc_label, "");
    lv_obj_clear_flag(ui_step_arc_label,
                      LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                      LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_font(ui_step_arc_label, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_top_panel = lv_obj_create(ui_digital_watchface);
    lv_obj_set_width(ui_top_panel, lv_pct(100));
    lv_obj_set_height(ui_top_panel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_top_panel, 0);
    lv_obj_set_y(ui_top_panel, -70);
    lv_obj_set_align(ui_top_panel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_top_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_top_panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_top_panel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_bg_color(ui_top_panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_top_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_top_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_top_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_top_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_top_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_top_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_top_panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_top_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_day_label = lv_label_create(ui_top_panel);
    lv_obj_set_width(ui_day_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_day_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_day_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_day_label, "");
    lv_obj_set_style_text_color(ui_day_label, lv_color_hex(0xA3A1A1), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_day_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_day_label, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_date_label = lv_label_create(ui_top_panel);
    lv_obj_set_width(ui_date_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_date_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_date_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_date_label, "");
    lv_obj_set_style_text_color(ui_date_label, lv_color_hex(0xFF8600), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_date_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_date_label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_notifications = lv_obj_create(ui_top_panel);
    lv_obj_set_width(ui_notifications, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_notifications, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_notifications, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_notifications, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_notifications, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_notifications, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_notifications, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_notifications, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_notifications, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_notifications, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_notifications, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_notifications, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_notifications, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_notifications, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_notification_icon = lv_img_create(ui_notifications);
    lv_img_set_src(ui_notification_icon, ZSW_LV_IMG_USE(ui_img_chat_png));
    lv_obj_set_width(ui_notification_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_notification_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_notification_icon, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_notification_icon, LV_OBJ_FLAG_SCROLLABLE);

    ui_notification_count_label = lv_label_create(ui_notification_icon);
    lv_obj_set_width(ui_notification_count_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_notification_count_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_notification_count_label, -3);
    lv_obj_set_y(ui_notification_count_label, -3);
    lv_obj_set_align(ui_notification_count_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_notification_count_label, "");
    lv_obj_set_style_text_font(ui_notification_count_label, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_bt_icon = lv_img_create(ui_notifications);
    lv_img_set_src(ui_bt_icon, ZSW_LV_IMG_USE(ui_img_bluetooth_png));
    lv_obj_set_width(ui_bt_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_bt_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_bt_icon, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_bt_icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_img_recolor(ui_bt_icon, lv_color_hex(0x0082FC), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_bt_icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_weather_temperature_label = lv_label_create(ui_digital_watchface);
    lv_obj_set_width(ui_weather_temperature_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_weather_temperature_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_weather_temperature_label, 12);
    lv_obj_set_y(ui_weather_temperature_label, 95);
    lv_obj_set_align(ui_weather_temperature_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_weather_temperature_label, "-°");
    lv_obj_clear_flag(ui_weather_temperature_label,
                      LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);

    ui_weather_icon = lv_img_create(ui_digital_watchface);
    lv_color_t icon_color;

    // Just use a default dummy image by default
    const lv_img_dsc_t *icon = zsw_ui_utils_icon_from_weather_code(802, &icon_color);
    lv_img_set_src(ui_weather_icon, icon);
    lv_obj_set_width(ui_weather_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_weather_icon, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_weather_icon, -12);
    lv_obj_set_y(ui_weather_icon, 95);
    lv_obj_set_align(ui_weather_icon, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_weather_icon, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                      LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_img_recolor(ui_weather_icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_weather_icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

#if defined(CONFIG_LV_Z_USE_FILESYSTEM)
    if (settings->animations_on) {
        lv_obj_t *img = lv_gif_create(ui_digital_watchface);
        lv_gif_set_src(img, "/lvgl_lfs/snoopy.gif");
        lv_obj_set_align(img, LV_ALIGN_CENTER);
        lv_obj_set_width(img, LV_SIZE_CONTENT);
        lv_obj_set_height(img, LV_SIZE_CONTENT);
        lv_obj_set_y(img, 45);
    }
#endif

    // Listeners
    lv_obj_add_event_cb(ui_battery_arc, arc_event_pressed, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_step_arc, arc_event_pressed, LV_EVENT_CLICKED, NULL);
}

static void watchface_remove(void)
{
    if (!root_page) {
        return;
    }
    lv_obj_del(root_page);
    root_page = NULL;
}

static void watchface_set_battery_percent(int32_t percent, int32_t value)
{
    if (!root_page) {
        return;
    }
    lv_arc_set_value(ui_battery_arc, percent);
    lv_label_set_text_fmt(ui_battery_percent_label, "%d", value);
}

static void watchface_set_hrm(int32_t value)
{
    if (!root_page) {
        return;
    }
}

static void watchface_set_step(int32_t value)
{
    if (!root_page) {
        return;
    }
    lv_arc_set_value(ui_step_arc, value);
    lv_label_set_text_fmt(ui_step_arc_label, "%d", value);
}

static void watchface_set_time(int32_t hour, int32_t minute, int32_t second, uint32_t usec)
{
    if (!root_page) {
        return;
    }

    if (last_hour != hour) {
        lv_label_set_text_fmt(ui_min_label, "%02d", hour);
        last_hour = hour;
    }
    if (last_minute != minute) {
        lv_label_set_text_fmt(ui_hour_label, "%02d", minute);
        last_minute = minute;
    }
    if (last_second != second) {
        lv_label_set_text_fmt(ui_sec_label, "%02d", second);
        last_second = second;
    }
}

static void watchface_set_num_notifcations(int32_t value)
{
    if (!root_page) {
        return;
    }

    if (value == last_num_not) {
        return;
    }

    if (value > 0) {
        lv_label_set_text_fmt(ui_notification_count_label, "%d", value);
        lv_obj_clear_flag(ui_notification_icon, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui_notification_icon, LV_OBJ_FLAG_HIDDEN);
    }
}

static void watchface_set_ble_connected(bool connected)
{
    if (!root_page) {
        return;
    }

    if (connected) {
        lv_obj_clear_flag(ui_bt_icon, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui_bt_icon, LV_OBJ_FLAG_HIDDEN);
    }
}

static void watchface_set_weather(int8_t temperature, int weather_code)
{
    if (!root_page) {
        return;
    }
    lv_color_t icon_color;
    const lv_img_dsc_t *icon;

    lv_label_set_text_fmt(ui_weather_temperature_label, "%d°", temperature);
    icon = zsw_ui_utils_icon_from_weather_code(weather_code, &icon_color);
    lv_img_set_src(ui_weather_icon, icon);

    lv_obj_set_style_img_recolor_opa(ui_weather_icon, LV_OPA_COVER, 0);
    lv_obj_set_style_img_recolor(ui_weather_icon, icon_color, 0);
}

static void watchface_set_date(int day_of_week, int date)
{
    if (!root_page) {
        return;
    }
    char *days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

    lv_label_set_text_fmt(ui_day_label, "%s", days[day_of_week]);
    lv_label_set_text_fmt(ui_date_label, "%d", date);
}

#ifdef CONFIG_EXTERNAL_USE_BOSCH_BSEC
static lv_color_t color_from_iaq(int iaq)
{
    lv_color_t color = lv_palette_main(LV_PALETTE_INDIGO);
    if (iaq < 51) {
        color = lv_color_hex(0x00E400);
    } else if (iaq < 101) {
        color = lv_color_hex(0x92D050);
    } else if (iaq < 151) {
        color = lv_color_hex(0xFFFF00);
    } else if (iaq < 201) {
        color = lv_color_hex(0xFF7E00);
    } else if (iaq < 251) {
        color = lv_color_hex(0xFF0000);
    } else if (iaq < 351) {
        color = lv_color_hex(0x99004C);
    } else {
        color = lv_color_hex(0x663300);
    }
    return color;
}

static lv_color_t color_from_co2(int iaq)
{
    lv_color_t color = lv_palette_main(LV_PALETTE_INDIGO);
    if (iaq < 600) {
        color = lv_color_hex(0x1CB149);
    } else if (iaq < 900) {
        color = lv_color_hex(0x97C93A);
    } else if (iaq < 1600) {
        color = lv_color_hex(0xF9C510);
    } else if (iaq < 2000) {
        color = lv_color_hex(0xF39027);
    } else {
        color = lv_color_hex(0xEB1E27);
    }
    return color;
}
#endif

static void watchface_set_watch_env_sensors(int temperature, int humidity, int pressure, float iaq, float co2)
{
    if (!root_page) {
        return;
    }

    // Compensate for ui_humidity_arc is using REVERSE mode, hence lv_arc_get_max_value is needed
    lv_arc_set_value(ui_humidity_arc, lv_arc_get_max_value(ui_humidity_arc) - humidity);
    lv_label_set_text_fmt(ui_watch_temperature_label, "%d°", temperature);

#ifdef CONFIG_EXTERNAL_USE_BOSCH_BSEC
    lv_arc_set_value(ui_iaq_or_pressure_arc, iaq);
    lv_obj_set_style_arc_color(ui_iaq_or_pressure_arc, color_from_iaq(iaq), LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_arc_set_value(ui_co2_arc, co2);
    lv_obj_set_style_arc_color(ui_co2_arc, color_from_co2(co2), LV_PART_INDICATOR | LV_STATE_DEFAULT);
#else
    lv_arc_set_range(ui_iaq_or_pressure_arc, 950, 1050);
    lv_arc_set_value(ui_iaq_or_pressure_arc, pressure / 100);
#endif
}

static void watchface_ui_invalidate_cached(void)
{
    last_hour = -1;
    last_minute = -1;
    last_num_not = -1;
    last_second = -1;
}

static void arc_event_pressed(lv_event_t *e)
{
    if (lv_event_get_target(e) == ui_battery_arc) {
        ui_evt_cb(WATCHFACE_APP_EVT_CLICK_BATT);
    } else if (lv_event_get_target(e) == ui_step_arc) {
        ui_evt_cb(WATCHFACE_APP_EVT_CLICK_STEP);
    }
}

static watchface_ui_api_t ui_api = {
    .show = watchface_show,
    .remove = watchface_remove,
    .set_battery_percent = watchface_set_battery_percent,
    .set_hrm = watchface_set_hrm,
    .set_step = watchface_set_step,
    .set_time = watchface_set_time,
    .set_ble_connected = watchface_set_ble_connected,
    .set_num_notifcations = watchface_set_num_notifcations,
    .set_weather = watchface_set_weather,
    .set_date = watchface_set_date,
    .set_watch_env_sensors = watchface_set_watch_env_sensors,
    .ui_invalidate_cached = watchface_ui_invalidate_cached,
};

static int watchface_init(void)
{
    watchface_app_register_ui(&ui_api);

    return 0;
}

SYS_INIT(watchface_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
