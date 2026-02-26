/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2025 ZSWatch Project, Leonardo Bispo, Jakob Krantz.
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

typedef struct sensor_fusion {
    float roll;
    float pitch;
    float yaw;
    float x;
    float y;
    float z;
} sensor_fusion_t;

typedef struct zsw_quat {
    float w;
    float x;
    float y;
    float z;
} zsw_quat_t;

int zsw_sensor_fusion_init(void);

void zsw_sensor_fusion_deinit(void);

int zsw_sensor_fusion_fetch_all(sensor_fusion_t *p_readings);

int zsw_sensor_fusion_get_heading(float *heading);

int zsw_sensor_fusion_get_quaternion(zsw_quat_t *q);
