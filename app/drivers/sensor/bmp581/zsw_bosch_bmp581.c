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

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include "bmp5.h"
#include "zsw_bosch_bmp581.h"

#define DT_DRV_COMPAT                   zswatch_bmp581

LOG_MODULE_REGISTER(zsw_bosch_bmp581, CONFIG_ZSW_BOSCH_BMP581_LOG_LEVEL);

#if(DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) == 0)
#warning "bmp581 driver enabled without any devices"
#endif

struct bmp581_config {
    struct i2c_dt_spec i2c;
};

static struct bmp5_osr_odr_press_config bmp5_osr_odr_press_cfg;
static const struct device *device;
static struct bmp5_dev bmp5_dev;
static K_MUTEX_DEFINE(bmp581_lock);
static bosch_bmp581_configuration_t applied_configuration;
static bosch_bmp581_sample_t cached_sample;
static bool configuration_valid;
static bool configuration_saved;
static bool sample_valid;
static bool sensor_active = true;
static uint32_t configuration_generation;

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

static bool bmp581_config_equal(const bosch_bmp581_configuration_t *first,
                               const bosch_bmp581_configuration_t *second)
{
    return first->mode == second->mode && first->odr == second->odr &&
           first->osr_p == second->osr_p && first->osr_t == second->osr_t &&
           first->iir_p == second->iir_p && first->iir_t == second->iir_t &&
           first->fifo_enabled == second->fifo_enabled;
}

static int bmp581_get_config_locked(bosch_bmp581_configuration_t *configuration)
{
    struct bmp5_osr_odr_press_config oversampling = {0};
    struct bmp5_iir_config filtering = {0};
    struct bmp5_fifo fifo = {0};
    enum bmp5_powermode mode;
    uint8_t interrupt_config[2];
    uint8_t odr_config;
    uint8_t dsp_config;

    if (bmp5_get_osr_odr_press_config(&oversampling, &bmp5_dev) != BMP5_OK ||
        bmp5_get_iir_config(&filtering, &bmp5_dev) != BMP5_OK ||
        bmp5_get_fifo_configuration(&fifo, &bmp5_dev) != BMP5_OK ||
        bmp5_get_power_mode(&mode, &bmp5_dev) != BMP5_OK ||
        bmp5_get_regs(BMP5_REG_INT_CONFIG, interrupt_config, sizeof(interrupt_config), &bmp5_dev) != BMP5_OK ||
        bmp5_get_regs(BMP5_REG_ODR_CONFIG, &odr_config, 1, &bmp5_dev) != BMP5_OK ||
        bmp5_get_regs(BMP5_REG_DSP_CONFIG, &dsp_config, 1, &bmp5_dev) != BMP5_OK) {
        return -EIO;
    }

    if (oversampling.press_en != BMP5_ENABLE || filtering.shdw_set_iir_p != BMP5_ENABLE ||
        filtering.shdw_set_iir_t != BMP5_ENABLE || filtering.iir_flush_forced_en != BMP5_DISABLE ||
        (interrupt_config[0] & BMP5_INT_EN_MSK) != 0 ||
        (interrupt_config[1] & BMP5_INT_ASSERTED_DRDY) == 0 ||
        (odr_config & BMP5_DEEP_DISABLE_MSK) == 0 || (dsp_config & 0x03) != 0x03 ||
        (fifo.frame_sel != BMP5_FIFO_NOT_ENABLED && fifo.frame_sel != BMP5_FIFO_PRESS_TEMP_DATA)) {
        return -EIO;
    }

    *configuration = (bosch_bmp581_configuration_t) {
        .mode = mode,
        .odr = oversampling.odr,
        .osr_p = oversampling.osr_p,
        .osr_t = oversampling.osr_t,
        .iir_p = filtering.set_iir_p,
        .iir_t = filtering.set_iir_t,
        .fifo_enabled = fifo.frame_sel != BMP5_FIFO_NOT_ENABLED,
    };
    return 0;
}

