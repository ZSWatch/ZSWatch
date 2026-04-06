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

#include <zephyr/logging/log.h>
#include <zephyr/logging/log_backend.h>
#include <zephyr/logging/log_ctrl.h>

#include "zsw_fs_log_backend.h"

LOG_MODULE_REGISTER(zsw_fs_log_backend, CONFIG_ZSW_APP_LOG_LEVEL);

static bool first_enable;
static bool backend_active;

void zsw_fs_log_backend_set_enabled(bool enable)
{
    if (enable == backend_active) {
        return;
    }

    const struct log_backend *backend = log_backend_get_by_name("log_backend_fs");

    if (backend == NULL) {
        LOG_ERR("FS log backend not found");
        return;
    }

    if (enable) {
        if (!first_enable) {
            log_backend_enable(backend, NULL, CONFIG_LOG_MAX_LEVEL);
            first_enable = true;
        } else {
            log_backend_activate(backend, NULL);
        }
        backend_active = true;
    } else {
        if (first_enable) {
            log_backend_deactivate(backend);
        }
        backend_active = false;
    }
}
