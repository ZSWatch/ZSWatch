/* bosch_bmp581.h - Driver for Bosch BMP581 pressure sensor. */

/*
 * Copyright (c 2023, Daniel Kampert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

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