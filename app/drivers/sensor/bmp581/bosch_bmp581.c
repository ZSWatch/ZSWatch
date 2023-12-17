/* bosch_bmp581.c - Driver for Bosch BMP581 pressure sensor. */

/*
 * Copyright (c) 2023, Daniel Kampert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

#include "bmp5.h"
#include "bosch_bmp581.h"

#define DT_DRV_COMPAT                   bosch_bmp581

LOG_MODULE_REGISTER(bosch_bmp581, CONFIG_BOSCH_BMP581_LOG_LEVEL);

#if(DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 0)
#warning "bmp581 driver enabled without any devices"
#endif

struct bmp581_config {
    struct i2c_dt_spec i2c;
};

static struct bmp5_osr_odr_press_config bmp5_osr_odr_press_cfg;
static const struct device *device;
static struct bmp5_dev bmp5_dev;

/** @brief              Platform specific i2c read function.
 *  @param reg_addr     Register address
 *  @param p_reg_data   Register data
 *  @param len          Data length
 *  @param p_intf       Pointer to interface
 *  @return             0 when successful
*/
static BMP5_INTF_RET_TYPE bmp5_i2c_read(uint8_t reg_addr, uint8_t *p_reg_data, uint32_t len, void *p_intf)
{
    (void)p_intf;

    const struct bmp581_config *config = device->config;
    return i2c_burst_read_dt(&config->i2c, reg_addr, p_reg_data, len);
}

/** @brief              Platform specific i2c write function.
 *  @param reg_addr     Register address
 *  @param p_reg_data   Register data
 *  @param len          Data length
 *  @param p_intf       Pointer to interface
 *  @return             0 when successful
*/
static BMP5_INTF_RET_TYPE bmp5_i2c_write(uint8_t reg_addr, const uint8_t *p_reg_data, uint32_t len, void *p_intf)
{
    (void)p_intf;

    const struct bmp581_config *config = device->config;
    return i2c_burst_write_dt(&config->i2c, reg_addr, p_reg_data, len);
}

/** @brief          Platform specific us delay function.
 *  @param period   Sleep period
 *  @param p_intf   Pointer to interface
*/
static void bmp5_delay_us(uint32_t period, void *p_intf)
{
    (void)p_intf;

    k_usleep(period);
}

/** @brief          
 *  @param p_cfg    
 *  @param p_dev    
 *  @return         0 when successful
*/
static int8_t bmp5_set_config(struct bmp5_osr_odr_press_config *p_cfg, struct bmp5_dev *p_dev)
{
    int8_t rslt;
    struct bmp5_iir_config set_iir_cfg;

    rslt = bmp5_set_power_mode(BMP5_POWERMODE_STANDBY, p_dev);
    if (rslt == BMP5_OK) {
        rslt = bmp5_get_osr_odr_press_config(p_cfg, p_dev);

        if (rslt == BMP5_OK) {
            p_cfg->odr = BOSCH_BMP581_ODR_DEFAULT;
            p_cfg->press_en = BMP5_ENABLE;
            p_cfg->osr_t = BMP5_OVERSAMPLING_64X;
            p_cfg->osr_p = BMP5_OVERSAMPLING_4X;

            rslt = bmp5_set_osr_odr_press_config(p_cfg, p_dev);
        }

        if (rslt == BMP5_OK) {
            set_iir_cfg.set_iir_t = BMP5_IIR_FILTER_COEFF_63;
            set_iir_cfg.set_iir_p = BMP5_IIR_FILTER_COEFF_63;
            set_iir_cfg.shdw_set_iir_t = BMP5_ENABLE;
            set_iir_cfg.shdw_set_iir_p = BMP5_ENABLE;

            rslt = bmp5_set_iir_config(&set_iir_cfg, p_dev);
        }

        rslt = bmp5_set_power_mode(BMP5_POWERMODE_NORMAL, p_dev);
    }

    return rslt;
}

