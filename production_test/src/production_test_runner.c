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

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/input/input.h>
#include <zephyr/pm/device.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/reboot.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <assert.h>

#include "production_test_runner.h"
#include "screens/button_test_screen.h"
#include "screens/vibration_test_screen.h"
#include "screens/touch_test_screen.h"
#include "screens/microphone_test_screen.h"
#include "screens/sensor_scan_screen.h"
#include "screens/result_screen.h"
#include "drivers/zsw_vibration_motor.h"
#include "drivers/zsw_microphone.h"
#include "applications/mic/spectrum_analyzer.h"

LOG_MODULE_REGISTER(prod_test_runner, LOG_LEVEL_INF);

#define TEST_TIMEOUT_SEC               15
#define SENSOR_SUMMARY_DISPLAY_SEC     8

#define AUTOPASS_DELAY_MS              1000

#define MIC_BLOCKS_TO_SKIP             250
#define MIC_ACTIVITY_THRESHOLD         20
#define MIC_MIN_SAMPLES_FOR_PASS       3
#define MIC_SPECTRUM_BANDS             30

#define VIBRATION_REPEAT_MS            1000

#define NUM_BUTTONS                    4

#define MAX_NUM_TEST_METADATA 20

typedef void (*button_handler_t)(uint32_t button_code);
typedef void (*void_handler_t)(void);

typedef struct {
    production_test_runner_state_t state;
    const char *name;
    uint8_t timeout_sec;
    void_handler_t on_enter;
    button_handler_t on_button;
    void_handler_t on_touch;
    void_handler_t on_timeout;
    void_handler_t on_exit;
} test_step_t;

typedef struct {
    const char *name;
    const struct device *device;
    test_result_t *slot;
} device_check_t;

static void state_work_handler(struct k_work *work);
static void timeout_work_handler(struct k_work *work);
static void countdown_update_handler(struct k_work *work);
static void vibration_repeat_handler(struct k_work *work);
static void mic_level_update_handler(struct k_work *work);
static void input_callback(struct input_event *evt, void *user_data);
static void microphone_audio_callback(void *audio_data, size_t size);

static void button_step_enter(void);
static void button_step_on_button(uint32_t button_code);
static void button_step_on_timeout(void);

static void vibration_step_enter(void);
static void vibration_step_on_button(uint32_t button_code);
static void vibration_step_on_timeout(void);
static void vibration_step_on_exit(void);

static void touch_step_enter(void);
static void touch_step_on_touch(void);
static void touch_step_on_timeout(void);

static void microphone_step_enter(void);
static void microphone_step_on_timeout(void);
static void microphone_step_on_exit(void);

static void sensor_scan_step_enter(void);
static void sensor_scan_step_on_timeout(void);

static void final_result_step_enter(void);
static void final_result_step_on_button(uint32_t button_code);

static void cleanup_microphone_test(void);
static void stop_countdown_timer(void);
static void start_countdown_timer(uint8_t seconds);
static void advance_to_next_step(k_timeout_t delay);
static void schedule_state_transition(production_test_runner_state_t next_state, k_timeout_t delay);
static const test_step_t *current_step(void);
static const test_step_t *find_step(production_test_runner_state_t state);
static int button_index_from_code(uint32_t button_code);
static bool set_device_result(test_result_t *slot, const struct device *dev, const char *name);

K_WORK_DELAYABLE_DEFINE(state_work, state_work_handler);
K_WORK_DELAYABLE_DEFINE(timeout_work, timeout_work_handler);
K_WORK_DELAYABLE_DEFINE(countdown_update_work, countdown_update_handler);
K_WORK_DELAYABLE_DEFINE(vibration_repeat_work, vibration_repeat_handler);
K_WORK_DEFINE(mic_level_update_work, mic_level_update_handler);

static production_test_runner_context_t test_context;
static production_test_runner_state_t pending_state = TEST_STATE_COMPLETE;
static bool pending_state_valid;

// Microphone measurement data
static int mic_sample_count;
static int mic_max_activity;
static int16_t mic_audio_buffer[SPECTRUM_FFT_SIZE];
static uint8_t mic_spectrum_data[MIC_SPECTRUM_BANDS];
static int mic_buffer_pos;
static int mic_blocks_to_skip = MIC_BLOCKS_TO_SKIP;

