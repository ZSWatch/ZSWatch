/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Leonardo Bispo, Jakob Krantz.
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

#ifdef CONFIG_SEND_SENSOR_READING_OVER_RTT
#define UP_BUFFER_SIZE 256
static uint8_t up_buffer[UP_BUFFER_SIZE];
#endif

static void sensor_fusion_timeout(struct k_work *)
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
        LOG_ERR("zsw_imu_fetch_gyro_f err: %d", ret);
    }
    // IMU driver converts to m/s2 by multiplying to 10, convert back to g-force
    accelerometer.axis.x /= SENSOR_GF;
    accelerometer.axis.y /= SENSOR_GF;
    accelerometer.axis.z /= SENSOR_GF;

    ret = zsw_magnetometer_get_all(&magnetometer.axis.x, &magnetometer.axis.y, &magnetometer.axis.z);
    if (ret != 0) {
        LOG_ERR("zsw_imu_fetch_gyro_f err: %d", ret);
    }

    // Apply calibration
    gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
    accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity,
                                              accelerometerOffset);
    magnetometer = FusionCalibrationMagnetic(magnetometer, softIronMatrix, hardIronOffset);

    // Update gyroscope offset correction algorithm
    gyroscope = FusionOffsetUpdate(&offset, gyroscope);

    // Calculate delta time (in seconds) to account for gyroscope sample clock error
    const float deltaTime = (start - previousTimestamp) / 1000.0f;
    previousTimestamp = start;

    // Update gyroscope AHRS algorithm
#ifdef CONFIG_SENSOR_FUSION_INCLUDE_MAGNETOMETER
    FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);
#else
    FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, deltaTime);
#endif
    // Print algorithm outputs
    const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
    const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);
    float heading = FusionCompassCalculateHeading(FusionConventionNwu, accelerometer, magnetometer);

    readings.pitch = euler.angle.pitch;
    readings.roll = euler.angle.roll;
    readings.yaw = euler.angle.yaw;
    readings.x = earth.axis.x;
    readings.y = earth.axis.y;
    readings.z = earth.axis.z;

    LOG_DBG("Roll %0.1f, Pitch %0.1f, Yaw %0.1f, Head: %01f, X %0.2f, Y %0.2f, Z %0.1f, X %0.1f, Y %0.1f, Z %0.1f, X %0.1f, Y %0.1f, Z %0.1f\n",
            euler.angle.roll, euler.angle.pitch,
            euler.angle.yaw, heading, /*earth.axis.x, earth.axis.y, earth.axis.z*/ accelerometer.axis.x, accelerometer.axis.y,
            accelerometer.axis.z, gyroscope.axis.x, gyroscope.axis.y, gyroscope.axis.z, magnetometer.axis.x, magnetometer.axis.y,
            magnetometer.axis.z );
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

    k_work_schedule(&sensor_fusion_timer, K_MSEC((1000 / SAMPLE_RATE_HZ) - (k_uptime_get_32() - start)));
}

void zsw_sensor_fusion_init(void)
{
#if CONFIG_SEND_SENSOR_READING_OVER_RTT
    SEGGER_RTT_ConfigUpBuffer(CONFIG_SENSOR_LOG_RTT_TRANSFER_CHANNEL, "FUSION",
                              up_buffer, UP_BUFFER_SIZE,
                              SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#endif
    zsw_imu_feature_enable(ZSW_IMU_FEATURE_GYRO, false);
    zsw_magnetometer_set_enable(true);

    memset(&ahrs, 0, sizeof(ahrs));

    FusionOffsetInitialise(&offset, SAMPLE_RATE_HZ);
    FusionAhrsInitialise(&ahrs);

    // Set AHRS algorithm settings
    /// @todo may want to tune more.
    const FusionAhrsSettings settings = {
        .convention = FusionConventionNwu,
        .gain = 0.5f,
        .gyroscopeRange = 2000.0f, /* app/drivers/sensor/bmi270/bosch_bmi270.c:426 */
        .accelerationRejection = 10.0f,
        .magneticRejection = 10.0f,
        .recoveryTriggerPeriod = 5 * SAMPLE_RATE_HZ, /* 5 seconds */
    };

    FusionAhrsSetSettings(&ahrs, &settings);

    k_work_schedule(&sensor_fusion_timer, K_MSEC(1000 / SAMPLE_RATE_HZ));
}

void zsw_sensor_fusion_deinit(void)
{
    k_work_cancel_delayable_sync(&sensor_fusion_timer, &cancel_work_sync);
    zsw_imu_feature_disable(ZSW_IMU_FEATURE_GYRO);
    zsw_magnetometer_set_enable(false);
}

int zsw_sensor_fusion_fetch_all(sensor_fusion_t *p_readings)
{
    memcpy(p_readings, &readings, sizeof(sensor_fusion_t));
    return 0;
}

int zsw_sensor_fusion_get_heading(float *heading)
{
    // @todo: implement, this is not correct magnetic heading. Use FusionCompassCalculateHeading
    *heading = readings.yaw;
    return 0;
}