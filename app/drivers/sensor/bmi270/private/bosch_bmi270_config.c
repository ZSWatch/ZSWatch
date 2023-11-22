/* bosch_bmi270_config.c- Driver for Bosch BMI270 IMU. */

/*
 * Copyright (c) 2023, Daniel Kampert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>

#include "bosch_bmi270_config.h"

LOG_MODULE_REGISTER(bmi270_config, CONFIG_BOSCH_BMI270_PLUS_LOG_LEVEL);

typedef void(*feature_config_func)(struct bmi2_sens_config *p_config, struct bmi270_data *p_data);

typedef struct bosch_bmi270_feature_config_set_t {
    uint8_t                 sensor_id;
    feature_config_func     cfg_func;
    bool                    isr_disable;
    bool                    skip_enable;
} bosch_bmi270_feature_config_set_t;

static void bmi2_configure_accel(struct bmi2_sens_config *p_config, struct bmi270_data *p_data);
static void bmi2_configure_step_counter(struct bmi2_sens_config *p_config, struct bmi270_data *p_data);
static void bmi2_configure_gyro(struct bmi2_sens_config *p_config, struct bmi270_data *p_data);
static void bmi2_configure_anymotion(struct bmi2_sens_config *p_config, struct bmi270_data *p_data);
static void bmi2_configure_gesture_detect(struct bmi2_sens_config *p_config, struct bmi270_data *p_data);
static void bmi2_configure_wrist_wakeup(struct bmi2_sens_config *p_config, struct bmi270_data *p_data);
static void bmi2_configure_no_motion(struct bmi2_sens_config *p_config, struct bmi270_data *p_data);

static bosch_bmi270_feature_config_set_t bmi270_enabled_features[] = {
    { .sensor_id = BMI2_ACCEL, .cfg_func = bmi2_configure_accel},
    // Gyro not used for now, disable to keep power consumption down
    { .sensor_id = BMI2_GYRO, .cfg_func = bmi2_configure_gyro, .skip_enable = true},
    { .sensor_id = BMI2_STEP_COUNTER, .cfg_func = bmi2_configure_step_counter},
    { .sensor_id = BMI2_SIG_MOTION, .cfg_func = NULL, .isr_disable = true},
    { .sensor_id = BMI2_ANY_MOTION, .cfg_func = bmi2_configure_anymotion, .isr_disable = true, .skip_enable = true},
    { .sensor_id = BMI2_STEP_ACTIVITY, .cfg_func = NULL, .skip_enable = true},
    { .sensor_id = BMI2_WRIST_GESTURE, .cfg_func = bmi2_configure_gesture_detect},
    { .sensor_id = BMI2_WRIST_WEAR_WAKE_UP, .cfg_func = bmi2_configure_wrist_wakeup},
    { .sensor_id = BMI2_NO_MOTION, .cfg_func = bmi2_configure_no_motion},
};

/** @brief              
 *  @param sensor_id       
 *  @return             true when feature available
*/
static bool bmi2_is_sensor_feature(uint8_t sensor_id)
{
    switch (sensor_id) {
        case BMI2_SIG_MOTION:
        case BMI2_WRIST_GESTURE:
        case BMI2_ANY_MOTION:
        case BMI2_NO_MOTION:
        case BMI2_STEP_COUNTER:
        case BMI2_STEP_DETECTOR:
        case BMI2_STEP_ACTIVITY:
        case BMI2_WRIST_WEAR_WAKE_UP:
            return true;
        default:
            return false;
    }
}

