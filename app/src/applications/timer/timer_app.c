#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/settings/settings.h>

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "timer_ui.h"
#include "zsw_alarm.h"
#include "drivers/zsw_vibration_motor.h"
#include "events/zsw_periodic_event.h"
#include "ui/popup/zsw_popup_window.h"
#include "zsw_clock.h"
LOG_MODULE_REGISTER(timer_app, LOG_LEVEL_DBG);

#define SETTINGS_NAME_TIMER_APP     "timer_app"
#define SETTINGS_KEY_TIMERS         "timers"
#define SETTINGS_TIMERS_LIST         SETTINGS_NAME_TIMER_APP "/" SETTINGS_KEY_TIMERS

static void timer_app_start(lv_obj_t *root, lv_group_t *group);
static void timer_app_stop(void);

static void on_timer_created_cb(uint32_t hour, uint32_t min, uint32_t sec, ui_timer_type_t type);
static void on_timer_event_cb(timer_event_type_t evt_type, uint32_t timer_id);
static void alarm_triggered_cb(void *user_data);
static void zbus_periodic_1s_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_LISTENER_DEFINE(timer_app_1s_event_listener, zbus_periodic_1s_callback);

ZSW_LV_IMG_DECLARE(timer_app_icon);

static timer_app_timer_t timers[TIMER_UI_MAX_TIMERS];
static bool app_is_open;

static application_t app = {
    .name = "Timers",
    .icon = ZSW_LV_IMG_USE(timer_app_icon),
    .start_func = timer_app_start,
    .stop_func = timer_app_stop
};

static void timer_app_start(lv_obj_t *root, lv_group_t *group)
{
    timer_ui_show(root, on_timer_created_cb, on_timer_event_cb);
    for (int i = 0; i < TIMER_UI_MAX_TIMERS; i++) {
        if (timers[i].used) {
            timer_ui_add_timer(timers[i]);
        }
    }
    app_is_open = true;
    zsw_periodic_chan_add_obs(&periodic_event_1s_chan, &timer_app_1s_event_listener);
}

static void timer_app_stop(void)
{
    app_is_open = false;
    timer_ui_remove();
    zsw_periodic_chan_rm_obs(&periodic_event_1s_chan, &timer_app_1s_event_listener);
}

static void alarm_triggered_cb(void *user_data)
{
    // TODO also check:
    // Check so alarm still valid, it could have been deleted/state changed
    // While the alarm was queued up
    uint32_t timer_id = (uint32_t)user_data;
    if (timers[timer_id].type == TYPE_TIMER) {
        timers[timer_id].state = TIMER_STATE_STOPPED;
        timers[timer_id].remaining_hour = timers[timer_id].hour;
        timers[timer_id].remaining_min = timers[timer_id].min;
        timers[timer_id].remaining_sec = timers[timer_id].sec;
    } else if (timers[timer_id].type == TYPE_ALARM) {
        timers[timer_id].state = TIMER_STATE_STOPPED;
        timers[timer_id].zsw_alarm_timer_id = ZSW_ALARM_INVALID_ID;
    }

    if (app_is_open) {
        timer_ui_update_timer(timers[timer_id]);
    }

    static char buf[50];
    snprintf(buf, sizeof(buf), "Timer %d triggered", timer_id);

    zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_NOTIFICATION);
    // TODO: Make a different popup for timer, and make it vibrate until timer is dismissed
    zsw_popup_show("Timer", buf, NULL, 10, false);
}

static int find_free_timer_slot(void)
{
    for (int i = 0; i < TIMER_UI_MAX_TIMERS; i++) {
        if (!timers[i].used) {
            return i;
        }
    }

    return -ENOMEM;
}

static void on_timer_created_cb(uint32_t hour, uint32_t min, uint32_t sec, ui_timer_type_t type)
{
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
        .state = TIMER_STATE_STOPPED,
        .type = type,
    };

    timers[alarm_index] = timer;
    timer_ui_add_timer(timer);

    settings_save_one(SETTINGS_TIMERS_LIST, &timers, sizeof(timers));
}

