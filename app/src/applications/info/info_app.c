#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zsw_retained_ram_storage.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

#include "info_ui.h"
#include "managers/zsw_power_manager.h"
#include "managers/zsw_app_manager.h"

LOG_MODULE_REGISTER(info_app, LOG_LEVEL_DBG);

#define INFO_REFRESH_INTERVAL_MS  1000

// Functions needed for all applications
static void info_app_start(lv_obj_t *root, lv_group_t *group);
static void info_app_stop(void);

// Functions related to app functionality
static void timer_callback(lv_timer_t *timer);
static void on_reset_pressed(void);

LV_IMG_DECLARE(statistic_icon);

static application_t app = {
    .name = "System Info",
    .icon = &statistic_icon,
    .start_func = info_app_start,
    .stop_func = info_app_stop
};

static lv_timer_t *refresh_timer;

static void info_app_start(lv_obj_t *root, lv_group_t *group)
{
    bt_addr_le_t local_addr;
    char addr[BT_ADDR_LE_STR_LEN];
    size_t addr_count = 1;

    info_ui_show(root, on_reset_pressed);
    info_ui_set_uptime_sec(k_uptime_get() / 1000);
    info_ui_set_total_uptime_sec(retained.uptime_sum / 1000);
    info_ui_set_wakeup_time_sec(retained.wakeup_time / 1000, (retained.wakeup_time / (double)retained.uptime_sum) * 100);
    info_ui_set_ref_off_time_sec(retained.display_off_time / 1000,
                                 (retained.display_off_time / (double)retained.uptime_sum) * 100);
    info_ui_set_time_to_inactive_sec(zsw_power_manager_get_ms_to_inactive() / 1000);
    info_ui_set_resets(retained.boots);

    bt_id_get(&local_addr, &addr_count);
    bt_addr_le_to_str(&local_addr, addr, sizeof(addr));
    info_ui_set_mac_addr(addr);

    refresh_timer = lv_timer_create(timer_callback, INFO_REFRESH_INTERVAL_MS,  NULL);
}

static void info_app_stop(void)
{
    lv_timer_del(refresh_timer);
    info_ui_remove();
}

static void timer_callback(lv_timer_t *timer)
{
    info_ui_set_uptime_sec(k_uptime_get() / 1000);
    info_ui_set_total_uptime_sec(retained.uptime_sum / 1000);
    info_ui_set_wakeup_time_sec(retained.wakeup_time / 1000, (retained.wakeup_time / (double)retained.uptime_sum) * 100);
    info_ui_set_ref_off_time_sec(retained.display_off_time / 1000,
                                 (retained.display_off_time / (double)retained.uptime_sum) * 100);
    info_ui_set_time_to_inactive_sec(zsw_power_manager_get_ms_to_inactive() / 1000);
}

static void on_reset_pressed(void)
{
    zsw_retained_ram_reset();
}

static int info_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(info_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
