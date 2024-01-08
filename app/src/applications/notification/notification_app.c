#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

#include "ui_export/notification_ui.h"
#include "events/zsw_notification_event.h"
#include "managers/zsw_app_manager.h"
#include "managers/zsw_notification_manager.h"

LOG_MODULE_REGISTER(notification_app, CONFIG_NOTIFICATION_APP_LOG_LEVEL);

static void notification_app_start(lv_obj_t *root, lv_group_t *group);
static void notification_app_stop(void);
static void notification_app_zbus_notification_callback(const struct zbus_channel *chan);
static void notification_app_zbus_notification_remove_callback(const struct zbus_channel *chan);

ZBUS_LISTENER_DEFINE(notification_app_lis, notification_app_zbus_notification_callback);
ZBUS_LISTENER_DEFINE(notification_app_remove_lis, notification_app_zbus_notification_remove_callback);

static lv_group_t *notification_group;

static application_t app = {
    .name = "Notification",
    .hidden = true,
    .start_func = notification_app_start,
    .stop_func = notification_app_stop
};

static void notification_app_zbus_notification_callback(const struct zbus_channel *chan)
{
    zsw_not_mngr_notification_t *not;

    LOG_DBG("New notification available");

    not = zsw_notification_manager_get_newest();
    notifications_ui_add_notification(not, notification_group);
}

static void notification_app_zbus_notification_remove_callback(const struct zbus_channel *chan)
{
    const struct zsw_notification_remove_event *evt = zbus_chan_const_msg(chan);
    uint32_t id = evt->notification.id;

    LOG_DBG("Remove notification with ID: %u", id);
    notifications_ui_remove_notification(id);
}

static void on_notification_page_notification_close(uint32_t not_id)
{
    // Inform the notification manager to remove the notification.
    zsw_notification_manager_remove(not_id);
}

static void notification_app_start(lv_obj_t *root, lv_group_t *group)
{
    static uint32_t num_notifications;

    // TODO: Can we remove this buffer in some way because we already have one notification buffer?
    static zsw_not_mngr_notification_t notifications[ZSW_NOTIFICATION_MGR_MAX_STORED];

    notification_group = group;

    zsw_notification_manager_get_all(notifications, &num_notifications);
    notifications_ui_page_init(on_notification_page_notification_close);
    notifications_ui_page_create(root, notification_group);

    for (uint32_t i = 0; i < num_notifications; i++) {
        notifications_ui_add_notification(&notifications[i], notification_group);
    }
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