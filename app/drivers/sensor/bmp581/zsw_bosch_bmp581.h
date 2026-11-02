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

#pragma once

#include <zephyr/device.h>
#include <stdbool.h>
#include <stdint.h>

enum bosch_bmp581_mode {
	BOSCH_BMP581_MODE_STANDBY = 0,
	BOSCH_BMP581_MODE_NORMAL = 1,
};

enum bosch_bmp581_osr {
	BOSCH_BMP581_OSR_1X = 0,
	BOSCH_BMP581_OSR_2X,
	BOSCH_BMP581_OSR_4X,
	BOSCH_BMP581_OSR_8X,
	BOSCH_BMP581_OSR_16X,
	BOSCH_BMP581_OSR_32X,
	BOSCH_BMP581_OSR_64X,
	BOSCH_BMP581_OSR_128X,
};

enum bosch_bmp581_iir {
	BOSCH_BMP581_IIR_BYPASS = 0,
	BOSCH_BMP581_IIR_1,
	BOSCH_BMP581_IIR_3,
	BOSCH_BMP581_IIR_7,
	BOSCH_BMP581_IIR_15,
	BOSCH_BMP581_IIR_31,
	BOSCH_BMP581_IIR_63,
	BOSCH_BMP581_IIR_127,
};

typedef struct {
	uint8_t mode;
	uint8_t odr;
	uint8_t osr_p;
	uint8_t osr_t;
	uint8_t iir_p;
	uint8_t iir_t;
	bool fifo_enabled;
} bosch_bmp581_configuration_t;

typedef struct {
	float pressure;
	float temperature;
	uint32_t generation;
	uint32_t sequence;
} bosch_bmp581_sample_t;

int zsw_bmp581_apply_config(const struct device *dev, const bosch_bmp581_configuration_t *configuration,
						   uint32_t *generation);
int zsw_bmp581_get_config(const struct device *dev, bosch_bmp581_configuration_t *configuration);
int zsw_bmp581_read_sample(const struct device *dev, bosch_bmp581_sample_t *sample);

#define BOSCH_BMP581_ODR_240_HZ                         0x00
#define BOSCH_BMP581_ODR_218_5_HZ                       0x01
#define BOSCH_BMP581_ODR_199_1_HZ                       0x02
#define BOSCH_BMP581_ODR_179_2_HZ                       0x03
#define BOSCH_BMP581_ODR_160_HZ                         0x04
#define BOSCH_BMP581_ODR_149_3_HZ                       0x05
#define BOSCH_BMP581_ODR_140_HZ                         0x06
#define BOSCH_BMP581_ODR_129_8_HZ                       0x07
#define BOSCH_BMP581_ODR_120_HZ                         0x08
#define BOSCH_BMP581_ODR_110_1_HZ                       0x09
#define BOSCH_BMP581_ODR_100_2_HZ                       0x0A
#define BOSCH_BMP581_ODR_89_6_HZ                        0x0B
#define BOSCH_BMP581_ODR_80_HZ                          0x0C
#define BOSCH_BMP581_ODR_70_HZ                          0x0D
#define BOSCH_BMP581_ODR_60_HZ                          0x0E
#define BOSCH_BMP581_ODR_50_HZ                          0x0F
#define BOSCH_BMP581_ODR_45_HZ                          0x10
#define BOSCH_BMP581_ODR_40_HZ                          0x11
#define BOSCH_BMP581_ODR_35_HZ                          0x12
#define BOSCH_BMP581_ODR_30_HZ                          0x13
#define BOSCH_BMP581_ODR_25_HZ                          0x14
#define BOSCH_BMP581_ODR_20_HZ                          0x15
#define BOSCH_BMP581_ODR_15_HZ                          0x16
#define BOSCH_BMP581_ODR_10_HZ                          0x17
#define BOSCH_BMP581_ODR_05_HZ                          0x18
#define BOSCH_BMP581_ODR_04_HZ                          0x19
#define BOSCH_BMP581_ODR_03_HZ                          0x1A
#define BOSCH_BMP581_ODR_02_HZ                          0x1B
#define BOSCH_BMP581_ODR_01_HZ                          0x1C
#define BOSCH_BMP581_ODR_0_5_HZ                         0x1D
#define BOSCH_BMP581_ODR_0_250_HZ                       0x1E
#define BOSCH_BMP581_ODR_0_125_HZ                       0x1F
#define BOSCH_BMP581_ODR_DEFAULT                        BOSCH_BMP581_ODR_0_250_HZ
