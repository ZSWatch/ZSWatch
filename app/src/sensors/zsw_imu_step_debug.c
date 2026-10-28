/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>

#include "../../drivers/sensor/bmi270/zsw_bosch_bmi270.h"

#include "sensors/zsw_imu_step_debug.h"

LOG_MODULE_REGISTER(zsw_imu_step_dbg, CONFIG_ZSW_BOSCH_BMI270_LOG_LEVEL);

#define STEP_DEBUG_INTERVAL     K_SECONDS(10)
#define STEPS_PER_WATERMARK     20

static void step_debug_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(step_debug_work, step_debug_work_handler);

static const struct device *imu;
static struct k_spinlock epoch_lock;
static uint32_t epoch_revision;
static uint32_t resets_pending;
static uint32_t reset_count;
static uint32_t baseline_step_irqs;

static void step_debug_work_handler(struct k_work *work)
{
    struct zsw_bmi270_step_debug drv;
    struct sensor_value val;
    k_spinlock_key_t key;
    uint32_t revision;
    uint32_t resets;
    uint32_t steps;
    uint32_t nominal_step_irqs;
    uint32_t epoch_step_irqs;
    uint32_t gap;

    ARG_UNUSED(work);
    k_work_reschedule(&step_debug_work, STEP_DEBUG_INTERVAL);

    key = k_spin_lock(&epoch_lock);
    revision = epoch_revision;
    if (resets_pending != 0) {
        k_spin_unlock(&epoch_lock, key);
        return;
    }
    k_spin_unlock(&epoch_lock, key);

    if (sensor_channel_get(imu, SENSOR_CHAN_STEPS, &val) != 0) {
        LOG_DBG("Step diagnostic read failed");
        return;
    }
    steps = val.val1;
    zsw_bmi270_step_debug_get(&drv);

    key = k_spin_lock(&epoch_lock);
    if (resets_pending != 0 || revision != epoch_revision) {
        k_spin_unlock(&epoch_lock, key);
        return;
    }
    resets = reset_count;
    nominal_step_irqs = steps / STEPS_PER_WATERMARK;
    epoch_step_irqs = drv.step_irqs - baseline_step_irqs;
    gap = nominal_step_irqs > epoch_step_irqs ? nominal_step_irqs - epoch_step_irqs : 0;
    k_spin_unlock(&epoch_lock, key);

    LOG_DBG("Steps %u IRQs gpio=%u processed=%u step=%u gap=%u resets=%u",
            steps, drv.gpio_irqs, drv.processed_irqs, drv.step_irqs, gap, resets);
}

void zsw_imu_step_debug_start(const struct device *p_imu)
{
    imu = p_imu;
    k_work_reschedule(&step_debug_work, STEP_DEBUG_INTERVAL);
}

void zsw_imu_step_debug_reset_begin(void)
{
    k_spinlock_key_t key = k_spin_lock(&epoch_lock);

    epoch_revision++;
    resets_pending++;
    k_spin_unlock(&epoch_lock, key);
}

void zsw_imu_step_debug_reset_end(bool success)
{
    struct zsw_bmi270_step_debug drv;
    k_spinlock_key_t key;

    zsw_bmi270_step_debug_get(&drv);
    key = k_spin_lock(&epoch_lock);
    if (success) {
        reset_count++;
        baseline_step_irqs = drv.step_irqs;
    }
    resets_pending--;
    epoch_revision++;
    k_spin_unlock(&epoch_lock, key);
}
