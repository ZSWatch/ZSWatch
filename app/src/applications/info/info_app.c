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
#include "ble/ble_comm.h"
#include "zsw_coredump.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(info_app, LOG_LEVEL_DBG);

#define INFO_REFRESH_INTERVAL_MS  1000

// Functions needed for all applications
static void info_app_start(lv_obj_t *root, lv_group_t *group);
static void info_app_stop(void);

// Functions related to app functionality
static void timer_callback(lv_timer_t *timer);
static void on_reset_pressed(void);
static void ble_connected(struct bt_conn *conn, uint8_t err);
static void ble_disconnected(struct bt_conn *conn, uint8_t reason);
static void param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);
static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err);

typedef struct {
    bool connected;
    char remote_addr[BT_ADDR_LE_STR_LEN];
    uint32_t connection_interval;
    uint32_t connection_latency;
    uint32_t connection_timeout;
    bt_security_t security_level;
    uint32_t mtu;
    bool notifications_enabled;
} ble_connection_info_t;

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = ble_connected,
    .disconnected = ble_disconnected,
    .le_param_updated = param_updated,
    .security_changed = security_changed,
};

ZSW_LV_IMG_DECLARE(statistic_icon);

static application_t app = {
    .name = "Debug",
    .icon = ZSW_LV_IMG_USE(statistic_icon),
    .start_func = info_app_start,
    .stop_func = info_app_stop,
    .category = ZSW_APP_CATEGORY_SYSTEM,
};

static lv_timer_t *refresh_timer;
static bool running;
static ble_connection_info_t ble_info;

static void info_app_start(lv_obj_t *root, lv_group_t *group)
{
    bt_addr_le_t local_addr;
    char addr[BT_ADDR_LE_STR_LEN];
    size_t addr_count = 1;
    zsw_coredump_summary_t summary;
    int num_read_dumps = 0;

#ifdef CONFIG_DEBUG_COREDUMP
    zsw_coredump_get_summary(&summary, 1, &num_read_dumps);
#endif

    info_ui_show(root, on_reset_pressed, &summary, num_read_dumps);
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

    if (ble_info.connected) {
        info_app_ui_set_conn_mac(strlen(ble_info.remote_addr) > 0 ? ble_info.remote_addr : "Not connected");
        info_app_ui_set_conn_params(ble_info.connection_interval, ble_info.connection_latency, ble_info.connection_timeout);
        info_app_ui_set_conn_security_info(ble_info.security_level, 0);
        if (ble_comm_send("A", strlen("A")) < 0) { // Try if possible to send data to phone, i.e. CCCD notifications enabled
            ble_info.notifications_enabled = false;
        } else {
            ble_info.notifications_enabled = true;
        }
        info_ui_set_gatt_status(ble_info.notifications_enabled, ble_info.mtu);
    }

    refresh_timer = lv_timer_create(timer_callback, INFO_REFRESH_INTERVAL_MS,  NULL);
    running = true;
}

static void info_app_stop(void)
{
    lv_timer_del(refresh_timer);
    info_ui_remove();
    running = false;
}

static void timer_callback(lv_timer_t *timer)
{
    info_ui_set_uptime_sec(k_uptime_get() / 1000);
    info_ui_set_total_uptime_sec(retained.uptime_sum / 1000);
    info_ui_set_wakeup_time_sec(retained.wakeup_time / 1000, (retained.wakeup_time / (double)retained.uptime_sum) * 100);
    info_ui_set_ref_off_time_sec(retained.display_off_time / 1000,
                                 (retained.display_off_time / (double)retained.uptime_sum) * 100);
    info_ui_set_time_to_inactive_sec(zsw_power_manager_get_ms_to_inactive() / 1000);

    info_ui_set_gatt_status(ble_info.notifications_enabled, ble_comm_get_mtu());
}

static void param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
    ble_info.connection_interval = interval;
    ble_info.connection_latency = latency;
    ble_info.connection_timeout = timeout;

    if (!running) {
        return;
    }

    LOG_INF("Updated => Interval: %d, latency: %d, timeout: %d", interval, latency, timeout);
    info_app_ui_set_conn_params(interval, latency, timeout);
}

static void ble_connected(struct bt_conn *conn, uint8_t err)
{
    struct bt_conn_info info;
    if (err) {
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), ble_info.remote_addr, sizeof(ble_info.remote_addr));
    bt_conn_get_info(conn, &info);
    ble_info.connection_interval = info.le.interval;
    ble_info.connection_latency = info.le.latency;
    ble_info.connection_timeout = info.le.timeout;

    ble_info.connected = true;

    if (running) {
        info_app_ui_set_conn_mac(ble_info.remote_addr);
        info_app_ui_set_conn_params(info.le.interval, info.le.latency, info.le.timeout);
    }
}

static void ble_disconnected(struct bt_conn *conn, uint8_t reason)
{
    memset(&ble_info, 0, sizeof(ble_connection_info_t));
    if (running) {
        info_app_ui_set_conn_mac("Not connected");
    }
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{

    ble_info.security_level = level;

    if (running) {
        info_app_ui_set_conn_security_info(level, err);
    }
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
