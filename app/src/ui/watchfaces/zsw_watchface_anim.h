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
#include <lvgl.h>

/*
 * Wake-up animations. The screen is off most of the time, so a face shows the
 * time with a short intro when it turns on: hands sweep from 12 and digits rise
 * into place. A face resets its parts in its wake() callback and starts the
 * animations on the next set_datetime(), when it knows the time to animate to.
 */

#define ZSW_WATCHFACE_ANIM_SWEEP_MS 650
#define ZSW_WATCHFACE_ANIM_RISE_MS  400

/** @brief Turn a hand back to 12, so a sweep starts from there without a jump. */
void zsw_watchface_anim_reset_hand(lv_obj_t *hand);

/** @brief          Sweep a hand clockwise from 12 to rotation (0.1 degrees).
 *  @param delay_ms Start later than the other hands for a staggered sweep
*/
void zsw_watchface_anim_sweep(lv_obj_t *hand, int32_t rotation, uint32_t delay_ms);

/** @brief  Set a hand's rotation. While it sweeps, the sweep ends there instead. */
void zsw_watchface_anim_set_rotation(lv_obj_t *hand, int32_t rotation);

/** @brief Hide a label's text until it rises. */
void zsw_watchface_anim_reset_label(lv_obj_t *label);

/** @brief          Slide a label up into place while its text fades in.
 *  @param delay_ms Start later than other labels for a staggered rise
*/
void zsw_watchface_anim_rise(lv_obj_t *label, uint32_t delay_ms);

/** @brief          Fill an arc from its minimum to its current value.
 *  @param delay_ms Start later than other parts for a staggered intro
*/
void zsw_watchface_anim_fill(lv_obj_t *arc, uint32_t delay_ms);

/** @brief  Set an arc's value. While it fills, the fill ends there instead. */
void zsw_watchface_anim_set_arc(lv_obj_t *arc, int32_t value);
