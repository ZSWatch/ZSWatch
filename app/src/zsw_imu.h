#ifndef ZSW_IMU_H_
#define ZSW_IMU_H_

#include <zephyr/drivers/sensor.h>
#include <bmi270.h>

typedef enum zsw_imu_feature_t {
    ZSW_IMU_FEATURE_ACCEL = BMI2_ACCEL,
    ZSW_IMU_FEATURE_GYRO = BMI2_GYRO,
    ZSW_IMU_FEATURE_STEP_COUNTER = BMI2_STEP_COUNTER,
    ZSW_IMU_FEATURE_SIG_MOTION = BMI2_SIG_MOTION,
    ZSW_IMU_FEATURE_ANY_MOTION = BMI2_ANY_MOTION,
    ZSW_IMU_FEATURE_STEP_ACTIVITY = BMI2_STEP_ACTIVITY,
    ZSW_IMU_FEATURE_WRIST_GESTURE = BMI2_WRIST_GESTURE,
    ZSW_IMU_FEATURE_WRIST_WEAR_WAKE_UP = BMI2_WRIST_WEAR_WAKE_UP,
    ZSW_IMU_FEATURE_NO_MOTION = BMI2_NO_MOTION,
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

int zsw_imu_fetch_accel(int16_t *x, int16_t *y, int16_t *z);

int zsw_imu_fetch_gyro(int16_t *x, int16_t *y, int16_t *z);

int zsw_imu_fetch_temperature(struct sensor_value *temperature);

int zsw_imu_fetch_num_steps(uint32_t *num_steps);

int zsw_imu_reset_step_count(void);

int zsw_imu_feature_enable(zsw_imu_feature_t feature, bool int_en);

int zsw_imu_feature_disable(zsw_imu_feature_t feature);

#endif