static void on_timer_event_cb(timer_event_type_t evt_type, uint32_t timer_id)
{
    ui_timer_type_t timer_type = timers[timer_id].type;

    switch (evt_type) {
        case TIMER_EVT_START_PAUSE_RESUME: {
            LOG_DBG("Timer %d start/pause/resume", timer_id);
            if (timers[timer_id].state == TIMER_STATE_STOPPED || timers[timer_id].state == TIMER_STATE_PAUSED) {
                timers[timer_id].state = TIMER_STATE_PLAYING;
                int zsw_timer_id;
                if (timers[timer_id].type == TYPE_ALARM) {
                    struct rtc_time time = {0};
                    time.tm_hour = timers[timer_id].hour;
                    time.tm_min = timers[timer_id].min;
                    time.tm_sec = timers[timer_id].sec;
                    zsw_timer_id = zsw_alarm_add(time, alarm_triggered_cb, (void *)timer_id);
                } else {
                    zsw_timer_id = zsw_alarm_add_timer(timers[timer_id].remaining_hour, timers[timer_id].remaining_min,
                                                       timers[timer_id].remaining_sec, alarm_triggered_cb, (void *)timer_id);
                }
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
                LOG_ERR("Invalid timer state %d for timer evnt type %d", timers[timer_id].state, evt_type);
            }
            break;
        }
        case TIMER_EVT_RESET: {
            LOG_DBG("Timer %d reset", timer_id);
            __ASSERT(timer_type == TYPE_TIMER, "Invalid timer type for reset event");
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
        case TIMER_EVT_DELETE: {
            LOG_DBG("Timer %d delete", timer_id);
            if (timers[timer_id].used && timers[timer_id].state == TIMER_STATE_PLAYING) {
                int ret = zsw_alarm_remove(timers[timer_id].zsw_alarm_timer_id);
                if (ret < 0) {
                    LOG_ERR("Failed to pause timer");
                    return;
                }
            }
            timer_ui_remove_timer(timers[timer_id]);

            memset(&timers[timer_id], 0, sizeof(timer_app_timer_t));
            settings_save_one(SETTINGS_TIMERS_LIST, &timers, sizeof(timers));
            break;
        }
        default:
            __ASSERT(false, "Invalid timer event type");
    }
    if (timers[timer_id].used) {
        timer_ui_update_timer(timers[timer_id]);
    }
}

static void zbus_periodic_1s_callback(const struct zbus_channel *chan)
{
    // Re-calculate remanining for all timers
    for (int i = 0; i < TIMER_UI_MAX_TIMERS; i++) {
        if (timers[i].used && timers[i].type == TYPE_TIMER && timers[i].state == TIMER_STATE_PLAYING) {
            zsw_alarm_get_remaining(timers[i].zsw_alarm_timer_id, &timers[i].remaining_hour, &timers[i].remaining_min,
                                    &timers[i].remaining_sec);
            timer_ui_update_timer(timers[i]);
        }
    }

    zsw_timeval_t time;
    zsw_clock_get_time(&time);
    timer_ui_set_time(time.tm.tm_hour, time.tm.tm_min, time.tm.tm_sec);
}

static int timers_settings_load_cb(const char *p_key, size_t len,
                                   settings_read_cb read_cb, void *p_cb_arg, void *p_param)
{
    ARG_UNUSED(p_key);

    if (len != sizeof(timers)) {
        LOG_ERR("Invalid length of timers structure array in settings");
        return -EINVAL;
    }

    if (read_cb(p_cb_arg, &timers, len) != sizeof(timers)) {
        LOG_ERR("Error reading timers from settings");
        return -EIO;
    }

    for (int i = 0; i < TIMER_UI_MAX_TIMERS; i++) {
        if (timers[i].used) {
            // Reset the timer state as it may have been stored when it was running
            // We reset all timers at startup
            // We don't reset alarms
            if (timers[i].type == TYPE_TIMER) {
                timers[i].state = TIMER_STATE_STOPPED;
                timers[i].remaining_hour = timers[i].hour;
                timers[i].remaining_min = timers[i].min;
                timers[i].remaining_sec = timers[i].sec;
            } else {
                struct rtc_time time = {0};
                time.tm_hour = timers[i].hour;
                time.tm_min = timers[i].min;
                time.tm_sec = timers[i].sec;
                zsw_alarm_add(time, alarm_triggered_cb, (void *)i);
            }
        }
    }

    return 0;
}

static int timer_app_add(void)
{
    zsw_app_manager_add_application(&app);

    if (settings_subsys_init()) {
        LOG_ERR("Error during settings_subsys_init!");
        return -EFAULT;
    }

    memset(timers, 0, sizeof(timers));

    if (settings_load_subtree_direct(SETTINGS_TIMERS_LIST, timers_settings_load_cb, NULL)) {
        LOG_ERR("Error during settings_load_subtree!");
        return -EFAULT;
    }

    return 0;
}

SYS_INIT(timer_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);