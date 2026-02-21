/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
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

/*
 * Central export table for LLEXT dynamic apps.
 * All symbols that LLEXT apps need to call must be exported here.
 */

#include <zephyr/llext/symbol.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/zbus/zbus.h>
#include <lvgl.h>

#include "managers/zsw_app_manager.h"
#include "events/battery_event.h"
#include "events/ble_event.h"
#include "events/activity_event.h"
#include "events/periodic_event.h"
#include "events/accel_event.h"
#include "events/magnetometer_event.h"
#include "events/pressure_event.h"
#include "events/light_event.h"
#include "events/music_event.h"
#include "ui/utils/zsw_ui_utils.h"
#include "history/zsw_history.h"
#include "filesystem/zsw_filesystem.h"

#include <zephyr/settings/settings.h>

#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
#include "fuel_gauge/zsw_pmic.h"
#endif

// TODO: Maybe move exports to the actual files, at least for zsw exports.

/* ---- Zephyr kernel ---- */
EXPORT_SYMBOL(k_msleep);
EXPORT_SYMBOL(k_malloc);
EXPORT_SYMBOL(k_free);
EXPORT_SYMBOL(k_uptime_get);
EXPORT_SYMBOL(printk);

/* ---- LLEXT logging ---- */
extern void zsw_llext_log(uint8_t level, const char *fmt, ...);
EXPORT_SYMBOL(zsw_llext_log);

/* ---- ZSWatch app manager ---- */
EXPORT_SYMBOL(zsw_app_manager_add_application);
EXPORT_SYMBOL(zsw_app_manager_app_close_request);
EXPORT_SYMBOL(zsw_app_manager_exit_app);
EXPORT_SYMBOL(zsw_app_manager_get_num_apps);

/* ---- LVGL core objects ---- */
EXPORT_SYMBOL(lv_obj_create);
EXPORT_SYMBOL(lv_obj_delete);
EXPORT_SYMBOL(lv_obj_set_size);
EXPORT_SYMBOL(lv_obj_set_width);
EXPORT_SYMBOL(lv_obj_set_height);
EXPORT_SYMBOL(lv_obj_set_x);
EXPORT_SYMBOL(lv_obj_set_y);
EXPORT_SYMBOL(lv_obj_set_align);
EXPORT_SYMBOL(lv_obj_align);
EXPORT_SYMBOL(lv_obj_set_flex_flow);
EXPORT_SYMBOL(lv_obj_set_flex_align);
EXPORT_SYMBOL(lv_obj_set_scroll_dir);
EXPORT_SYMBOL(lv_obj_set_scrollbar_mode);
EXPORT_SYMBOL(lv_obj_remove_flag);
EXPORT_SYMBOL(lv_obj_add_flag);
EXPORT_SYMBOL(lv_obj_remove_style_all);
EXPORT_SYMBOL(lv_obj_add_event_cb);

/* ---- LVGL style setters ---- */
EXPORT_SYMBOL(lv_obj_set_style_bg_color);
EXPORT_SYMBOL(lv_obj_set_style_bg_opa);
EXPORT_SYMBOL(lv_obj_set_style_border_width);
EXPORT_SYMBOL(lv_obj_set_style_border_color);
EXPORT_SYMBOL(lv_obj_set_style_border_opa);
EXPORT_SYMBOL(lv_obj_set_style_text_font);
EXPORT_SYMBOL(lv_obj_set_style_text_color);
EXPORT_SYMBOL(lv_obj_set_style_text_align);
EXPORT_SYMBOL(lv_obj_set_style_text_opa);

/* ---- LVGL fonts ---- */
EXPORT_SYMBOL(lv_font_montserrat_10);
EXPORT_SYMBOL(lv_font_montserrat_12);
EXPORT_SYMBOL(lv_font_montserrat_16);
EXPORT_SYMBOL(lv_font_montserrat_18);

