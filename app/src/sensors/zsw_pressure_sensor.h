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

#pragma once

#include <zephyr/drivers/sensor.h>

#include "ext_drivers/BMP5-Sensor-API/bmp5_defs.h"

typedef enum {
    PRESSURE_SENSOR_ODR_240_HZ = BMP5_ODR_240_HZ,
    PRESSURE_SENSOR_ODR_218_5_HZ = BMP5_ODR_218_5_HZ,
    PRESSURE_SENSOR_ODR_199_1_HZ = BMP5_ODR_199_1_HZ,
    PRESSURE_SENSOR_ODR_179_2_HZ = BMP5_ODR_179_2_HZ,
    PRESSURE_SENSOR_ODR_160_HZ = BMP5_ODR_160_HZ,
    PRESSURE_SENSOR_ODR_149_3_HZ = BMP5_ODR_149_3_HZ,
    PRESSURE_SENSOR_ODR_140_HZ = BMP5_ODR_140_HZ,
    PRESSURE_SENSOR_ODR_129_8_HZ = BMP5_ODR_129_8_HZ,
    PRESSURE_SENSOR_ODR_120_HZ = BMP5_ODR_120_HZ,
    PRESSURE_SENSOR_ODR_110_1_HZ = BMP5_ODR_110_1_HZ,
    PRESSURE_SENSOR_ODR_100_2_HZ = BMP5_ODR_100_2_HZ,
    PRESSURE_SENSOR_ODR_89_6_HZ = BMP5_ODR_89_6_HZ,
    PRESSURE_SENSOR_ODR_80_HZ = BMP5_ODR_80_HZ,
    PRESSURE_SENSOR_ODR_70_HZ = BMP5_ODR_70_HZ,
    PRESSURE_SENSOR_ODR_60_HZ = BMP5_ODR_60_HZ,
    PRESSURE_SENSOR_ODR_50_HZ = BMP5_ODR_50_HZ,
    PRESSURE_SENSOR_ODR_45_HZ = BMP5_ODR_45_HZ,
    PRESSURE_SENSOR_ODR_40_HZ = BMP5_ODR_40_HZ,
    PRESSURE_SENSOR_ODR_35_HZ = BMP5_ODR_35_HZ,
    PRESSURE_SENSOR_ODR_30_HZ = BMP5_ODR_30_HZ,
    PRESSURE_SENSOR_ODR_25_HZ = BMP5_ODR_25_HZ,
    PRESSURE_SENSOR_ODR_20_HZ = BMP5_ODR_20_HZ,
    PRESSURE_SENSOR_ODR_15_HZ = BMP5_ODR_15_HZ,
    PRESSURE_SENSOR_ODR_10_HZ = BMP5_ODR_10_HZ,
    PRESSURE_SENSOR_ODR_05_HZ = BMP5_ODR_05_HZ,
    PRESSURE_SENSOR_ODR_04_HZ = BMP5_ODR_04_HZ,
    PRESSURE_SENSOR_ODR_03_HZ = BMP5_ODR_03_HZ,
    PRESSURE_SENSOR_ODR_02_HZ = BMP5_ODR_02_HZ,
    PRESSURE_SENSOR_ODR_01_HZ = BMP5_ODR_01_HZ,
    PRESSURE_SENSOR_ODR_0_5_HZ = BMP5_ODR_0_5_HZ,
    PRESSURE_SENSOR_ODR_0_250_HZ = BMP5_ODR_0_250_HZ,
    PRESSURE_SENSOR_ODR_0_125_HZ = BMP5_ODR_0_125_HZ,
    PRESSURE_SENSOR_ODR_DEFAULT
} bmp5_odr_t;

int zsw_pressure_sensor_init(void);

int zsw_pressure_sensor_set_odr(uint8_t odr);

int zsw_pressure_sensor_get_pressure(float *pressure);

int zsw_pressure_sensor_get_temperature(float *temperature);