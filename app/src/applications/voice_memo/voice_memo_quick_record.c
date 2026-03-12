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
 * Works from watchface or display-off state.
 */

#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>

#include "ui/zsw_ui_controller.h"

LOG_MODULE_REGISTER(voice_memo_qr, CONFIG_ZSW_VOICE_MEMO_LOG_LEVEL);

#if CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON > 0

/* External voice memo app functions */
extern int voice_memo_shell_start(void);
extern int voice_memo_shell_stop(void);
extern bool voice_memo_store_is_recording(void);
extern void voice_memo_set_quick_record_exit(void);

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

    if (voice_memo_store_is_recording()) {
        int ret = voice_memo_shell_stop();
        if (ret == 0) {
            LOG_INF("voice_memo: quick-record stopped");
        } else {
            LOG_ERR("Quick-record stop failed: %d", ret);
        }
    } else {
        int ret = voice_memo_shell_start();
        if (ret == 0) {
            LOG_INF("voice_memo: quick-record started");
            /* Auto-close the app when this quick-record session ends. */
            voice_memo_set_quick_record_exit();
            zsw_ui_controller_launch_app("Voice Memo");
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
        /* Button pressed */
        button_pressed = true;
        long_press_fired = false;
        k_timer_start(&long_press_timer, K_MSEC(LONG_PRESS_MS), K_NO_WAIT);
    } else if (evt->value == 0) {
        /* Button released */
        button_pressed = false;
        if (!long_press_fired) {
            /* Short press — cancel timer, let normal handler process it */
            k_timer_stop(&long_press_timer);
        }
    }
}

INPUT_CALLBACK_DEFINE(NULL, quick_record_input_cb, NULL);

static int voice_memo_quick_record_init(void)
{
    k_timer_init(&long_press_timer, long_press_timer_cb, NULL);
    k_work_init(&quick_record_work, quick_record_work_fn);
    LOG_INF("Quick-record shortcut enabled on button %d",
            CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON);
    return 0;
}

SYS_INIT(voice_memo_quick_record_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif /* CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON > 0 */
