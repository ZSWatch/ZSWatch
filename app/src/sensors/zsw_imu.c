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

#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/sys/atomic.h>

#include "zsw_clock.h"

#include "../../drivers/sensor/bmi270/zsw_bosch_bmi270.h"

#include "events/accel_event.h"
#include "sensors/zsw_imu.h"

LOG_MODULE_REGISTER(zsw_imu, CONFIG_ZSW_SENSORS_LOG_LEVEL);

ZBUS_CHAN_DECLARE(accel_data_chan);
static const struct device *const bmi270 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bmi270));
static struct sensor_trigger bmi270_trigger;
static int step_offset = 0;
static zsw_imu_data_step_activity_t last_step_activity = ZSW_IMU_EVT_STEP_ACTIVITY_UNKNOWN;
static atomic_t feature_refcount[BOSCH_BMI270_FEAT_WEAR_WAKE_UP + 1];
static K_MUTEX_DEFINE(feature_lock);
static atomic_t motion_state;
static bool tracking_any_motion;
static bool tracking_no_motion;
static bool low_power_requested;
static bool low_power_applied;
// Cleared when a rate write fails, so the next request writes both settings again
static bool accel_rate_valid = true;

#define MOTION_TRACKING_RETRY_S 30

static void motion_tracking_work_handler(struct k_work *work);
static int apply_accel_rate_locked(void);
static K_WORK_DELAYABLE_DEFINE(motion_tracking_work, motion_tracking_work_handler);

// Follows the motion events on the channel rather than the trigger handler, so injected
// events (shell "hr motion") update the state the same way as real interrupts.
static void motion_event(const struct zbus_channel *chan)
{
    const struct accel_event *event = zbus_chan_const_msg(chan);
    zsw_imu_motion_state_t state;

    if (event->data.type == ZSW_IMU_EVT_TYPE_NO_MOTION) {
        state = ZSW_IMU_MOTION_STATIONARY;
    } else if (event->data.type == ZSW_IMU_EVT_TYPE_ANY_MOTION) {
        state = ZSW_IMU_MOTION_MOVING;
    } else {
        return;
    }
    if (atomic_set(&motion_state, state) != (int)state) {
        k_work_reschedule(&motion_tracking_work, K_NO_WAIT);
    }
}

ZBUS_LISTENER_DEFINE(imu_motion_listener, motion_event);
ZBUS_CHAN_ADD_OBS(accel_data_chan, imu_motion_listener, 0);

static void bmi270_trigger_handler(const struct device *dev, const struct sensor_trigger *trig)
{
    zsw_imu_evt_t evt;

    LOG_DBG("BMI270 trigger handler. Type: %u", trig->type);

    switch ((int)trig->type) {
        case SENSOR_TRIG_SIG_MOTION: {
            evt.type = ZSW_IMU_EVT_TYPE_SIGNIFICANT_MOTION;

            break;
        }
        case SENSOR_TRIG_STEP: {
            struct sensor_value sensor_val;

            if (sensor_channel_get(bmi270, SENSOR_CHAN_STEPS, &sensor_val) == 0) {

                evt.type = ZSW_IMU_EVT_TYPE_STEP;
                evt.data.step.count = sensor_val.val1;

                LOG_DBG("No of steps counted  = %u", evt.data.step.count);
            }

            break;
        }
        case SENSOR_TRIG_ACTIVITY: {
            struct sensor_value sensor_val;

            if (sensor_channel_get(bmi270, SENSOR_CHAN_ACTIVITY, &sensor_val) == 0) {

                evt.type = ZSW_IMU_EVT_TYPE_STEP_ACTIVITY;
                evt.data.step_activity = sensor_val.val1;
                last_step_activity = evt.data.step_activity;

                const char *activity_output[4] = { "BMI2_STILL", "BMI2_WALK", "BMI2_RUN", "BMI2_UNKNOWN" };
                LOG_DBG("Step activity: %s", activity_output[evt.data.step_activity]);
            }

            break;
        }
        case SENSOR_TRIG_WRIST_WAKE: {
            evt.type = ZSW_IMU_EVT_TYPE_WRIST_WAKEUP;

            break;
        }
        case SENSOR_TRIG_WRIST_GESTURE: {
            struct sensor_value sensor_val;

            if (sensor_channel_get(bmi270, SENSOR_CHAN_GESTURE, &sensor_val) == 0) {

                evt.type = ZSW_IMU_EVT_TYPE_GESTURE;
                evt.data.gesture = sensor_val.val1;

                const char *gesture_output[6] = { "unknown_gesture", "push_arm_down", "pivot_up", "wrist_shake_jiggle", "flick_in", "flick_out" };
                LOG_DBG("Gesture detected: %s", gesture_output[evt.data.gesture]);
            }

            break;
        }
        case SENSOR_TRIG_STATIONARY: {
            evt.type = ZSW_IMU_EVT_TYPE_NO_MOTION;

            break;
        }
        case SENSOR_TRIG_MOTION: {
            evt.type = ZSW_IMU_EVT_TYPE_ANY_MOTION;

            break;
        }
        default: {
            break;
        }
    }

    zbus_chan_pub(&accel_data_chan, &evt, K_MSEC(250));
}

