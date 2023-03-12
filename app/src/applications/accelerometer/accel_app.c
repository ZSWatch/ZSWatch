#include <accelerometer/accel_ui.h>
#include <application_manager.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <accelerometer.h>

static void fetch_accel_data(struct k_work *item);
static void accel_app_start(lv_obj_t *root, lv_group_t *group);
static void accel_app_stop(void);
static void on_close_accel(void);

LV_IMG_DECLARE(move);

static application_t app = {
    .name = "Accelerometer",
    .icon = &move,
    .start_func = accel_app_start,
    .stop_func = accel_app_stop
};

K_WORK_DELAYABLE_DEFINE(work, fetch_accel_data);
static struct k_work_sync work_sync;

static void accel_app_start(lv_obj_t *root, lv_group_t *group)
{
    accel_ui_show(root, on_close_accel);
    int res = k_work_reschedule(&work, K_MSEC(100));
    __ASSERT_NO_MSG(0 <= res);
}

static void accel_app_stop(void)
{
    k_work_cancel_delayable_sync(&work, &work_sync);
    accel_ui_remove();
}

static void fetch_accel_data(struct k_work *item)
{
    int16_t x;
    int16_t y;
    int16_t z;
    int res = accelerometer_fetch_xyz(&x, &y, &z);
    if (res == 0) {
        accel_ui_set_values(x, y, z);
    }
    res = k_work_reschedule(&work, K_MSEC(100));
    __ASSERT_NO_MSG(0 <= res);
}

static void on_close_accel(void)
{
    application_manager_app_close_request(&app);
}

static int accel_app_add(const struct device *arg)
{
    application_manager_add_application(&app);

    return 0;
}

SYS_INIT(accel_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
