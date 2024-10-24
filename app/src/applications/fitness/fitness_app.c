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
#include "history/zsw_history.h"
#include "ui/zsw_ui.h"
#include "zsw_clock.h"

LOG_MODULE_REGISTER(fitness_app, LOG_LEVEL_DBG);

#define STEP_RESET_COUNTER_INTERVAL_S   50
#define DAYS_IN_WEEK                    7

#define SETTING_BATTERY_HIST    "fitness/step/hist"
#define SAMPLE_INTERVAL_MIN     60
#define SAMPLE_INTERVAL_MS      (SAMPLE_INTERVAL_MIN * 60 * 1000)
#define MAX_SAMPLES             (7 * 24) // One week of hourly samples

typedef struct {
    zsw_timeval_t time; // TODO optimize size as NVS settings backend can do < 4096 bytes per store
    uint32_t steps;
} zsw_step_sample_t;

static void fitness_app_start(lv_obj_t *root, lv_group_t *group);
static void fitness_app_stop(void);

static void step_sample_work(struct k_work *work);

ZSW_LV_IMG_DECLARE(move);

static application_t app = {
    .name = "Fitness",
    .start_func = fitness_app_start,
    .stop_func = fitness_app_stop,
    .icon = ZSW_LV_IMG_USE(move),
};

static zsw_history_t fitness_history_context;
static zsw_step_sample_t samples[MAX_SAMPLES];

K_WORK_DELAYABLE_DEFINE(sample_step_work, step_sample_work);

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

static void step_sample_work(struct k_work *work)
{
    zsw_step_sample_t sample;
    int next_sample_seconds;

    if (zsw_imu_fetch_num_steps(&sample.steps) != 0) {
#ifdef CONFIG_BOARD_NATIVE_POSIX
        sample.steps = rand() % 1000;
#else
        LOG_WRN("Error during fetching of steps!");
        return;
#endif
    }
    zsw_clock_get_time(&sample.time);
    zsw_history_add(&fitness_history_context, &sample);
    if (zsw_history_save(&fitness_history_context)) {
        LOG_ERR("Error during saving of step samples!");
    }
    LOG_DBG("______STEP HIST ADD________");
    LOG_DBG("Step sample hist add: %d", sample.steps);
    LOG_DBG("Time: %d:%d:%d", sample.time.tm.tm_hour, sample.time.tm.tm_min, sample.time.tm.tm_sec);
    next_sample_seconds = 60 * (SAMPLE_INTERVAL_MIN - sample.time.tm.tm_min) - sample.time.tm.tm_sec;
    LOG_DBG("Next sample in %d:%d", next_sample_seconds / 60, next_sample_seconds % 60);
    k_work_reschedule(&sample_step_work, K_SECONDS(next_sample_seconds));
}

static void get_steps_per_day(uint16_t weekdays[DAYS_IN_WEEK])
{
    int day;
    int num_samples = zsw_history_samples(&fitness_history_context);

    for (int i = 0; i < num_samples; i++) {
        zsw_step_sample_t sample;
        zsw_history_get(&fitness_history_context, &sample, i);
        day = sample.time.tm.tm_wday;
        LOG_DBG("Day: %d, HH: %d, Steps: %d", day, sample.time.tm.tm_hour, sample.steps);
        weekdays[day] = MAX(sample.steps, weekdays[day]);
    }
}

static void shift_array_n_left(uint16_t *arr, int n, int size)
{
    for (int i = 0; i < n; i++) {
        int first = arr[0];
        for (int j = 0; j < size - 1; j++) {
            arr[j] = arr[j + 1];
        }
        arr[size - 1] = first;
    }
}

static void shift_char_array_n_left(char **arr, int n, int size)
{
    for (int i = 0; i < n; i++) {
        char *first = arr[0];
        for (int j = 0; j < size - 1; j++) {
            arr[j] = arr[j + 1];
        }
        arr[size - 1] = first;
    }
}

static void fitness_app_start(lv_obj_t *root, lv_group_t *group)
{
    zsw_timeval_t time;
    uint16_t step_weekdays[DAYS_IN_WEEK] = {0};
    static char *weekday_names[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
    zsw_clock_get_time(&time);
    get_steps_per_day(step_weekdays);

    // Rotate the array (left/counter-clockwise) so the last element is the current day
    int shifts = time.tm.tm_wday + 1;
    shift_array_n_left(step_weekdays, shifts, DAYS_IN_WEEK);
    shift_char_array_n_left(weekday_names, shifts, DAYS_IN_WEEK);

    for (int i = 0; i < DAYS_IN_WEEK; i++) {
        printk("%s %d: %d\n", weekday_names[i], i, step_weekdays[i]);
    }

    fitness_ui_show(root, DAYS_IN_WEEK);
    fitness_ui_set_weekly_steps(step_weekdays, weekday_names, DAYS_IN_WEEK);
}

static void fitness_app_stop(void)
{
    fitness_ui_remove();
}

static int fitness_app_add(void)
{
    struct tm tm;
    struct tm prev_tm;
    int num_hist_samples;
    zsw_timeval_t time;
    zsw_step_sample_t sample;
    int next_sample_seconds = 0;
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

    zsw_history_init(&fitness_history_context, MAX_SAMPLES, sizeof(zsw_step_sample_t), samples, SETTING_BATTERY_HIST);

    if (zsw_history_load(&fitness_history_context)) {
        LOG_ERR("Error during settings_load_subtree!");
        return -EFAULT;
    }

    num_hist_samples = zsw_history_samples(&fitness_history_context);

    zsw_clock_get_time(&time);

    // If watch was reset the step counter restarts at 0, so we need to update the offset.
    if (num_hist_samples > 0 && time.tm.tm_mday == samples[num_hist_samples - 1].time.tm.tm_mday) {
        zsw_imu_set_step_offset(samples[num_hist_samples - 1].steps);
    }

    // Print curent minute and second
    LOG_DBG("fitness_app_add time: %d:%d:%d", time.tm.tm_hour, time.tm.tm_min, time.tm.tm_sec);

    // Try to sample about every full hour
    next_sample_seconds = 60 * (SAMPLE_INTERVAL_MIN - time.tm.tm_min) - time.tm.tm_sec;

    LOG_DBG("Next sample in %d:%d", next_sample_seconds / 60, next_sample_seconds % 60);
    k_work_reschedule(&sample_step_work, K_SECONDS(next_sample_seconds));

    return 0;
}

SYS_INIT(fitness_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
