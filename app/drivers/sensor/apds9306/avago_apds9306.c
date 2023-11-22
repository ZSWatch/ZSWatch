/* avago_apds9306.c - Driver for Broadcom / Avago APDS9306 light sensor. */

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

#include "avago_apds9306.h"

#define APDS9306_REGISTER_MAIN_CTRL         0x00
#define APDS9306_REGISTER_ALS_MEAS_RATE     0x04
#define APDS9306_REGISTER_ALS_GAIN          0x05
#define APDS9306_REGISTER_PART_ID           0x06
#define APDS9306_REGISTER_MAIN_STATUS       0x07
#define APDS9306_REGISTER_CLEAR_DATA_0      0x0A
#define APDS9306_REGISTER_CLEAR_DATA_1      0x0B
#define APDS9306_REGISTER_CLEAR_DATA_2      0x0C
#define APDS9306_REGISTER_ALS_DATA_0        0x0D
#define APDS9306_REGISTER_ALS_DATA_1        0x0E
#define APDS9306_REGISTER_ALS_DATA_2        0x0F
#define APDS9306_REGISTER_INT_CFG           0x19
#define APDS9306_REGISTER_INT_PERSISTENCE   0x1A
#define APDS9306_REGISTER_ALS_THRES_UP_0    0x21
#define APDS9306_REGISTER_ALS_THRES_UP_1    0x22
#define APDS9306_REGISTER_ALS_THRES_UP_2    0x23
#define APDS9306_REGISTER_ALS_THRES_LOW_0   0x24
#define APDS9306_REGISTER_ALS_THRES_LOW_1   0x25
#define APDS9306_REGISTER_ALS_THRES_LOW_2   0x26
#define APDS9306_REGISTER_ALS_THRES_VAR     0x27

#define ADPS9306_BIT_ALS_EN                 BIT(0x01)
#define ADPS9306_BIT_ALS_DATA_STATUS        BIT(0x03)
#define APDS9306_BIT_SW_RESET               BIT(0x04)
#define ADPS9306_BIT_ALS_INTERRUPT_STATUS   BIT(0x03)
#define APDS9306_BIT_POWER_ON_STATUS        BIT(0x05)

#ifdef CONFIG_APDS9306_IS_APDS9306_065
#define APDS_9306_CHIP_ID                   0xB3
#else
#define APDS_9306_CHIP_ID                   0xB1
#endif

#define DT_DRV_COMPAT                       avago_apds9306

LOG_MODULE_REGISTER(avago_apds9306, CONFIG_AVAGO_APDS9306_LOG_LEVEL);

#if(DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 0)
#warning "apds9306 driver enabled without any devices"
#endif

struct apds9306_data {
    uint32_t light;
};

struct apds9306_config {
    struct i2c_dt_spec i2c;
    uint8_t resolution;
    uint8_t frequency;
    uint8_t gain;
};

struct apds9306_worker_item_t {
    struct k_work_delayable dwork;
    const struct device *dev;
} apds9306_worker_item;

/** @brief          Get the conversion time for a given sensor resolution.
 *  @param value    Sensor resolution
 *  @return         Conversion time in ms
*/
static uint32_t apds9306_get_time_for_resolution(uint8_t value)
{
    switch (value) {
        case APDS9306_RES_20:
            return 400;
        case APDS9306_RES_19:
            return 200.0;
        case APDS9306_RES_18:
            return 100.0;
        case APDS9306_RES_17:
            return 50.0;
        case APDS9306_RES_16:
            return 25.0;
        case APDS9306_RES_13:
            return 4;
        default:
            return 100.0;
    }
}

/** @brief          Enable the ALS measurement.
 *  @param p_dev    Pointer to sensor device
 *  @return         0 when successful
*/
static int apds9306_enable(const struct device *p_dev)
{
    const struct apds9306_config *config = p_dev->config;

    return i2c_reg_update_byte_dt(&config->i2c, APDS9306_REGISTER_MAIN_CTRL, ADPS9306_BIT_ALS_EN, ADPS9306_BIT_ALS_EN);
}

