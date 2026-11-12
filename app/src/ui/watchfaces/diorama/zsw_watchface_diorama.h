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

#include <stdbool.h>

/**
 * Replaces the clock, the weather and the accelerometer as what the Diorama watchface shows,
 * for testing. minute_of_day is the local time, weather a diorama_weather_t, intensity 1 to 3,
 * and the tilt -127 to 127 with x to the right and y down on the display. Call with enable
 * false to go back. Call with the LVGL lock held.
 */
void zsw_watchface_diorama_override(bool enable, int minute_of_day, int weather, int intensity, int tilt_x, int tilt_y);
