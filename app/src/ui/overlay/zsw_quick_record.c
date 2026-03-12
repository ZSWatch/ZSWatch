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

#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>

#include "managers/zsw_recording_manager.h"
#include "zsw_recording_overlay.h"

LOG_MODULE_REGISTER(zsw_quick_record, CONFIG_ZSW_VOICE_MEMO_LOG_LEVEL);

static struct k_work quick_record_work;

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

static void quick_record_input_cb(struct input_event *evt, void *user_data)
{
    ARG_UNUSED(user_data);

    if (evt->type != INPUT_EV_KEY || evt->value != 1) {
        return;
    }

    if (evt->code == INPUT_KEY_F1) {
        /* Long press detected by DTS longpress node: toggle recording */
        k_work_submit(&quick_record_work);
    } else if (evt->code == INPUT_KEY_4 && zsw_recording_manager_is_recording()) {
        /* Short press while recording: stop */
        k_work_submit(&quick_record_work);
    }
}

INPUT_CALLBACK_DEFINE(NULL, quick_record_input_cb, NULL);

static int zsw_quick_record_sys_init(void)
{
    k_work_init(&quick_record_work, quick_record_work_fn);
    LOG_INF("Quick-record shortcut enabled on button 4");
    return 0;
}

SYS_INIT(zsw_quick_record_sys_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
