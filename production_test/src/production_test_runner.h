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

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TEST_STATE_BUTTON_TEST,
    TEST_STATE_VIBRATION_TEST,
    TEST_STATE_TOUCH_TEST,
    TEST_STATE_MICROPHONE_TEST,
    TEST_STATE_SENSOR_SCAN,
    TEST_STATE_FINAL_RESULT,
    TEST_STATE_COMPLETE
} production_test_runner_state_t;

typedef enum {
    TEST_RESULT_PENDING,
    TEST_RESULT_RUNNING,
    TEST_RESULT_PASSED,
    TEST_RESULT_FAILED
} test_result_t;

typedef struct {
    test_result_t buttons;
    test_result_t vibration;
    test_result_t touch;
    test_result_t display;
    test_result_t imu;
    test_result_t pressure;
    test_result_t magnetometer;
    test_result_t light;
    test_result_t flash;
    test_result_t microphone;
    test_result_t rtc;
} test_results_t;

typedef struct {
    production_test_runner_state_t current_state;
    test_results_t results;
    uint8_t buttons_pressed_mask;  // Bitmask of pressed buttons
    int countdown_seconds;
    bool test_timeout;
    int total_tests;
    int passed_tests;
    bool button_test_started;
} production_test_runner_context_t;

/**
 * @brief Initialize the production test runner
 */
void production_test_runner_init(void);

/**
 * @brief Start the production test sequence
 */
void production_test_runner_start(void);

/**
 * @brief Get current test context
 * @return Pointer to current test context
 */
production_test_runner_context_t *production_test_runner_get_context(void);

/**
 * @brief Signal that current test passed and continue to next step
 */
void production_test_runner_test_passed(void);
