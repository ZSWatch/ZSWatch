#include "ble_find_device.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "drivers/zsw_vibration_motor.h"
#include "managers/zsw_power_manager.h"

LOG_MODULE_REGISTER(ble_find_device, CONFIG_ZSW_BLE_LOG_LEVEL);

#define FIND_MAX_DURATION_MS 10000

static struct k_spinlock find_lock;
static bool find_active;
static int64_t find_deadline;

static void find_keep_awake(struct k_work *work);
static void find_wake_timer_cb(struct k_timer *timer);
static void find_timeout_timer_cb(struct k_timer *timer);

K_WORK_DEFINE(find_wake_work, find_keep_awake);
K_TIMER_DEFINE(find_wake_timer, find_wake_timer_cb, NULL);
K_TIMER_DEFINE(find_timeout_timer, find_timeout_timer_cb, NULL);

/* Caller holds find_lock. Motor stop is also used from its normal timer ISR. */
static void find_stop_locked(void)
{
    find_active = false;
    k_timer_stop(&find_wake_timer);
    k_timer_stop(&find_timeout_timer);
    /* A local timer may already have stopped Find and begun another pattern. */
    (void)zsw_vibration_stop_pattern(ZSW_VIBRATION_PATTERN_FIND);
}

static void find_keep_awake(struct k_work *work)
{
    ARG_UNUSED(work);
    k_spinlock_key_t key = k_spin_lock(&find_lock);
    bool active = find_active && k_uptime_get() < find_deadline;
    k_spin_unlock(&find_lock, key);
    if (active) {
        (void)zsw_power_manager_reset_idle_timout();
    }
}

static void find_wake_timer_cb(struct k_timer *timer)
{
    ARG_UNUSED(timer);
    k_work_submit(&find_wake_work);
}

static void find_timeout_timer_cb(struct k_timer *timer)
{
    ARG_UNUSED(timer);
    k_spinlock_key_t key = k_spin_lock(&find_lock);
    /* A callback already in flight must not stop a newer Find request. */
    if (find_active && k_uptime_get() >= find_deadline) {
        find_stop_locked();
        LOG_INF("Find stopped at maximum duration");
    }
    k_spin_unlock(&find_lock, key);
}

int ble_find_device_set(bool enabled)
{
    k_spinlock_key_t key = k_spin_lock(&find_lock);
    int ret = 0;
    if (!enabled) {
        if (find_active) {
            find_stop_locked();
        }
    } else if (!find_active) {
        ret = zsw_vibration_run_pattern_loop(ZSW_VIBRATION_PATTERN_FIND);
        if (ret == 0) {
            find_active = true;
            find_deadline = k_uptime_get() + FIND_MAX_DURATION_MS;
            k_work_submit(&find_wake_work);
            k_timer_start(&find_wake_timer, K_MSEC(1100), K_MSEC(1100));
            k_timer_start(&find_timeout_timer, K_MSEC(FIND_MAX_DURATION_MS), K_NO_WAIT);
        }
    }
    /* Repeated true is idempotent and cannot extend a single activation. */
    k_spin_unlock(&find_lock, key);
    return ret;
}

void ble_find_device_disconnected(void)
{
    k_spinlock_key_t key = k_spin_lock(&find_lock);
    if (find_active) {
        find_stop_locked();
        LOG_INF("Find stopped on BLE disconnect");
    }
    k_spin_unlock(&find_lock, key);
}
