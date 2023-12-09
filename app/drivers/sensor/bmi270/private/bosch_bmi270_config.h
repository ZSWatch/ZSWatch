/* bosch_bmi270_config.h - Driver for Bosch BMI270 IMU. */

/*
 * Copyright (c) 2023, Daniel Kampert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "bmi2.h"
#include "bmi270.h"
#include "../bosch_bmi270.h"
#include "bosch_bmi270_types.h"

#define BOSCH_BMI270_REG_CHIP_ID         0x00
#define BOSCH_BMI270_REG_ERROR           0x02
#define BOSCH_BMI270_REG_STATUS          0x03
#define BOSCH_BMI270_REG_AUX_X_LSB       0x04
#define BOSCH_BMI270_REG_ACC_X_LSB       0x0C
#define BOSCH_BMI270_REG_GYR_X_LSB       0x12
#define BOSCH_BMI270_REG_SENSORTIME_0    0x18
#define BOSCH_BMI270_REG_EVENT           0x1B
#define BOSCH_BMI270_REG_INT_STATUS_0    0x1C
#define BOSCH_BMI270_REG_SC_OUT_0        0x1E
#define BOSCH_BMI270_REG_WR_GEST_ACT     0x20
#define BOSCH_BMI270_REG_INTERNAL_STATUS 0x21
#define BOSCH_BMI270_REG_TEMPERATURE_0   0x22
#define BOSCH_BMI270_REG_FIFO_LENGTH_0   0x24
#define BOSCH_BMI270_REG_FIFO_DATA       0x26
#define BOSCH_BMI270_REG_FEAT_PAGE       0x2F
#define BOSCH_BMI270_REG_FEATURES_0      0x30
#define BOSCH_BMI270_REG_ACC_CONF        0x40
#define BOSCH_BMI270_REG_ACC_RANGE       0x41
#define BOSCH_BMI270_REG_GYR_CONF        0x42
#define BOSCH_BMI270_REG_GYR_RANGE       0x43
#define BOSCH_BMI270_REG_AUX_CONF        0x44
#define BOSCH_BMI270_REG_FIFO_DOWNS      0x45
#define BOSCH_BMI270_REG_FIFO_WTM_0      0x46
#define BOSCH_BMI270_REG_FIFO_CONFIG_0   0x48
#define BOSCH_BMI270_REG_SATURATION      0x4A
#define BOSCH_BMI270_REG_AUX_DEV_ID      0x4B
#define BOSCH_BMI270_REG_AUX_IF_CONF     0x4C
#define BOSCH_BMI270_REG_AUX_RD_ADDR     0x4D
#define BOSCH_BMI270_REG_AUX_WR_ADDR     0x4E
#define BOSCH_BMI270_REG_AUX_WR_DATA     0x4F
#define BOSCH_BMI270_REG_ERR_REG_MSK     0x52
#define BOSCH_BMI270_REG_INT1_IO_CTRL    0x53
#define BOSCH_BMI270_REG_INT2_IO_CTRL    0x54
#define BOSCH_BMI270_REG_INT_LATCH       0x55
#define BOSCH_BMI270_REG_INT1_MAP_FEAT   0x56
#define BOSCH_BMI270_REG_INT2_MAP_FEAT   0x57
#define BOSCH_BMI270_REG_INT_MAP_DATA    0x58
#define BOSCH_BMI270_REG_INIT_CTRL       0x59
#define BOSCH_BMI270_REG_INIT_ADDR_0     0x5B
#define BOSCH_BMI270_REG_INIT_DATA       0x5E
#define BOSCH_BMI270_REG_INTERNAL_ERROR  0x5F
#define BOSCH_BMI270_REG_AUX_IF_TRIM     0x68
#define BOSCH_BMI270_REG_GYR_CRT_CONF    0x69
#define BOSCH_BMI270_REG_NVM_CONF        0x6A
#define BOSCH_BMI270_REG_IF_CONF         0x6B
#define BOSCH_BMI270_REG_DRV             0x6C
#define BOSCH_BMI270_REG_ACC_SELF_TEST   0x6D
#define BOSCH_BMI270_REG_GYR_SELF_TEST   0x6E
#define BOSCH_BMI270_REG_NV_CONF         0x70
#define BOSCH_BMI270_REG_OFFSET_0        0x71
#define BOSCH_BMI270_REG_PWR_CONF        0x7C
#define BOSCH_BMI270_REG_PWR_CTRL        0x7D
#define BOSCH_BMI270_REG_CMD             0x7E

/** @brief
 *  @param p_dev
 *  @param p_data
 *  @return         0 when successful
*/
int bmi2_configure_enable_all(const struct device *p_dev, struct bmi270_data *p_data);

/** @brief
 *  @param p_dev
 *  @param p_range
 *  @return         0 when successful
*/
int bmi2_set_accel_range(const struct device *p_dev, const struct sensor_value *p_range);

/** @brief
 *  @param p_dev
 *  @param p_odr
 *  @return         0 when successful
*/
int bmi2_set_accel_odr(const struct device *p_dev, const struct sensor_value *p_odr);

/** @brief
 *  @param p_dev
 *  @param p_osr
 *  @return         0 when successful
*/
int bmi2_set_accel_osr(const struct device *p_dev, const struct sensor_value *p_osr);

/** @brief
 *  @param p_dev
 *  @param p_range
 *  @return         0 when successful
*/
int bmi2_set_gyro_range(const struct device *p_dev, const struct sensor_value *p_range);

/** @brief
 *  @param p_dev
 *  @param p_odr
 *  @return         0 when successful
*/
int bmi2_set_gyro_odr(const struct device *p_dev, const struct sensor_value *p_odr);

/** @brief
 *  @param p_dev
 *  @param p_osr
 *  @return         0 when successful
*/
int bmi2_set_gyro_osr(const struct device *p_dev, const struct sensor_value *p_osr);

/** @brief
 *  @param p_dev
 *  @param feature
 *  @return         0 when successful
*/
int bmi2_disable_feature(const struct device *p_dev, uint8_t feature);

/** @brief
 *  @param p_dev
 *  @param feature
 *  @param int_en
 *  @return         0 when successful
*/
int bmi2_enable_feature(const struct device *p_dev, uint8_t feature, bool int_en);

/** @brief
 *  @param p_dev
 *  @return         0 when successful
*/
int bmi2_reset_step_counter(const struct device *p_dev);