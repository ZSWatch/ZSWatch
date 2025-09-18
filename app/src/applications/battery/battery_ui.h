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
#include <stdbool.h>
#include <lvgl.h>

#define BATTERY_APP_SAMPLE_INTERVAL_MIN         1

typedef void(*on_clear_history)(void);

void battery_ui_show(lv_obj_t *root, on_clear_history clear_hist_cb, int max_samples, bool include_pmic_ui);

void battery_ui_remove(void);

void battery_ui_add_measurement(int percent, int voltage);

void battery_ui_update(int ttf, int tte, int status, int error, int charging);
