/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2025 ZSWatch Project, Leonardo Bispo, Jakob Krantz.
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

/*
 * @see https://github.com/xioTechnologies/Fusion
 */

#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <errno.h>

#include "../ext_drivers/fusion/Fusion/Fusion.h"
#include "../ext_drivers/fusion/Fusion/FusionCompass.h"

#include "sensor_fusion/zsw_sensor_fusion.h"
#include "../sensors/zsw_imu.h"
#include "../sensors/zsw_magnetometer.h"
#include "../ble/zsw_gatt_sensor_server.h"
#include <string.h>

#ifdef CONFIG_SEND_SENSOR_READING_OVER_RTT
#include <SEGGER_RTT.h>
#endif

#define SAMPLE_RATE_HZ  100
#define SENSOR_GF       9.806650

LOG_MODULE_REGISTER(sf, CONFIG_ZSW_SENSORS_FUSION_LOG_LEVEL);

static void sensor_fusion_timeout(struct k_work *item);
K_WORK_DELAYABLE_DEFINE(sensor_fusion_timer, sensor_fusion_timeout);

// Define calibration (replace with actual calibration data if available)
static const FusionMatrix gyroscopeMisalignment = {.element.xx = 1.0f,
                                                   .element.xy = 0.0f,
                                                   .element.xz = 0.0f,
                                                   .element.yx = 0.0f,
                                                   .element.yy = 1.0f,
                                                   .element.yz = 0.0f,
                                                   .element.zx = 0.0f,
                                                   .element.zy = 0.0f,
                                                   .element.zz = 1.0f
                                                  };
static const FusionVector gyroscopeSensitivity = {{1.0f, 1.0f, 1.0f}};
static const FusionVector gyroscopeOffset = {{0.0f, 0.0f, 0.0f}};
static const FusionMatrix accelerometerMisalignment = {.element.xx = 1.0f,
                                                       .element.xy = 0.0f,
                                                       .element.xz = 0.0f,
                                                       .element.yx = 0.0f,
                                                       .element.yy = 1.0f,
                                                       .element.yz = 0.0f,
                                                       .element.zx = 0.0f,
                                                       .element.zy = 0.0f,
                                                       .element.zz = 1.0f
                                                      };
static const FusionVector accelerometerSensitivity = {{1.0f, 1.0f, 1.0f}};
static const FusionVector accelerometerOffset = {{0.0f, 0.0f, 0.0f}};
static const FusionMatrix softIronMatrix = {.element.xx = 1.0f,
                                            .element.xy = 0.0f,
                                            .element.xz = 0.0f,
                                            .element.yx = 0.0f,
                                            .element.yy = 1.0f,
                                            .element.yz = 0.0f,
                                            .element.zx = 0.0f,
                                            .element.zy = 0.0f,
                                            .element.zz = 1.0f
                                           };
static const FusionVector hardIronOffset = {{0.0f, 0.0f, 0.0f}};

// Initialise algorithms
static FusionOffset offset;
static FusionAhrs ahrs;
static int32_t previousTimestamp;
static sensor_fusion_t readings;
static struct k_work_sync cancel_work_sync;
static zsw_quat_t readings_quat;
static float last_delta_time_s = 0.0f;
static float last_heading = 0.0f;

#ifdef CONFIG_SEND_SENSOR_READING_OVER_RTT
#define UP_BUFFER_SIZE 256
static uint8_t up_buffer[UP_BUFFER_SIZE];
#endif

static void sensor_fusion_timeout(struct k_work *work)
{
    int ret = 0;

    FusionVector gyroscope;
    FusionVector accelerometer;
    FusionVector magnetometer;

    uint32_t start = k_uptime_get_32();
    ret = zsw_imu_fetch_gyro_f(&gyroscope.axis.x, &gyroscope.axis.y, &gyroscope.axis.z);
    if (ret != 0) {
        LOG_ERR("zsw_imu_fetch_gyro_f err: %d", ret);
    }
    // Convert from rad/s to deg/s
    gyroscope.axis.x = gyroscope.axis.x * (180.0F / M_PI);
    gyroscope.axis.y = gyroscope.axis.y * (180.0F / M_PI);
    gyroscope.axis.z = gyroscope.axis.z * (180.0F / M_PI);

    ret = zsw_imu_fetch_accel_f(&accelerometer.axis.x, &accelerometer.axis.y, &accelerometer.axis.z);
    if (ret != 0) {
        LOG_ERR("zsw_imu_fetch_accel_f err: %d", ret);
    }
    // IMU driver converts to m/s2 by multiplying to 10, convert back to g-force
    accelerometer.axis.x /= SENSOR_GF;
    accelerometer.axis.y /= SENSOR_GF;
    accelerometer.axis.z /= SENSOR_GF;

#ifdef CONFIG_SENSOR_FUSION_INCLUDE_MAGNETOMETER
    ret = zsw_magnetometer_get_all(&magnetometer.axis.x, &magnetometer.axis.y, &magnetometer.axis.z);
    if (ret != 0) {
        LOG_ERR("zsw_magnetometer_get_all err: %d", ret);
    }
#endif

    // Apply calibration
    gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
    accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity,
                                              accelerometerOffset);
