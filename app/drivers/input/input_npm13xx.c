/*
 * This file is part of ZSWatch project <https://github.com/ZSWatch/ZSWatch/>.
 * Copyright (c) 2025 Jakob Krantz.
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

#define DT_DRV_COMPAT nordic_npm13xx_input

#include <zephyr/device.h>
#include <zephyr/drivers/mfd/npm13xx.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(npm13xx_input, CONFIG_INPUT_LOG_LEVEL);

struct npm13xx_input_config {
    const struct device *mfd_dev;
    uint32_t zephyr_code;
};

struct npm13xx_input_data {
    const struct device *input_dev;
    struct gpio_callback cb_data;
    bool button_pressed;
};

static void npm13xx_input_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    struct npm13xx_input_data *data = CONTAINER_OF(cb, struct npm13xx_input_data, cb_data);
    const struct npm13xx_input_config *cfg = data->input_dev->config;

    if (pins & BIT(NPM13XX_EVENT_SHIPHOLD_PRESS)) {
        LOG_DBG("NPM1300 button pressed");
        data->button_pressed = true;
        input_report_key(data->input_dev, cfg->zephyr_code, 1, true, K_FOREVER);
    }

    if (pins & BIT(NPM13XX_EVENT_SHIPHOLD_RELEASE)) {
        LOG_DBG("NPM1300 button released");
        data->button_pressed = false;
        input_report_key(data->input_dev, cfg->zephyr_code, 0, true, K_FOREVER);
    }
}

static int npm13xx_input_init(const struct device *dev)
{
    const struct npm13xx_input_config *cfg = dev->config;
    struct npm13xx_input_data *data = dev->data;

    LOG_INF("NPM1300 input driver initializing for device: %s", dev->name);

    data->input_dev = dev;

    if (!device_is_ready(cfg->mfd_dev)) {
        LOG_ERR("NPM1300 MFD device not ready");
        return -ENODEV;
    }

    // For some reason, probably a bug in nPM1300 driver, to get INT from SHIPHOLD, we also need to trigger an INT from some other event.
    gpio_init_callback(&data->cb_data, npm13xx_input_callback,
                       BIT(NPM13XX_EVENT_SHIPHOLD_PRESS) | BIT(NPM13XX_EVENT_SHIPHOLD_RELEASE) | BIT(NPM13XX_EVENT_CHG_COMPLETED) | BIT(
                           NPM13XX_EVENT_VBUS_DETECTED) | BIT(NPM13XX_EVENT_VBUS_REMOVED));

    int ret = mfd_npm13xx_add_callback(cfg->mfd_dev, &data->cb_data);
    if (ret < 0) {
        LOG_ERR("Failed to add NPM1300 callback: %d", ret);
        return ret;
    }

    LOG_INF("NPM1300 input driver initialized");
    return 0;
}

#define NPM13XX_INPUT_INIT(inst)                                                                       \
    static const struct npm13xx_input_config npm13xx_input_config_##inst = {                      \
        .mfd_dev = DEVICE_DT_GET(DT_INST_PHANDLE(inst, mfd_device)),                          \
        .zephyr_code = DT_INST_PROP(inst, zephyr_code),                                        \
    };                                                                                             \
                                                                                                   \
    static struct npm13xx_input_data npm13xx_input_data_##inst;                                   \
                                                                                                   \
    DEVICE_DT_INST_DEFINE(inst, npm13xx_input_init, NULL,                                         \
                  &npm13xx_input_data_##inst, &npm13xx_input_config_##inst,               \
                  POST_KERNEL, CONFIG_INPUT_INIT_PRIORITY, NULL);

DT_INST_FOREACH_STATUS_OKAY(NPM13XX_INPUT_INIT)