int zsw_imu_init(void)
{
    if (!device_is_ready(bmi270)) {
        LOG_ERR("No IMU found!");
        return -ENODEV;
    }

    // The type doesn´t care when all channels were selected.
    // We use the global handler here, because we don´t want to
    // separate the callbacks for now.
    //bmi270_trigger.type = SENSOR_TRIG_MOTION;
    bmi270_trigger.chan = SENSOR_CHAN_ALL;

    sensor_trigger_set(bmi270, &bmi270_trigger, bmi270_trigger_handler);

    // Always track whether the watch is moving or stationary.
    k_work_reschedule(&motion_tracking_work, K_NO_WAIT);

    return 0;
}

int zsw_imu_fetch_accel_f(float *x, float *y, float *z)
{
    struct sensor_value x_temp;
    struct sensor_value y_temp;
    struct sensor_value z_temp;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_ALL) != 0) {
        return -ENODATA;
    }

    if ((sensor_channel_get(bmi270, SENSOR_CHAN_ACCEL_X, &x_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_ACCEL_Y, &y_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_ACCEL_Z, &z_temp) != 0)) {
        return -ENODATA;
    }

    *x = sensor_value_to_float(&x_temp);
    *y = sensor_value_to_float(&y_temp);
    *z = sensor_value_to_float(&z_temp);

    return 0;
}

int zsw_imu_fetch_gyro_f(float *x, float *y, float *z)
{
    struct sensor_value x_temp;
    struct sensor_value y_temp;
    struct sensor_value z_temp;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_ALL) != 0) {
        return -ENODATA;
    }

    if ((sensor_channel_get(bmi270, SENSOR_CHAN_GYRO_X, &x_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_GYRO_Y, &y_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_GYRO_Z, &z_temp) != 0)) {
        return -ENODATA;
    }

    *x = sensor_value_to_float(&x_temp);
    *y = sensor_value_to_float(&y_temp);
    *z = sensor_value_to_float(&z_temp);

    return 0;
}

int zsw_imu_fetch_accel(int16_t *x, int16_t *y, int16_t *z)
{
    struct sensor_value x_temp;
    struct sensor_value y_temp;
    struct sensor_value z_temp;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_ALL) != 0) {
        return -ENODATA;
    }

    if ((sensor_channel_get(bmi270, SENSOR_CHAN_ACCEL_X, &x_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_ACCEL_Y, &y_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_ACCEL_Z, &z_temp) != 0)) {
        return -ENODATA;
    }

    *x = x_temp.val1;
    *y = y_temp.val1;
    *z = z_temp.val1;

    return 0;
}

