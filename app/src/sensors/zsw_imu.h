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

#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/slist.h>

#include "../../drivers/sensor/bmi270/zsw_bosch_bmi270.h"

/** @brief Caller-owned request; zero-initialize and keep alive until released. */
struct zsw_imu_step_snapshot_request {
    sys_snode_t node;
    bool active;
};

typedef enum zsw_imu_feature_t {
    ZSW_IMU_FEATURE_ACCEL = BOSCH_BMI270_FEAT_ACCEL,
    ZSW_IMU_FEATURE_GYRO = BOSCH_BMI270_FEAT_GYRO,
    ZSW_IMU_FEATURE_STEP_COUNTER = BOSCH_BMI270_FEAT_STEP_COUNTER,
    ZSW_IMU_FEATURE_SIG_MOTION = BOSCH_BMI270_FEAT_SIG_MOTION,
    ZSW_IMU_FEATURE_ANY_MOTION = BOSCH_BMI270_FEAT_ANY_MOTION,
    ZSW_IMU_FEATURE_STEP_ACTIVITY = BOSCH_BMI270_FEAT_STEP_ACTIVITY,
    ZSW_IMU_FEATURE_WRIST_GESTURE = BOSCH_BMI270_FEAT_WRIST_GESTURE,
    ZSW_IMU_FEATURE_WRIST_WEAR_WAKE_UP = BOSCH_BMI270_FEAT_WEAR_WAKE_UP,
    ZSW_IMU_FEATURE_NO_MOTION = BOSCH_BMI270_FEAT_NO_MOTION,
} zsw_imu_feature_t;

typedef enum zsw_imu_evt_type_t {
    ZSW_IMU_EVT_TYPE_XYZ,
    ZSW_IMU_EVT_TYPE_DOOUBLE_TAP,
    ZSW_IMU_EVT_TYPE_STEP,
    ZSW_IMU_EVT_TYPE_STEP_ACTIVITY,
    ZSW_IMU_EVT_TYPE_TILT,
    ZSW_IMU_EVT_TYPE_GESTURE,
    ZSW_IMU_EVT_TYPE_SIGNIFICANT_MOTION,
    ZSW_IMU_EVT_TYPE_WRIST_WAKEUP,
    ZSW_IMU_EVT_TYPE_NO_MOTION,
    ZSW_IMU_EVT_TYPE_ANY_MOTION,
    ZSW_IMU_EVT_TYPE_STEP_SNAPSHOT,
} zsw_imu_evt_type_t;

typedef enum zsw_imu_data_step_activity_t {
    ZSW_IMU_EVT_STEP_ACTIVITY_STILL,
    ZSW_IMU_EVT_STEP_ACTIVITY_WALK,
    ZSW_IMU_EVT_STEP_ACTIVITY_RUN,
    ZSW_IMU_EVT_STEP_ACTIVITY_UNKNOWN,
} zsw_imu_data_step_activity_t;

typedef enum zsw_imu_data_step_gesture_t {
    ZSW_IMU_EVT_GESTURE_UNKNOWN,
    ZSW_IMU_EVT_GESTURE_PUSH_ARM_DOWN,
    ZSW_IMU_EVT_GESTURE_PIVOT_UP,
    ZSW_IMU_EVT_GESTURE_WRIST_SHAKE,
    ZSW_IMU_EVT_GESTURE_FLICK_IN,
    ZSW_IMU_EVT_GESTURE_FLICK_OUT
} zsw_imu_data_step_gesture_t;

typedef struct zsw_imu_data_xyz_t {
    int16_t x;
    int16_t y;
    int16_t z;
} zsw_imu_data_xyz_t;

typedef struct zsw_imu_data_step_t {
    uint32_t count;
} zsw_imu_data_step_t;

typedef struct zsw_imu_evt_t {
    zsw_imu_evt_type_t type;
    union {
        zsw_imu_data_xyz_t            xyz;
        zsw_imu_data_step_t           step;
        zsw_imu_data_step_activity_t  step_activity;
        zsw_imu_data_step_gesture_t   gesture;
    } data;
} zsw_imu_evt_t;

typedef enum {
    ZSW_IMU_MOTION_UNKNOWN,
    ZSW_IMU_MOTION_MOVING,
    ZSW_IMU_MOTION_STATIONARY,
} zsw_imu_motion_state_t;

/** @brief Initialize IMU triggers, activity detection, and motion tracking. */
int zsw_imu_init(void);

/**
 * @brief Request or release one-second step snapshots. Thread-only and idempotent.
 *
 * Polling stops after all requests are released; step interrupts are unaffected.
 * Do not copy or modify req. Scheduling is asynchronous; failures are retried.
 * @return 0 if accepted; negative errno leaves the request unchanged.
 */
int zsw_imu_request_step_snapshots(struct zsw_imu_step_snapshot_request *req, bool enable);

/** @brief Fetch acceleration components in m/s^2. */
int zsw_imu_fetch_accel_f(float *x, float *y, float *z);

/** @brief Fetch angular velocity components in rad/s. */
int zsw_imu_fetch_gyro_f(float *x, float *y, float *z);

/** @brief Fetch integer acceleration components in m/s^2; discard fractional parts. */
int zsw_imu_fetch_accel(int16_t *x, int16_t *y, int16_t *z);

/** @brief Fetch integer angular velocity components in rad/s; discard fractional parts. */
int zsw_imu_fetch_gyro(int16_t *x, int16_t *y, int16_t *z);

/** @brief Fetch IMU temperature in Celsius. */
int zsw_imu_fetch_temperature(float *temperature);

/** @brief Fetch the cumulative step count including the configured offset. */
int zsw_imu_fetch_num_steps(uint32_t *num_steps);

/** @brief Get the last activity reported by the IMU. */
int zsw_imu_fetch_step_activity(zsw_imu_data_step_activity_t *activity);

/** @brief Set the fetched step-count offset; raw events and snapshots are unchanged. */
int zsw_imu_set_step_offset(int starting_step_offset);

/** @brief Reset the hardware step counter and clear the step-count offset. */
int zsw_imu_reset_step_count(void);

/** @brief Release a feature claim; disable the feature after its last release. */
int zsw_imu_feature_disable(zsw_imu_feature_t feature);

/** @brief Claim a feature; int_en selects interrupts on the first enable. */
int zsw_imu_feature_enable(zsw_imu_feature_t feature, bool int_en);

/**
 * @brief Request 50 Hz acceleration, or restore 100 Hz when enable is false.
 * An active gyro keeps the full rate. Returns 0 on success or negative errno.
 */
int zsw_imu_set_low_power(bool enable);

/** @brief Get the motion state tracked from boot, independent of display power. */
zsw_imu_motion_state_t zsw_imu_get_motion_state(void);
