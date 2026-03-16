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

/**
 * @brief Quick-Record shortcut: long-press a button to start/stop voice recording.
 *
 * Uses zsw_recording_manager and zsw_recording_overlay instead of launching the app.
 */

#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>

#include "managers/zsw_recording_manager.h"
#include "zsw_recording_overlay.h"

LOG_MODULE_REGISTER(zsw_quick_record, CONFIG_ZSW_VOICE_MEMO_LOG_LEVEL);

#if CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON > 0

/* Map button number (1-4) to input key code */
#if CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON == 1
#define QUICK_RECORD_KEY_CODE INPUT_KEY_1
#elif CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON == 2
#define QUICK_RECORD_KEY_CODE INPUT_KEY_2
#elif CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON == 3
#define QUICK_RECORD_KEY_CODE INPUT_KEY_3
#elif CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON == 4
#define QUICK_RECORD_KEY_CODE INPUT_KEY_4
#endif

#define LONG_PRESS_MS  1000

static bool button_pressed;
static struct k_timer long_press_timer;
static struct k_work quick_record_work;
static bool long_press_fired;

static void quick_record_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);

    if (zsw_recording_manager_is_recording()) {
        int ret = zsw_recording_manager_stop();
        if (ret == 0) {
            LOG_INF("Quick-record stopped");
            zsw_recording_overlay_hide();
        } else {
            LOG_ERR("Quick-record stop failed: %d", ret);
        }
    } else {
        int ret = zsw_recording_manager_start();
        if (ret == 0) {
            LOG_INF("Quick-record started");
            zsw_recording_overlay_show();
        } else {
            LOG_ERR("Quick-record start failed: %d", ret);
        }
    }
}

static void long_press_timer_cb(struct k_timer *timer)
{
    ARG_UNUSED(timer);
    long_press_fired = true;
    k_work_submit(&quick_record_work);
}

static void quick_record_input_cb(struct input_event *evt, void *user_data)
{
    ARG_UNUSED(user_data);

    if (evt->type != INPUT_EV_KEY || evt->code != QUICK_RECORD_KEY_CODE) {
        return;
    }

    if (evt->value == 1) {
        button_pressed = true;
        long_press_fired = false;
        if (zsw_recording_manager_is_recording()) {
            /* Short press stops recording immediately */
            long_press_fired = true;
            k_work_submit(&quick_record_work);
        } else {
            k_timer_start(&long_press_timer, K_MSEC(LONG_PRESS_MS), K_NO_WAIT);
        }
    } else if (evt->value == 0) {
        button_pressed = false;
        if (!long_press_fired) {
            k_timer_stop(&long_press_timer);
        }
    }
}

INPUT_CALLBACK_DEFINE(NULL, quick_record_input_cb, NULL);

static int zsw_quick_record_sys_init(void)
{
    k_timer_init(&long_press_timer, long_press_timer_cb, NULL);
    k_work_init(&quick_record_work, quick_record_work_fn);
    LOG_INF("Quick-record shortcut enabled on button %d",
            CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON);
    return 0;
}

SYS_INIT(zsw_quick_record_sys_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif /* CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON > 0 */