// All devices where the driver will fail if the hardware is not working
static const device_check_t device_checks[] = {
    { "BMI270", DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bmi270)), &test_context.results.imu },
    { "BMP581", DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bmp581)), &test_context.results.pressure },
    { "LIS2MDL", DEVICE_DT_GET_OR_NULL(DT_NODELABEL(lis2mdl)), &test_context.results.magnetometer },
    { "APDS9306", DEVICE_DT_GET_OR_NULL(DT_NODELABEL(apds9306)), &test_context.results.light },
    { "External flash", DEVICE_DT_GET_OR_NULL(DT_CHOSEN(nordic_pm_ext_flash)), &test_context.results.flash },
    { "RTC", DEVICE_DT_GET_OR_NULL(DT_ALIAS(rtc)), &test_context.results.rtc },
};

// All test steps, the order is important as it determines the test order
// TODO: We need to test display backlight also
static const test_step_t test_steps[] = {
    {
        .state = TEST_STATE_BUTTON_TEST,
        .name = "Buttons",
        .timeout_sec = 0, // Starts when user begins pressing
        .on_enter = button_step_enter,
        .on_button = button_step_on_button,
        .on_timeout = button_step_on_timeout,
    },
    {
        .state = TEST_STATE_VIBRATION_TEST,
        .name = "Vibration",
        .timeout_sec = TEST_TIMEOUT_SEC,
        .on_enter = vibration_step_enter,
        .on_button = vibration_step_on_button,
        .on_timeout = vibration_step_on_timeout,
        .on_exit = vibration_step_on_exit,
    },
    {
        .state = TEST_STATE_TOUCH_TEST,
        .name = "Touch",
        .timeout_sec = TEST_TIMEOUT_SEC,
        .on_enter = touch_step_enter,
        .on_touch = touch_step_on_touch,
        .on_timeout = touch_step_on_timeout,
    },
    {
        .state = TEST_STATE_MICROPHONE_TEST,
        .name = "Microphone",
        .timeout_sec = TEST_TIMEOUT_SEC,
        .on_enter = microphone_step_enter,
        .on_timeout = microphone_step_on_timeout,
        .on_exit = microphone_step_on_exit,
    },
    {
        .state = TEST_STATE_SENSOR_SCAN,
        .name = "Sensor scan",
        .timeout_sec = 10,
        .on_enter = sensor_scan_step_enter,
        .on_timeout = sensor_scan_step_on_timeout,
    },
    {
        .state = TEST_STATE_FINAL_RESULT,
        .name = "Final result",
        .timeout_sec = 0,
        .on_enter = final_result_step_enter,
        .on_button = final_result_step_on_button,
    },
};

static const test_step_t *find_step(production_test_runner_state_t state)
{
    for (size_t i = 0; i < ARRAY_SIZE(test_steps); i++) {
        if (test_steps[i].state == state) {
            return &test_steps[i];
        }
    }
    return NULL;
}

static const test_step_t *current_step(void)
{
    return find_step(test_context.current_state);
}

static void schedule_state_transition(production_test_runner_state_t next_state, k_timeout_t delay)
{
    pending_state = next_state;
    pending_state_valid = true;
    k_work_reschedule(&state_work, delay);
}

static void advance_to_next_step(k_timeout_t delay)
{
    const test_step_t *step = current_step();
    if (!step) {
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(test_steps); i++) {
        if (test_steps[i].state == step->state) {
            if (i + 1 < ARRAY_SIZE(test_steps)) {
                schedule_state_transition(test_steps[i + 1].state, delay);
            } else {
                schedule_state_transition(TEST_STATE_COMPLETE, delay);
            }
            return;
        }
    }
}

static void stop_countdown_timer(void)
{
    k_work_cancel_delayable(&timeout_work);
    k_work_cancel_delayable(&countdown_update_work);
    test_context.countdown_seconds = 0;
}

static void start_countdown_timer(uint8_t seconds)
{
    stop_countdown_timer();

    if (seconds == 0) {
        return;
    }

    test_context.countdown_seconds = seconds;
    test_context.test_timeout = false;

    k_work_schedule(&countdown_update_work, K_SECONDS(1));
    k_work_schedule(&timeout_work, K_SECONDS(seconds));
}

static void cleanup_microphone_test(void)
{
    int ret = zsw_microphone_driver_stop();
    if (ret && ret != -EALREADY) {
        LOG_DBG("Microphone stop returned %d", ret);
    }
    spectrum_analyzer_cleanup();
}

static void start_vibration_repeats(void)
{
    LOG_DBG("Starting repeating vibration pattern");
    zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_CLICK);
    k_work_schedule(&vibration_repeat_work, K_MSEC(VIBRATION_REPEAT_MS));
}

