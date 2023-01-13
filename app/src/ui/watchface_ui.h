#ifndef __WATCH_UI_H
#define __WATCH_UI_H
#include <inttypes.h>
#include <stdbool.h>

void watchface_init(void);

void watchface_show(void);

void watchface_remove(void);

void watchface_set_battery_percent(int32_t percent, int32_t value);

void watchface_set_hrm(int32_t value);

void watchface_set_step(int32_t value);

void watchface_set_time(int32_t hour, int32_t minute, int32_t second);

void watchface_set_ble_connected(bool connected);

void watchface_set_num_notifcations(int32_t value);

void watchface_set_weather(int8_t temperature, int weather_code);

void watchface_set_date(int day_of_week, int date);

#endif