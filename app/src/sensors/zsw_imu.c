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

#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "zsw_clock.h"

#include "../drivers/sensor/bmi270/bosch_bmi270.h"

#include "events/zsw_periodic_event.h"
#include "events/accel_event.h"
#include "sensors/zsw_imu.h"

LOG_MODULE_REGISTER(zsw_imu, CONFIG_ZSW_SENSORS_LOG_LEVEL);

static void zbus_periodic_slow_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(accel_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_LISTENER_DEFINE(zsw_imu_lis, zbus_periodic_slow_callback);
static const struct device *const bmi270 = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(bmi270));
static struct sensor_trigger bmi270_trigger;

static void zbus_periodic_slow_callback(const struct zbus_channel *chan)
{
    zsw_timeval_t time;
    struct accel_event evt = {
        .data.type = ZSW_IMU_EVT_TYPE_STEP,
        .data.data.step.count = 0
    };
    zsw_clock_get_time(&time);

    if ((time.tm.tm_hour == 23) && (time.tm.tm_min == 59)) {

        LOG_DBG("Reset step counter");
        zsw_imu_reset_step_count();
    }

    zbus_chan_pub(&accel_data_chan, &evt, K_MSEC(250));
}

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

    zsw_periodic_chan_add_obs(&periodic_event_1s_chan, &zsw_imu_lis);

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

    *num_steps = sensor_val.val1;

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

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    value.val1 = feature;
    value.val2 = BOSCH_BMI270_FEAT_DISABLE;

    if (sensor_attr_set(bmi270, SENSOR_CHAN_FEATURE, SENSOR_ATTR_CONFIGURATION, &value) != 0) {
        return -EFAULT;
    }

    return 0;
}

int zsw_imu_feature_enable(zsw_imu_feature_t feature, bool int_en)
{
    struct sensor_value value;

    if (!device_is_ready(bmi270)) {
        return -ENODEV;
    }

    value.val1 = feature;
    value.val2 = (int_en << 1) | BOSCH_BMI270_FEAT_ENABLE;

    if (sensor_attr_set(bmi270, SENSOR_CHAN_FEATURE, SENSOR_ATTR_CONFIGURATION, &value) != 0) {
        return -EFAULT;
    }

    return 0;
}