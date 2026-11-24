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
#include "zsw_quick_record.h"

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

bool zsw_quick_record_handle_input(const struct input_event *evt)
{
    // The release of the press that stopped a recording must not reach the UI,
    // as the overlay is gone by then and it would open the app picker.
    static bool swallow_release;

    if (evt->type != INPUT_EV_KEY) {
        return false;
    }

    if (evt->code == INPUT_KEY_F1) {
        if (evt->value == 1) {
            k_work_submit(&quick_record_work);
        }
        return true;
    }

    if (evt->code == INPUT_KEY_1) {
        if (evt->value == 1 && zsw_recording_manager_is_recording()) {
            swallow_release = true;
            k_work_submit(&quick_record_work);
            return true;
        }
        if (evt->value == 0 && swallow_release) {
            swallow_release = false;
            return true;
        }
    }

    return false;
}

static int zsw_quick_record_sys_init(void)
{
    k_work_init(&quick_record_work, quick_record_work_fn);
    LOG_INF("Quick-record shortcut enabled on the top right button");
    return 0;
}

SYS_INIT(zsw_quick_record_sys_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