/** @brief          Disable the ALS measurement.
 *  @param p_dev    Pointer to sensor device
 *  @return         0 when successful
*/
static int apds9306_standby(const struct device *p_dev)
{
    const struct apds9306_config *config = p_dev->config;

    return i2c_reg_update_byte_dt(&config->i2c, APDS9306_REGISTER_MAIN_CTRL, ADPS9306_BIT_ALS_EN, 0x00);
}

/** @brief          Sensor worker handler.
 *  @param p_work   Pointer to worker object
*/
static void apds9306_worker(struct k_work *p_work)
{
    uint8_t buffer[3];
    uint8_t reg;
    struct k_work_delayable *dwork = k_work_delayable_from_work(p_work);
    struct apds9306_worker_item_t *item = CONTAINER_OF(dwork, struct  apds9306_worker_item_t, dwork);
    struct apds9306_data *data = item->dev->data;
    const struct apds9306_config *config = item->dev->config;

    if (i2c_reg_read_byte_dt(&config->i2c, APDS9306_REGISTER_MAIN_STATUS, &buffer[0])) {
        LOG_ERR("Failed to read ALS status!");
        return;
    }

    if(!(buffer[0] & ADPS9306_BIT_ALS_DATA_STATUS)) {
        LOG_DBG("No data ready!");
        return;
    }

    if (apds9306_standby(item->dev) != 0) {
        LOG_ERR("Can not disable ALS!");
        return;
    }

    reg = APDS9306_REGISTER_ALS_DATA_0;
    if (i2c_write_read_dt(&config->i2c, &reg, sizeof(reg), &buffer, sizeof(buffer)) < 0) {
        return;
    }

    data->light = sys_get_le24(buffer);

    LOG_DBG("Last measurement: %u", data->light);
}

/** @brief              
 *  @param p_dev        Pointer to sensor device
 *  @param channel      
 *  @param attribute    
 *  @param p_value      
 *  @return             0 when successful
*/
static int apds9306_attr_set(const struct device *p_dev, enum sensor_channel channel, enum sensor_attribute attribute,
                             const struct sensor_value *p_value)
{
    uint8_t reg;
    uint8_t mask;
    uint8_t value;
    const struct apds9306_config *config = p_dev->config;

    __ASSERT_NO_MSG(p_value != NULL);

    if (channel != SENSOR_CHAN_LIGHT) {
        return -ENOTSUP;
    }

    if (attribute == SENSOR_ATTR_SAMPLING_FREQUENCY) {
        reg = APDS9306_REGISTER_ALS_MEAS_RATE;
        mask = (0x07) << 0x00;
        value = p_value->val1 & 0x07;
    } else if (attribute == SENSOR_APDS9306_ATTR_GAIN) {
        reg = APDS9306_REGISTER_ALS_GAIN;
        mask = (0x07) << 0x00;
        value = p_value->val1 & 0x07;
    } else if (attribute == SENSOR_APDS9306_ATTR_RESOLUTION) {
        reg = APDS9306_REGISTER_ALS_MEAS_RATE;
        mask = (0x07) << 0x04;
        value = (p_value->val1 & 0x07) << 0x04;
    } else {
        return -ENOTSUP;
    }

    if (i2c_reg_update_byte_dt(&config->i2c, reg, mask, value)) {
        LOG_ERR("Failed to set sensor attribute!");
        return -EFAULT;
    }

    return 0;
}

/** @brief              
 *  @param p_dev        Pointer to sensor device
 *  @param channel      
 *  @param attribute    
 *  @param p_value      
 *  @return             0 when successful
*/
static int apds9306_attr_get(const struct device *p_dev, enum sensor_channel channel, enum sensor_attribute attribute,
                             struct sensor_value *p_value)
{
    uint8_t mask;
    uint8_t value;
    uint8_t reg;
    const struct apds9306_config *config = p_dev->config;

    __ASSERT_NO_MSG(p_value != NULL);

    if (channel != SENSOR_CHAN_LIGHT) {
        return -ENOTSUP;
    }

