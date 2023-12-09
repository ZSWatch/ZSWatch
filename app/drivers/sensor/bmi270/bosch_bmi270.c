/* bosch_bmi270.c - Driver for Bosch BMI270 IMU. */

/*
 * Copyright (c) 2023, Daniel Kampert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

#include "bosch_bmi270.h"
#include "private/bosch_bmi270_config.h"

#if CONFIG_BMI270_PLUS_TRIGGER
#include "trigger/bosch_bmi270_interrupt.h"
#endif

#define DT_DRV_COMPAT                   bosch_bmi270_plus
#define BMI2_READ_WRITE_LEN             UINT8_C(46)

LOG_MODULE_REGISTER(bmi270, CONFIG_BOSCH_BMI270_PLUS_LOG_LEVEL);

#if(DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 0)
#warning "bmi270 driver enabled without any devices"
#endif

static const struct device *device;

/** @brief              Platform specific i2c read function.
 *  @param reg_addr     Register address
 *  @param p_reg_data   Register data
 *  @param len          Data length
 *  @param p_intf       Pointer to interface
 *  @return             0 when successful
*/
static BMI2_INTF_RETURN_TYPE bmi2_i2c_read(uint8_t reg_addr, uint8_t *p_reg_data, uint32_t len, void *p_intf)
{
    (void)p_intf;

    const struct bmi270_config *config = device->config;
    return i2c_burst_read_dt(&config->i2c, reg_addr, p_reg_data, len);
}

/** @brief              Platform specific i2c write function.
 *  @param reg_addr     Register address
 *  @param p_reg_data   Register data
 *  @param len          Data length
 *  @param p_intf       Pointer to interface
 *  @return             0 when successful
*/
static BMI2_INTF_RETURN_TYPE bmi2_i2c_write(uint8_t reg_addr, const uint8_t *p_reg_data, uint32_t len, void *p_intf)
{
    (void)p_intf;

    const struct bmi270_config *config = device->config;
    return i2c_burst_write_dt(&config->i2c, reg_addr, p_reg_data, len);
}

/** @brief          Platform specific us delay function.
 *  @param period   Sleep period
 *  @param p_intf   Pointer to interface
*/
static void bmi2_delay_us(uint32_t period, void *p_intf)
{
    (void)p_intf;

    k_usleep(period);
}

/** @brief
 *  @param p_val
 *  @param raw_val
 *  @param range
*/
static void bmi2_raw2accel_convert(struct sensor_value *p_val, int64_t raw_val, uint8_t range)
{
    raw_val = (raw_val * SENSOR_G * ((int64_t)range)) / INT16_MAX;

    p_val->val1 = raw_val / 1000000LL;
    p_val->val2 = raw_val % 1000000LL;
}

/** @brief
 *  @param p_val
 *  @param raw_val
 *  @param range
*/
static void bmi2_raw2gyro_convert(struct sensor_value *p_val, int64_t raw_val, uint16_t range)
{
    p_val->val1 = ((raw_val * ((int64_t)range) * SENSOR_PI) / (180LL * INT16_MAX)) / 1000000LL;
    p_val->val2 = ((raw_val * ((int64_t)range) * SENSOR_PI) / (180LL * INT16_MAX)) % 1000000LL;
}

/** @brief      Swap the X or Y axis.
 *  @param axis Target axis
 *  @return     Swapped axis
*/
static int bmi2_swap_axis(int axis)
{
    switch (axis) {
        case BMI2_X:
            return BMI2_Y;
        case BMI2_Y:
            return BMI2_X;
        case BMI2_NEG_X:
            return BMI2_NEG_Y;
        case BMI2_NEG_Y:
            return BMI2_NEG_X;
        default:
            return axis;
    }
}

/** @brief      Swap the sign of an axis.
 *  @param axis Target axis
 *  @return     Axis with swapped sign
*/
static int bmi2_swap_axis_sign(int axis)
{
    switch (axis) {
        case BMI2_X:
            return BMI2_NEG_X;
        case BMI2_Y:
            return BMI2_NEG_Y;
        case BMI2_NEG_X:
            return BMI2_X;
        case BMI2_NEG_Y:
            return BMI2_Y;
        default:
            return axis;
    }
}