static void stop_vibration_repeats(void)
{
    LOG_DBG("Stopping vibration repeats");
    k_work_cancel_delayable(&vibration_repeat_work);
}

static void production_test_runner_timeout(void)
{
    test_context.test_timeout = true;
    const test_step_t *step = current_step();
    if (step && step->on_timeout) {
        step->on_timeout();
    }
}

void production_test_runner_init(void)
{
    LOG_INF("Initializing production test runner");

    INPUT_CALLBACK_DEFINE(NULL, input_callback, NULL);

    const struct device *buttons_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(buttons));
    if (buttons_dev) {
        pm_device_action_run(buttons_dev, PM_DEVICE_ACTION_RESUME);
    } else {
        LOG_WRN("Buttons device not available");
    }

    memset(&test_context, 0, sizeof(test_context));

    button_test_screen_init();
    vibration_test_screen_init();
    touch_test_screen_init();
    microphone_test_screen_init();
    sensor_scan_screen_init();
    result_screen_init();

    // Set the device results as they are known when application starts
    for (size_t i = 0; i < ARRAY_SIZE(device_checks); i++) {
        set_device_result(device_checks[i].slot, device_checks[i].device, device_checks[i].name);
    }
}

void production_test_runner_start(void)
{
    LOG_INF("Starting production test sequence");

    test_context.current_state = TEST_STATE_COMPLETE;
    test_context.results.display = TEST_RESULT_PASSED;

    schedule_state_transition(TEST_STATE_BUTTON_TEST, K_NO_WAIT);
}

production_test_runner_context_t *production_test_runner_get_context(void)
{
    return &test_context;
}

void production_test_runner_test_passed(void)
{
    if (test_context.current_state == TEST_STATE_COMPLETE) {
        LOG_INF("Retest requested - rebooting");
        k_sleep(K_MSEC(500));
        sys_reboot(SYS_REBOOT_COLD);
        LOG_ERR("Reboot failed - continuing running instance");
        return;
    }

    const test_step_t *step = current_step();
    if (step && step->on_touch) {
        step->on_touch();
    }
}

static void state_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (!pending_state_valid) {
        return;
    }

    production_test_runner_state_t target = pending_state;
    pending_state_valid = false;

    stop_countdown_timer();

    const test_step_t *previous = current_step();
    if (previous && previous->on_exit) {
        previous->on_exit();
    }

    if (target == TEST_STATE_COMPLETE) {
        test_context.current_state = TEST_STATE_COMPLETE;
        return;
    }

    const test_step_t *next = find_step(target);
    if (!next) {
        LOG_ERR("Unknown state transition: %d", target);
        test_context.current_state = TEST_STATE_COMPLETE;
        return;
    }

    test_context.current_state = target;
    if (next->timeout_sec) {
        start_countdown_timer(next->timeout_sec);
    }

    LOG_INF("Starting %s", next->name);

    if (next->on_enter) {
        next->on_enter();
    }
}

static void timeout_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);
    production_test_runner_timeout();
}

static void countdown_update_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (test_context.countdown_seconds > 0) {
        test_context.countdown_seconds--;

        switch (test_context.current_state) {
            case TEST_STATE_BUTTON_TEST:
                button_test_screen_update_countdown(test_context.countdown_seconds);
                break;
            case TEST_STATE_VIBRATION_TEST:
                vibration_test_screen_update_countdown(test_context.countdown_seconds);
                break;
            case TEST_STATE_TOUCH_TEST:
                touch_test_screen_update_countdown(test_context.countdown_seconds);
                break;
            case TEST_STATE_MICROPHONE_TEST:
                microphone_test_screen_update_countdown(test_context.countdown_seconds);
                break;
            case TEST_STATE_SENSOR_SCAN:
                sensor_scan_screen_update_countdown(test_context.countdown_seconds);
                break;
            default:
                break;
        }

        if (test_context.countdown_seconds > 0) {
            k_work_schedule(&countdown_update_work, K_SECONDS(1));
        }
    }
}

static void vibration_repeat_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (test_context.current_state != TEST_STATE_VIBRATION_TEST ||
        test_context.results.vibration != TEST_RESULT_RUNNING) {
        return;
    }

    LOG_DBG("Repeating vibration pulse");
    zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_CLICK);
    k_work_schedule(&vibration_repeat_work, K_MSEC(VIBRATION_REPEAT_MS));
}