    if (attribute == SENSOR_ATTR_SAMPLING_FREQUENCY) {
        reg = APDS9306_REGISTER_ALS_MEAS_RATE;
        mask = 0x00;
    } else if (attribute == SENSOR_APDS9306_ATTR_GAIN) {
        reg = APDS9306_REGISTER_ALS_GAIN;
        mask = 0x00;
    } else if (attribute == SENSOR_APDS9306_ATTR_RESOLUTION) {
        reg = APDS9306_REGISTER_ALS_MEAS_RATE;
        mask = 0x04;
    } else {
        return -ENOTSUP;
    }

    if (i2c_reg_read_byte_dt(&config->i2c, reg, &value)) {
        LOG_ERR("Failed to read sensor attribute!");
        return -EFAULT;
    }

    p_value->val1 = (value >> mask) & 0x07;
    p_value->val2 = 0;

    return 0;
}

/** @brief          
 *  @param p_dev    Pointer to sensor device
 *  @param channel  
 *  @return         0 when successful
*/
static int apds9306_sample_fetch(const struct device *p_dev, enum sensor_channel channel)
{
    uint8_t buffer;
    uint8_t resolution;
    uint16_t delay;
    enum pm_device_state pm_state;
    const struct apds9306_config *config = p_dev->config;

    pm_device_state_get(p_dev, &pm_state);
    if (pm_state != PM_DEVICE_STATE_ACTIVE) {
        return -EFAULT;
    }

    if ((channel != SENSOR_CHAN_ALL) && (channel != SENSOR_CHAN_LIGHT)) {
        return -ENOTSUP;
    }

    LOG_DBG("Start a new measurement...");
    if (apds9306_enable(p_dev) != 0) {
        LOG_ERR("Can not enable ALS!");
        return -EFAULT;
    }

    // Get the measurement resolution.
    if (i2c_reg_read_byte_dt(&config->i2c, APDS9306_REGISTER_ALS_MEAS_RATE, &buffer)) {
        LOG_ERR("Failed reading resolution");
        return -EFAULT;
    }

    // Convert the resolution into a delay time and wait for the result.
    resolution = (buffer >> 4) & 0x07;
    delay = apds9306_get_time_for_resolution(resolution);
    LOG_DBG("Measurement resolution: %u", resolution);
    LOG_DBG("Wait for %u ms", delay);

    // We add a bit more delay to cover the startup time etc.
    if (!k_work_delayable_is_pending(&apds9306_worker_item.dwork)) {
        LOG_DBG("Schedule new work");

        apds9306_worker_item.dev = p_dev;
        k_work_init_delayable(&apds9306_worker_item.dwork, apds9306_worker);
        k_work_schedule(&apds9306_worker_item.dwork, K_MSEC(delay + 100));
    }
    else {
        LOG_DBG("Work pending. Wait for completion.");
    }

    return 0;
}

/** @brief          
 *  @param p_dev    Pointer to sensor device
 *  @param channel  
 *  @param p_value  
 *  @return         0 when successful
*/
static int apds9306_channel_get(const struct device *p_dev, enum sensor_channel channel, struct sensor_value *p_value)
{
    struct apds9306_data *data = p_dev->data;

    if (channel != SENSOR_CHAN_LIGHT) {
        return -ENOTSUP;
    }

    __ASSERT_NO_MSG(p_value != NULL);

    // TODO: Conversion to lux is missing here

    p_value->val1 = data->light;
    p_value->val2 = 0;

    return 0;
}

/** @brief          
 *  @param p_dev    Pointer to sensor device
 *  @return         0 when successful
*/
static int apds9306_sensor_setup(const struct device *p_dev)
{
    uint32_t now;
    uint8_t temp;
    const struct apds9306_config *config = p_dev->config;

    // Wait for the device to become ready after a power cycle.
    now = k_uptime_get_32();
    do {
        i2c_reg_read_byte_dt(&config->i2c, APDS9306_REGISTER_MAIN_STATUS, &temp);

        // We wait 100 ms maximum for the device to become ready.
        if ((k_uptime_get_32() - now) > 100) {
            LOG_ERR("Sensor timeout!");
            return -EFAULT;
        }

        k_msleep(10);
    } while (temp & APDS9306_BIT_POWER_ON_STATUS);

    if (i2c_reg_read_byte_dt(&config->i2c, APDS9306_REGISTER_PART_ID, &temp)) {
        LOG_ERR("Failed reading chip id!");
        return -EFAULT;
    }

    if (temp != APDS_9306_CHIP_ID) {
        LOG_ERR("Invalid chip id! Found 0x%X, expect 0x%X", temp, APDS_9306_CHIP_ID);
        return -EFAULT;
    }

    // Reset the sensor.
    if (i2c_reg_write_byte_dt(&config->i2c, APDS9306_REGISTER_MAIN_CTRL, APDS9306_BIT_SW_RESET)) {
        LOG_ERR("Can not reset the sensor!");
        return -EFAULT;
    }
    k_msleep(10);

    return 0;
}

