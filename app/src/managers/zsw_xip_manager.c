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
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zsw_xip_manager, LOG_LEVEL_DBG);

static const struct device *qspi_dev = DEVICE_DT_GET_OR_NULL(DT_CHOSEN(nordic_pm_ext_flash));

static bool first_xip_disable_done = false;

int _zsw_xip_enable(const char *requester)
{
    LOG_WRN("XIP ENABLE(1) request from: %s", requester);

    if (!qspi_dev) {
        return 0;
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
    LOG_WRN("XIP DISABLE(0) request from: %s", requester);

    if (!qspi_dev) {
        return 0;
    }

    if (!device_is_ready(qspi_dev)) {
        LOG_ERR("QSPI device not ready");
        return -ENODEV;
    }

    // On first disable we need to disable XIP twice, since the QSPI driver
    // will have enabled XIP internally during initialization so we need to ref count down one extra.
    if (!first_xip_disable_done) {
        nrf_qspi_nor_xip_enable(qspi_dev, false);
        first_xip_disable_done = true;
    }

    nrf_qspi_nor_xip_enable(qspi_dev, false);
    return 0;
}
