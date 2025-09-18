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

#include <inttypes.h>
#include <lvgl.h>

#include <zephyr/devicetree.h>

typedef void(*on_ui_close_cb_t)(void);

typedef void(*on_reference_set_cb_t)(void);

void sensors_summary_ui_show(lv_obj_t *root, on_ui_close_cb_t close_cb, on_reference_set_cb_t ref_set_cb);

void sensors_summary_ui_remove(void);

void sensors_summary_ui_set_pressure(float pressure);

void sensors_summary_ui_set_humidity(float humidity);

void sensors_summary_ui_set_temp(float temp);

void sensors_summary_ui_set_rel_height(float rel_height);

void sensors_summary_ui_set_light(float light);

void sensors_summary_ui_set_iaq(float iaq);

void sensors_summary_ui_set_co2(float co2);