static void mic_level_update_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (test_context.current_state == TEST_STATE_MICROPHONE_TEST) {
        microphone_test_screen_update_spectrum(mic_spectrum_data, MIC_SPECTRUM_BANDS);
    }
}

static int button_index_from_code(uint32_t button_code)
{
    switch (button_code) {
        case INPUT_KEY_1:
            return 0; /* Top right */
        case INPUT_KEY_2:
            return 1; /* Bottom left */
        case INPUT_KEY_3:
        case INPUT_KEY_KP0:
            return 2; /* Bottom right */
        case INPUT_KEY_4:
            return 3; /* Top left */
        default:
            LOG_WRN("Unknown button code: %u", button_code);
            return -1;
    }
}

static bool set_device_result(test_result_t *slot, const struct device *dev, const char *name)
{
    if (!slot) {
        return false;
    }

    if (!dev) {
        LOG_DBG("Skipping %s check - device not present", name);
        *slot = TEST_RESULT_PENDING;
        return false;
    }

    if (device_is_ready(dev)) {
        *slot = TEST_RESULT_PASSED;
        LOG_DBG("%s ready", name);
        return true;
    }

    *slot = TEST_RESULT_FAILED;
    LOG_WRN("%s device present but not ready", name);
    return true;
}

/* ------------------------------------------------------------------------- */
/* Button test                                                               */
/* ------------------------------------------------------------------------- */
static void button_step_enter(void)
{
    test_context.button_test_started = false;
    test_context.buttons_pressed_mask = 0;
    test_context.results.buttons = TEST_RESULT_PENDING;

    button_test_screen_show();
}

static void button_step_on_button(uint32_t button_code)
{
    int button_index = button_index_from_code(button_code);
    if (button_index < 0) {
        return;
    }

    if (!test_context.button_test_started) {
        test_context.button_test_started = true;
        test_context.results.buttons = TEST_RESULT_RUNNING;
        button_test_screen_start_test();
        start_countdown_timer(TEST_TIMEOUT_SEC);
    }

    uint8_t bit = BIT(button_index);
    button_test_screen_update_button(button_code);

    if (!(test_context.buttons_pressed_mask & bit)) {
        test_context.buttons_pressed_mask |= bit;
        int pressed_count = __builtin_popcount(test_context.buttons_pressed_mask);

        if (pressed_count >= NUM_BUTTONS) {
            test_context.results.buttons = TEST_RESULT_PASSED;
            advance_to_next_step(K_MSEC(AUTOPASS_DELAY_MS));
        }
    }
}

static void button_step_on_timeout(void)
{
    test_context.results.buttons = TEST_RESULT_FAILED;
    LOG_ERR("Button test failed - timeout");
    advance_to_next_step(K_NO_WAIT);
}

/* ------------------------------------------------------------------------- */
/* Vibration test                                                            */
/* ------------------------------------------------------------------------- */
static void vibration_step_enter(void)
{
    test_context.results.vibration = TEST_RESULT_RUNNING;
    vibration_test_screen_show();
    start_vibration_repeats();
}

static void vibration_step_on_button(uint32_t button_code)
{
    ARG_UNUSED(button_code);
    test_context.results.vibration = TEST_RESULT_PASSED;
    stop_vibration_repeats();
    advance_to_next_step(K_MSEC(AUTOPASS_DELAY_MS));
}

static void vibration_step_on_timeout(void)
{
    test_context.results.vibration = TEST_RESULT_FAILED;
    stop_vibration_repeats();
    LOG_ERR("Vibration test failed - timeout");
    advance_to_next_step(K_NO_WAIT);
}

static void vibration_step_on_exit(void)
{
    stop_vibration_repeats();
}

/* ------------------------------------------------------------------------- */
/* Touch test                                                                */
/* ------------------------------------------------------------------------- */
static void touch_step_enter(void)
{
    const struct device *touch_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(cst816s));
    const struct device *sdl_touch_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(input_sdl_touch));
    bool hardware_ready = touch_dev && device_is_ready(touch_dev);
    bool sim_ready = sdl_touch_dev && device_is_ready(sdl_touch_dev);

    if (!hardware_ready && !sim_ready) {
        LOG_WRN("No touch controller available - skipping touch test");
        test_context.results.touch = TEST_RESULT_FAILED;
        test_context.results.display = TEST_RESULT_PASSED;
        advance_to_next_step(K_NO_WAIT);
        return;
    }

    test_context.results.touch = TEST_RESULT_RUNNING;
    touch_test_screen_show();
}

