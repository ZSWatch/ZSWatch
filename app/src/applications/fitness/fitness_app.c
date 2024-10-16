#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "fitness_ui.h"
#include "managers/zsw_app_manager.h"
#include "zephyr/zbus/zbus.h"
#include "events/accel_event.h"
#include "sensors/zsw_imu.h"
#include "zsw_clock.h"
#include "zsw_alarm.h"

LOG_MODULE_REGISTER(fitness_app, LOG_LEVEL_DBG);

#define STEP_RESET_COUNTER_INTERVAL_S 50

static void fitness_app_start(lv_obj_t *root, lv_group_t *group);
static void fitness_app_stop(void);

static application_t app = {
    .name = "Fitness",
    .start_func = fitness_app_start,
    .stop_func = fitness_app_stop,
    .hidden = true,
};

ZBUS_CHAN_DECLARE(accel_data_chan);
#ifndef CONFIG_RTC
static void step_work_callback(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(step_work, step_work_callback);
#endif

#ifdef CONFIG_RTC
static void step_work_callback(void *user_data)
#else
static void step_work_callback(struct k_work *work)
#endif
{
    bool should_reset_step = true;
    struct accel_event evt = {
        .data.type = ZSW_IMU_EVT_TYPE_STEP,
        .data.data.step.count = 0
    };
#ifndef CONFIG_RTC
    zsw_timeval_t time;
    zsw_clock_get_time(&time);

    if ((time.tm.tm_hour != 23) || (time.tm.tm_min != 59)) {
        should_reset_step = false;
    }
    k_work_reschedule(&step_work, K_SECONDS(STEP_RESET_COUNTER_INTERVAL_S));
#endif
    if (should_reset_step) {
        LOG_DBG("Reset step counter");
        zsw_imu_reset_step_count();
        zbus_chan_pub(&accel_data_chan, &evt, K_MSEC(250));
    }
}

static void fitness_app_start(lv_obj_t *root, lv_group_t *group)
{
    fitness_ui_show(root);
}

static void fitness_app_stop(void)
{
    fitness_ui_remove();
}

static int fitness_app_add(void)
{
    zsw_app_manager_add_application(&app);

#ifdef CONFIG_RTC
    struct rtc_time expiry_time = {
        .tm_hour = 23,
        .tm_min = 59,
        .tm_sec = 59
    };
    zsw_alarm_add(expiry_time, step_work_callback, NULL);
#else
    k_work_reschedule(&step_work, K_SECONDS(STEP_RESET_COUNTER_INTERVAL_S));
#endif

    return 0;
}

SYS_INIT(fitness_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