static int bmp581_apply_config_locked(const bosch_bmp581_configuration_t *configuration)
{
    struct bmp5_osr_odr_press_config oversampling = {
        .odr = configuration->odr,
        .osr_p = configuration->osr_p,
        .osr_t = configuration->osr_t,
        .press_en = BMP5_ENABLE,
    };
    struct bmp5_iir_config filtering = {
        .set_iir_p = configuration->iir_p,
        .set_iir_t = configuration->iir_t,
        .shdw_set_iir_p = BMP5_ENABLE,
        .shdw_set_iir_t = BMP5_ENABLE,
        .iir_flush_forced_en = BMP5_DISABLE,
    };
    struct bmp5_fifo fifo = {
        .frame_sel = configuration->fifo_enabled ? BMP5_FIFO_PRESS_TEMP_DATA : BMP5_FIFO_NOT_ENABLED,
    };
    struct bmp5_int_source_select interrupt_source = {.drdy_en = BMP5_ENABLE};
    bosch_bmp581_configuration_t readback;
    bosch_bmp581_configuration_t standby_configuration = *configuration;
    enum bmp5_powermode mode;
    uint8_t interrupt_config;
    uint8_t dsp_config;
    uint8_t interrupt_status;
    int ret = -EIO;

    if (configuration->mode > BOSCH_BMP581_MODE_NORMAL || configuration->odr > BMP5_ODR_0_125_HZ ||
        configuration->osr_p > BMP5_OVERSAMPLING_128X || configuration->osr_t > BMP5_OVERSAMPLING_128X ||
        configuration->iir_p > BOSCH_BMP581_IIR_127 || configuration->iir_t > BOSCH_BMP581_IIR_127) {
        return -EINVAL;
    }

    configuration_valid = false;
    sample_valid = false;
    configuration_generation++;

    if (bmp5_set_power_mode(BMP5_POWERMODE_STANDBY, &bmp5_dev) != BMP5_OK ||
        bmp5_get_power_mode(&mode, &bmp5_dev) != BMP5_OK || mode != BMP5_POWERMODE_STANDBY ||
        bmp5_set_osr_odr_press_config(&oversampling, &bmp5_dev) != BMP5_OK ||
        bmp5_set_iir_config(&filtering, &bmp5_dev) != BMP5_OK ||
        bmp5_set_fifo_configuration(&fifo, &bmp5_dev) != BMP5_OK ||
        bmp5_get_regs(BMP5_REG_DSP_CONFIG, &dsp_config, 1, &bmp5_dev) != BMP5_OK) {
        goto failed;
    }

    dsp_config |= 0x03;
    if (bmp5_set_regs(BMP5_REG_DSP_CONFIG, &dsp_config, 1, &bmp5_dev) != BMP5_OK ||
        bmp5_get_regs(BMP5_REG_INT_CONFIG, &interrupt_config, 1, &bmp5_dev) != BMP5_OK) {
        goto failed;
    }
    interrupt_config &= ~BMP5_INT_EN_MSK;
    if (bmp5_set_regs(BMP5_REG_INT_CONFIG, &interrupt_config, 1, &bmp5_dev) != BMP5_OK ||
        bmp5_int_source_select(&interrupt_source, &bmp5_dev) != BMP5_OK) {
        goto failed;
    }

    standby_configuration.mode = BOSCH_BMP581_MODE_STANDBY;
    ret = bmp581_get_config_locked(&readback);
    if (ret != 0 || !bmp581_config_equal(&readback, &standby_configuration)) {
        ret = -EIO;
        goto failed;
    }

    ret = -EIO;
    if (bmp5_get_interrupt_status(&interrupt_status, &bmp5_dev) != BMP5_OK ||
        bmp5_set_power_mode(configuration->mode, &bmp5_dev) != BMP5_OK) {
        goto failed;
    }
    ret = bmp581_get_config_locked(&readback);
    if (ret != 0 || !bmp581_config_equal(&readback, configuration)) {
        ret = -EIO;
        goto failed;
    }

    bmp5_osr_odr_press_cfg = oversampling;
    applied_configuration = *configuration;
    configuration_saved = true;
    configuration_valid = true;
    return 0;

failed:
    bmp5_set_power_mode(BMP5_POWERMODE_STANDBY, &bmp5_dev);
    return ret;
}

int zsw_bmp581_apply_config(const struct device *dev, const bosch_bmp581_configuration_t *configuration,
                           uint32_t *generation)
{
    if (dev == NULL || dev != device || configuration == NULL || generation == NULL) {
        return -EINVAL;
    }

    k_mutex_lock(&bmp581_lock, K_FOREVER);
    int ret = sensor_active ? bmp581_apply_config_locked(configuration) : -EBUSY;
    if (ret == 0) {
        *generation = configuration_generation;
    }
    k_mutex_unlock(&bmp581_lock);
    return ret;
}

