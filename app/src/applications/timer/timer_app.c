#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/settings/settings.h>

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "timer_ui.h"
#include "zsw_alarm.h"
#include "events/zsw_periodic_event.h"

LOG_MODULE_REGISTER(timer_app, LOG_LEVEL_DBG);

#define SETTING_TIMERS_LIST    "timer_app/timers"

static void timer_app_start(lv_obj_t *root, lv_group_t *group);
static void timer_app_stop(void);

static void on_timer_created_cb(uint32_t hour, uint32_t min, uint32_t sec);
static void on_timer_event_cb(timer_event_type_t type, uint32_t timer_id);

static void zbus_periodic_1s_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_LISTENER_DEFINE(timer_app_1s_event_listener, zbus_periodic_1s_callback);

ZSW_LV_IMG_DECLARE(battery_app_icon);

static timer_app_timer_t timers[TIMER_UI_MAX_TIMERS];

static application_t app = {
    .name = "Timer",
    .icon = ZSW_LV_IMG_USE(battery_app_icon),
    .start_func = timer_app_start,
    .stop_func = timer_app_stop
};


static void timer_app_start(lv_obj_t *root, lv_group_t *group)
{
    // TODO
    // Load timers from flash
    // Load alarms from flash
    timer_ui_show(root, on_timer_created_cb, on_timer_event_cb);
}

static void timer_app_stop(void)
{
    timer_ui_remove();
}

static void alarm_triggered_cb(void* user_data) {
    // TODO also check:
    // Check so alarm still valid, it could have been deleted/state changed
    // While the alarm was qued up
    uint32_t timer_id = (uint32_t)user_data;
    timers[timer_id].state = TIMER_STATE_STOPPED;
    timers[timer_id].remaining_hour = timers[timer_id].hour;
    timers[timer_id].remaining_min = timers[timer_id].min;
    timers[timer_id].remaining_sec = timers[timer_id].sec;
    timer_ui_update_timer(timers[timer_id]);
}

static int find_free_timer_slot(void) {
    for (int i = 0; i < TIMER_UI_MAX_TIMERS; i++) {
        if (!timers[i].used) {
            return i;
        }
    }

    return -ENOMEM;
}

static void on_timer_created_cb(uint32_t hour, uint32_t min, uint32_t sec) {
    // Save timer to flash
    // Add timer to UI
    LOG_DBG("Timer created: %d:%d:%d", hour, min, sec);
    
    int alarm_index = find_free_timer_slot();
    if (alarm_index < 0) {
        LOG_ERR("No free timer slot");
        return;
    }

    timer_app_timer_t timer = {
        .used = true,
        .timer_id = alarm_index,
        .hour = hour,
        .min = min,
        .sec = sec,
        .remaining_hour = hour,
        .remaining_min = min,
        .remaining_sec = sec,
        .state = TIMER_STATE_STOPPED
    };
    
    timers[alarm_index] = timer;
    timer_ui_add_timer(timer);
}

static void on_timer_event_cb(timer_event_type_t type, uint32_t timer_id) {
    // Handle timer event
    switch (type) {
        case TIMER_EVT_START_PAUSE_RESUME:
        {
            LOG_DBG("Timer %d start/pause/resume", timer_id);
            if (timers[timer_id].state == TIMER_STATE_STOPPED || timers[timer_id].state == TIMER_STATE_PAUSED) {
                timers[timer_id].state = TIMER_STATE_PLAYING;
                int zsw_timer_id = zsw_alarm_add_timer(timers[timer_id].remaining_hour, timers[timer_id].remaining_min, timers[timer_id].remaining_sec, alarm_triggered_cb, (void*)timer_id);
                if (zsw_timer_id < 0) {
                    LOG_ERR("Failed to add timer");
                    return;
                }
                timers[timer_id].zsw_alarm_timer_id = zsw_timer_id;
            } else if (timers[timer_id].state == TIMER_STATE_PLAYING) {
                timers[timer_id].state = TIMER_STATE_PAUSED;
                int ret = zsw_alarm_remove(timers[timer_id].zsw_alarm_timer_id);
                if (ret < 0) {
                    LOG_ERR("Failed to pause timer");
                    return;
                }
            } else {
                LOG_ERR("Invalid timer state %d for timer evnt type %d", timers[timer_id].state, type);
            }
            break;
        }
        case TIMER_EVT_RESET:
        {
            LOG_DBG("Timer %d reset", timer_id);
            bool was_running = timers[timer_id].state == TIMER_STATE_PLAYING;
            timers[timer_id].remaining_hour = timers[timer_id].hour;
            timers[timer_id].remaining_min = timers[timer_id].min;
            timers[timer_id].remaining_sec = timers[timer_id].sec;
            timers[timer_id].state = TIMER_STATE_STOPPED;

            if (was_running) {
                int ret = zsw_alarm_remove(timers[timer_id].zsw_alarm_timer_id);
                if (ret < 0) {
                    LOG_ERR("Failed to pause timer");
                    return;
                }
            }
            break;
        }
        case TIMER_EVT_DELETE:
        {
            LOG_DBG("Timer %d delete", timer_id);
            int ret = zsw_alarm_remove(timers[timer_id].zsw_alarm_timer_id);
            if (ret < 0) {
                LOG_ERR("Failed to pause timer");
                return;
            }
            memset(&timers[timer_id], 0, sizeof(timer_app_timer_t));
            break;
        }
        default:
            __ASSERT(false, "Invalid timer event type");
    }
    timer_ui_update_timer(timers[timer_id]);
}

static void zbus_periodic_1s_callback(const struct zbus_channel *chan)
{
    // Re-calculate remanining for all timers
    for (int i = 0; i < TIMER_UI_MAX_TIMERS; i++) {
        if (timers[i].used) {
            LOG_DBG("Timer[%d]: timer_id=%d, hour=%d, min=%d, sec=%d, remaining_hour=%d, remaining_min=%d, remaining_sec=%d, state=%d",
                i, timers[i].timer_id, timers[i].hour, timers[i].min, timers[i].sec,
                timers[i].remaining_hour, timers[i].remaining_min, timers[i].remaining_sec,
                timers[i].state);
        }
        if (timers[i].used && timers[i].state == TIMER_STATE_PLAYING) {
            zsw_alarm_get_remaining(timers[i].zsw_alarm_timer_id, &timers[i].remaining_hour, &timers[i].remaining_min, &timers[i].remaining_sec);
            timer_ui_update_timer(timers[i]);
        }
    }
}

static int timer_app_add(void)
{
    zsw_app_manager_add_application(&app);

    if (settings_subsys_init()) {
        LOG_ERR("Error during settings_subsys_init!");
        return -EFAULT;
    }

    memset(timers, 0, sizeof(timers));

    zsw_periodic_chan_add_obs(&periodic_event_1s_chan, &timer_app_1s_event_listener);

    return 0;
}

SYS_INIT(timer_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);