int zsw_imu_fetch_gyro(int16_t *x, int16_t *y, int16_t *z)
{
    struct sensor_value x_temp;
    struct sensor_value y_temp;
    struct sensor_value z_temp;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_ALL) != 0) {
        return -ENODATA;
    }

    if ((sensor_channel_get(bmi270, SENSOR_CHAN_GYRO_X, &x_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_GYRO_Y, &y_temp) != 0) ||
        (sensor_channel_get(bmi270, SENSOR_CHAN_GYRO_Z, &z_temp) != 0)) {
        return -ENODATA;
    }

    *x = x_temp.val1;
    *y = y_temp.val1;
    *z = z_temp.val1;

    return 0;
}

int zsw_imu_fetch_num_steps(uint32_t *num_steps)
{
    struct sensor_value sensor_val;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_ALL) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bmi270, SENSOR_CHAN_STEPS, &sensor_val) != 0) {
        return -ENODATA;
    }

    *num_steps = sensor_val.val1 + step_offset;

    return 0;
}

int zsw_imu_fetch_step_activity(zsw_imu_data_step_activity_t *activity)
{
    if (activity == NULL) {
        return -EINVAL;
    }

    *activity = last_step_activity;
    return 0;
}

int zsw_imu_set_step_offset(int starting_step_offset)
{
    step_offset = starting_step_offset;
    return 0;
}

int zsw_imu_fetch_temperature(float *temperature)
{
    struct sensor_value sensor_val;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    if (sensor_sample_fetch_chan(bmi270, SENSOR_CHAN_AMBIENT_TEMP) != 0) {
        return -ENODATA;
    }

    if (sensor_channel_get(bmi270, SENSOR_CHAN_AMBIENT_TEMP, &sensor_val) != 0) {
        return -ENODATA;
    }

    *temperature = sensor_value_to_float(&sensor_val);

    return 0;
}

int zsw_imu_reset_step_count(void)
{
    struct sensor_value value;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    step_offset = 0;
    value.val1 = 0;
    value.val2 = 0;

    if (sensor_attr_set(bmi270, SENSOR_CHAN_STEPS, SENSOR_ATTR_OFFSET, &value) != 0) {
        return -EFAULT;
    }

    return 0;
}

int zsw_imu_feature_disable(zsw_imu_feature_t feature)
{
    struct sensor_value value;
    int rc = 0;

    if ((feature < 0) || (feature > BOSCH_BMI270_FEAT_WEAR_WAKE_UP)) {
        return -EINVAL;
    }

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    // Serialize claims, the power manager and HR motion tracking share these features.
    k_mutex_lock(&feature_lock, K_FOREVER);

    if (atomic_get(&feature_refcount[feature]) <= 0) {
        LOG_WRN("Feature %d already disabled", feature);
        goto out;
    }

    if (atomic_dec(&feature_refcount[feature]) != 1) {
        goto out;
    }

    value.val1 = feature;
    value.val2 = BOSCH_BMI270_FEAT_DISABLE;

    if (sensor_attr_set(bmi270, SENSOR_CHAN_FEATURE, SENSOR_ATTR_CONFIGURATION, &value) != 0) {
        atomic_inc(&feature_refcount[feature]);
        rc = -EFAULT;
    } else if (feature == ZSW_IMU_FEATURE_GYRO && apply_accel_rate_locked() != 0) {
        LOG_WRN("Failed to update accelerometer rate");
    }

out:
    k_mutex_unlock(&feature_lock);
    return rc;
}