int zsw_bmp581_get_config(const struct device *dev, bosch_bmp581_configuration_t *configuration)
{
    if (dev == NULL || dev != device || configuration == NULL) {
        return -EINVAL;
    }

    k_mutex_lock(&bmp581_lock, K_FOREVER);
    int ret = sensor_active ? bmp581_get_config_locked(configuration) : -EBUSY;
    k_mutex_unlock(&bmp581_lock);
    return ret;
}

static int bmp581_fetch_locked(const struct device *dev)
{
    struct bmp5_sensor_data measurement = {0};
    struct bmp5_osr_odr_eff effective = {0};
    uint8_t interrupt_status;

    if (!sensor_active) {
        return -EBUSY;
    }
    if (!configuration_valid) {
        return -EIO;
    }
    if (applied_configuration.mode != BOSCH_BMP581_MODE_NORMAL) {
        return -EAGAIN;
    }
    if (bmp5_get_interrupt_status(&interrupt_status, &bmp5_dev) != BMP5_OK) {
        goto failed;
    }
    if (interrupt_status & BMP5_INT_ASSERTED_POR_SOFTRESET_COMPLETE) {
        goto failed;
    }
    if ((interrupt_status & BMP5_INT_ASSERTED_DRDY) == 0) {
        return sample_valid ? 0 : -EAGAIN;
    }
    if (bmp5_get_osr_odr_eff(&effective, &bmp5_dev) != BMP5_OK || !effective.odr_is_valid ||
        effective.osr_p_eff != applied_configuration.osr_p ||
        effective.osr_t_eff != applied_configuration.osr_t ||
        bmp5_get_sensor_data(&measurement, &bmp5_osr_odr_press_cfg, &bmp5_dev) != BMP5_OK) {
        goto failed;
    }

    *(struct bmp5_sensor_data *)dev->data = measurement;
    cached_sample.pressure = measurement.pressure;
    cached_sample.temperature = measurement.temperature;
    cached_sample.generation = configuration_generation;
    cached_sample.sequence++;
    sample_valid = true;
    return 0;

failed:
    configuration_valid = false;
    sample_valid = false;
    return -EIO;
}

int zsw_bmp581_read_sample(const struct device *dev, bosch_bmp581_sample_t *sample)
{
    if (dev == NULL || dev != device || sample == NULL) {
        return -EINVAL;
    }

    k_mutex_lock(&bmp581_lock, K_FOREVER);
    int ret = bmp581_fetch_locked(dev);
    if (ret == 0) {
        *sample = cached_sample;
    }
    k_mutex_unlock(&bmp581_lock);
    return ret;
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
        ((attribute != SENSOR_ATTR_SAMPLING_FREQUENCY) && (attribute != SENSOR_ATTR_OVERSAMPLING))) {
        return -ENOTSUP;
    }

    if (p_value->val1 < 0 || p_value->val1 >
        (attribute == SENSOR_ATTR_OVERSAMPLING ? BMP5_OVERSAMPLING_128X : BMP5_ODR_0_125_HZ)) {
        return -EINVAL;
    }

    k_mutex_lock(&bmp581_lock, K_FOREVER);
    bosch_bmp581_configuration_t configuration;
    int ret = sensor_active ? bmp581_get_config_locked(&configuration) : -EBUSY;
    if (ret != 0) {
        goto out;
    }
    if (attribute == SENSOR_ATTR_OVERSAMPLING) {
        if (channel == SENSOR_CHAN_ALL) {
            configuration.osr_t = p_value->val1;
            configuration.osr_p = p_value->val1;
        }
        else if (channel == SENSOR_CHAN_AMBIENT_TEMP) {
            configuration.osr_t = p_value->val1;
        }
        else if (channel == SENSOR_CHAN_PRESS) {
            configuration.osr_p = p_value->val1;
        }
    }
    else {
        configuration.odr = p_value->val1;
    }
    if (!configuration_valid || !bmp581_config_equal(&configuration, &applied_configuration)) {
        ret = bmp581_apply_config_locked(&configuration);
    }
