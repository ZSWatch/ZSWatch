#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "template_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(template_app, CONFIG_ZSW_TEMPLATE_APP_LOG_LEVEL);

// Functions needed for all applications
static void template_app_start(lv_obj_t *root, lv_group_t *group);
static void template_app_stop(void);
static void on_incrementation(void);

// Functions related to app functionality
static void timer_callback(lv_timer_t *timer);

ZSW_LV_IMG_DECLARE(templates);

static application_t app = {
    .name = "Template",
    .icon = ZSW_LV_IMG_USE(templates),
    .start_func = template_app_start,
    .stop_func = template_app_stop
};

static lv_timer_t *counter_timer;
static int timer_counter;
static int btn_counter;

static void template_app_start(lv_obj_t *root, lv_group_t *group)
{
    template_ui_show(root, on_incrementation);
    counter_timer = lv_timer_create(timer_callback, 500,  NULL);
}

static void template_app_stop(void)
{
    lv_timer_del(counter_timer);
    template_ui_remove();
}

static void timer_callback(lv_timer_t *timer)
{
    timer_counter++;
    template_ui_set_timer_counter_value(timer_counter);
}

static void on_incrementation(void)
{
    btn_counter++;
    template_ui_set_button_counter_value(btn_counter);
}

static int template_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(template_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
