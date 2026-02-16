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

/* Zephyr kernel */
EXPORT_SYMBOL(k_msleep);
EXPORT_SYMBOL(k_malloc);
EXPORT_SYMBOL(k_free);
EXPORT_SYMBOL(printk);

/* ZSWatch app manager */
EXPORT_SYMBOL(zsw_app_manager_add_application);
EXPORT_SYMBOL(zsw_app_manager_app_close_request);
EXPORT_SYMBOL(zsw_app_manager_exit_app);
EXPORT_SYMBOL(zsw_app_manager_get_num_apps);

/* LVGL core — curated subset, add more as needed by apps.
 * Note: lv_obj_clear_flag is a macro alias for lv_obj_remove_flag (lv_api_map_v8.h),
 * so we only export lv_obj_remove_flag.
 */
EXPORT_SYMBOL(lv_obj_create);
EXPORT_SYMBOL(lv_obj_delete);
EXPORT_SYMBOL(lv_obj_set_size);
EXPORT_SYMBOL(lv_obj_set_width);
EXPORT_SYMBOL(lv_obj_set_height);
EXPORT_SYMBOL(lv_obj_set_style_bg_color);
EXPORT_SYMBOL(lv_obj_set_style_bg_opa);
EXPORT_SYMBOL(lv_obj_set_style_border_width);
EXPORT_SYMBOL(lv_obj_set_style_text_font);
EXPORT_SYMBOL(lv_obj_set_style_text_color);
EXPORT_SYMBOL(lv_obj_set_style_text_align);
EXPORT_SYMBOL(lv_obj_set_style_pad_left);
EXPORT_SYMBOL(lv_obj_set_style_pad_right);
EXPORT_SYMBOL(lv_obj_set_style_pad_top);
EXPORT_SYMBOL(lv_obj_set_style_pad_bottom);
EXPORT_SYMBOL(lv_obj_set_style_pad_row);
EXPORT_SYMBOL(lv_obj_set_style_pad_column);
EXPORT_SYMBOL(lv_obj_set_align);
EXPORT_SYMBOL(lv_obj_align);
EXPORT_SYMBOL(lv_obj_set_flex_flow);
EXPORT_SYMBOL(lv_obj_set_flex_align);
EXPORT_SYMBOL(lv_obj_set_scroll_dir);
EXPORT_SYMBOL(lv_obj_set_scrollbar_mode);
EXPORT_SYMBOL(lv_obj_remove_flag);
EXPORT_SYMBOL(lv_obj_add_flag);
EXPORT_SYMBOL(lv_obj_remove_style_all);

EXPORT_SYMBOL(lv_label_create);
EXPORT_SYMBOL(lv_label_set_text);
EXPORT_SYMBOL(lv_label_set_text_fmt);

EXPORT_SYMBOL(lv_image_create);
EXPORT_SYMBOL(lv_image_set_src);

EXPORT_SYMBOL(lv_color_make);
EXPORT_SYMBOL(lv_color_white);

/* Zbus runtime API */
EXPORT_SYMBOL(zbus_chan_read);
EXPORT_SYMBOL(zbus_chan_add_obs);
EXPORT_SYMBOL(zbus_chan_rm_obs);
EXPORT_SYMBOL(zbus_chan_pub);

/* Zbus channels — so LLEXT apps can observe/read them */
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
