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

#include "../../drivers/sensor/bmp581/bosch_zsw_bmp581.h"

int zsw_pressure_sensor_init(void);

int zsw_pressure_sensor_set_odr(uint8_t odr);

int zsw_pressure_sensor_get_pressure(float *pressure);

int zsw_pressure_sensor_get_temperature(float *temperature);