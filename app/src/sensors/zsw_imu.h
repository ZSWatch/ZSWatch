/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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

#include "../../drivers/sensor/bmi270/bosch_bmi270.h"

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
    ZSW_IMU_EVT_TYPE_ANY_MOTION
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
    int16_t count;
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

int zsw_imu_init(void);

/*
* Get the accelerometer data in m/s^2.
*/
int zsw_imu_fetch_accel_f(float *x, float *y, float *z);

/*
* Get the gyroscope data in rad/s.
*/
int zsw_imu_fetch_gyro_f(float *x, float *y, float *z);

int zsw_imu_fetch_accel(int16_t *x, int16_t *y, int16_t *z);

int zsw_imu_fetch_gyro(int16_t *x, int16_t *y, int16_t *z);

int zsw_imu_fetch_temperature(float *temperature);

int zsw_imu_fetch_num_steps(uint32_t *num_steps);

/*
* Set an offset when fetching the number of steps.
* This can be used if zsw_imu_fetch_num_steps should
* return daily steps.
*/
int zsw_imu_set_step_offset(int starting_step_offset);

int zsw_imu_reset_step_count(void);

int zsw_imu_feature_disable(zsw_imu_feature_t feature);

int zsw_imu_feature_enable(zsw_imu_feature_t feature, bool int_en);