/** @brief
 *  @param p_dev
 *  @return         0 when successful
*/
static int bmi2_configure_axis_remapping(const struct device *p_dev)
{
    struct bmi2_remap remapped_axis;
    struct bmi2_remap remapped_axis_read;
    struct bmi270_data *data = p_dev->data;
    const struct bmi270_config *config = p_dev->config;

    if (bmi2_get_remap_axes(&remapped_axis, &data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    // Initialize
    // x -> x
    // y -> y
    // z -> z
    remapped_axis.x = BMI2_X;
    remapped_axis.y = BMI2_Y;
    remapped_axis.z = BMI2_Z;

    // Invert x
    // x -> !x
    // y -> y
    if (config->invert_x) {
        remapped_axis.x = BMI2_NEG_X;
    }

    // Invert y
    // x -> x
    // y -> !y
    if (config->invert_y) {
        remapped_axis.y = BMI2_NEG_Y;
    }

    // Invert y
    // x -> y
    // y -> x
    if (config->swap_xy) {
        uint8_t temp;

        temp = remapped_axis.x;

        remapped_axis.x = remapped_axis.y;
        remapped_axis.y = temp;
    }

    if (config->rotation == 90) {
        remapped_axis.x = bmi2_swap_axis(remapped_axis.x);
        remapped_axis.x = bmi2_swap_axis_sign(remapped_axis.x);
        remapped_axis.y = bmi2_swap_axis(remapped_axis.y);
    } else if (config->rotation == 180) {
        remapped_axis.x = bmi2_swap_axis_sign(remapped_axis.x);
        remapped_axis.y = bmi2_swap_axis_sign(remapped_axis.y);
    } else if (config->rotation == 270) {
        remapped_axis.x = bmi2_swap_axis(remapped_axis.x);
        remapped_axis.y = bmi2_swap_axis(remapped_axis.y);
        remapped_axis.y = bmi2_swap_axis_sign(remapped_axis.y);
    }

    if ((bmi2_set_remap_axes(&remapped_axis, &data->bmi2) != BMI2_OK) ||
        (bmi2_get_remap_axes(&remapped_axis_read, &data->bmi2) != BMI2_OK)) {
        return -EFAULT;
    }

    if (!((remapped_axis.x == remapped_axis_read.x) && (remapped_axis.y == remapped_axis_read.y) &&
          (remapped_axis.z == remapped_axis_read.z))) {
        LOG_ERR("Wrong axis remapping read after setting");
        return -EFAULT;
    }

    return 0;
}

/** @brief
 *  @param p_dev
 *  @param channel
 *  @param attribute
 *  @param p_value
 *  @return             0 when successful
*/
static int bmi270_attr_set(const struct device *p_dev, enum sensor_channel channel, enum sensor_attribute attribute,
                           const struct sensor_value *p_value)
{
    __ASSERT_NO_MSG(p_value != NULL);

    if ((channel == SENSOR_CHAN_ACCEL_X) || (channel == SENSOR_CHAN_ACCEL_Y) || (channel == SENSOR_CHAN_ACCEL_Z) ||
        (channel == SENSOR_CHAN_ACCEL_XYZ)) {
        // Accelerometer configuration channel. Supported options:
        //  - Sampling frequency
        //  - Oversampling rate
        //  - Measurement range
        switch (attribute) {
            case SENSOR_ATTR_SAMPLING_FREQUENCY:
                return bmi2_set_accel_odr(p_dev, p_value);
            case SENSOR_ATTR_OVERSAMPLING:
                return bmi2_set_accel_osr(p_dev, p_value);
            case SENSOR_ATTR_FULL_SCALE:
                return bmi2_set_accel_range(p_dev, p_value);
            /*
            #if CONFIG_BMI270_PLUS_TRIGGER
                    case SENSOR_ATTR_SLOPE_DUR:
                        return bmi270_write_anymo_duration(p_dev, p_value->val1);
                    case SENSOR_ATTR_SLOPE_TH:
                        return bmi270_write_anymo_threshold(p_dev, *p_value);
            #endif
            */
            default:
                return -ENOTSUP;
        }
    } else if ((channel == SENSOR_CHAN_GYRO_X) || (channel == SENSOR_CHAN_GYRO_Y) || (channel == SENSOR_CHAN_GYRO_Z) ||
               (channel == SENSOR_CHAN_GYRO_XYZ)) {
        // Gyroscope configuration channel. Supported options:
        //  - Sampling frequency
        //  - Oversampling rate
        //  - Measurement range
        switch (attribute) {
            case SENSOR_ATTR_SAMPLING_FREQUENCY:
                return bmi2_set_gyro_odr(p_dev, p_value);
            case SENSOR_ATTR_OVERSAMPLING:
                return bmi2_set_gyro_osr(p_dev, p_value);
            case SENSOR_ATTR_FULL_SCALE:
                return bmi2_set_gyro_range(p_dev, p_value);
            default:
                return -ENOTSUP;
        }
    } else if (channel == SENSOR_CHAN_STEPS) {
        // Step counter configuration channel. Supported options:
        //  - Offset:
        //      Values are ignored. Only reset
        switch (attribute) {
            case SENSOR_ATTR_OFFSET:
                return bmi2_reset_step_counter(p_dev);
            default:
                return -ENOTSUP;
        }
    } else if (channel == SENSOR_CHAN_FEATURE) {
        // Feature configuration channel. Supported options:
        //  - Configuration
        //      p_value.val1:
        //          - BMI270 feature
        //      p_value.val2 Bit 0:
        //          - 1 - Enable the feature
        //          - 0 - Disable the feature
        //      p_value.val2 Bit 1:
        //          - 1 - Enable interrupts (only when Bit 0 is set)
        //          - 0 - Disable interrupts (only when Bit 0 is set)
        switch (attribute) {
            case SENSOR_ATTR_CONFIGURATION:
                if ((p_value->val2 & 0x01) == 0) {
                    return bmi2_disable_feature(p_dev, p_value->val1 & 0xFF);
                } else if ((p_value->val2 & 0x01) == 1) {
                    return bmi2_enable_feature(p_dev, p_value->val1, (p_value->val2 >> 1) & 0x01);
                }
            default:
                return -ENOTSUP;
        }
    } else if (channel == SENSOR_CHAN_CONFIG) {
        // TODO: Implement this
        return -ENOTSUP;
    }

    return -ENOTSUP;
}

/** @brief
 *  @param p_dev
 *  @param channel
 *  @return         0 when successful
*/
static int bmi270_sample_fetch(const struct device *p_dev, enum sensor_channel channel)
{
    uint16_t temp;
    enum pm_device_state pm_state;
    struct bmi270_data *data = p_dev->data;
    struct bmi2_sens_data sensor_data;

    pm_device_state_get(p_dev, &pm_state);
    if (pm_state != PM_DEVICE_STATE_ACTIVE) {
        return -EFAULT;
    }

    if ((channel != SENSOR_CHAN_ALL) && (channel != SENSOR_CHAN_ACCEL_XYZ) && (channel != SENSOR_CHAN_GYRO_XYZ) &&
        (channel != SENSOR_CHAN_ACCEL_X) && (channel != SENSOR_CHAN_ACCEL_Y) && (channel != SENSOR_CHAN_ACCEL_Z) &&
        (channel != SENSOR_CHAN_GYRO_X) && (channel != SENSOR_CHAN_GYRO_Y) && (channel != SENSOR_CHAN_GYRO_Z) &&
        (channel != SENSOR_CHAN_AMBIENT_TEMP)) {
        return -ENOTSUP;
    }

    LOG_DBG("Start fetching new data...");

    if (bmi2_get_sensor_data(&sensor_data, &data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    if ((channel == SENSOR_CHAN_ALL) || (channel == SENSOR_CHAN_AMBIENT_TEMP)) {
        if (bmi2_get_temperature_data(&temp, &data->bmi2) != BMI2_OK) {
            return -EFAULT;
        }

        data->temp = temp;
    }

    data->ax = sensor_data.acc.x;
    data->ay = sensor_data.acc.y;
    data->az = sensor_data.acc.z;

    data->gx = sensor_data.gyr.x;
    data->gy = sensor_data.gyr.y;
    data->gz = sensor_data.gyr.z;

    LOG_DBG("New data ready");

    return 0;
}

/** @brief
 *  @param p_dev
 *  @param channel
 *  @param p_value
 *  @return         0 when successful
*/
static int bmi270_channel_get(const struct device *p_dev, enum sensor_channel channel, struct sensor_value *p_value)
{
    struct bmi270_data *data = p_dev->data;

    __ASSERT_NO_MSG(p_value != NULL);

    if (channel == SENSOR_CHAN_ACCEL_X) {
        bmi2_raw2accel_convert(p_value, data->ax, data->acc_range);
    } else if (channel == SENSOR_CHAN_ACCEL_Y) {
        bmi2_raw2accel_convert(p_value, data->ay, data->acc_range);
    } else if (channel == SENSOR_CHAN_ACCEL_Z) {
        bmi2_raw2accel_convert(p_value, data->az, data->acc_range);
    } else if (channel == SENSOR_CHAN_ACCEL_XYZ) {
        bmi2_raw2accel_convert(&p_value[0], data->ax, data->acc_range);
        bmi2_raw2accel_convert(&p_value[1], data->ay, data->acc_range);
        bmi2_raw2accel_convert(&p_value[2], data->az, data->acc_range);
    } else if (channel == SENSOR_CHAN_GYRO_X) {
        bmi2_raw2gyro_convert(p_value, data->gx, data->gyr_range);
    } else if (channel == SENSOR_CHAN_GYRO_Y) {
        bmi2_raw2gyro_convert(p_value, data->gy, data->gyr_range);
    } else if (channel == SENSOR_CHAN_GYRO_Z) {
        bmi2_raw2gyro_convert(p_value, data->gz, data->gyr_range);
    } else if (channel == SENSOR_CHAN_GYRO_XYZ) {
        bmi2_raw2gyro_convert(&p_value[0], data->gx, data->gyr_range);
        bmi2_raw2gyro_convert(&p_value[1], data->gy, data->gyr_range);
        bmi2_raw2gyro_convert(&p_value[2], data->gz, data->gyr_range);
    } else if (channel == SENSOR_CHAN_AMBIENT_TEMP) {
        float temperature = (((float)((int16_t)data->temp)) / 512.0) + 23.0;
        sensor_value_from_float(p_value, temperature);
    } else if (channel == SENSOR_CHAN_STEPS) {
        struct bmi2_feat_sensor_data sensor_data;

        sensor_data.type = BMI2_STEP_COUNTER;
        if (bmi270_get_feature_data(&sensor_data, 1, &data->bmi2) != BMI2_OK) {
            return -EFAULT;
        }

        p_value->val1 = sensor_data.sens_data.step_counter_output;
    } else if (channel == SENSOR_CHAN_ACTIVITY) {
        struct bmi2_feat_sensor_data sensor_data;

        sensor_data.type = BMI2_STEP_ACTIVITY;
        if (bmi270_get_feature_data(&sensor_data, 1, &data->bmi2) != BMI2_OK) {
            return -EFAULT;
        }

        p_value->val1 = sensor_data.sens_data.activity_output;
    } else if (channel == SENSOR_CHAN_GESTURE) {
        struct bmi2_feat_sensor_data sensor_data;

        sensor_data.type = BMI2_WRIST_GESTURE;
        if (bmi270_get_feature_data(&sensor_data, 1, &data->bmi2) != BMI2_OK) {
            return -EFAULT;
        }

        p_value->val1 = sensor_data.sens_data.wrist_gesture_output;
    } else {
        return -ENOTSUP;
    }

    return 0;
}

static const struct sensor_driver_api bmi270_driver_api = {
    .attr_set = bmi270_attr_set,
    .sample_fetch = bmi270_sample_fetch,
    .channel_get = bmi270_channel_get,
#ifdef CONFIG_BMI270_PLUS_TRIGGER
    .trigger_set = bmi270_trigger_set,
#endif
};

/** @brief
 *  @param p_dev
 *  @return         0 when successful
*/
static int bmi270_sensor_init(const struct device *p_dev)
{
    const struct bmi270_config *config = p_dev->config;
    struct bmi270_data *data = p_dev->data;

    LOG_DBG("Initialize BMI270...");

    if (!device_is_ready(config->i2c.bus)) {
        LOG_ERR("I2C bus device not ready!");
        return -ENODEV;
    }

    device = p_dev;
    data->bmi2.intf = BMI2_I2C_INTF;
    data->bmi2.read = bmi2_i2c_read;
    data->bmi2.write = bmi2_i2c_write;
    data->bmi2.delay_us = bmi2_delay_us;
    data->bmi2.read_write_len = BMI2_READ_WRITE_LEN;
    data->bmi2.config_file_ptr = NULL;

    LOG_DBG("Invert x: %u", config->invert_x);
    LOG_DBG("Invert y: %u", config->invert_y);
    LOG_DBG("Swap x and y: %u", config->swap_xy);

    if (bmi270_init(&data->bmi2) != BMI2_OK) {
        LOG_ERR("Can not initialize BMI270!");
        return -EFAULT;
    }

    if (bmi2_soft_reset(&data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    // Initialize with reset values from the datasheet.
    data->acc_odr = BOSCH_BMI270_ACC_ODR_100_HZ;
    data->acc_range = 8;
    data->gyr_odr = BOSCH_BMI270_GYR_ODR_200_HZ;
    data->gyr_range = 2000;

    if (bmi2_configure_enable_all(p_dev, data) != BMI2_OK) {
        return -EFAULT;
    }

#ifdef CONFIG_BMI270_PLUS_TRIGGER
    if (config->int_gpio.port) {
        if (bmi2_init_interrupt(p_dev)) {
            LOG_ERR("Could not initialize interrupts!");
            return -EFAULT;
        }
    }
#endif

    if (bmi2_configure_axis_remapping(p_dev) != BMI2_OK) {
        return -EFAULT;
    }

    LOG_DBG("Initialization successful");

    return 0;
}

#ifdef CONFIG_PM_DEVICE
/** @brief
 *  @param p_dev
 *  @param action
 *  @return         0 when successful
*/
static int bmi270_pm_action(const struct device *p_dev, enum pm_device_action action)
{
    int8_t rslt = 0;

    switch (action) {
        case PM_DEVICE_ACTION_TURN_ON:
        case PM_DEVICE_ACTION_RESUME: {
            break;
        }
        case PM_DEVICE_ACTION_SUSPEND: {
            break;
        }
        case PM_DEVICE_ACTION_TURN_OFF: {
            break;
        }
        default: {
            return -ENOTSUP;
        }
    }

    if (rslt != BMI2_OK) {
        return -EFAULT;
    }

    return 0;
}
#endif

#define BMI270_INIT(inst)                                                           \
    static struct bmi270_data bmi270_data_##inst;                                   \
                                                                                    \
    static const struct bmi270_config bmi270_config_##inst = {                      \
        .i2c = I2C_DT_SPEC_INST_GET(inst),                                          \
        .swap_xy = DT_INST_PROP(inst, swap_xy),                                     \
        .invert_x = DT_INST_PROP(inst, invert_x),                                   \
        .invert_y = DT_INST_PROP(inst, invert_y),                                   \
        .rotation = DT_INST_PROP(inst, rotation),                       \
        IF_ENABLED(CONFIG_BMI270_PLUS_TRIGGER,                                      \
            (.int_gpio = GPIO_DT_SPEC_INST_GET_OR(inst, int_gpios, { 0 }),))        \
    };                                                                              \
                                                                                    \
    PM_DEVICE_DT_INST_DEFINE(inst, bmi270_pm_action);                               \
                                                                                    \
    SENSOR_DEVICE_DT_INST_DEFINE(inst, bmi270_sensor_init,                          \
                  PM_DEVICE_DT_INST_GET(inst),                                      \
                  &bmi270_data_##inst,                                              \
                  &bmi270_config_##inst, POST_KERNEL,                               \
                  CONFIG_SENSOR_INIT_PRIORITY,                                      \
                  &bmi270_driver_api);

DT_INST_FOREACH_STATUS_OKAY(BMI270_INIT)