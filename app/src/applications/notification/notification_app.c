#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include "notification_ui.h"
#include "managers/zsw_notification_manager.h"
#include "managers/zsw_app_manager.h"

static void notification_app_start(lv_obj_t *root, lv_group_t *group);
static void notification_app_stop(void);

static application_t app = {
    .name = "Notification",
    .hidden = true,
    .start_func = notification_app_start,
    .stop_func = notification_app_stop
};

static lv_group_t *notification_group;

// Test
void my_work_handler(struct k_work *work);
void my_timer_handler(struct k_timer *timer_id);
K_WORK_DEFINE(my_work, my_work_handler);
K_TIMER_DEFINE(my_timer, my_timer_handler, NULL);
zsw_not_mngr_notification_t not = {
    .id = 0,
    .title = "Hallo",
    .body = "Test"
};


void my_work_handler(struct k_work *work)
{
    sprintf(not.body, "Test: %u", not.id);
    not.id++;
    notifications_ui_add_notification(&not, notification_group);
}

void my_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&my_work);
}
//

static void on_notification_page_notification_close(uint32_t not_id)
{
    // TODO send to phone that the notification was read.
    zsw_notification_manager_remove(not_id);
}

static void notification_app_start(lv_obj_t *root, lv_group_t *group)
{
    int num_unread;
    zsw_not_mngr_notification_t notifications[NOTIFICATION_MANAGER_MAX_STORED];

    notification_group = group;

    zsw_notification_manager_get_all(notifications, &num_unread);
    notifications_ui_page_init(on_notification_page_notification_close);
    notifications_ui_page_create(notifications, num_unread, notification_group);

    k_timer_start(&my_timer, K_SECONDS(10), K_SECONDS(2));
}

static void notification_app_stop(void)
{
    notifications_ui_page_close();
}

static int notification_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(notification_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);