EXPORT_SYMBOL(lv_obj_set_style_pad_left);
EXPORT_SYMBOL(lv_obj_set_style_pad_right);
EXPORT_SYMBOL(lv_obj_set_style_pad_top);
EXPORT_SYMBOL(lv_obj_set_style_pad_bottom);
EXPORT_SYMBOL(lv_obj_set_style_pad_row);
EXPORT_SYMBOL(lv_obj_set_style_pad_column);
EXPORT_SYMBOL(lv_obj_set_style_pad_gap);
EXPORT_SYMBOL(lv_obj_set_style_line_color);
EXPORT_SYMBOL(lv_obj_set_style_line_opa);
EXPORT_SYMBOL(lv_obj_set_style_line_width);
EXPORT_SYMBOL(lv_obj_set_style_size);
EXPORT_SYMBOL(lv_obj_set_style_width);
EXPORT_SYMBOL(lv_obj_set_style_height);

/* ---- LVGL color (non-inline helpers) ---- */
EXPORT_SYMBOL(lv_color_hex);

/* ---- Zephyr internal ---- */
EXPORT_SYMBOL(assert_post_action);

/* ---- Picolibc assert (used by C stdlib assert()) ---- */
extern void __assert_no_args(void);
EXPORT_SYMBOL(__assert_no_args);

/* ---- LVGL label ---- */
EXPORT_SYMBOL(lv_label_create);
EXPORT_SYMBOL(lv_label_set_text);
EXPORT_SYMBOL(lv_label_set_text_fmt);

/* ---- LVGL object tree / scroll / position (additional) ---- */
EXPORT_SYMBOL(lv_obj_align_to);
EXPORT_SYMBOL(lv_obj_get_parent);

/* ---- LVGL image ---- */
EXPORT_SYMBOL(lv_image_create);
EXPORT_SYMBOL(lv_image_set_src);

/* ---- LVGL chart ---- */
EXPORT_SYMBOL(lv_chart_create);
EXPORT_SYMBOL(lv_chart_set_type);
EXPORT_SYMBOL(lv_chart_set_point_count);
EXPORT_SYMBOL(lv_chart_set_axis_range);
EXPORT_SYMBOL(lv_chart_set_div_line_count);
EXPORT_SYMBOL(lv_chart_add_series);
EXPORT_SYMBOL(lv_chart_set_next_value);

/* ---- LVGL style (additional) ---- */
EXPORT_SYMBOL(lv_obj_set_style_radius);

/* ---- LVGL tileview ---- */
EXPORT_SYMBOL(lv_tileview_create);
EXPORT_SYMBOL(lv_tileview_add_tile);
EXPORT_SYMBOL(lv_tileview_get_tile_active);

/* ---- LVGL scale ---- */
EXPORT_SYMBOL(lv_scale_create);
EXPORT_SYMBOL(lv_scale_set_mode);
EXPORT_SYMBOL(lv_scale_set_range);
EXPORT_SYMBOL(lv_scale_set_total_tick_count);
EXPORT_SYMBOL(lv_scale_set_major_tick_every);
EXPORT_SYMBOL(lv_scale_set_label_show);

/* ---- LVGL LED ---- */
EXPORT_SYMBOL(lv_led_create);
EXPORT_SYMBOL(lv_led_set_color);
EXPORT_SYMBOL(lv_led_off);

/* ---- LVGL draw helpers ---- */
EXPORT_SYMBOL(lv_draw_task_get_draw_dsc);
EXPORT_SYMBOL(lv_draw_task_get_label_dsc);
EXPORT_SYMBOL(lv_event_get_draw_task);

/* ---- LVGL string / memory ---- */
EXPORT_SYMBOL(lv_snprintf);
EXPORT_SYMBOL(lv_strdup);
EXPORT_SYMBOL(lv_free);

/* ---- LVGL color (non-inline helpers) ---- */
EXPORT_SYMBOL(lv_color_make);
EXPORT_SYMBOL(lv_color_white);

/* ---- LVGL event ---- */
EXPORT_SYMBOL(lv_event_get_code);

