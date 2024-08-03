#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/rtc.h>

#define MAX_ALARMS 10

typedef void(*alarm_cb)(void* user_data);

typedef struct {
    struct rtc_time expiry_time;
    bool used;
    alarm_cb cb;
    void* user_data;
} zsw_alarm_t;

int zsw_alarm_add(struct rtc_time expiry_time, alarm_cb callback, void* user_data);

int zsw_alarm_add_timer(uint16_t hour, uint16_t min, uint16_t sec, alarm_cb callback, void* user_data);

int zsw_alarm_remove(uint32_t alarm_id);