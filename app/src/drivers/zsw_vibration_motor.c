/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
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

#include "drivers/zsw_vibration_motor.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zsw_vibration_motor, LOG_LEVEL_WRN);

typedef struct vib_motor_state {
    bool enabled;
    uint8_t percent;
    uint32_t delay;
} vib_motor_state_t;

static void vibration_motor_set_power(uint8_t percent);
static void vibration_motor_set_on(bool on);
static void run_next_motor_state(vib_motor_state_t *state);
static void pattern_timer_timeout(struct k_timer *timer_id);

K_TIMER_DEFINE(vibration_timer, pattern_timer_timeout, NULL);

static const struct pwm_dt_spec vib_motor = PWM_DT_SPEC_GET_OR(DT_ALIAS(vibrator_pwm), {});
static const struct gpio_dt_spec enable_gpio = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(vib_pwr), enable_gpios, {});

static vib_motor_state_t press_pattern[] = {
    {.enabled = true, .percent = 90, .delay = 40},
    {.enabled = false, .percent = 0, .delay = 0},
};

static vib_motor_state_t not_pattern[] = {
    {.enabled = true, .percent = 100, .delay = 50},
    {.enabled = false, .percent = 100, .delay = 50},
    {.enabled = true, .percent = 100, .delay = 50},
    {.enabled = false, .percent = 100, .delay = 50},
    {.enabled = true, .percent = 0, .delay = 50},
    {.enabled = false, .percent = 0, .delay = 0},
};

static vib_motor_state_t alarm_pattern[] = {
    {.enabled = true, .percent = 100, .delay = 200},
    {.enabled = false, .percent = 0, .delay = 100},
    {.enabled = true, .percent = 100, .delay = 200},
    {.enabled = false, .percent = 0, .delay = 100},
    {.enabled = true, .percent = 100, .delay = 200},
    {.enabled = false, .percent = 0, .delay = 300},
    {.enabled = true, .percent = 100, .delay = 100},
    {.enabled = false, .percent = 0, .delay = 100},
    {.enabled = true, .percent = 100, .delay = 100},
    {.enabled = false, .percent = 0, .delay = 100},
    {.enabled = true, .percent = 100, .delay = 100},
    {.enabled = false, .percent = 0, .delay = 0},
    {.enabled = true, .percent = 100, .delay = 100},
    {.enabled = false, .percent = 0, .delay = 100},
    {.enabled = true, .percent = 100, .delay = 100},
    {.enabled = false, .percent = 0, .delay = 100},
    {.enabled = true, .percent = 100, .delay = 100},
    {.enabled = false, .percent = 0, .delay = 0},
};

static vib_motor_state_t *active_pattern;
static uint8_t active_pattern_index;
static uint8_t active_pattern_len;
static bool vib_motor_busy;
static bool vib_motor_enabled;

int zsw_vibration_run_pattern(zsw_vibration_pattern_t pattern)
{
    if (!device_is_ready(vib_motor.dev)) {
        return -ENODEV;
    }

    if (!device_is_ready(enable_gpio.port)) {
        return -ENODEV;
    }

    if (vib_motor_busy) {
        return -EBUSY;
    }

    if (!vib_motor_enabled) {
        return -EPERM;
    }

    switch (pattern) {
        case ZSW_VIBRATION_PATTERN_CLICK:
            active_pattern = press_pattern;
            active_pattern_len = ARRAY_SIZE(press_pattern);
            break;
        case ZSW_VIBRATION_PATTERN_NOTIFICATION:
            active_pattern = not_pattern;
            active_pattern_len = ARRAY_SIZE(not_pattern);
            break;
        case ZSW_VIBRATION_PATTERN_ALARM:
            active_pattern = alarm_pattern;
            active_pattern_len = ARRAY_SIZE(alarm_pattern);
            break;
        default:
            __ASSERT(false, "Invalid vibration pattern");
            return -EINVAL;
            break;
    }

    vib_motor_busy = true;
    active_pattern_index = 0;
    run_next_motor_state(&active_pattern[active_pattern_index]);

    return 0;
}

int zsw_vibration_set_enabled(bool enable)
{
    if (!device_is_ready(enable_gpio.port)) {
        return -ENODEV;
    }

    vib_motor_enabled = enable;

    return 0;
}

static void run_next_motor_state(vib_motor_state_t *state)
{
    vibration_motor_set_on(state->enabled);
    vibration_motor_set_power(state->percent);
    k_timer_start(&vibration_timer, K_MSEC(state->delay), K_NO_WAIT);
}

static void vibration_motor_set_power(uint8_t percent)
{
    int ret;
    uint32_t step = vib_motor.period / 100;
    uint32_t pulse_width = step * percent;

    if (!device_is_ready(vib_motor.dev)) {
        return;
    }

    ret = pwm_set_pulse_dt(&vib_motor, pulse_width);
    __ASSERT(ret == 0, "pwm error: %d for pulse: %d", ret, pulse_width);
}

static void vibration_motor_set_on(bool on)
{
    if (!device_is_ready(enable_gpio.port)) {
        return;
    }
    gpio_pin_set_dt(&enable_gpio, on);
}

static void pattern_timer_timeout(struct k_timer *timer_id)
{
    active_pattern_index++;
    if (active_pattern_index < active_pattern_len) {
        run_next_motor_state(&active_pattern[active_pattern_index]);
    } else {
        // Pattern done
        vib_motor_busy = false;
    }
}

static int vibration_motor_init(void)
{
    int rc;

    if (!device_is_ready(vib_motor.dev)) {
        LOG_WRN("Vibration motor control not supported");
        return -ENODEV;
    }
    if (!device_is_ready(enable_gpio.port)) {
        LOG_WRN("Vibration motor enable/disable not supported");
        return -ENODEV;
    }
    rc = gpio_pin_configure_dt(&enable_gpio, GPIO_OUTPUT_LOW);
    if (rc != 0) {
        printk("Failed init vibration motor enable pin\n");
    }

    vibration_motor_set_on(false);

    vib_motor_enabled = true;

    return 0;
}

SYS_INIT(vibration_motor_init, APPLICATION, CONFIG_DEFAULT_CONFIGURATION_DRIVER_INIT_PRIORITY);
