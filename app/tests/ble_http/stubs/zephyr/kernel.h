#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#define ARG_UNUSED(x) (void)(x)
#define K_MSEC(x) (x)
#define K_SECONDS(x) ((x) * 1000)
struct k_spinlock { int unused; };
typedef unsigned int k_spinlock_key_t;
struct k_work { void (*handler)(struct k_work *); };
struct k_work_delayable { struct k_work work; int64_t due; };
#define K_WORK_DELAYABLE_DEFINE(name, fn) struct k_work_delayable name = { .work.handler = fn }
int64_t k_uptime_get(void);
k_spinlock_key_t k_spin_lock(struct k_spinlock *lock);
void k_spin_unlock(struct k_spinlock *lock, k_spinlock_key_t key);
int k_work_reschedule(struct k_work_delayable *work, int64_t delay);
void *k_malloc(size_t bytes);
void k_free(void *ptr);
