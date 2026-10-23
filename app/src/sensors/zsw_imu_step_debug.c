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
#include <zephyr/sys/atomic.h>

#include "../../drivers/sensor/bmi270/zsw_bosch_bmi270.h"

#include "sensors/zsw_imu_step_debug.h"

LOG_MODULE_REGISTER(zsw_imu_step_dbg, LOG_LEVEL_INF);

#define STEP_DEBUG_INTERVAL     K_SECONDS(10)
#define STEPS_PER_WATERMARK     20

static void step_debug_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(step_debug_work, step_debug_work_handler);

static const struct device *imu;
static atomic_t trig_total;
static atomic_t trig_step;
static uint32_t last_steps;
static uint32_t expected_step_irqs;
static uint32_t reported_gap;

static void step_debug_work_handler(struct k_work *work)
{
    struct zsw_bmi270_step_debug drv;
    struct sensor_value val;
    uint32_t steps;
    uint32_t gap;

    k_work_reschedule(&step_debug_work, STEP_DEBUG_INTERVAL);

    if (sensor_channel_get(imu, SENSOR_CHAN_STEPS, &val) != 0) {
        LOG_WRN("Step read failed");
        return;
    }
    steps = val.val1;
    zsw_bmi270_step_debug_get(&drv);

    // A lower count means the counter was reset since the last check.
    if (steps < last_steps) {
        last_steps = 0;
    }
    expected_step_irqs += (steps / STEPS_PER_WATERMARK) - (last_steps / STEPS_PER_WATERMARK);
    last_steps = steps;

    LOG_INF("steps %u exp_step_int %u step_int %u int %u gpio %u trig %u trig_step %u",
            steps, expected_step_irqs, drv.step_irqs, drv.processed_irqs, drv.gpio_irqs,
            (uint32_t)atomic_get(&trig_total), (uint32_t)atomic_get(&trig_step));

    // Allow one interrupt of slack, it can land just after this check.
    gap = expected_step_irqs > drv.step_irqs ? expected_step_irqs - drv.step_irqs : 0;
    if ((gap > 1) && (gap > reported_gap)) {
        LOG_WRN("Missed step INT: expected %u, got %u", expected_step_irqs, drv.step_irqs);
    }
    reported_gap = gap;
}

void zsw_imu_step_debug_start(const struct device *p_imu)
{
    imu = p_imu;
    k_work_reschedule(&step_debug_work, STEP_DEBUG_INTERVAL);
}

void zsw_imu_step_debug_on_trigger(int type)
{
    atomic_inc(&trig_total);
    if (type == SENSOR_TRIG_STEP) {
        atomic_inc(&trig_step);
    }
}
