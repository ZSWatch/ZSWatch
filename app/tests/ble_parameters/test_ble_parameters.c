#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#define LOG_DBG(...) ((void)0)
#define LOG_INF(...) ((void)0)
#define LOG_ERR(...) ((void)0)
#define ARG_UNUSED(x) ((void)(x))
#define K_MSEC(x) (x)
#define IS_ENABLED(x) (x)
#define CONFIG_MCUMGR_TRANSPORT_BT_CONN_PARAM_CONTROL 1
#define CONFIG_BT_PERIPHERAL_PREF_MIN_INT 24
#define CONFIG_BT_PERIPHERAL_PREF_MAX_INT 40
#define CONFIG_BT_PERIPHERAL_PREF_LATENCY 0
#define CONFIG_BT_PERIPHERAL_PREF_TIMEOUT 400
#define BLE_COMM_LONG_INT_MIN_MS 320
#define BLE_COMM_LONG_INT_MAX_MS 400
#define BLE_COMM_CONN_INT_UPDATE_TIMEOUT_MS 5000
#define SMP_AUTO_DISABLE_TIMEOUT_SEC 180
#define BT_ADDR_LE_STR_LEN 30
#define BT_SECURITY_L2 2
#define BT_HCI_ERR_AUTH_FAIL 5
struct bt_conn { int refs; };
struct k_work { int dummy; };
struct k_work_delayable { bool scheduled; };
struct bt_le_conn_param { int interval_min, interval_max, latency, timeout; };
struct bt_conn_info { struct { int interval, latency, timeout; } le; };
static struct bt_conn *current_conn;
static struct k_work_delayable conn_interval_slow_work, conn_interval_fast_work, smp_auto_disable_work;
static bool smp_enabled, auto_disable_active;
static int pairing_enabled, max_send_len, updates, unregister_result;
static struct bt_le_conn_param last;
static bool zsw_smp_manager_is_enabled(void) { return smp_enabled; }
static int k_work_cancel_delayable(struct k_work_delayable *w) { w->scheduled = false; return 0; }
static int k_work_schedule(struct k_work_delayable *w, int delay) { (void)delay; w->scheduled = true; return 0; }
static int bt_conn_le_param_update(struct bt_conn *c, const struct bt_le_conn_param *p)
{ assert(c); updates++; last = *p; return 0; }
static struct bt_conn *bt_conn_ref(struct bt_conn *c) { c->refs++; return c; }
static void bt_conn_unref(struct bt_conn *c) { c->refs--; }
static const void *bt_conn_get_dst(struct bt_conn *c) { return c; }
static void bt_addr_le_to_str(const void *addr, char *buf, int len) { (void)addr; (void)buf; (void)len; }
static void request_mtu_exchange(void) {}
static int bt_conn_get_info(struct bt_conn *c, struct bt_conn_info *i) { (void)c; *i=(struct bt_conn_info){0}; return 0; }
static int bt_conn_set_security(struct bt_conn *c, int level) { (void)c; (void)level; return 0; }
static int bt_conn_disconnect(struct bt_conn *c, int reason) { (void)c; (void)reason; return 0; }
static void ble_chronos_state(bool s) { (void)s; }
static void ble_find_device_disconnected(void) {}
static int smp_bt_unregister(void) { return unregister_result; }
static int ble_comm_set_default_adv_interval(void) { return 0; }
static void unblock_fs_write_render(void) {}
static void zsw_xip_disable(void) {}
#include "callbacks.inc"
int main(void)
{
    struct bt_conn c = {0};
    /* SMP registration survives a service-change reconnect: no new idle timer. */
    smp_enabled = true;
    ble_connected(&c, 0);
    assert(current_conn == &c && c.refs == 1 && !conn_interval_slow_work.scheduled);
    /* An already queued idle callback and sensor CCC cleanup cannot replace
       the transport's negotiated fast interval. */
    conn_interval_slow_work.scheduled = true;
    update_conn_interval_slow_handler(NULL);
    assert(updates == 0 && !conn_interval_slow_work.scheduled);
    assert(ble_comm_set_short_connection_interval() == 0 && updates == 0);
    assert(ble_comm_set_default_connection_interval() == 0 && updates == 0);
    /* Explicit disable must restore battery-saving parameters. */
    assert(zsw_smp_manager_disable() == 0);
    assert(!smp_enabled && updates == 1 && last.interval_min == 320 && last.interval_max == 400);
    smp_enabled = auto_disable_active = true;
    smp_auto_disable_work_handler(NULL);
    assert(!smp_enabled && !auto_disable_active && updates == 2 && last.interval_min == 320);
    /* Failed unregister keeps ownership and must not slow an active upload. */
    smp_enabled = auto_disable_active = true; unregister_result = -EIO;
    assert(zsw_smp_manager_disable() == -EIO && smp_enabled && updates == 2);
    smp_auto_disable_work_handler(NULL);
    assert(smp_enabled && updates == 2);
    /* A disconnect cancels both deferred requests; no null-connection update. */
    conn_interval_fast_work.scheduled = conn_interval_slow_work.scheduled = true;
    ble_disconnected(&c, 0);
    assert(!current_conn && c.refs == 0 && !conn_interval_fast_work.scheduled && !conn_interval_slow_work.scheduled);
    smp_enabled = false;
    assert(ble_comm_set_default_connection_interval() == -ENOTCONN);
    ble_connected(&c, 0);
    assert(conn_interval_slow_work.scheduled);
    assert(ble_comm_set_short_connection_interval() == 0 && last.interval_min == 24 && !conn_interval_slow_work.scheduled);
    update_conn_interval_slow_handler(NULL);
    assert(last.interval_min == 320);
    puts("SMP reconnect/idle parameter policy passed");
}