int zsw_imu_feature_enable(zsw_imu_feature_t feature, bool int_en)
{
    struct sensor_value value;
    int rc = 0;

    if ((feature < 0) || (feature > BOSCH_BMI270_FEAT_WEAR_WAKE_UP)) {
        return -EINVAL;
    }

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    k_mutex_lock(&feature_lock, K_FOREVER);

    // TODO: handle interrupt enable/disable properly
    if (atomic_inc(&feature_refcount[feature]) != 0) {
        goto out;
    }

    value.val1 = feature;
    value.val2 = (int_en << 1) | BOSCH_BMI270_FEAT_ENABLE;

    if (sensor_attr_set(bmi270, SENSOR_CHAN_FEATURE, SENSOR_ATTR_CONFIGURATION, &value) != 0) {
        atomic_dec(&feature_refcount[feature]);
        rc = -EFAULT;
    } else if (feature == ZSW_IMU_FEATURE_GYRO && apply_accel_rate_locked() != 0) {
        LOG_WRN("Failed to update accelerometer rate");
    }

out:
    k_mutex_unlock(&feature_lock);
    return rc;
}

int zsw_imu_set_low_power(bool enable)
{
    int rc;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    k_mutex_lock(&feature_lock, K_FOREVER);
    low_power_requested = enable;
    rc = apply_accel_rate_locked();
    k_mutex_unlock(&feature_lock);

    return rc;
}

/*
 * The motion features run at 50 Hz, so they keep working at the lower rate with less
 * averaging. The gyro (sensor fusion) needs the full accelerometer rate.
 */
static int apply_accel_rate_locked(void)
{
    bool low_power = low_power_requested && atomic_get(&feature_refcount[ZSW_IMU_FEATURE_GYRO]) == 0;
    struct sensor_value odr = {.val1 = low_power ? BOSCH_BMI270_ACC_ODR_50_HZ : BOSCH_BMI270_ACC_ODR_100_HZ};
    struct sensor_value avg = {.val1 = low_power ? BOSCH_BMI270_AVG2 : BOSCH_BMI270_AVG4};
    int rc;

    if (accel_rate_valid && low_power == low_power_applied) {
        return 0;
    }

    rc = sensor_attr_set(bmi270, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &odr);
    if (rc == 0) {
        rc = sensor_attr_set(bmi270, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_OVERSAMPLING, &avg);
    }
    accel_rate_valid = rc == 0;
    if (rc == 0) {
        low_power_applied = low_power;
    }

    return rc;
}

static int tracking_feature(zsw_imu_feature_t feature, bool want, bool *held)
{
    if (want == *held) {
        return 0;
    }
    int rc = want ? zsw_imu_feature_enable(feature, true) : zsw_imu_feature_disable(feature);
    if (!rc) {
        *held = want;
    }
    return rc;
}

static void motion_tracking_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);
    zsw_imu_motion_state_t state = atomic_get(&motion_state);
    bool want_any = state != ZSW_IMU_MOTION_MOVING;
    bool want_no = state != ZSW_IMU_MOTION_STATIONARY;
    int rc = 0;

    /* Arm the next transition before dropping the previous claim. */
    if (want_any) {
        rc = tracking_feature(ZSW_IMU_FEATURE_ANY_MOTION, true, &tracking_any_motion);
    }
    if (!rc && want_no) {
        rc = tracking_feature(ZSW_IMU_FEATURE_NO_MOTION, true, &tracking_no_motion);
    }
    if (!rc && !want_any) {
        rc = tracking_feature(ZSW_IMU_FEATURE_ANY_MOTION, false, &tracking_any_motion);
    }
    if (!rc && !want_no) {
        rc = tracking_feature(ZSW_IMU_FEATURE_NO_MOTION, false, &tracking_no_motion);
    }
    if (rc) {
        // Keep a newer state from a motion event that arrived meanwhile.
        atomic_cas(&motion_state, state, ZSW_IMU_MOTION_UNKNOWN);
        LOG_ERR("Motion tracking configuration failed: %d", rc);
        k_work_schedule(&motion_tracking_work, K_SECONDS(MOTION_TRACKING_RETRY_S));
    }
}

zsw_imu_motion_state_t zsw_imu_get_motion_state(void)
{
    return atomic_get(&motion_state);
}
