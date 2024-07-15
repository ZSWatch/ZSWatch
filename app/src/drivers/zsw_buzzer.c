/*
 * Copyright (c) 2022-2023 Golioth, Inc.
 * Copyright (c) 2024 Jakob Krantz
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "drivers/zsw_buzzer.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/logging/log.h>

#if CONFIG_ZSWATCH_PCB_REV >= 5

LOG_MODULE_REGISTER(zsw_buzzer, LOG_LEVEL_WRN);

#define sixteenth 38
#define eigth 75
#define quarter 150
#define half 300
#define whole 600

#define C4  262
#define Db4 277
#define D4  294
#define Eb4 311
#define E4  330
#define F4  349
#define Gb4 370
#define G4  392
#define Ab4 415
#define A4  440
#define Bb4 466
#define B4  494
#define C5  523
#define Db5 554
#define D5  587
#define Eb5 622
#define E5  659
#define F5  698
#define Gb5 740
#define G5  784
#define Ab5 831
#define A5  880
#define Bb5 932
#define B5  988
#define C6  1046
#define Db6 1109
#define D6  1175
#define Eb6 1245
#define E6  1319
#define F6  1397
#define Gb6 1480
#define G6  1568
#define Ab6 1661
#define A6  1760
#define Bb6 1865
#define B6  1976

#define REST 1

typedef struct note_duration_t {
    int note;     // hz
    int duration; // msec
} note_duration_t;

static void buzzer_set_power(uint8_t percent);
static void buzzer_off_work_handler(struct k_work *work);
static void run_next_tone_state(note_duration_t *state);
static void pattern_timer_timeout(struct k_timer *timer_id);

static struct note_duration_t funkytown_song[] = {
    {.note = C5, .duration = quarter},
    {.note = REST, .duration = eigth},
    {.note = C5, .duration = quarter},
    {.note = Bb4, .duration = quarter},
    {.note = C5, .duration = quarter},
    {.note = REST, .duration = quarter},
    {.note = G4, .duration = quarter},
    {.note = REST, .duration = quarter},
    {.note = G4, .duration = quarter},
    {.note = C5, .duration = quarter},
    {.note = F5, .duration = quarter},
    {.note = E5, .duration = quarter},
    {.note = C5, .duration = quarter}
};

static struct note_duration_t mario_song[] = {
    {.note = E6, .duration = quarter},
    {.note = REST, .duration = eigth},
    {.note = E6, .duration = quarter},
    {.note = REST, .duration = quarter},
    {.note = E6, .duration = quarter},
    {.note = REST, .duration = quarter},
    {.note = C6, .duration = quarter},
    {.note = E6, .duration = half},
    {.note = G6, .duration = half},
    {.note = REST, .duration = quarter},
    {.note = G4, .duration = whole},
    {.note = REST, .duration = whole},
    // break in sound
    {.note = C6, .duration = half},
    {.note = REST, .duration = quarter},
    {.note = G5, .duration = half},
    {.note = REST, .duration = quarter},
    {.note = E5, .duration = half},
    {.note = REST, .duration = quarter},
    {.note = A5, .duration = quarter},
    {.note = REST, .duration = quarter},
    {.note = B5, .duration = quarter},
    {.note = REST, .duration = quarter},
    {.note = Bb5, .duration = quarter},
    {.note = A5, .duration = half},
    {.note = G5, .duration = quarter},
    {.note = E6, .duration = quarter},
    {.note = G6, .duration = quarter},
    {.note = A6, .duration = half},
    {.note = F6, .duration = quarter},
    {.note = G6, .duration = quarter},
    {.note = REST, .duration = quarter},
    {.note = E6, .duration = quarter},
    {.note = REST, .duration = quarter},
    {.note = C6, .duration = quarter},
    {.note = D6, .duration = quarter},
    {.note = B5, .duration = quarter}
};

struct note_duration_t beep_song[] = {
    {.note = G5, .duration = 150},
    {.note = REST, .duration = 150},
    {.note = G5, .duration = 150},
};

static const struct pwm_dt_spec buzzer_dt = PWM_DT_SPEC_GET(DT_ALIAS(buzzer_pwm));
static const struct device *const reg_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(regulator_buzzer));

K_TIMER_DEFINE(buzzer_timer, pattern_timer_timeout, NULL);

K_WORK_DELAYABLE_DEFINE(buzzer_off_work, buzzer_off_work_handler);

static note_duration_t *active_pattern;
static uint8_t active_pattern_index;
static uint8_t active_pattern_len;
static bool buzzer_dt_busy;
static bool buzzer_initialized;

int zsw_buzzer_run_melody(zsw_buzzer_melody_t melody)
{
    if (!device_is_ready(buzzer_dt.dev)) {
        return -ENODEV;
    }

    if (buzzer_dt_busy) {
        return -EBUSY;
    }

    if (!buzzer_initialized) {
        return -EPERM;
    }

    switch (melody) {
        case ZSW_BUZZER_PATTERN_BEEP:
            active_pattern = beep_song;
            active_pattern_len = ARRAY_SIZE(beep_song);
            break;
        case ZSW_BUZZER_PATTERN_MARIO:
            active_pattern = mario_song;
            active_pattern_len = ARRAY_SIZE(mario_song);
            break;
        default:
            __ASSERT(false, "Invalid vibration pattern");
            return -EINVAL;
            break;
    }

    buzzer_dt_busy = true;
    active_pattern_index = 0;
    regulator_enable(reg_dev);
    run_next_tone_state(&active_pattern[active_pattern_index]);

    return 0;
}

static void run_next_tone_state(note_duration_t *state)
{
    buzzer_set_power(state->note);
    k_timer_start(&buzzer_timer, K_MSEC(state->duration), K_NO_WAIT);
}

static void buzzer_set_power(uint8_t note)
{
    int ret;

    if (!device_is_ready(buzzer_dt.dev)) {
        return;
    }

    if (note < 10) {
        ret = pwm_set_pulse_dt(&buzzer_dt, 0);
    } else {
        ret = pwm_set_dt(&buzzer_dt, PWM_HZ(note), PWM_HZ((note)) / 2);
    }
    __ASSERT(ret == 0, "pwm error: %d", ret);
}

static void pattern_timer_timeout(struct k_timer *timer_id)
{
    active_pattern_index++;
    if (active_pattern_index < active_pattern_len) {
        run_next_tone_state(&active_pattern[active_pattern_index]);
    } else {
        // Power off regulator
        k_work_schedule(&buzzer_off_work, K_MSEC(1));
        // Disable PWM
        int ret = pwm_set_pulse_dt(&buzzer_dt, 0);
        __ASSERT(ret == 0, "pwm error: %d", ret);
    }
}

static void buzzer_off_work_handler(struct k_work *work)
{
    // Pattern done
    buzzer_dt_busy = false;
    if (!device_is_ready(reg_dev)) {
        return;
    }
    regulator_disable(reg_dev);
}

static int buzzer_init(void)
{
    if (!device_is_ready(buzzer_dt.dev)) {
        LOG_WRN("Vibration motor control not supported");
        return -ENODEV;
    }

    if (!device_is_ready(reg_dev)) {
        LOG_WRN("Buzzer regulator control not supported");
    }

    int ret = pwm_set_pulse_dt(&buzzer_dt, 0);
    __ASSERT(ret == 0, "pwm error: %d", ret);

    buzzer_initialized = true;

    return 0;
}

SYS_INIT(buzzer_init, APPLICATION, CONFIG_DEFAULT_CONFIGURATION_DRIVER_INIT_PRIORITY);

#endif