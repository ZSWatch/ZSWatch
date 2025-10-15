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

typedef void(*on_close_cb_t)(void);
typedef void(*fusion_ui_zero_cb_t)(void);

void fusion_ui_show(lv_obj_t *root, on_close_cb_t close_cb,
                    fusion_ui_zero_cb_t zero_cb);

void fusion_ui_remove(void);

void fusion_ui_set_values(int32_t x, int32_t y, int32_t z);

void fusion_ui_set_quaternion(float w, float x, float y, float z);

void fusion_ui_set_stats(float render_hz);
