#pragma once

#include <inttypes.h>
#include <lvgl.h>

typedef void(*on_reset_ui_event_cb_t)(void);

void info_ui_show(lv_obj_t *root, on_reset_ui_event_cb_t reset_cb);

void info_ui_remove(void);

void info_ui_set_uptime_sec(uint32_t uptime);

void info_ui_set_resets(uint32_t resets);

void info_ui_set_total_uptime_sec(uint32_t uptime_seconds);

void info_ui_set_wakeup_time_sec(uint64_t total_wake_time, uint32_t percent_used);

void info_ui_set_ref_off_time_sec(uint64_t total_off_time, uint32_t percent_off);

void info_ui_set_time_to_inactive_sec(uint32_t time_left_seconds);

void info_ui_set_mac_addr(char *mac_str);