out:
    k_mutex_unlock(&bmp581_lock);
    return ret;
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
        ((attribute != SENSOR_ATTR_SAMPLING_FREQUENCY) && (attribute != SENSOR_ATTR_OVERSAMPLING))) {
        return -ENOTSUP;
    }

    bosch_bmp581_configuration_t configuration;
    int ret = zsw_bmp581_get_config(p_dev, &configuration);
    if (ret != 0) {
        return ret;
    }

    p_value->val2 = 0;
    if (attribute == SENSOR_ATTR_OVERSAMPLING) {
        if (channel == SENSOR_CHAN_ALL) {
            p_value->val1 = configuration.osr_t;
            p_value->val2 = configuration.osr_p;
        }
        else if (channel == SENSOR_CHAN_AMBIENT_TEMP) {
            p_value->val1 = configuration.osr_t;
            p_value->val2 = 0;
        }
        else if (channel == SENSOR_CHAN_PRESS) {
            p_value->val1 = configuration.osr_p;
            p_value->val2 = 0;
        }
    }
    else if (attribute == SENSOR_ATTR_SAMPLING_FREQUENCY) {
        p_value->val1 = configuration.odr;
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
    if ((channel != SENSOR_CHAN_ALL) && (channel != SENSOR_CHAN_AMBIENT_TEMP) && (channel != SENSOR_CHAN_PRESS)) {
        return -ENOTSUP;
    }

    k_mutex_lock(&bmp581_lock, K_FOREVER);
    int ret = bmp581_fetch_locked(p_dev);
    k_mutex_unlock(&bmp581_lock);
    return ret;
}

/** @brief
 *  @param p_dev
 *  @param channel
 *  @param p_value
 *  @return         0 when successful
*/
static int bmp581_channel_get(const struct device *p_dev, enum sensor_channel channel, struct sensor_value *p_value)
{
    __ASSERT_NO_MSG(p_value != NULL);

    k_mutex_lock(&bmp581_lock, K_FOREVER);
    int ret;
    if (!sample_valid || !configuration_valid || !sensor_active) {
        ret = -ENODATA;
    }
    else if (channel == SENSOR_CHAN_AMBIENT_TEMP) {
        ret = sensor_value_from_float(p_value, cached_sample.temperature);
    }
    else if (channel == SENSOR_CHAN_PRESS) {
        ret = sensor_value_from_float(p_value, cached_sample.pressure);
    }
    else {
        ret = -ENOTSUP;
    }
    k_mutex_unlock(&bmp581_lock);
    return ret;
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
        LOG_DBG("Reset successful! Set configuration...");
        const bosch_bmp581_configuration_t configuration = {
            .mode = BOSCH_BMP581_MODE_NORMAL,
            .odr = BOSCH_BMP581_ODR_DEFAULT,
            .osr_p = BOSCH_BMP581_OSR_4X,
            .osr_t = BOSCH_BMP581_OSR_64X,
            .iir_p = BOSCH_BMP581_IIR_63,
            .iir_t = BOSCH_BMP581_IIR_63,
        };
        uint32_t generation;
        return zsw_bmp581_apply_config(p_dev, &configuration, &generation);
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
    int ret;

    if (action != PM_DEVICE_ACTION_TURN_ON && action != PM_DEVICE_ACTION_RESUME &&
        action != PM_DEVICE_ACTION_SUSPEND && action != PM_DEVICE_ACTION_TURN_OFF) {
        return -ENOTSUP;
    }
    k_mutex_lock(&bmp581_lock, K_FOREVER);
    sample_valid = false;
    configuration_valid = false;

    switch (action) {
        case PM_DEVICE_ACTION_TURN_ON:
        case PM_DEVICE_ACTION_RESUME: {
            sensor_active = true;
            ret = configuration_saved ? bmp581_apply_config_locked(&applied_configuration) : -ENODATA;
            break;
        }
        case PM_DEVICE_ACTION_SUSPEND: {
            ret = bmp5_set_power_mode(BMP5_POWERMODE_STANDBY, &bmp5_dev) == BMP5_OK ? 0 : -EIO;
            if (ret == 0) {
                sensor_active = false;
            }
            break;
        }
        case PM_DEVICE_ACTION_TURN_OFF: {
            ret = bmp5_set_power_mode(BMP5_POWERMODE_DEEP_STANDBY, &bmp5_dev) == BMP5_OK ? 0 : -EIO;
            if (ret == 0) {
                sensor_active = false;
            }
            break;
        }
        default: {
            ret = -ENOTSUP;
            break;
        }
    }
    k_mutex_unlock(&bmp581_lock);
    return ret;
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
