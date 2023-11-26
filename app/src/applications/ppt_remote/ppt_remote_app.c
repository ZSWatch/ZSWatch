#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "ppt_remote_ui.h"
#include "managers/zsw_app_manager.h"
#include "ble/ble_hid.h"

LOG_MODULE_REGISTER(ppt_remote_app, CONFIG_ZSW_REMOTE_APP_LOG_LEVEL);

// Functions needed for all applications
static void ppt_remote_app_start(lv_obj_t *root, lv_group_t *group);
static void ppt_remote_app_stop(void);
static void on_next(void);
static void on_prev(void);

// Functions related to app functionality
static void timer_callback(lv_timer_t *timer);

LV_IMG_DECLARE(remote_control);

static application_t app = {
    .name = "Ppt_remote",
    .icon = &remote_control,
    .start_func = ppt_remote_app_start,
    .stop_func = ppt_remote_app_stop
};

static lv_timer_t *counter_timer = NULL;
static int timer_counter;

static void ppt_remote_app_start(lv_obj_t *root, lv_group_t *group)
{
    ppt_remote_ui_show(root, on_next, on_prev);
}

static void ppt_remote_app_stop(void)
{
    if (counter_timer != NULL) {
        lv_timer_del(counter_timer);
    }
    ppt_remote_ui_remove();
}

static void timer_callback(lv_timer_t *timer)
{
    timer_counter++;
    ppt_remote_ui_set_timer_counter_value(timer_counter);
}

static int ppt_remote_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

static void on_next(void)
{
    // Start the timer when the presentation starts
    static bool is_first_time = true;
    if (is_first_time == true) {
        counter_timer = lv_timer_create(timer_callback, 1000, NULL);
        is_first_time = false;
    }

    ble_hid_next();
}

static void on_prev(void)
{
    ble_hid_previous();
}

SYS_INIT(ppt_remote_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
