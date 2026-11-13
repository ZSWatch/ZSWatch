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

/*
 * Faster drawing of RGB565 images with LV_BLEND_MODE_MULTIPLY, used for the sky tint.
 * LVGL's own code handles every blend mode in its per pixel loop, which makes the tint
 * cost more than the rest of the watchface together.
 */
void living_sky_blend_register(void);

void living_sky_blend_unregister(void);
