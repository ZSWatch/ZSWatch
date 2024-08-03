#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/settings/settings.h>

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "timer_ui.h"
#include "zsw_alarm.h"

LOG_MODULE_REGISTER(timer_app, LOG_LEVEL_DBG);

#define SETTING_TIMERS_LIST    "timer_app/timers"

static void timer_app_start(lv_obj_t *root, lv_group_t *group);
static void timer_app_stop(void);

static void on_timer_created_cb(uint32_t hour, uint32_t min, uint32_t sec);
static void on_timer_event_cb(timer_event_type_t type, uint32_t timer_id);

ZSW_LV_IMG_DECLARE(battery_app_icon);

static application_t app = {
    .name = "Timer",
    .icon = ZSW_LV_IMG_USE(battery_app_icon),
    .start_func = timer_app_start,
    .stop_func = timer_app_stop
};

static int timer_index = 0;
static timer_app_timer_t timers[TIMER_UI_MAX_TIMERS];

static void timer_app_start(lv_obj_t *root, lv_group_t *group)
{
    // Load timers from flash
    // Load alarms from flash
    timer_ui_show(root, on_timer_created_cb, on_timer_event_cb);
    /*
    for (int i = 0; i < 2; i++) {
        timer_app_timer_t timer = {
            .timer_id = timer_index,
            .hour = (i+1)*2,
            .min = (i+1)*3,
            .sec = (i+1)*4,
            .state = TIMER_STATE_STOPPED
        };
        memcpy(&timers[timer_index], &timer, sizeof(timer_app_timer_t));
        timer_ui_add_timer(timer);
        timer_index++;
    }
    */
    for (int i = 0; i < 3; i++) {
        //timer_ui_add_alarm(/*alarm*/);
    }
}

static void timer_app_stop(void)
{
    timer_ui_remove();
}

static void alarm_triggered_cb(void* user_data) {
    // Handle alarm triggered
}

static void on_timer_created_cb(uint32_t hour, uint32_t min, uint32_t sec) {
    // Save timer to flash
    // Add timer to UI
    LOG_DBG("Timer created: %d:%d:%d", hour, min, sec);
    timer_app_timer_t timer = {
        .timer_id = timer_index,
        .hour = hour,
        .min = min,
        .sec = sec,
        .state = TIMER_STATE_STOPPED
    };
    memcpy(&timers[timer_index], &timer, sizeof(timer_app_timer_t));
    timer_ui_add_timer(timer);

    zsw_alarm_add_timer(hour, min, sec, alarm_triggered_cb, (void*)timer_index);
    timer_index++;
}

static void on_timer_event_cb(timer_event_type_t type, uint32_t timer_id) {
    // Handle timer event
    switch (type) {
        case TIMER_EVT_START_PAUSE_RESUME:
            LOG_DBG("Timer %d start/pause/resume", timer_id);
            break;
        case TIMER_EVT_RESET:
            LOG_DBG("Timer %d reset", timer_id);
            break;
        case TIMER_EVT_DELETE:
            LOG_DBG("Timer %d delete", timer_id);
            break;
        default:
            __ASSERT(false, "Invalid timer event type");
    }
}

static int timer_app_add(void)
{
    zsw_app_manager_add_application(&app);

    if (settings_subsys_init()) {
        LOG_ERR("Error during settings_subsys_init!");
        return -EFAULT;
    }

    return 0;
}

SYS_INIT(timer_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);