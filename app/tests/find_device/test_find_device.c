#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

/* Compile the real controller with deterministic kernel/device adapters. */
#include "ble/gadgetbridge/ble_find_device.c"

static int64_t now_ms;
static int starts, stops, wakes, irq_depth;
static bool vib_motor_busy;
static int find_pattern[1], alarm_pattern[1];
static void *active_pattern;

int64_t k_uptime_get(void) { return now_ms; }
k_spinlock_key_t k_spin_lock(struct k_spinlock *lock) { (void)lock; return 0; }
void k_spin_unlock(struct k_spinlock *lock, k_spinlock_key_t key) { (void)lock; (void)key; }
void k_timer_start(struct k_timer *timer, int64_t delay, int64_t period)
{ timer->running = true; timer->due = now_ms + delay; timer->period = period; }
void k_timer_stop(struct k_timer *timer) { timer->running = false; }
int k_work_submit(struct k_work *work) { work->queued = true; return 0; }
int zsw_power_manager_reset_idle_timout(void) { wakes++; return 0; }
int zsw_vibration_run_pattern_loop(zsw_vibration_pattern_t pattern)
{
    assert(pattern == ZSW_VIBRATION_PATTERN_FIND);
    if (vib_motor_busy) return -EBUSY;
    vib_motor_busy = true; active_pattern = find_pattern; starts++; return 0;
}
static unsigned int irq_lock(void) { return irq_depth++; }
static void irq_unlock(unsigned int key) { irq_depth = key; }
static void zsw_vibration_stop(void) { vib_motor_busy = false; stops++; }
/* This include is extracted from the actual driver by the host test launcher. */
#include "stop_find_impl.inc"

static void run_work(void)
{
    if (find_wake_work.queued) {
        find_wake_work.queued = false;
        find_wake_work.handler(&find_wake_work);
    }
}
static void advance(int64_t milliseconds)
{
    now_ms += milliseconds;
    struct k_timer *timers[] = { &find_wake_timer, &find_timeout_timer };
    for (unsigned i = 0; i < sizeof(timers) / sizeof(timers[0]); i++) {
        struct k_timer *timer = timers[i];
        if (timer->running && timer->due <= now_ms) {
            if (timer->period) timer->due = now_ms + timer->period;
            else timer->running = false;
            timer->handler(timer);
        }
    }
    run_work();
}
static void reset(void)
{
    ble_find_device_disconnected();
    find_wake_work.queued = false;
    now_ms = starts = stops = wakes = irq_depth = 0;
    vib_motor_busy = false; active_pattern = NULL;
}
int main(void)
{
    /* Normal explicit false and late work cannot continue waking the display. */
    reset(); assert(ble_find_device_set(true) == 0); run_work();
    assert(starts == 1 && wakes == 1 && find_active);
    assert(ble_find_device_set(false) == 0);
    assert(stops == 1 && !find_active && !find_wake_timer.running && !find_timeout_timer.running);
    find_wake_work.handler(&find_wake_work); advance(30000);
    assert(stops == 1 && wakes == 1);

    /* The phone cannot extend a single activation with duplicate true frames. */
    reset(); assert(ble_find_device_set(true) == 0);
    advance(5000); assert(ble_find_device_set(true) == 0);
    assert(starts == 1 && find_deadline == 10000);
    advance(4999); assert(find_active && stops == 0);
    advance(1); assert(!find_active && stops == 1 && !vib_motor_busy);

    /* Disconnect stops promptly; old timeout cannot stop a new activation. */
    reset(); assert(ble_find_device_set(true) == 0);
    ble_find_device_disconnected(); assert(stops == 1 && !find_active);
    advance(2000); assert(ble_find_device_set(true) == 0);
    find_timeout_timer.handler(&find_timeout_timer);
    assert(find_active && vib_motor_busy && stops == 1);
    advance(10000); assert(!find_active && stops == 2);

    /* Busy/rejected acquisition owns neither the motor nor the keep-awake timers. */
    reset(); vib_motor_busy = true; active_pattern = alarm_pattern;
    assert(ble_find_device_set(true) == -EBUSY);
    assert(!find_active && !find_wake_timer.running && !find_timeout_timer.running);
    assert(ble_find_device_set(false) == 0); ble_find_device_disconnected();
    assert(stops == 0 && vib_motor_busy && active_pattern == alarm_pattern);

    /* Alarm replacing Find survives delayed false, watchdog and disconnect. */
    reset(); assert(ble_find_device_set(true) == 0);
    zsw_vibration_stop(); vib_motor_busy = true; active_pattern = alarm_pattern;
    advance(10000); assert(stops == 1 && vib_motor_busy && active_pattern == alarm_pattern);
    assert(ble_find_device_set(false) == 0); ble_find_device_disconnected();
    assert(stops == 1 && irq_depth == 0);
    return 0;
}
