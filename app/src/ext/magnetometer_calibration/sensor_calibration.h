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

#include <stdbool.h>
#include "matrix.h"
#include "vector.h"

typedef struct {
    Vector offset;
    Matrix transform;
} Calibration_t;

Calibration_t calib_calibrate_sensor(Vector x, Vector y, Vector z);
bool calib_calibration_success(Calibration_t calib);
void calib_calibrate_multiple_points(Calibration_t calib, Vector x, Vector y, Vector z);
void calib_calibrate_point(Calibration_t calib, Vector point);
double square_distance_variance(Vector x, Vector y, Vector z);
void calib_free(Calibration_t calibA);
