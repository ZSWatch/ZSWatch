/**
 * @file sensor_fusion.c
 * @author Leonardo Bispo
 *
 * @brief Sensor fusion for BMI270 IMU+Gyro+Magnetometer
 *
 * @see https://github.com/xioTechnologies/Fusion
 *
 * */

#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "../ext_drivers/fusion/Fusion/Fusion.h"

#include "sensor_fusion/sensor_fusion.h"
#include "../sensors/zsw_imu.h"
#include "../sensors/zsw_magnetometer.h"

#define SAMPLE_RATE_HZ 100

LOG_MODULE_REGISTER(sensor_fusion, CONFIG_ZSW_SENSORS_FUSION_LOG_LEVEL);

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
static sensor_fusion_t _readings;
static struct k_work_sync cancel_work_sync;

static void sensor_fusion_timeout(struct k_work *item);
K_WORK_DELAYABLE_DEFINE(sensor_fusion_timer, sensor_fusion_timeout);

static void sensor_fusion_timeout(struct k_work *item)
{
    int ret = 0;

    // Acquire latest sensor data
    const int64_t timestamp = k_uptime_get();

    FusionVector gyroscope;

    ret = zsw_imu_fetch_gyro_f(&gyroscope.axis.x, &gyroscope.axis.y, &gyroscope.axis.z);
    if (ret != 0) {
        LOG_ERR("zsw_imu_fetch_gyro_f err: %d", ret);
    }

    FusionVector accelerometer;

    /// @todo needs to implement get_raw()
    ret = zsw_imu_fetch_accel_f(&accelerometer.axis.x, &accelerometer.axis.y, &accelerometer.axis.z);
    if (ret != 0) {
        LOG_ERR("zsw_imu_fetch_gyro_f err: %d", ret);
    }
    /// @todo IMU driver converts to m/s2 by multiplying to 10, convert back to g-force
    accelerometer.axis.x /= 10.0F;
    accelerometer.axis.y /= 10.0F;
    accelerometer.axis.z /= 10.0F;

    FusionVector magnetometer;

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
    static int64_t previousTimestamp;
    const float deltaTime = (float)(timestamp - previousTimestamp) / (float)CONFIG_SYS_CLOCK_TICKS_PER_SEC;
    previousTimestamp = timestamp;

    // Update gyroscope AHRS algorithm
    FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);

    // Print algorithm outputs
    const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
    const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);

    _readings.pitch = euler.angle.pitch;
    _readings.roll = euler.angle.roll;
    _readings.yaw = euler.angle.yaw;
    _readings.x = earth.axis.x;
    _readings.y = earth.axis.y;
    _readings.z = earth.axis.z;

    LOG_DBG("Roll %0.1f, Pitch %0.1f, Yaw %0.1f, X %0.1f, Y %0.1f, Z %0.1f\n", euler.angle.roll, euler.angle.pitch,
            euler.angle.yaw, earth.axis.x, earth.axis.y, earth.axis.z);

    k_work_schedule(&sensor_fusion_timer, K_MSEC(1000 / SAMPLE_RATE_HZ));
}

void sensor_fusion_fetch_all(sensor_fusion_t *readings)
{
    *readings = _readings;
}

void sensor_fusion_init()
{
    zsw_imu_feature_enable(ZSW_IMU_FEATURE_GYRO, false);

    FusionOffsetInitialise(&offset, SAMPLE_RATE_HZ);
    FusionAhrsInitialise(&ahrs);

    // Set AHRS algorithm settings
    /// @todo need to change those, don't know what to tho
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

void sensor_fusion_deinit()
{
    k_work_cancel_delayable_sync(&sensor_fusion_timer, &cancel_work_sync);
    zsw_imu_feature_disable(ZSW_IMU_FEATURE_GYRO);
}