int8_t bmi2_configure_enable_all(const struct device *p_dev, struct bmi270_data *p_data)
{
    uint8_t num_features;
    uint8_t num_enabled_features;
    struct bmi270_data *data = p_dev->data;

    // Structure to define all sensors and their configs
    struct bmi2_sens_config config[ARRAY_SIZE(bmi270_enabled_features)];

    // To enable the sensors the Bosch API expects a list of all features.
    uint8_t all_sensors[ARRAY_SIZE(bmi270_enabled_features)];

    // There is a difference between a "sensor" and a "feature".
    // Accel, Gyro are sensors, but step counter is a feature.
    // We map sensor INT to INT1 pin and feature ISR to INT2 pin.
    // The API needs a list of all those features to do that map.
    struct bmi2_sens_int_config all_features[ARRAY_SIZE(bmi270_enabled_features)];

    num_features = 0;
    num_enabled_features = 0;

    for (uint8_t i = 0; i < ARRAY_SIZE(bmi270_enabled_features); i++) {
        config[i].type = bmi270_enabled_features[i].sensor_id;
        if (!bmi270_enabled_features[i].skip_enable) {
            all_sensors[num_enabled_features] = bmi270_enabled_features[i].sensor_id;
            num_enabled_features++;
        }

        if (bmi2_is_sensor_feature(bmi270_enabled_features[i].sensor_id)) {
            all_features[num_features].type = bmi270_enabled_features[i].sensor_id;
            if (bmi270_enabled_features[i].isr_disable) {
                all_features[num_features].hw_int_pin = BMI2_INT_NONE;
            } else {
                all_features[num_features].hw_int_pin = BMI2_INT2;
            }

            num_features++;
        }
    }

    // Get default configurations for the type of feature selected.
    if (bmi270_get_sensor_config(config, ARRAY_SIZE(bmi270_enabled_features), &data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    for (int i = 0; i < ARRAY_SIZE(bmi270_enabled_features); i++) {
        if (bmi270_enabled_features[i].cfg_func) {
            bmi270_enabled_features[i].cfg_func(&config[i], p_data);
        }
    }

    // Accel and Gyro enable must be done after setting configurations.
    if ((bmi270_sensor_enable(all_sensors, num_enabled_features, &data->bmi2) != BMI2_OK) ||
        (bmi270_set_sensor_config(config, ARRAY_SIZE(bmi270_enabled_features), &data->bmi2) != BMI2_OK) ||
        (bmi270_map_feat_int(all_features, num_features, &data->bmi2) != BMI2_OK)) {
        return -EFAULT;
    }

    return 0;
}

/** @brief          
 *  @param p_config 
 *  @param p_data   
*/
static void bmi2_configure_accel(struct bmi2_sens_config *p_config, struct bmi270_data *p_data)
{
    /* NOTE: The user can change the following configuration parameters according to their requirement. */
    /* Set Output Data Rate */
    p_config->cfg.acc.odr = BMI2_ACC_ODR_100HZ;

    /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
    p_config->cfg.acc.range = BMI2_ACC_RANGE_2G;

    /* The bandwidth parameter is used to bmi2_configure the number of sensor samples that are averaged
        * if it is set to 2, then 2^(bandwidth parameter) samples
        * are averaged, resulting in 4 averaged samples.
        * Note1 : For more information, refer the datasheet.
        * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
        * this has an adverse effect on the power consumed.
        */
    p_config->cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

    /* Enable the filter performance mode where averaging of samples
        * will be done based on above set bandwidth and ODR.
        * There are two modes
        *  0 -> Ultra low power mode
        *  1 -> High performance mode(Default)
        * For more info refer datasheet.
        */
    p_config->cfg.acc.filter_perf = BMI2_POWER_OPT_MODE;

    switch(p_config->cfg.acc.range) {
        case BMI2_ACC_RANGE_2G:
            p_data->acc_range = 2;
            break;
        case BMI2_ACC_RANGE_4G:
            p_data->acc_range = 4;
            break;
        case BMI2_ACC_RANGE_8G:
            p_data->acc_range = 8;
            break;
        case BMI2_ACC_RANGE_16G:
            p_data->acc_range = 16;
            break;
    }

    p_data->acc_odr = p_config->cfg.acc.odr;
}

/** @brief          
 *  @param p_config 
 *  @param p_data   
*/
static void bmi2_configure_gyro(struct bmi2_sens_config *p_config, struct bmi270_data *p_data)
{
    /* The user can change the following configuration parameters according to their requirement. */
    /* Set Output Data Rate */
    p_config->cfg.gyr.odr = BMI2_GYR_ODR_25HZ;

    /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
    p_config->cfg.gyr.range = BMI2_GYR_RANGE_2000;

    /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
    p_config->cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

    /* Enable/Disable the noise performance mode for precision yaw rate sensing
        * There are two modes
        *  0 -> Ultra low power mode(Default)
        *  1 -> High performance mode
        */
    p_config->cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

    /* Enable/Disable the filter performance mode where averaging of samples
        * will be done based on above set bandwidth and ODR.
        * There are two modes
        *  0 -> Ultra low power mode
        *  1 -> High performance mode(Default)
        */
    p_config->cfg.gyr.filter_perf = BMI2_POWER_OPT_MODE;

    switch(p_config->cfg.gyr.range) {
        case BMI2_GYR_RANGE_2000:
            p_data->gyr_range = 2000;
            break;
        case BMI2_GYR_RANGE_1000:
            p_data->gyr_range = 1000;
            break;
        case BMI2_GYR_RANGE_500:
            p_data->gyr_range = 500;
            break;
        case BMI2_GYR_RANGE_250:
            p_data->gyr_range = 250;
            break;
        case BMI2_GYR_RANGE_125:
            p_data->gyr_range = 125;
            break;
    }

    p_data->gyr_odr = p_config->cfg.gyr.odr;
}

/** @brief          
 *  @param p_config 
 *  @param p_data   
*/
static void bmi2_configure_step_counter(struct bmi2_sens_config *p_config, struct bmi270_data *p_data)
{
    p_config->cfg.step_counter.watermark_level = 1;
}

/** @brief          
 *  @param p_config 
 *  @param p_data   
*/
static void bmi2_configure_anymotion(struct bmi2_sens_config *p_config, struct bmi270_data *p_data)
{
    /* 1LSB equals 20ms. Default is 100ms, setting to 80ms. */
    p_config->cfg.any_motion.duration = 0x04;

    /* 1LSB equals to 0.48mg. Default is 83mg, setting to 50mg. */
    p_config->cfg.any_motion.threshold = 0x68;
}

/** @brief          
 *  @param p_config 
 *  @param p_data   
*/
static void bmi2_configure_gesture_detect(struct bmi2_sens_config *p_config, struct bmi270_data *p_data)
{
    p_config->cfg.wrist_gest.wearable_arm = BMI2_ARM_LEFT;
}

/** @brief          
 *  @param p_config 
 *  @param p_data   
*/
static void bmi2_configure_wrist_wakeup(struct bmi2_sens_config *p_config, struct bmi270_data *p_data)
{
    p_config->cfg.wrist_gest_w.device_position = BMI2_ARM_LEFT;
    // TODO many things to bmi2_configure here
}

/** @brief          
 *  @param p_config 
 *  @param p_data   
*/
static void bmi2_configure_no_motion(struct bmi2_sens_config *p_config, struct bmi270_data *p_data)
{
    // 1LSB equals 20ms. Default is 100ms, setting to 80ms.
    // Max value is 163 seconds, hence below calc. We want max.
    p_config->cfg.no_motion.duration = (160 * 1000) / 20;
}

int bmi2_set_accel_range(const struct device *p_dev, const struct sensor_value *p_range)
{
    uint8_t config;
    struct bmi270_data *data = p_dev->data;

    if (p_range->val1 > BOSCH_BMI270_ACC_RANGE_16G) {
        return -ENOTSUP;
    }

    if (data->bmi2.read(BOSCH_BMI270_REG_ACC_RANGE, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    LOG_DBG("Set ACC range to %u", p_range->val1);

    config &= ~0x03;

    switch (p_range->val1) {
    case BOSCH_BMI270_ACC_RANGE_2G:
        config |= BOSCH_BMI270_ACC_RANGE_2G;
        data->acc_range = 2;
        break;
    case BOSCH_BMI270_ACC_RANGE_4G:
        config |= BOSCH_BMI270_ACC_RANGE_4G;
        data->acc_range = 4;
        break;
    case BOSCH_BMI270_ACC_RANGE_8G:
        config |= BOSCH_BMI270_ACC_RANGE_8G;
        data->acc_range = 8;
        break;
    case BOSCH_BMI270_ACC_RANGE_16G:
        config |= BOSCH_BMI270_ACC_RANGE_16G;
        data->acc_range = 16;
        break;
    default:
        return -ENOTSUP;
    }

    if (data->bmi2.write(BOSCH_BMI270_REG_ACC_RANGE, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    return 0;
}

int bmi2_set_accel_odr(const struct device *p_dev, const struct sensor_value *p_odr)
{
    struct bmi270_data *data = p_dev->data;
    uint8_t config;
    uint8_t pwr_ctrl;

    if ((p_odr->val1 == 0) || (p_odr->val1 > BOSCH_BMI270_ACC_ODR_1600_HZ)) {
        return -ENOTSUP;
    }

    if ((data->bmi2.read(BOSCH_BMI270_REG_ACC_CONF, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) ||
        (data->bmi2.read(BOSCH_BMI270_REG_PWR_CTRL, &pwr_ctrl, 1, data->bmi2.intf_ptr) != BMI2_OK)) {
        return -EFAULT;
    }

    LOG_DBG("Set ACC ODR to %u", p_odr->val1);

    config &= ~0x0F;
    config |= p_odr->val1;

    // Value 0 disables the ACC.
    if (p_odr->val1) {
        pwr_ctrl |= 0x01 << 0x02;
    } else {
        pwr_ctrl &= ~(0x01 << 0x02);
    }

    // If the Sampling frequency is higher than 100Hz, enter performance mode else, power optimized.
    if (p_odr->val1 >= BOSCH_BMI270_ACC_ODR_100_HZ) {
        config |= (0x01 << 0x07);     
    } else {
        config &= ~(0x01 << 0x07);
    }

    data->acc_odr = p_odr->val1;

    if (data->bmi2.write(BOSCH_BMI270_REG_ACC_CONF, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    // Assuming we have advance power save enabled.
    k_usleep(450);

    pwr_ctrl &= 0x0F;
    if (data->bmi2.write(BOSCH_BMI270_REG_PWR_CTRL, &pwr_ctrl, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    return 0;
}

int bmi2_set_accel_osr(const struct device *p_dev, const struct sensor_value *p_osr)
{
    struct bmi270_data *data = p_dev->data;
    uint8_t config;
    uint8_t pwr_ctrl;

    if (p_osr->val1 > BOSCH_BMI270_AVG128) {
        return -ENOTSUP;
    }

    if ((data->bmi2.read(BOSCH_BMI270_REG_ACC_CONF, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) ||
        (data->bmi2.read(BOSCH_BMI270_REG_PWR_CTRL, &pwr_ctrl, 1, data->bmi2.intf_ptr) != BMI2_OK)) {
        return -EFAULT;
    }

    LOG_DBG("Set ACC OSR to %u", p_osr->val1);

    config &= ~(0x07 << 0x04);

    if (data->acc_odr >= BOSCH_BMI270_ACC_ODR_100_HZ) {
        LOG_DBG("Performance mode active");

        switch (p_osr->val1) {
        case BOSCH_BMI270_ACC_OSR4:
            config |= (0x00 << 0x04);
            break;
        case BOSCH_BMI270_ACC_OSR2:
            config |= (0x01 << 0x04);
            break;
        case BOSCH_BMI270_ACC_OSR1:
            config |= (0x02 << 0x04);
            break;
        default:
            config |= (0x03 << 0x04);
            break;
        }
    } else {
        LOG_DBG("Low-Power mode active");

        switch (p_osr->val1) {
        case BOSCH_BMI270_AVG1:
            config |= (0x00 << 0x04);
            break;
        case BOSCH_BMI270_AVG2:
            config |= (0x01 << 0x04);
            break;
        case BOSCH_BMI270_AVG4:
            config |= (0x02 << 0x04);
            break;
        case BOSCH_BMI270_AVG8:
            config |= (0x03 << 0x04);
            break;
        case BOSCH_BMI270_AVG16:
            config |= (0x04 << 0x04);
            break;
        case BOSCH_BMI270_AVG32:
            config |= (0x05 << 0x04);
            break;
        case BOSCH_BMI270_AVG64:
            config |= (0x06 << 0x04);
            break;
        case BOSCH_BMI270_AVG128:
            config |= (0x07 << 0x04);
            break;
        default:
            return -ENOTSUP;
        }
    }

    data->acc_odr = p_osr->val1;

    if (data->bmi2.write(BOSCH_BMI270_REG_ACC_CONF, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    // Assuming we have advance power save enabled.
    k_usleep(450);

    pwr_ctrl &= 0x0F;
    if (data->bmi2.write(BOSCH_BMI270_REG_PWR_CTRL, &pwr_ctrl, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    return 0;
}

int bmi2_set_gyro_range(const struct device *p_dev, const struct sensor_value *p_range)
{
    uint8_t config;
    struct bmi270_data *data = p_dev->data;

    if (p_range->val1 > BOSCH_BMI270_GYR_RANGE_125) {
        return -ENOTSUP;
    }

    if (data->bmi2.read(BOSCH_BMI270_REG_GYR_RANGE, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    LOG_DBG("Set GYR range to %u", p_range->val1);

    config &= ~0x07;

    switch (p_range->val1) {
    case BOSCH_BMI270_GYR_RANGE_2000:
        config |= BOSCH_BMI270_GYR_RANGE_2000;
        data->gyr_range = 2000;
        break;
    case BOSCH_BMI270_GYR_RANGE_1000:
        config |= BOSCH_BMI270_GYR_RANGE_1000;
        data->gyr_range = 1000;
        break;
    case BOSCH_BMI270_GYR_RANGE_500:
        config |= BOSCH_BMI270_GYR_RANGE_500;
        data->gyr_range = 500;
        break;
    case BOSCH_BMI270_GYR_RANGE_250:
        config |= BOSCH_BMI270_GYR_RANGE_250;
        data->gyr_range = 250;
        break;
    case BOSCH_BMI270_GYR_RANGE_125:
        config |= BOSCH_BMI270_GYR_RANGE_125;
        data->gyr_range = 125;
        break;
    default:
        return -ENOTSUP;
    }

    if (data->bmi2.write(BOSCH_BMI270_REG_GYR_RANGE, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    return 0;
}

int bmi2_set_gyro_odr(const struct device *p_dev, const struct sensor_value *p_odr)
{
    struct bmi270_data *data = p_dev->data;
    uint8_t config;

    if ((p_odr->val1 < BOSCH_BMI270_GYR_ODR_25_HZ) ||
        (p_odr->val1 > BOSCH_BMI270_GYR_ODR_3200_HZ)) {
        return -ENOTSUP;
    }

    if (data->bmi2.read(BOSCH_BMI270_REG_GYR_CONF, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    LOG_DBG("Set GYR ODR to %u", p_odr->val1);

    config &= ~0x0F;
    config |= p_odr->val1;

    data->gyr_odr = p_odr->val1;

    if (data->bmi2.write(BOSCH_BMI270_REG_GYR_CONF, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    return 0;
}

int bmi2_set_gyro_osr(const struct device *p_dev, const struct sensor_value *p_osr)
{
    struct bmi270_data *data = p_dev->data;
    uint8_t config;

    if (p_osr->val1 > BOSCH_BMI270_GYR_OSR1) {
        return -ENOTSUP;
    }

    if (data->bmi2.read(BOSCH_BMI270_REG_GYR_CONF, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    LOG_DBG("Set GYR OSR to %u", p_osr->val1);

    config &= ~(0x03 << 4);
    config |= p_osr->val1 << 4;

    data->gyr_osr = p_osr->val1;

    if (data->bmi2.write(BOSCH_BMI270_REG_GYR_CONF, &config, 1, data->bmi2.intf_ptr) != BMI2_OK) {
        return -EFAULT;
    }

    return 0;
}

int bmi2_disable_feature(const struct device *p_dev, uint8_t feature)
{
    uint16_t int_status;
    uint8_t feature_disable;
    struct bmi270_data *data = p_dev->data;

    LOG_DBG("Disable feature: %u", feature);

    feature_disable = feature;
    if (bmi270_sensor_disable(&feature_disable, 1, &data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    // Clear int_status register.
    if (bmi2_get_int_status(&int_status, &data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    return 0;
}

int bmi2_enable_feature(const struct device *p_dev, uint8_t feature, bool int_en)
{
    uint16_t int_status;
    uint8_t feature_disable;
    struct bmi2_sens_int_config cfg;
    struct bmi270_data *data = p_dev->data;

    LOG_DBG("Enable feature: %u", feature);
    LOG_DBG("Use interrupts: %u", int_en);

    feature_disable = feature;
    if (bmi270_sensor_enable(&feature_disable, 1, &data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    // Clear int_status register.
    if (bmi2_get_int_status(&int_status, &data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    if (int_en) {
        cfg.hw_int_pin = BMI2_INT2;
    } else {
        cfg.hw_int_pin = BMI2_INT_NONE;
    }
    cfg.type = feature;

    if (bmi270_map_feat_int(&cfg, 1, &data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    return 0;
}