static void touch_step_on_touch(void)
{
    test_context.results.touch = TEST_RESULT_PASSED;
    LOG_INF("Touch test passed");
    advance_to_next_step(K_MSEC(AUTOPASS_DELAY_MS));
}

static void touch_step_on_timeout(void)
{
    test_context.results.touch = TEST_RESULT_FAILED;
    LOG_ERR("Touch test failed - timeout");
    advance_to_next_step(K_NO_WAIT);
}

/* ------------------------------------------------------------------------- */
/* Microphone test                                                           */
/* ------------------------------------------------------------------------- */
static void microphone_step_enter(void)
{
    test_context.results.display = TEST_RESULT_PASSED;
    test_context.results.microphone = TEST_RESULT_RUNNING;

    mic_sample_count = 0;
    mic_max_activity = 0;
    mic_buffer_pos = 0;
    mic_blocks_to_skip = MIC_BLOCKS_TO_SKIP;
    memset(mic_spectrum_data, 0, sizeof(mic_spectrum_data));

    int ret = spectrum_analyzer_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize spectrum analyzer: %d", ret);
        test_context.results.microphone = TEST_RESULT_FAILED;
        advance_to_next_step(K_NO_WAIT);
        return;
    }

    ret = zsw_microphone_init(microphone_audio_callback);
    if (ret != 0) {
        LOG_ERR("Failed to initialize microphone: %d", ret);
        test_context.results.microphone = TEST_RESULT_FAILED;
        spectrum_analyzer_cleanup();
        advance_to_next_step(K_NO_WAIT);
        return;
    }

    ret = zsw_microphone_driver_start();
    if (ret != 0) {
        LOG_ERR("Failed to start microphone: %d", ret);
        test_context.results.microphone = TEST_RESULT_FAILED;
        cleanup_microphone_test();
        advance_to_next_step(K_NO_WAIT);
        return;
    }

    microphone_test_screen_show();
}

static void microphone_step_on_timeout(void)
{
    test_context.results.microphone = TEST_RESULT_FAILED;
    cleanup_microphone_test();
    LOG_ERR("Microphone test failed - timeout (max activity: %d%%)", mic_max_activity);
    advance_to_next_step(K_NO_WAIT);
}

static void microphone_step_on_exit(void)
{
    cleanup_microphone_test();
}

/* ------------------------------------------------------------------------- */
/* Sensor scan                                                               */
/* ------------------------------------------------------------------------- */
static void sensor_scan_step_enter(void)
{
    int count = 0;
    test_metadata_t metadata[MAX_NUM_TEST_METADATA];

    // Add interactive tests (these are tested by user interaction)
    metadata[count++] = (test_metadata_t) {
        "Buttons", &test_context.results.buttons
    };
    metadata[count++] = (test_metadata_t) {
        "Vibration", &test_context.results.vibration
    };
    metadata[count++] = (test_metadata_t) {
        "Touch", &test_context.results.touch
    };
    metadata[count++] = (test_metadata_t) {
        "Display", &test_context.results.display
    };
    metadata[count++] = (test_metadata_t) {
        "Microphone", &test_context.results.microphone
    };

    // Add device/sensor tests
    for (size_t i = 0; i < ARRAY_SIZE(device_checks); i++) {
        if (count < MAX_NUM_TEST_METADATA) {
            metadata[count++] = (test_metadata_t) {
                device_checks[i].name, device_checks[i].slot
            };
        }
    }

    assert(count <= MAX_NUM_TEST_METADATA);

    // We want to show failed tests first in list, so "sort" the metadata array
    test_metadata_t sorted_metadata[MAX_NUM_TEST_METADATA];
    int sorted_count = 0;

    // Pick all failed tests
    for (int i = 0; i < count; i++) {
        if (*metadata[i].result_ptr == TEST_RESULT_FAILED) {
            sorted_metadata[sorted_count++] = metadata[i];
        }
    }

    // Pick the rest
    for (int i = 0; i < count; i++) {
        if (*metadata[i].result_ptr != TEST_RESULT_FAILED) {
            sorted_metadata[sorted_count++] = metadata[i];
        }
    }

    sensor_scan_screen_show(sorted_metadata, sorted_count);
}

static void sensor_scan_step_on_timeout(void)
{
    advance_to_next_step(K_NO_WAIT);
}

