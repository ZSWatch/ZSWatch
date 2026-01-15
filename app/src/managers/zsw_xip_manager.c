#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash/nrf_qspi_nor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zsw_xip_manager, LOG_LEVEL_INF);

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