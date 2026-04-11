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

#ifdef CONFIG_MAX32664C_USE_FIRMWARE_LOADER
#define FW_VERSION_MAJOR 30
#define FW_VERSION_MINOR 13
#define FW_VERSION_PATCH 31
#include "ext_drivers/firmware/MAX32664C_HSP2_WHRM_AEC_SCD_WSPO2_C_30_13_31.h"

static const struct device *const sensor_hub = DEVICE_DT_GET_OR_NULL(DT_ALIAS(hr_hub));
#endif

static void setup_hr_firmware(void)
{
#ifdef CONFIG_MAX32664C_USE_FIRMWARE_LOADER

    uint8_t major, minor, patch;
    int err = max32664c_read_fw_version(sensor_hub, &major, &minor, &patch);
    if (err || major != FW_VERSION_MAJOR || minor != FW_VERSION_MINOR || patch != FW_VERSION_PATCH) {
        if (err) {
            LOG_ERR("Failed to read firmware version");
        }
        LOG_DBG("Updating firmware to version %u.%u.%u", FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH);
        max32664c_bl_enter(sensor_hub, MAX32664C_HSP2_WHRM_AEC_SCD_WSPO2_C_30_13_31, sizeof(MAX32664C_HSP2_WHRM_AEC_SCD_WSPO2_C_30_13_31));
        max32664c_bl_leave(sensor_hub);

        return 0;
    } else {
        LOG_INF("Firmware up to date: %u.%u.%u", major, minor, patch);
    }
#endif /* CONFIG_MAX32664C_USE_FIRMWARE_LOADER */
}

static void production_test_init_work(struct k_work *work)
{
    ARG_UNUSED(work);

    const char *target = IS_ENABLED(CONFIG_BOARD_NATIVE_SIM) ? "native_sim" : "hardware";
    LOG_INF("Production test starting on %s", target);

    zsw_display_control_init();
    zsw_display_control_sleep_ctrl(true);
    zsw_display_control_set_brightness(100);  // Max brightness for production test

    setup_hr_firmware();

    production_test_runner_init();
    production_test_runner_start();
}

int main(void)
{
    // Submit initialization work to system workqueue
    k_work_submit(&init_work);

    return 0;
}