/** @brief
 *  @param p_dev        
 *  @param channel      
 *  @param attribute    
 *  @param p_value      
 *  @return             0 when successful
*/
static int bmp581_attr_set(const struct device *p_dev, enum sensor_channel channel, enum sensor_attribute attribute,
                             const struct sensor_value *p_value)
{
    __ASSERT_NO_MSG(p_value != NULL);

    if (((channel != SENSOR_CHAN_ALL) && (channel != SENSOR_CHAN_AMBIENT_TEMP) && (channel != SENSOR_CHAN_PRESS)) ||
        ((attribute != SENSOR_ATTR_SAMPLING_FREQUENCY) && (attribute == SENSOR_ATTR_OVERSAMPLING))) {
        return -ENOTSUP;
    }

    if (bmp5_get_osr_odr_press_config(&bmp5_osr_odr_press_cfg, &bmp5_dev) != BMP5_OK) {
        return -EFAULT;
    }

    if (attribute == SENSOR_ATTR_OVERSAMPLING) {
        if (p_value->val1 > BMP5_OVERSAMPLING_128X) {
            return -ENOTSUP;
        }

        if (channel == SENSOR_CHAN_ALL) {
            bmp5_osr_odr_press_cfg.osr_t = p_value->val1;
            bmp5_osr_odr_press_cfg.osr_p = p_value->val1;
        }
        else if (channel == SENSOR_CHAN_AMBIENT_TEMP) {
            bmp5_osr_odr_press_cfg.osr_t = p_value->val1;
        }
        else if (channel == SENSOR_CHAN_PRESS) {
            bmp5_osr_odr_press_cfg.osr_p = p_value->val1;
        }
    }
    else if (attribute == SENSOR_ATTR_SAMPLING_FREQUENCY) {
        if (p_value->val1 > BMP5_ODR_0_125_HZ) {
            return -ENOTSUP;
        }

        bmp5_osr_odr_press_cfg.odr = p_value->val1;
    }

    bmp5_osr_odr_press_cfg.press_en = BMP5_ENABLE;

    if (bmp5_set_osr_odr_press_config(&bmp5_osr_odr_press_cfg, &bmp5_dev) != BMP5_OK) {
        LOG_ERR("Failed to set sensor attribute!");
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
static int bmp581_attr_get(const struct device *p_dev, enum sensor_channel channel, enum sensor_attribute attribute,
                             struct sensor_value *p_value)
{
    __ASSERT_NO_MSG(p_value != NULL);

    if (((channel != SENSOR_CHAN_ALL) && (channel != SENSOR_CHAN_AMBIENT_TEMP) && (channel != SENSOR_CHAN_PRESS)) ||
        ((attribute != SENSOR_ATTR_SAMPLING_FREQUENCY) && (attribute == SENSOR_ATTR_OVERSAMPLING))) {
        return -ENOTSUP;
    }

    if (attribute == SENSOR_ATTR_SAMPLING_FREQUENCY) {
    } else {
        return -ENOTSUP;
    }

    if (bmp5_get_osr_odr_press_config(&bmp5_osr_odr_press_cfg, &bmp5_dev) != BMP5_OK) {
        LOG_ERR("Failed to read sensor attribute!");
        return -EFAULT;
    }

    if (attribute == SENSOR_ATTR_OVERSAMPLING) {
        if (channel == SENSOR_CHAN_ALL) {;
            p_value->val1 = bmp5_osr_odr_press_cfg.osr_t;
            p_value->val2 = bmp5_osr_odr_press_cfg.osr_p;
        }
        else if (channel == SENSOR_CHAN_AMBIENT_TEMP) {
            p_value->val1 = bmp5_osr_odr_press_cfg.osr_t;
            p_value->val2 = 0;
        }
        else if (channel == SENSOR_CHAN_PRESS) {
            p_value->val1 = bmp5_osr_odr_press_cfg.osr_p;
            p_value->val2 = 0;
        }
    }
    else if (attribute == SENSOR_ATTR_SAMPLING_FREQUENCY) {
        p_value->val1 = bmp5_osr_odr_press_cfg.odr;
        p_value->val2 = 0;
    }

    return 0;
}

/** @brief          
 *  @param p_dev    
 *  @param channel  
 *  @return         0 when successful
*/
static int bmp581_sample_fetch(const struct device *p_dev, enum sensor_channel channel)
{
    enum pm_device_state pm_state;
    struct bmp5_sensor_data *data = p_dev->data;

    pm_device_state_get(p_dev, &pm_state);
    if (pm_state != PM_DEVICE_STATE_ACTIVE) {
        return -EFAULT;
    }

    if ((channel != SENSOR_CHAN_ALL) && (channel != SENSOR_CHAN_AMBIENT_TEMP) && (channel != SENSOR_CHAN_PRESS)) {
        return -ENOTSUP;
    }

    LOG_DBG("Start a new measurement...");

    if (bmp5_get_sensor_data(data, &bmp5_osr_odr_press_cfg, &bmp5_dev) != BMP5_OK) {
        LOG_ERR("Measurement error!");
    }

    return 0;
}

/** @brief          
 *  @param p_dev    
 *  @param channel  
 *  @param p_value  
 *  @return         0 when successful
*/
static int bmp581_channel_get(const struct device *p_dev, enum sensor_channel channel, struct sensor_value *p_value)
{
	const struct bmp5_sensor_data *data = p_dev->data;

    __ASSERT_NO_MSG(p_value != NULL);

    if (channel == SENSOR_CHAN_AMBIENT_TEMP) {
        sensor_value_from_float(p_value, data->temperature);
    }
    else if (channel == SENSOR_CHAN_PRESS) {
        sensor_value_from_float(p_value, data->pressure);
    }
    else {
        return -ENOTSUP;
    }

    return 0;
}

static const struct sensor_driver_api bmp581_driver_api = {
    .attr_set = bmp581_attr_set,
    .attr_get = bmp581_attr_get,
    .sample_fetch = bmp581_sample_fetch,
    .channel_get = bmp581_channel_get,
};

/** @brief          
 *  @param p_dev    
 *  @return         0 when successful
*/
static int bmp581_init(const struct device *p_dev)
{
    int8_t rslt;
    const struct bmp581_config *config = p_dev->config;

    LOG_DBG("Start to initialize BMP581...");

    if (!device_is_ready(config->i2c.bus)) {
        LOG_ERR("I2C bus device not ready!");
        return -ENODEV;
    }

    device = p_dev;
    bmp5_dev.read = bmp5_i2c_read;
    bmp5_dev.write = bmp5_i2c_write;
    bmp5_dev.intf = BMP5_I2C_INTF;
    bmp5_dev.delay_us = bmp5_delay_us;

    rslt = bmp5_init(&bmp5_dev);
    if (rslt == BMP5_E_POWER_UP) {
        // Fails when expecting BMP5_INT_ASSERTED_POR_SOFTRESET_COMPLETE
        // being set, but it's not. Can't see any actual reset being made.
        // For now ignore, works anyway.
        // Do a soft reset just in case.
        rslt = bmp5_soft_reset(&bmp5_dev);
    }

    if (rslt == BMP5_OK) {
        if (bmp5_set_config(&bmp5_osr_odr_press_cfg, &bmp5_dev) != BMP5_OK) {
            return -EFAULT;
        }
    }
    else {
        LOG_ERR("Can not initialize BMP581!");
        return -ENODEV;
    }

    return 0;
}

#ifdef CONFIG_PM_DEVICE
static int bmp581_pm_action(const struct device *p_dev, enum pm_device_action action)
{
    int8_t rslt;

    switch (action) {
        case PM_DEVICE_ACTION_TURN_ON:
        case PM_DEVICE_ACTION_RESUME: {
                rslt = bmp5_set_power_mode(BMP5_POWERMODE_NORMAL, &bmp5_dev);
            break;
        }
        case PM_DEVICE_ACTION_SUSPEND: {
                rslt = bmp5_set_power_mode(BMP5_POWERMODE_STANDBY, &bmp5_dev);
            break;
        }
        case PM_DEVICE_ACTION_TURN_OFF: {
                rslt = bmp5_set_power_mode(BMP5_POWERMODE_DEEP_STANDBY, &bmp5_dev);
            break;
        }
        default: {
            return -ENOTSUP;
        }
    }

    if (rslt != BMP5_OK) {
        return -EFAULT;
    }

    return 0;
}
#endif

#define BMP581_INIT(inst)                                               \
    static struct bmp5_sensor_data bmp5_sensor_data_##inst;             \
                                                                        \
    static const struct bmp581_config bmp581_config_##inst = {          \
        .i2c = I2C_DT_SPEC_INST_GET(inst),                              \
    };                                                                  \
                                                                        \
    PM_DEVICE_DT_INST_DEFINE(inst, bmp581_pm_action);                   \
                                                                        \
    SENSOR_DEVICE_DT_INST_DEFINE(inst, bmp581_init,                     \
                  PM_DEVICE_DT_INST_GET(inst),                          \
                  &bmp5_sensor_data_##inst,                             \
                  &bmp581_config_##inst, POST_KERNEL,                   \
                  CONFIG_SENSOR_INIT_PRIORITY,                          \
                  &bmp581_driver_api);

DT_INST_FOREACH_STATUS_OKAY(BMP581_INIT)