static const struct sensor_driver_api apds9306_driver_api = {
    .attr_set = apds9306_attr_set,
    .attr_get = apds9306_attr_get,
    .sample_fetch = apds9306_sample_fetch,
    .channel_get = apds9306_channel_get,
};

/** @brief          
 *  @param p_dev    Pointer to sensor device
 *  @return         0 when successful
*/
static int apds9306_init(const struct device *p_dev)
{
    uint8_t value;
    const struct apds9306_config *config = p_dev->config;

    LOG_DBG("Start to initialize APDS9306...");

    if (!device_is_ready(config->i2c.bus)) {
        LOG_ERR("Bus device is not ready");
        return -EINVAL;
    }

    if (apds9306_sensor_setup(p_dev) < 0) {
        LOG_ERR("Failed to setup device!");
        return -EFAULT;
    }

    value = ((config->resolution & 0x07) << 4) | (config->frequency & 0x0F);
    LOG_DBG("Write configuration 0x%x to register 0x%x", value, APDS9306_REGISTER_ALS_MEAS_RATE);
    if (i2c_reg_write_byte_dt(&config->i2c, APDS9306_REGISTER_ALS_MEAS_RATE, value)) {
        return -EFAULT;
    }

    value = config->gain;
    LOG_DBG("Write configuration 0x%x to register 0x%x", value, APDS9306_REGISTER_ALS_GAIN);
    if (i2c_reg_write_byte_dt(&config->i2c, APDS9306_REGISTER_ALS_GAIN, value)) {
        return -EFAULT;
    }

    LOG_DBG("APDS9306 initialization successful!");

    return 0;
}

#ifdef CONFIG_PM_DEVICE
/** @brief          
 *  @param p_dev    Pointer to sensor device
 *  @param action   action
 *  @return         0 when successful
*/
static int apds9306_pm_action(const struct device *p_dev, enum pm_device_action action)
{
    switch (action) {
        case PM_DEVICE_ACTION_SUSPEND:
        case PM_DEVICE_ACTION_RESUME:
        case PM_DEVICE_ACTION_TURN_OFF:
        case PM_DEVICE_ACTION_TURN_ON: {
            break;
        }
        default: {
            return -ENOTSUP;
        }
    }

    return 0;
}
#endif

#define APDS9306_INIT(inst)                                             \
    static struct apds9306_data apds9306_data_##inst;                   \
                                                                        \
    static const struct apds9306_config apds9306_config_##inst = {      \
        .i2c = I2C_DT_SPEC_INST_GET(inst),                              \
        .resolution = DT_INST_PROP(inst, resolution),					\
        .gain = DT_INST_PROP(inst, gain),					            \
        .frequency = DT_INST_PROP(inst, frequency),					    \
    };                                                                  \
                                                                        \
    PM_DEVICE_DT_INST_DEFINE(inst, apds9306_pm_action);                 \
                                                                        \
    SENSOR_DEVICE_DT_INST_DEFINE(inst, apds9306_init,                   \
                  PM_DEVICE_DT_INST_GET(inst),                          \
                  &apds9306_data_##inst,                                \
                  &apds9306_config_##inst, POST_KERNEL,                 \
                  CONFIG_SENSOR_INIT_PRIORITY,                          \
                  &apds9306_driver_api);

DT_INST_FOREACH_STATUS_OKAY(APDS9306_INIT)