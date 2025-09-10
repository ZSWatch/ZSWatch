/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2025 Jakob Krantz.
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

#include <lvgl.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_SPECTRUM_BARS       30

typedef void (*watch_ui_toggle_cb_t)(void);
typedef void (*watch_ui_gain_change_cb_t)(float new_gain);
typedef void (*watch_ui_rtt_output_cb_t)(bool rtt_output);

/**
 * @brief Create the circular spectrum analyzer watch UI
 *
 * @param parent Parent LVGL object
 * @param toggle_cb Callback for start/stop button
 * @param gain_change_cb Callback for gain changes (can be NULL)
 * @param rtt_output_cb Callback for RTT output toggle (can be NULL)
 * @param initial_gain Initial gain value for the UI
 * @return lv_obj_t* Container object
 */
lv_obj_t *circular_spectrum_watch_ui_create(lv_obj_t *parent, watch_ui_toggle_cb_t toggle_cb,
                                            watch_ui_gain_change_cb_t gain_change_cb, 
                                            watch_ui_rtt_output_cb_t rtt_output_cb, float initial_gain);

void circular_spectrum_watch_ui_remove(void);

/**
 * @brief Update spectrum data with FFT magnitudes
 *
 * @param magnitudes Array of magnitude values [0-255]
 * @param count Number of magnitude values (shall be NUM_SPECTRUM_BARS)
 */
void circular_spectrum_watch_ui_update_spectrum(const uint8_t *magnitudes, size_t count);

/**
 * @brief Set the status text
 *
 * @param status Status string to display
 */
void circular_spectrum_watch_ui_set_status(const char *status);

/**
 * @brief Toggle the button state between Start/Stop
 *
 * @param is_recording true if recording, false if stopped
 */
void circular_spectrum_watch_ui_set_recording(bool is_recording);

#ifdef __cplusplus
}
#endif
