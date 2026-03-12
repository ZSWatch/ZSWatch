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
#include <zephyr/device.h>
#include <zephyr/drivers/flash/nrf_qspi_nor.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

static const struct device *qspi_dev = DEVICE_DT_GET_OR_NULL(DT_CHOSEN(nordic_pm_ext_flash));

LOG_MODULE_REGISTER(zsw_xip_manager, CONFIG_ZSW_XIP_MANAGER_LOG_LEVEL);

static int zsw_xip_manager_init(void)
{
    if (!qspi_dev) {
        LOG_WRN("No nordic_pm_ext_flash chosen");
        return 0;
    }

    if (!device_is_ready(qspi_dev)) {
        LOG_ERR("QSPI device not ready");
        return -ENODEV;
    }

    /*
     * The QSPI driver enables XIP during init and keeps one internal user
     * reference. Drop that startup reference here so every later enable/disable
     * in application code maps 1:1 to the driver's own refcount.
     */
    nrf_qspi_nor_xip_enable(qspi_dev, false);
    LOG_INF("Released startup XIP reference from QSPI init");

    return 0;
}

SYS_INIT(zsw_xip_manager_init, APPLICATION, 90);

int _zsw_xip_enable(const char *requester)
{
    LOG_WRN("XIP enable request from: %s", requester);

    if (!qspi_dev) {
        LOG_WRN("XIP enable request ignored: no nordic_pm_ext_flash chosen");
        return -ENODEV;
    }

    if (!device_is_ready(qspi_dev)) {
        LOG_ERR("QSPI device not ready");
        return -ENODEV;
    }

    nrf_qspi_nor_xip_enable(qspi_dev, true);
    return 0;
}

int _zsw_xip_disable(const char *requester)
{
    LOG_WRN("XIP disable request from: %s", requester);

    if (!qspi_dev) {
        return 0;
    }

    if (!device_is_ready(qspi_dev)) {
        LOG_ERR("QSPI device not ready");
        return -ENODEV;
    }

    nrf_qspi_nor_xip_enable(qspi_dev, false);
    return 0;
}
