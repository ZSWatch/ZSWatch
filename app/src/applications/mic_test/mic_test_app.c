#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include "mic_test_ui.h"
#include "spectrum_analyzer.h"
#include "managers/zsw_app_manager.h"
#include "managers/zsw_microphone_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(mic_test_app, LOG_LEVEL_DBG);

// Functions needed for all applications
static void mic_test_app_start(lv_obj_t *root, lv_group_t *group);
static void mic_test_app_stop(void);

// UI work items for LVGL thread safety
static struct k_work_delayable ui_reset_work;
static struct k_work spectrum_update_work;
static void ui_reset_work_handler(struct k_work *work);
static void spectrum_update_work_handler(struct k_work *work);

// Spectrum analysis buffers (use max size for both modes)
static uint8_t spectrum_magnitudes_circular[SPECTRUM_NUM_BARS_CIRCULAR];
static uint8_t spectrum_magnitudes_linear[SPECTRUM_NUM_BARS_LINEAR];
static int16_t audio_samples[SPECTRUM_FFT_SIZE];

// Functions related to app functionality
static void on_toggle_button_pressed(void);
static void mic_event_callback(zsw_mic_event_t event, void *data, void *user_data);

ZSW_LV_IMG_DECLARE(statistic_icon);

static application_t app = {
    .name = "Mic Test",
    .icon = ZSW_LV_IMG_USE(statistic_icon),
    .start_func = mic_test_app_start,
    .stop_func = mic_test_app_stop,
};

static bool running = false;
static size_t sample_buffer_index = 0;

static void mic_test_app_start(lv_obj_t *root, lv_group_t *group)
{
    k_work_init_delayable(&ui_reset_work, ui_reset_work_handler);
    k_work_init(&spectrum_update_work, spectrum_update_work_handler);

    // Initialize spectrum analyzer
    int ret = spectrum_analyzer_init();
    if (ret < 0) {
        LOG_ERR("Failed to initialize spectrum analyzer: %d", ret);
    }

    mic_test_ui_show(root, on_toggle_button_pressed);
    running = true;
    sample_buffer_index = 0;
    LOG_INF("Microphone test app started");
}

static void mic_test_app_stop(void)
{
    k_work_cancel_delayable(&ui_reset_work);
    k_work_cancel(&spectrum_update_work);

    if (zsw_microphone_manager_is_recording()) {
        zsw_microphone_stop_recording();
    }

    mic_test_ui_remove();
    running = false;
    LOG_INF("Microphone test app stopped");
}

static void on_toggle_button_pressed(void)
{
    if (!running) {
        return;
    }

    if (zsw_microphone_manager_is_recording()) {
        LOG_INF("Microphone stop button pressed");
        mic_test_ui_set_status("Stopping...");
        int ret = zsw_microphone_stop_recording();
        if (ret < 0) {
            LOG_ERR("Failed to stop recording: %d", ret);
            mic_test_ui_set_status("Stop Failed!");
        } else {
            mic_test_ui_set_status("Ready");
            mic_test_ui_toggle_button_state();
            LOG_INF("Recording stopped successfully");
        }
    } else {
        LOG_INF("Microphone start button pressed");

        if (zsw_microphone_manager_is_recording()) {
            LOG_WRN("Microphone is already busy");
            return;
        }

        int ret;
        zsw_mic_config_t config;
        zsw_microphone_manager_get_default_config(&config);
        config.duration_ms = 0;
        config.output = ZSW_MIC_OUTPUT_RAW; // Change to raw mode for real-time processing

        mic_test_ui_set_status("Starting...");

        ret = zsw_microphone_manager_start_recording(&config, mic_event_callback, NULL);
        if (ret < 0) {
            LOG_ERR("Failed to start recording: %d", ret);
            mic_test_ui_set_status("Start Failed!");
            k_work_schedule(&ui_reset_work, K_SECONDS(2));
        } else {
            mic_test_ui_set_status("Recording...");
            mic_test_ui_toggle_button_state();
            LOG_INF("Recording started successfully");
        }
    }
}

static void mic_event_callback(zsw_mic_event_t event, void *data, void *user_data)
{
    if (!running) {
        return;
    }

    switch (event) {
        case ZSW_MIC_EVENT_RECORDING_DATA:
            if (data) {
                zsw_mic_raw_block_t *block = (zsw_mic_raw_block_t *)data;

                // Process audio data for spectrum analysis
                int16_t *samples = (int16_t *)block->data;
                size_t num_samples = block->size / sizeof(int16_t);

                // Accumulate samples until we have enough for FFT
                for (size_t i = 0; i < num_samples && sample_buffer_index < SPECTRUM_FFT_SIZE; i++) {
                    audio_samples[sample_buffer_index++] = samples[i];
                }

                // Process when buffer is full
                if (sample_buffer_index >= SPECTRUM_FFT_SIZE) {
                    // Process for both modes
                    int ret1 = spectrum_analyzer_process(audio_samples, SPECTRUM_FFT_SIZE,
                                                         spectrum_magnitudes_circular, SPECTRUM_NUM_BARS_CIRCULAR);
                    int ret2 = spectrum_analyzer_process(audio_samples, SPECTRUM_FFT_SIZE,
                                                         spectrum_magnitudes_linear, SPECTRUM_NUM_BARS_LINEAR);
                    if (ret1 == 0 || ret2 == 0) {
                        // Submit work to update UI from main thread
                        k_work_submit(&spectrum_update_work);
                    }
                    sample_buffer_index = 0; // Reset buffer
                }
            }
            break;
        case ZSW_MIC_EVENT_RECORDING_TIMEOUT:
            mic_test_ui_set_status("Complete!");
            mic_test_ui_toggle_button_state();
            k_work_schedule(&ui_reset_work, K_SECONDS(2));
            break;
        default:
            break;
    }
}

static void ui_reset_work_handler(struct k_work *work)
{
    if (running) {
        mic_test_ui_set_status("Ready");
    }
}

static void spectrum_update_work_handler(struct k_work *work)
{
    if (running) {
        // Update both modes - UI will pick the right one
        mic_test_ui_update_spectrum(spectrum_magnitudes_circular, SPECTRUM_NUM_BARS_CIRCULAR);
        mic_test_ui_update_spectrum(spectrum_magnitudes_linear, SPECTRUM_NUM_BARS_LINEAR);
    }
}

static int mic_test_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(mic_test_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