#ifdef CONFIG_SENSOR_FUSION_INCLUDE_MAGNETOMETER
    magnetometer = FusionCalibrationMagnetic(magnetometer, softIronMatrix, hardIronOffset);
#endif

    // Update gyroscope offset correction algorithm
    gyroscope = FusionOffsetUpdate(&offset, gyroscope);

    // Calculate delta time (in seconds) to account for gyroscope sample clock error
    // Clamp deltaTime for robustness: seed on first call, clamp to reasonable range
    const float deltaTime = (start - previousTimestamp) / 1000.0f;
    previousTimestamp = start;
    
    // Seed deltaTime on first call (when previousTimestamp was 0), clamp to [0.001, 0.1]
    if (deltaTime <= 0.0f || deltaTime > 0.1f) {
        last_delta_time_s = (last_delta_time_s > 0.0f) ? last_delta_time_s : (1.0f / SAMPLE_RATE_HZ);
    } else {
        last_delta_time_s = deltaTime;
    }

    // Update gyroscope AHRS algorithm
#ifdef CONFIG_SENSOR_FUSION_INCLUDE_MAGNETOMETER
    FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, last_delta_time_s);
#else
    FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, last_delta_time_s);
#endif
    // Print algorithm outputs
    const FusionQuaternion q = FusionAhrsGetQuaternion(&ahrs);
    const FusionEuler euler = FusionQuaternionToEuler(q);
    const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);
    const FusionAhrsInternalStates states = FusionAhrsGetInternalStates(&ahrs);
    const FusionAhrsFlags flags = FusionAhrsGetFlags(&ahrs);
    
#ifdef CONFIG_SENSOR_FUSION_INCLUDE_MAGNETOMETER
    // Calculate proper magnetic heading using compass algorithm
    float heading = FusionCompassCalculateHeading(FusionConventionNwu, accelerometer, magnetometer);
#endif

    readings.pitch = euler.angle.pitch;
    readings.roll = euler.angle.roll;
    readings.yaw = euler.angle.yaw;
    readings.x = earth.axis.x;
    readings.y = earth.axis.y;
    readings.z = earth.axis.z;

    readings_quat.w = q.element.w;
    readings_quat.x = q.element.x;
    readings_quat.y = q.element.y;
    readings_quat.z = q.element.z;

#ifdef CONFIG_SENSOR_FUSION_INCLUDE_MAGNETOMETER
    last_heading = heading;
#endif

#ifdef CONFIG_SENSOR_FUSION_INCLUDE_MAGNETOMETER
    LOG_DBG("R %0.1f, P %0.1f, Y %0.1f, H %0.1f | Init:%d AngRec:%d AccRec:%d MagRec:%d | AccErr:%0.1f AccIgn:%d MagErr:%0.1f MagIgn:%d",
            euler.angle.roll, euler.angle.pitch, euler.angle.yaw, heading,
            flags.initialising, flags.angularRateRecovery, flags.accelerationRecovery, flags.magneticRecovery,
            states.accelerationError, states.accelerometerIgnored, states.magneticError, states.magnetometerIgnored);
#else
    LOG_DBG("R %0.1f, P %0.1f, Y %0.1f | Init:%d AngRec:%d AccRec:%d | AccErr:%0.1f AccIgn:%d",
            euler.angle.roll, euler.angle.pitch, euler.angle.yaw,
            flags.initialising, flags.angularRateRecovery, flags.accelerationRecovery,
            states.accelerationError, states.accelerometerIgnored);
#endif
#if CONFIG_SEND_SENSOR_READING_OVER_RTT
    uint8_t data_buf[UP_BUFFER_SIZE];
    int len = snprintf(data_buf, UP_BUFFER_SIZE,
                       "%0.5f, %0.1f, %0.1f, %0.1f, %0.5f, %0.5f, %0.5f, %0.5f, %0.5f, %0.5f, %0.5f, %0.5f, %0.5f\n",
                       k_uptime_get_32() / 1000.0, euler.angle.roll, euler.angle.pitch,
                       euler.angle.yaw, gyroscope.axis.x,
                       gyroscope.axis.y, gyroscope.axis.z,  accelerometer.axis.x, accelerometer.axis.y, accelerometer.axis.z,
                       magnetometer.axis.x, magnetometer.axis.y, magnetometer.axis.z);
    len = SEGGER_RTT_Write(CONFIG_SENSOR_LOG_RTT_TRANSFER_CHANNEL, data_buf, len);
