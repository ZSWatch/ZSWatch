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

#include "spectrum_analyzer.h"
#include <zephyr/logging/log.h>
#include <arm_math.h>
#include <string.h>
#include <math.h>

LOG_MODULE_REGISTER(spectrum_analyzer, LOG_LEVEL_DBG);

// CMSIS-DSP FFT instance
static arm_rfft_fast_instance_f32 rfft_instance;
static bool initialized = false;

// Working buffers for FFT processing
static float32_t input_buffer[SPECTRUM_FFT_SIZE];
static float32_t output_buffer[SPECTRUM_FFT_SIZE];
static float32_t magnitude_buffer[SPECTRUM_FFT_SIZE / 2];

// Smoothing for better visual effect (less smoothing for more responsiveness)
static float32_t smoothed_magnitudes[64]; // Use max possible bars
static const float32_t SMOOTHING_FACTOR = 0.3f; // Reduced for faster response

// Color mapping now uses HSV calculation for any number of bars

int spectrum_analyzer_init(void)
{
    if (initialized) {
        return 0;
    }

    // Initialize CMSIS-DSP Real FFT instance
    arm_status status = arm_rfft_fast_init_f32(&rfft_instance, SPECTRUM_FFT_SIZE);
    if (status != ARM_MATH_SUCCESS) {
        LOG_ERR("Failed to initialize CMSIS-DSP RFFT: %d", status);
        return -EIO;
    }

    // Clear working buffers
    memset(input_buffer, 0, sizeof(input_buffer));
    memset(output_buffer, 0, sizeof(output_buffer));
    memset(magnitude_buffer, 0, sizeof(magnitude_buffer));
    memset(smoothed_magnitudes, 0, sizeof(smoothed_magnitudes));

    initialized = true;
    LOG_INF("Spectrum analyzer initialized with %d-point FFT", SPECTRUM_FFT_SIZE);
    return 0;
}

int spectrum_analyzer_process(const int16_t *samples, size_t num_samples,
                              uint8_t *magnitudes, size_t num_bars, float gain_multiplier)
{
    if (!initialized) {
        LOG_ERR("Spectrum analyzer not initialized");
        return -EINVAL;
    }

    if (!samples || !magnitudes || num_bars == 0) {
        LOG_ERR("Invalid parameters");
        return -EINVAL;
    }

    if (num_samples < SPECTRUM_FFT_SIZE) {
        LOG_ERR("Not enough samples for FFT: %d < %d", num_samples, SPECTRUM_FFT_SIZE);
        return -EINVAL;
    }

    // Convert 16-bit PCM to float32 and normalize to [-1.0, 1.0]
    for (int i = 0; i < SPECTRUM_FFT_SIZE; i++) {
        input_buffer[i] = (float32_t)samples[i] / 32768.0f;
    }

    // Perform Real FFT
    arm_rfft_fast_f32(&rfft_instance, input_buffer, output_buffer, 0);

    // Calculate magnitude for each frequency bin
    // Note: RFFT output is [DC, Re1, Im1, Re2, Im2, ..., ReN/2-1, ImN/2-1, Nyquist]
    magnitude_buffer[0] = fabsf(output_buffer[0]); // DC component

    for (int i = 1; i < SPECTRUM_FFT_SIZE / 2; i++) {
        float32_t real = output_buffer[2 * i];
        float32_t imag = output_buffer[2 * i + 1];
        arm_sqrt_f32(real * real + imag * imag, &magnitude_buffer[i]);
    }

    // Group frequency bins into display bars
    // Each bar represents multiple frequency bins for better visualization
    int bins_per_bar = (SPECTRUM_FFT_SIZE / 2) / num_bars;
    if (bins_per_bar < 1) {
        bins_per_bar = 1;
    }

    for (int bar = 0; bar < num_bars; bar++) {
        float32_t bar_magnitude = 0.0f;
        int start_bin = bar * bins_per_bar;
        int end_bin = start_bin + bins_per_bar;

        if (end_bin > SPECTRUM_FFT_SIZE / 2) {
            end_bin = SPECTRUM_FFT_SIZE / 2;
        }

        // Average the magnitude over the frequency range for this bar
        for (int bin = start_bin; bin < end_bin; bin++) {
            bar_magnitude += magnitude_buffer[bin];
        }
        bar_magnitude /= (end_bin - start_bin);

        // Apply smoothing for better visual effect
        smoothed_magnitudes[bar] = SMOOTHING_FACTOR * smoothed_magnitudes[bar] +
                                   (1.0f - SMOOTHING_FACTOR) * bar_magnitude;

        // Convert to 8-bit magnitude (0-255) with much higher sensitivity
        float32_t log_magnitude = logf(1.0f + smoothed_magnitudes[bar] * 500.0f * gain_multiplier); // Apply gain multiplier
        uint8_t magnitude_8bit = (uint8_t)(log_magnitude * 40.0f); // Increased scaling

        if (magnitude_8bit > 255) {
            magnitude_8bit = 255;
        }
        magnitudes[bar] = magnitude_8bit;
    }

    return 0;
}
