#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define ARG_UNUSED(x) (void)(x)
#define K_MSEC(x) (x)
#define K_NO_WAIT 0
struct k_spinlock { int unused; };
typedef unsigned int k_spinlock_key_t;
struct k_work { void (*handler)(struct k_work *); bool queued; };
struct k_timer { void (*handler)(struct k_timer *); bool running; int64_t due; int64_t period; };
#define K_WORK_DEFINE(name, handler_fn) struct k_work name = { .handler = handler_fn }
#define K_TIMER_DEFINE(name, handler_fn, stop_fn) struct k_timer name = { .handler = handler_fn }
int64_t k_uptime_get(void);
k_spinlock_key_t k_spin_lock(struct k_spinlock *lock);
void k_spin_unlock(struct k_spinlock *lock, k_spinlock_key_t key);
void k_timer_start(struct k_timer *timer, int64_t delay, int64_t period);
void k_timer_stop(struct k_timer *timer);
int k_work_submit(struct k_work *work);
