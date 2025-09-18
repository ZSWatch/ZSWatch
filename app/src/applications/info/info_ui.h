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

#pragma once

#include <inttypes.h>
#include <lvgl.h>
#include <zsw_coredump.h>

typedef void(*on_reset_ui_event_cb_t)(void);

void info_ui_show(lv_obj_t *root, on_reset_ui_event_cb_t reset_cb, zsw_coredump_sumary_t *coredumps, int num_coredumps);

void info_ui_remove(void);

void info_ui_set_uptime_sec(uint32_t uptime);

void info_ui_set_resets(uint32_t resets);

void info_ui_set_total_uptime_sec(uint32_t uptime_seconds);

void info_ui_set_wakeup_time_sec(uint64_t total_wake_time, uint32_t percent_used);

void info_ui_set_ref_off_time_sec(uint64_t total_off_time, uint32_t percent_off);

void info_ui_set_time_to_inactive_sec(uint32_t time_left_seconds);

void info_ui_set_mac_addr(char *mac_str);

void info_ui_set_gatt_status(bool cccd_enabled, uint16_t mtu);

void info_app_ui_set_conn_params(uint16_t interval, uint16_t latency, uint16_t timeout);

void info_app_ui_set_conn_mac(char *mac_str);

void info_app_ui_set_conn_security_info(int info, int err);
