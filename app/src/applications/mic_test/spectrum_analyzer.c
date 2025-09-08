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
static float32_t smoothed_magnitudes[SPECTRUM_NUM_BARS_LINEAR]; // Use max size
static const float32_t SMOOTHING_FACTOR = 0.5f; // Reduced for faster response

// Color mapping constants for circular display
static const uint32_t spectrum_colors[SPECTRUM_NUM_BARS_CIRCULAR] = {
    // Red to Orange (Bass - 0-5)
    0xFF0000, 0xFF2000, 0xFF4000, 0xFF6000, 0xFF8000, 0xFFA000,
    // Orange to Yellow (Low-Mid - 6-11)
    0xFFC000, 0xFFE000, 0xFFFF00, 0xE0FF00, 0xC0FF00, 0xA0FF00,
    // Yellow to Green (Mid - 12-17)
    0x80FF00, 0x60FF00, 0x40FF00, 0x20FF00, 0x00FF00, 0x00FF20,
    // Green to Blue (High-Mid - 18-23)
    0x00FF40, 0x00FF60, 0x00FF80, 0x00FFA0, 0x00FFC0, 0x00FFFF
};

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
                              uint8_t *magnitudes, size_t num_bars)
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
        float32_t log_magnitude = logf(1.0f + smoothed_magnitudes[bar] * 100.0f); // 10x more sensitive
        uint8_t magnitude_8bit = (uint8_t)(log_magnitude * 25.0f); // Adjusted scaling

        if (magnitude_8bit > 255) {
            magnitude_8bit = 255;
        }
        magnitudes[bar] = magnitude_8bit;
    }

    return 0;
}

uint32_t spectrum_get_bar_color(uint8_t bar_index, uint8_t magnitude)
{
    if (bar_index >= SPECTRUM_NUM_BARS_CIRCULAR) {
        return 0x808080; // Gray for invalid index
    }

    // Get base color for this frequency band
    uint32_t base_color = spectrum_colors[bar_index];

    // Extract RGB components
    uint8_t red = (base_color >> 16) & 0xFF;
    uint8_t green = (base_color >> 8) & 0xFF;
    uint8_t blue = base_color & 0xFF;

    // Scale brightness based on magnitude (minimum 10% brightness, more dynamic range)
    float32_t brightness = 0.1f + (magnitude / 255.0f) * 0.9f;

    red = (uint8_t)(red * brightness);
    green = (uint8_t)(green * brightness);
    blue = (uint8_t)(blue * brightness);

    return (red << 16) | (green << 8) | blue;
}
