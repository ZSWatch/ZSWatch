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
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <lvgl.h>

#include "production_test_runner.h"
#include "drivers/zsw_display_control.h"

LOG_MODULE_REGISTER(production_test, LOG_LEVEL_INF);

static void production_test_init_work(struct k_work *work);

K_WORK_DEFINE(init_work, production_test_init_work);

static void production_test_init_work(struct k_work *work)
{
    ARG_UNUSED(work);

    const char *target = IS_ENABLED(CONFIG_BOARD_NATIVE_SIM) ? "native_sim" : "hardware";
    LOG_INF("Production test starting on %s", target);

    zsw_display_control_init();
    zsw_display_control_sleep_ctrl(true);

    production_test_runner_init();
    production_test_runner_start();
}

int main(void)
{
    // Submit initialization work to system workqueue
    k_work_submit(&init_work);

    return 0;
}
