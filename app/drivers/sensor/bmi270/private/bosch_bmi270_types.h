/* bosch_bmi270_types.h - Driver for Bosch BMI270 IMU. */

/*
 * Copyright (c) 2023, Daniel Kampert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>

#include "bmi2.h"
#include "bmi270.h"

/** @brief 
*/
struct bmi270_config {
    struct i2c_dt_spec i2c;
#ifdef CONFIG_BMI270_PLUS_TRIGGER
	struct gpio_dt_spec int_gpio;
#endif
	bool swap_xy;
	bool invert_x;
	bool invert_y;
	uint16_t rotation;
};

/** @brief 
*/
struct bmi270_data {
#ifdef CONFIG_BMI270_PLUS_TRIGGER
	struct gpio_callback gpio_handler;

	const struct device *dev;
	const struct sensor_trigger *trig;

	sensor_trigger_handler_t global;
	sensor_trigger_handler_t sig_motion;
	sensor_trigger_handler_t step;
	sensor_trigger_handler_t activity;
	sensor_trigger_handler_t wake;
	sensor_trigger_handler_t gesture;
	sensor_trigger_handler_t stationary;
	sensor_trigger_handler_t motion;
#endif

#ifdef CONFIG_BMI270_PLUS_TRIGGER_OWN_THREAD
	struct k_sem sem;
#endif

#ifdef CONFIG_BMI270_PLUS_TRIGGER_GLOBAL_THREAD
	struct k_work work;
#endif

	int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
    uint16_t temp;
	uint8_t acc_range;
    uint8_t acc_odr;
	uint16_t gyr_range;
    uint8_t gyr_odr;
    uint8_t gyr_osr;
	struct bmi2_dev bmi2;
};