/* ------------------------------------------------------------------------- */
/* Final result                                                              */
/* ------------------------------------------------------------------------- */
static int count_test_results(void)
{
    int total = 0;
    int passed = 0;

    test_result_t *results = (test_result_t *)&test_context.results;
    size_t count = sizeof(test_results_t) / sizeof(test_result_t);

    for (size_t i = 0; i < count; i++) {
        if (results[i] != TEST_RESULT_PENDING) {
            total++;
            if (results[i] == TEST_RESULT_PASSED) {
                passed++;
            }
        }
    }

    test_context.passed_tests = passed;
    return total;
}

static void final_result_step_enter(void)
{
    test_context.total_tests = count_test_results();

    // Build complete test names array (interactive + device tests)
    const char *test_names[MAX_NUM_TEST_METADATA];
    int num_test_names = 0;

    // Add interactive test names from test_steps array
    for (size_t i = 0; i < ARRAY_SIZE(test_steps); i++) {
        if (test_steps[i].state != TEST_STATE_FINAL_RESULT) {
            test_names[num_test_names++] = test_steps[i].name;
        }
    }

    // Add device/sensor test names from device_checks array
    for (size_t i = 0; i < ARRAY_SIZE(device_checks); i++) {
        if (num_test_names < MAX_NUM_TEST_METADATA) {
            test_names[num_test_names++] = device_checks[i].name;
        }
    }

    result_screen_show(&test_context, test_names, num_test_names);
    test_context.current_state = TEST_STATE_FINAL_RESULT;
}

static void final_result_step_on_button(uint32_t button_code)
{
    ARG_UNUSED(button_code);
    LOG_INF("Retest requested - rebooting");
    k_sleep(K_MSEC(500));
    sys_reboot(SYS_REBOOT_COLD);
    LOG_ERR("Reboot failed - continuing running instance");
}

static void microphone_audio_callback(void *audio_data, size_t size)
{
    if (test_context.current_state != TEST_STATE_MICROPHONE_TEST) {
        return;
    }

    if (mic_blocks_to_skip > 0) {
        mic_blocks_to_skip--;
        return;
    }

    if (mic_sample_count == 0) {
        LOG_DBG("Microphone warm-up done - analysing samples");
        mic_max_activity = 0;
    }

    int16_t *samples = (int16_t *)audio_data;
    int num_samples = size / sizeof(int16_t);

    for (int i = 0; i < num_samples && mic_buffer_pos < SPECTRUM_FFT_SIZE; i++) {
        mic_audio_buffer[mic_buffer_pos++] = samples[i];
    }

    if (mic_buffer_pos < SPECTRUM_FFT_SIZE) {
        return;
    }

    mic_buffer_pos = 0;
    int ret = spectrum_analyzer_process(mic_audio_buffer, SPECTRUM_FFT_SIZE,
                                        mic_spectrum_data, MIC_SPECTRUM_BANDS, 2.0f);
    if (ret != 0) {
        LOG_WRN("FFT processing failed: %d", ret);
        return;
    }

    int total_magnitude = 0;
    for (int i = 0; i < MIC_SPECTRUM_BANDS; i++) {
        total_magnitude += mic_spectrum_data[i];
    }

    int activity_percent = (total_magnitude * 100) / (MIC_SPECTRUM_BANDS * 255);

    if (activity_percent > mic_max_activity) {
        mic_max_activity = activity_percent;
    }

    mic_sample_count++;
    k_work_submit(&mic_level_update_work);

    if (mic_max_activity > MIC_ACTIVITY_THRESHOLD &&
        mic_sample_count > MIC_MIN_SAMPLES_FOR_PASS &&
        test_context.results.microphone == TEST_RESULT_RUNNING) {
        test_context.results.microphone = TEST_RESULT_PASSED;
        LOG_INF("Microphone test passed - audio activity detected (max: %d%%)", mic_max_activity);
        advance_to_next_step(K_MSEC(AUTOPASS_DELAY_MS));
    }
}

/* ------------------------------------------------------------------------- */
/* Input callback                                                            */
/* ------------------------------------------------------------------------- */
static void input_callback(struct input_event *evt, void *user_data)
{
    ARG_UNUSED(user_data);

    if (evt->value == 1) {
        if (test_context.current_state == TEST_STATE_COMPLETE) {
            LOG_INF("Retest requested - rebooting");
            k_sleep(K_MSEC(500));
            sys_reboot(SYS_REBOOT_COLD);
            LOG_ERR("Reboot failed - continuing running instance");
            return;
        }

        const test_step_t *step = current_step();
        if (step && step->on_button) {
            step->on_button(evt->code);
        }
    }
}