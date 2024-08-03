#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/rtc.h>

#define ZSW_MAX_ALARMS 20

typedef void(*alarm_cb)(void* user_data);

typedef struct {
    struct rtc_time expiry_time;
    bool used;
    bool enabled;
    alarm_cb cb;
    void* user_data;
} zsw_alarm_t;

int zsw_alarm_add(struct rtc_time expiry_time, alarm_cb callback, void* user_data);

int zsw_alarm_add_timer(uint16_t hour, uint16_t min, uint16_t sec, alarm_cb callback, void* user_data);

int zsw_alarm_get_remaining(uint32_t alarm_id, uint32_t* hour, uint32_t* min, uint32_t* sec);

int zsw_alarm_set_enabled(uint32_t alarm_id, bool enabled);

int zsw_alarm_get_enabled(uint32_t alarm_id, bool* enabled);

int zsw_alarm_remove(uint32_t alarm_id);