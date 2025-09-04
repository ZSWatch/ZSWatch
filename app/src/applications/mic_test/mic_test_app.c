#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include "mic_test_ui.h"
#include "managers/zsw_app_manager.h"
#include "managers/zsw_microphone_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(mic_test_app, LOG_LEVEL_DBG);

// Functions needed for all applications
static void mic_test_app_start(lv_obj_t *root, lv_group_t *group);
static void mic_test_app_stop(void);

// UI work items for LVGL thread safety
static struct k_work_delayable ui_reset_work;
static void ui_reset_work_handler(struct k_work *work);

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

static void mic_test_app_start(lv_obj_t *root, lv_group_t *group)
{
    k_work_init_delayable(&ui_reset_work, ui_reset_work_handler);

    mic_test_ui_show(root, on_toggle_button_pressed);
    running = true;
    LOG_INF("Microphone test app started");
}

static void mic_test_app_stop(void)
{
    k_work_cancel_delayable(&ui_reset_work);

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
        config.output = ZSW_MIC_OUTPUT_RTT;

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
                LOG_DBG("Received raw audio: %d bytes",
                        block->size);
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

static int mic_test_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(mic_test_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
