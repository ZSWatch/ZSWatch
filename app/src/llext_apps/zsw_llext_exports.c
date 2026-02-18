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

#include <zephyr/settings/settings.h>

#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
#include "fuel_gauge/zsw_pmic.h"
#endif

/* ---- Zephyr kernel ---- */
EXPORT_SYMBOL(k_msleep);
EXPORT_SYMBOL(k_malloc);
EXPORT_SYMBOL(k_free);
EXPORT_SYMBOL(k_uptime_get);
EXPORT_SYMBOL(printk);

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
