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

/** @brief
 *  @param p_dev
 *  @return         0 when successful
*/
int bmi2_init_interrupt(const struct device *p_dev);

/** @brief
 *  @param p_dev
 *  @param p_trig
 *  @param handler
 *  @return         0 when successful
*/
int bmi270_trigger_set(const struct device *p_dev, const struct sensor_trigger *p_trig, sensor_trigger_handler_t handler);
