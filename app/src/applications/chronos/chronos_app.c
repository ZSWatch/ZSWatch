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

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "chronos_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(chronos_app, LOG_LEVEL_INF);

// Functions needed for all applications
static void chronos_app_start(lv_obj_t *root, lv_group_t *group);
static void chronos_app_stop(void);

ZSW_LV_IMG_DECLARE(chronos_logo_icon);

static application_t app = {
    .name = "Chronos",
    .icon = ZSW_LV_IMG_USE(chronos_logo_icon),
    .start_func = chronos_app_start,
    .stop_func = chronos_app_stop,
    .category = ZSW_APP_CATEGORY_RANDOM,
};

static void chronos_app_start(lv_obj_t *root, lv_group_t *group)
{
    chronos_ui_init(root);
}

static void chronos_app_stop(void)
{
    chronos_ui_deinit();
}

static int chronos_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(chronos_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