/* ---- Zbus runtime API ---- */
EXPORT_SYMBOL(zbus_chan_read);
EXPORT_SYMBOL(zbus_chan_add_obs);
EXPORT_SYMBOL(zbus_chan_rm_obs);
EXPORT_SYMBOL(zbus_chan_pub);

/* ---- Zbus channels ---- */
ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_CHAN_DECLARE(activity_state_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_CHAN_DECLARE(periodic_event_100ms_chan);
ZBUS_CHAN_DECLARE(accel_data_chan);
ZBUS_CHAN_DECLARE(magnetometer_data_chan);
ZBUS_CHAN_DECLARE(pressure_data_chan);
ZBUS_CHAN_DECLARE(light_data_chan);
ZBUS_CHAN_DECLARE(music_control_data_chan);

EXPORT_SYMBOL(battery_sample_data_chan);
EXPORT_SYMBOL(ble_comm_data_chan);
EXPORT_SYMBOL(activity_state_data_chan);
EXPORT_SYMBOL(periodic_event_1s_chan);
EXPORT_SYMBOL(periodic_event_10s_chan);
EXPORT_SYMBOL(periodic_event_100ms_chan);
EXPORT_SYMBOL(accel_data_chan);
EXPORT_SYMBOL(magnetometer_data_chan);
EXPORT_SYMBOL(pressure_data_chan);
EXPORT_SYMBOL(light_data_chan);
EXPORT_SYMBOL(music_control_data_chan);

/* ---- ZSWatch UI utils ---- */
EXPORT_SYMBOL(zsw_ui_utils_seconds_to_day_hour_min);

/* ---- ZSWatch filesystem (about app) ---- */
EXPORT_SYMBOL(zsw_filesytem_get_total_size);
EXPORT_SYMBOL(zsw_filesytem_get_num_rawfs_files);

/* ---- ZSWatch PMIC (conditional) ---- */
#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
EXPORT_SYMBOL(zsw_pmic_charger_status_str);
EXPORT_SYMBOL(zsw_pmic_charger_error_str);
#endif

/* ---- ARM EABI compiler runtime helpers (float/double math) ---- */
extern double __aeabi_i2d(int);
extern double __aeabi_f2d(float);
extern double __aeabi_ddiv(double, double);
extern int __aeabi_f2iz(float);
extern float __aeabi_ui2f(unsigned int);
EXPORT_SYMBOL(__aeabi_i2d);
EXPORT_SYMBOL(__aeabi_f2d);
EXPORT_SYMBOL(__aeabi_ddiv);
EXPORT_SYMBOL(__aeabi_f2iz);
EXPORT_SYMBOL(__aeabi_ui2f);

/* ---- LVGL misc ---- */
EXPORT_SYMBOL(lv_log_add);
EXPORT_SYMBOL(lv_pct);

/* ---- Zephyr kernel (additional) ---- */
EXPORT_SYMBOL(z_impl_k_uptime_ticks);

/* ---- Zephyr settings API (for LLEXT apps linked with settings code) ---- */
EXPORT_SYMBOL(settings_subsys_init);
EXPORT_SYMBOL(settings_delete);

/* ---- ZSWatch history API ---- */
EXPORT_SYMBOL(zsw_history_init);
EXPORT_SYMBOL(zsw_history_add);
EXPORT_SYMBOL(zsw_history_del);
EXPORT_SYMBOL(zsw_history_get);
EXPORT_SYMBOL(zsw_history_load);
EXPORT_SYMBOL(zsw_history_save);
EXPORT_SYMBOL(zsw_history_samples);

/* ---- Display control (QR code app) ---- */
#include "drivers/zsw_display_control.h"
EXPORT_SYMBOL(zsw_display_control_get_brightness);
EXPORT_SYMBOL(zsw_display_control_set_brightness);

/* ---- Sensor fusion / magnetometer (compass app) ---- */
#include "sensor_fusion/zsw_sensor_fusion.h"
#include "zsw_magnetometer.h"
EXPORT_SYMBOL(zsw_sensor_fusion_init);
EXPORT_SYMBOL(zsw_sensor_fusion_deinit);
EXPORT_SYMBOL(zsw_sensor_fusion_get_heading);
EXPORT_SYMBOL(zsw_magnetometer_start_calibration);
EXPORT_SYMBOL(zsw_magnetometer_stop_calibration);

/* ---- Popup window (compass app) ---- */
#include "ui/popup/zsw_popup_window.h"
EXPORT_SYMBOL(zsw_popup_show);
EXPORT_SYMBOL(zsw_popup_show_with_icon);
EXPORT_SYMBOL(zsw_popup_remove);

/* ---- LVGL timer ---- */
EXPORT_SYMBOL(lv_timer_create);
EXPORT_SYMBOL(lv_timer_delete);

/* ---- LVGL image (additional) ---- */
EXPORT_SYMBOL(lv_image_set_pivot);
EXPORT_SYMBOL(lv_image_set_rotation);

/* ---- LVGL button ---- */
EXPORT_SYMBOL(lv_button_create);

/* ---- LVGL object (additional) ---- */
EXPORT_SYMBOL(lv_obj_set_pos);
EXPORT_SYMBOL(lv_obj_center);
EXPORT_SYMBOL(lv_obj_set_flex_grow);
EXPORT_SYMBOL(lv_obj_set_user_data);
EXPORT_SYMBOL(lv_obj_get_user_data);
EXPORT_SYMBOL(lv_obj_has_flag);
EXPORT_SYMBOL(lv_obj_set_style_pad_all);
EXPORT_SYMBOL(lv_obj_set_style_shadow_width);
EXPORT_SYMBOL(lv_obj_set_style_arc_color);
EXPORT_SYMBOL(lv_obj_set_style_arc_opa);

/* ---- LVGL color (additional) ---- */
EXPORT_SYMBOL(lv_palette_main);
EXPORT_SYMBOL(lv_color_black);

/* ---- LVGL tick ---- */
EXPORT_SYMBOL(lv_tick_get);
EXPORT_SYMBOL(lv_tick_elaps);

/* ---- LVGL label (additional) ---- */
EXPORT_SYMBOL(lv_label_set_long_mode);

/* ---- LVGL event (additional) ---- */
EXPORT_SYMBOL(lv_event_get_target);
EXPORT_SYMBOL(lv_event_get_target_obj);
EXPORT_SYMBOL(lv_event_get_user_data);

/* ---- LVGL msgbox ---- */
EXPORT_SYMBOL(lv_msgbox_create);
EXPORT_SYMBOL(lv_msgbox_add_text);
EXPORT_SYMBOL(lv_msgbox_add_footer_button);
EXPORT_SYMBOL(lv_msgbox_close);

/* ---- LVGL spinner ---- */
EXPORT_SYMBOL(lv_spinner_create);
EXPORT_SYMBOL(lv_spinner_set_anim_params);

/* ---- LLEXT runtime trampoline API ---- */
#include "managers/zsw_llext_iflash.h"
EXPORT_SYMBOL(zsw_llext_create_trampoline);

/* ---- Kernel work queue (calculator, weather) ---- */
EXPORT_SYMBOL(k_work_init);
EXPORT_SYMBOL(k_work_submit);
EXPORT_SYMBOL(k_work_cancel);
EXPORT_SYMBOL(k_work_init_delayable);
EXPORT_SYMBOL(k_work_reschedule);
EXPORT_SYMBOL(k_work_cancel_delayable);

/* ---- Kernel thread (calculator) ---- */
EXPORT_SYMBOL(k_thread_create);

/* ---- Kernel message queue (calculator) ---- */
EXPORT_SYMBOL(k_msgq_init);
EXPORT_SYMBOL(k_msgq_put);
EXPORT_SYMBOL(k_msgq_get);

/* ---- Zephyr SMF (calculator) ---- */
#include <zephyr/smf.h>
EXPORT_SYMBOL(smf_set_initial);
EXPORT_SYMBOL(smf_run_state);
EXPORT_SYMBOL(smf_set_state);

/* ---- C library (calculator, weather, trivia) ---- */
EXPORT_SYMBOL(strtod);
EXPORT_SYMBOL(snprintf);
EXPORT_SYMBOL(sprintf);
EXPORT_SYMBOL(strlen);
EXPORT_SYMBOL(strncpy);
EXPORT_SYMBOL(memset);
EXPORT_SYMBOL(memcpy);

/* ---- BLE HTTP (trivia, weather) ---- */
#include "ble/ble_http.h"
#include "ble/ble_comm.h"
EXPORT_SYMBOL(zsw_ble_http_get);
EXPORT_SYMBOL(ble_comm_request_gps_status);

/* ---- cJSON (trivia, weather) ---- */
#include "cJSON.h"
EXPORT_SYMBOL(cJSON_Parse);
EXPORT_SYMBOL(cJSON_GetObjectItem);
EXPORT_SYMBOL(cJSON_GetArrayItem);
EXPORT_SYMBOL(cJSON_GetArraySize);
EXPORT_SYMBOL(cJSON_Delete);

/* ---- ZSWatch clock (weather) ---- */
#include "zsw_clock.h"
EXPORT_SYMBOL(zsw_clock_get_time);

/* ---- ZSWatch weather utils (weather) ---- */
EXPORT_SYMBOL(zsw_ui_utils_icon_from_wmo_weather_code);
EXPORT_SYMBOL(wmo_code_to_weather_code);

/* ---- ARM EABI (additional float/double helpers) ---- */
extern double __aeabi_dmul(double, double);
extern double __aeabi_dsub(double, double);
extern double __aeabi_dadd(double, double);
extern int __aeabi_dcmpeq(double, double);
extern int __aeabi_dcmplt(double, double);
extern int __aeabi_dcmple(double, double);
extern int __aeabi_dcmpge(double, double);
extern int __aeabi_dcmpgt(double, double);
extern int __aeabi_d2iz(double);
extern float __aeabi_d2f(double);
extern float __aeabi_fmul(float, float);
extern float __aeabi_fdiv(float, float);
extern float __aeabi_fadd(float, float);
extern float __aeabi_fsub(float, float);
extern int __aeabi_fcmpeq(float, float);
extern int __aeabi_fcmplt(float, float);
extern int __aeabi_fcmple(float, float);
extern int __aeabi_fcmpge(float, float);
extern int __aeabi_fcmpgt(float, float);
extern double __aeabi_ul2d(unsigned long long);
extern double __aeabi_l2d(long long);
extern float __aeabi_i2f(int);
EXPORT_SYMBOL(__aeabi_dmul);
EXPORT_SYMBOL(__aeabi_dsub);
EXPORT_SYMBOL(__aeabi_dadd);
EXPORT_SYMBOL(__aeabi_dcmpeq);
EXPORT_SYMBOL(__aeabi_dcmplt);
EXPORT_SYMBOL(__aeabi_dcmple);
EXPORT_SYMBOL(__aeabi_dcmpge);
EXPORT_SYMBOL(__aeabi_dcmpgt);
EXPORT_SYMBOL(__aeabi_d2iz);
EXPORT_SYMBOL(__aeabi_d2f);
EXPORT_SYMBOL(__aeabi_fmul);
EXPORT_SYMBOL(__aeabi_fdiv);
EXPORT_SYMBOL(__aeabi_fadd);
EXPORT_SYMBOL(__aeabi_fsub);
EXPORT_SYMBOL(__aeabi_fcmpeq);
EXPORT_SYMBOL(__aeabi_fcmplt);
EXPORT_SYMBOL(__aeabi_fcmple);
EXPORT_SYMBOL(__aeabi_fcmpge);
EXPORT_SYMBOL(__aeabi_fcmpgt);
EXPORT_SYMBOL(__aeabi_ul2d);
EXPORT_SYMBOL(__aeabi_l2d);
EXPORT_SYMBOL(__aeabi_i2f);

extern unsigned int __aeabi_d2uiz(double);
EXPORT_SYMBOL(__aeabi_d2uiz);
