/* bosch_bmi270.h - Driver for Bosch BMI270 IMU. */

/*
 * Copyright (c) 2023, Daniel Kampert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/** @brief Step counting sensor channel.
*/
#define SENSOR_CHAN_STEPS               (SENSOR_CHAN_PRIV_START + 1)

/** @brief Activity detected channel.
*/
#define SENSOR_CHAN_ACTIVITY            (SENSOR_CHAN_PRIV_START + 2)

/** @brief Gesture detected channel.
*/
#define SENSOR_CHAN_GESTURE             (SENSOR_CHAN_PRIV_START + 3)

/** @brief Feature configuration channel.
*/
#define SENSOR_CHAN_FEATURE             (SENSOR_CHAN_PRIV_START + 4)

/** @brief Wrist gesture detection like flick in/out, push arm down(pivot up, wrist jiggle/shake).
*/
#define SENSOR_TRIG_WRIST_GESTURE       (SENSOR_TRIG_PRIV_START + 1)

/** @brief  Wrist wear wakeup feature is designed to detect any natural way of user moving the hand
 *          to see the watch dial when wearing a classical wrist watch.
*/
#define SENSOR_TRIG_WRIST_WAKE          (SENSOR_TRIG_PRIV_START + 2)

/** @brief Simple user activity detection like still, walking and running.
*/
#define SENSOR_TRIG_ACTIVITY            (SENSOR_TRIG_PRIV_START + 3)

/** @brief Step counter trigger channel.
*/
#define SENSOR_TRIG_STEP                (SENSOR_TRIG_PRIV_START + 4)

/** @brief  Significant motion detection (see https://source.android.com/docs/core/interaction/sensors/sensor-types?hl=de#significant_motion).
 *          Examples are walking, biking, sitting or moving a car.
*/
#define SENSOR_TRIG_SIG_MOTION          (SENSOR_TRIG_PRIV_START + 5)

/** @brief
*/
#define BOSCH_BMI270_FEAT_ENABLE        (0x01 << 0x00)

/** @brief
*/
#define BOSCH_BMI270_FEAT_DISABLE       (0x00 << 0x00)

/** @brief
*/
#define BOSCH_BMI270_FEAT_ACCEL         0

/** @brief
*/
#define BOSCH_BMI270_FEAT_GYRO          1

/** @brief
*/
#define BOSCH_BMI270_FEAT_SIG_MOTION    3

/** @brief
*/
#define BOSCH_BMI270_FEAT_ANY_MOTION    4

/** @brief
*/
#define BOSCH_BMI270_FEAT_NO_MOTION     5

/** @brief
*/
#define BOSCH_BMI270_FEAT_STEP_COUNTER  7

/** @brief
*/
#define BOSCH_BMI270_FEAT_STEP_ACTIVITY 8

/** @brief
*/
#define BOSCH_BMI270_FEAT_WRIST_GESTURE 19

/** @brief
*/
#define BOSCH_BMI270_FEAT_WEAR_WAKE_UP  20

#define BOSCH_BMI270_ACC_ODR_25D32_HZ   0x01
#define BOSCH_BMI270_ACC_ODR_25D16_HZ   0x02
#define BOSCH_BMI270_ACC_ODR_25D8_HZ    0x03
#define BOSCH_BMI270_ACC_ODR_25D4_HZ    0x04
#define BOSCH_BMI270_ACC_ODR_25D2_HZ    0x05
#define BOSCH_BMI270_ACC_ODR_25_HZ      0x06
#define BOSCH_BMI270_ACC_ODR_50_HZ      0x07
#define BOSCH_BMI270_ACC_ODR_100_HZ     0x08
#define BOSCH_BMI270_ACC_ODR_200_HZ     0x09
#define BOSCH_BMI270_ACC_ODR_400_HZ     0x0A
#define BOSCH_BMI270_ACC_ODR_800_HZ     0x0B
#define BOSCH_BMI270_ACC_ODR_1600_HZ    0x0C

#define BOSCH_BMI270_ACC_RANGE_2G       0x00
#define BOSCH_BMI270_ACC_RANGE_4G       0x01
#define BOSCH_BMI270_ACC_RANGE_8G       0x02
#define BOSCH_BMI270_ACC_RANGE_16G      0x03

#define BOSCH_BMI270_ACC_OSR1           0x01
#define BOSCH_BMI270_ACC_OSR2           0x02
#define BOSCH_BMI270_ACC_OSR4           0x04

#define BOSCH_BMI270_AVG1               0x01
#define BOSCH_BMI270_AVG2               0x02
#define BOSCH_BMI270_AVG4               0x04
#define BOSCH_BMI270_AVG8               0x08
#define BOSCH_BMI270_AVG16              0x10
#define BOSCH_BMI270_AVG32              0x20
#define BOSCH_BMI270_AVG64              0x40
#define BOSCH_BMI270_AVG128             0x80

#define BOSCH_BMI270_GYR_RANGE_2000     0x00
#define BOSCH_BMI270_GYR_RANGE_1000     0x01
#define BOSCH_BMI270_GYR_RANGE_500      0x02
#define BOSCH_BMI270_GYR_RANGE_250      0x03
#define BOSCH_BMI270_GYR_RANGE_125      0x04

#define BOSCH_BMI270_GYR_ODR_25_HZ      0x06
#define BOSCH_BMI270_GYR_ODR_50_HZ      0x07
#define BOSCH_BMI270_GYR_ODR_100_HZ     0x08
#define BOSCH_BMI270_GYR_ODR_200_HZ     0x09
#define BOSCH_BMI270_GYR_ODR_400_HZ     0x0A
#define BOSCH_BMI270_GYR_ODR_800_HZ     0x0B
#define BOSCH_BMI270_GYR_ODR_1600_HZ    0x0C
#define BOSCH_BMI270_GYR_ODR_3200_HZ    0x0D

#define BOSCH_BMI270_GYR_OSR4           0x00
#define BOSCH_BMI270_GYR_OSR2           0x01
#define BOSCH_BMI270_GYR_OSR1           0x02