#endif

    // Schedule next update, ensuring non-negative delay
    uint32_t elapsed_ms = k_uptime_get_32() - start;
    uint32_t target_period_ms = 1000 / SAMPLE_RATE_HZ;
    int32_t delay_ms = (int32_t)target_period_ms - (int32_t)elapsed_ms;
    
    // If processing took longer than target period, schedule immediately
    k_work_schedule(&sensor_fusion_timer, K_MSEC(delay_ms > 0 ? delay_ms : 0));
}

int zsw_sensor_fusion_init(void)
{
#if CONFIG_SEND_SENSOR_READING_OVER_RTT
    SEGGER_RTT_ConfigUpBuffer(CONFIG_SENSOR_LOG_RTT_TRANSFER_CHANNEL, "FUSION",
                              up_buffer, UP_BUFFER_SIZE,
                              SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#endif
    int ret;

    ret = zsw_imu_feature_enable(ZSW_IMU_FEATURE_GYRO, false);
    if (ret != 0) {
        LOG_ERR("zsw_imu_feature_enable err: %d", ret);
        return ret;
    }

#ifdef CONFIG_SENSOR_FUSION_INCLUDE_MAGNETOMETER
    ret = zsw_magnetometer_set_enable(true);
    if (ret != 0) {
        LOG_ERR("zsw_magnetometer_set_enable err: %d", ret);
        return ret;
    }
#endif

    memset(&ahrs, 0, sizeof(ahrs));

    // Gyroscope offset correction is tuned via constants in FusionOffset.c:
    // - CUTOFF_FREQUENCY (0.02 Hz): Filter cutoff for offset estimation
    // - TIMEOUT (5 seconds): Stationary period required before offset correction begins
    // - THRESHOLD (3.0 deg/s): Max angular rate considered stationary
    // Modify these constants in the Fusion library source if further tuning is needed
    FusionOffsetInitialise(&offset, SAMPLE_RATE_HZ);
    FusionAhrsInitialise(&ahrs);

    // Set AHRS algorithm settings
    // Tuned for faster recovery after aggressive motion:
    // - Higher gain (1.0f) for faster convergence to gravity after motion
    // - Loosened acceleration rejection (90.0f) so accelerometer is trusted more quickly after motion
    // - Shortened recovery trigger period (2s) for earlier snap-back after prolonged acceleration
    const FusionAhrsSettings settings = {
        .convention = FusionConventionNwu,
        .gain = 1.0f,
        .gyroscopeRange = 2000.0f, /* app/drivers/sensor/bmi270/bosch_bmi270.c:426 */
        .accelerationRejection = 90.0f,
        .magneticRejection = 10.0f,
        .recoveryTriggerPeriod = 2 * SAMPLE_RATE_HZ, /* 2 seconds */
    };

    FusionAhrsSetSettings(&ahrs, &settings);

    k_work_schedule(&sensor_fusion_timer, K_MSEC(1000 / SAMPLE_RATE_HZ));

    return 0;
}

void zsw_sensor_fusion_deinit(void)
{
    k_work_cancel_delayable_sync(&sensor_fusion_timer, &cancel_work_sync);
    zsw_imu_feature_disable(ZSW_IMU_FEATURE_GYRO);
#ifdef CONFIG_SENSOR_FUSION_INCLUDE_MAGNETOMETER
    zsw_magnetometer_set_enable(false);
#endif
}

int zsw_sensor_fusion_fetch_all(sensor_fusion_t *p_readings)
{
    memcpy(p_readings, &readings, sizeof(sensor_fusion_t));
    return 0;
}

int zsw_sensor_fusion_get_heading(float *heading)
{
#ifdef CONFIG_SENSOR_FUSION_INCLUDE_MAGNETOMETER
    // Return proper magnetic heading calculated via FusionCompassCalculateHeading
    *heading = last_heading;
#else
    // No magnetometer available, return yaw from gyroscope integration
    *heading = readings.yaw;
#endif
    return 0;
}

int zsw_sensor_fusion_get_quaternion(zsw_quat_t *q)
{
    if (!q) {
        return -EINVAL;
    }
    *q = readings_quat;
    return 0;
}
