/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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
#include <zephyr/sys/util.h>

#ifdef CONFIG_ZSW_BMI270_STEP_DEBUG
void zsw_imu_step_debug_start(const struct device *p_imu);
void zsw_imu_step_debug_on_trigger(int type);
#else
static inline void zsw_imu_step_debug_start(const struct device *p_imu)
{
    ARG_UNUSED(p_imu);
}

static inline void zsw_imu_step_debug_on_trigger(int type)
{
    ARG_UNUSED(type);
}
#endif
