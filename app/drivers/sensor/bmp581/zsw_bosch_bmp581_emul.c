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

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>
#include <zephyr/random/random.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/drivers/i2c_emul.h>

#include "bmp5.h"

#define BMP5_CHIP_ID_VALUE                        0x50

#define DT_DRV_COMPAT                             zswatch_bmp581

struct bmp581_emul_data {
    uint8_t regs[BMP5_REG_CMD + 1];
    uint8_t current_register;
};

struct bmp581_emul_cfg {
};

struct bmp581_worker_item_t {
    struct k_work_delayable dwork;
    struct bmp581_emul_data *data;
} bmp581_emul_worker_item;

LOG_MODULE_REGISTER(zsw_bosch_bmp581_emul, CONFIG_SENSOR_LOG_LEVEL);

static uint32_t bmp581_odr_to_ms(uint8_t odr_config)
{
    uint8_t odr = (odr_config >> 2) & 0x03;

    switch (odr)
    {
        case 0: return 1000.0/240;          /* 240 Hz*/
        case 1: return 1000.0/218.537;      /* 218.537 Hz */
        case 2: return 1000.0/199.111;      /* 199.111 Hz */
        case 3: return 1000.0/218.537;      /* 218.537 Hz */
        case 4: return 1000.0/240;          /* 240 Hz*/
        case 5: return 1000.0/218.537;      /* 218.537 Hz */
        case 6: return 1000.0/240;          /* 240 Hz*/
        case 7: return 1000.0/218.537;      /* 218.537 Hz */
        case 8: return 1000.0/240;          /* 240 Hz*/
        case 9: return 1000.0/218.537;      /* 218.537 Hz */
        case 0x0A: return 1000.0/100.299;   /* 100.299 Hz*/
        case 0x0B: return 1000.0/89.6;      /* 89.6 Hz */
        case 0x0C: return 1000.0/80;        /* 80 Hz*/
        case 0x0D: return 1000.0/70;        /* 70 Hz */
        case 0x0E: return 1000.0/60;        /* 60 Hz*/
        case 0x0F: return 1000.0/50.056;    /* 50.056 Hz */
        case 0x10: return 1000.0/45.025;    /* 45.025 Hz */
        case 0x11: return 1000.0/40;        /* 40 Hz */
        case 0x12: return 1000.0/35;        /* 35 Hz*/
        case 0x13: return 1000.0/30;        /* 30 Hz */
        case 0x14: return 1000.0/25;        /* 25 Hz*/
        case 0x15: return 1000.0/20;        /* 20 Hz */
        case 0x16: return 1000.0/15;        /* 15 Hz */
        case 0x17: return 1000.0/10;        /* 10 Hz */
        case 0x18: return 1000.0/5;         /* 5 Hz */
        case 0x19: return 1000.0/4;         /* 4 Hz */
        case 0x1A: return 1000.0/3;         /* 3 Hz */
        case 0x1B: return 1000.0/2;         /* 2 Hz */
        case 0x1C: return 1000.0/1;         /* 1 Hz */
        case 0x1D: return 1000.0/0.5;       /* 0.5 Hz */
        case 0x1E: return 1000.0/0.250;     /* 0.250 Hz*/
        case 0x1F: return 1000.0/0.125;     /* 0.125 Hz */
        default: return 1000.0/1;           /* 1 Hz */
    }
}

static void bmp581_emul_worker(struct k_work *p_work)
{
    int32_t temperature;
    int32_t pressure;
    struct k_work_delayable *dwork = k_work_delayable_from_work(p_work);
    struct bmp581_worker_item_t *item =
        CONTAINER_OF(dwork, struct bmp581_worker_item_t, dwork);
    struct bmp581_emul_data *data = item->data;

    /* Get a random pressure value in the range of 800 to 1100 */
    pressure = 800 + (sys_rand32_get() % 300);

    /* Get a random temperature value in the range of -20 to +20 */
    temperature = -20 + (sys_rand32_get() % 40);

    LOG_INF("New simulated temperature value: %i", temperature);
    LOG_INF("New simulated pressure value: %u", pressure);

    temperature = temperature * 65536;
    data->regs[BMP5_REG_TEMP_DATA_XLSB] = (temperature & 0xFF);
    data->regs[BMP5_REG_TEMP_DATA_LSB] = ((temperature >> 8) & 0xFF);
    data->regs[BMP5_REG_TEMP_DATA_MSB] = ((temperature >> 16) & 0xFF);

    pressure = pressure * 64;
    data->regs[BMP5_REG_PRESS_DATA_XLSB] = (pressure & 0xFF);
    data->regs[BMP5_REG_PRESS_DATA_LSB] = ((pressure >> 8) & 0xFF);
    data->regs[BMP5_REG_PRESS_DATA_MSB] = ((pressure >> 16) & 0xFF);

    uint8_t pwr_mode = data->regs[BMP5_REG_ODR_CONFIG] & 0x03;
    if (pwr_mode == 2) {
        LOG_DBG("Schedule new work. Delay: %u ms", bmp581_odr_to_ms(data->regs[BMP5_REG_ODR_CONFIG]));
        k_work_schedule(&bmp581_emul_worker_item.dwork, K_MSEC(bmp581_odr_to_ms(data->regs[BMP5_REG_ODR_CONFIG])));
    }
}

static int bmp581_emul_reg_read(const struct emul *target, uint8_t reg, uint8_t *out, uint8_t length)
{
    struct bmp581_emul_data *data = target->data;

    memcpy(out, data->regs + reg, length);

    LOG_DBG("Read register 0x%02X with length %u", reg, length);
    for (uint8_t i = 0; i < length; i++) {
        LOG_DBG(" Value[%u]: 0x%02X", i, out[i]);
    }

    return 0;
}

static int bmp581_emul_reg_write(const struct emul *target, uint8_t reg, uint8_t* in, uint8_t length)
{
    struct bmp581_emul_data *data = target->data;

    for (uint8_t i = 0; i < length; i++) {
        data->regs[reg + i] = in[i];
        LOG_DBG("Write register 0x%02X: 0x%02X", reg + i, data->regs[reg + i]);
    }

    switch (reg)
    {
        case BMP5_REG_ODR_CONFIG:
        {
            uint8_t pwr_mode = data->regs[BMP5_REG_ODR_CONFIG] & 0x03;

            switch (pwr_mode)
            {
                /* Sleep mode */
                case BMP5_POWERMODE_STANDBY:
                {
                    LOG_INF("Sleep mode");
                    k_work_cancel_delayable(&bmp581_emul_worker_item.dwork);
                    break;
                }
                /* Normal mode */
                case BMP5_POWERMODE_NORMAL:
                {
                    LOG_INF("Normal mode");

                    if (!k_work_delayable_is_pending(&bmp581_emul_worker_item.dwork)) {
                        LOG_DBG("Schedule new work. Delay: %u ms", bmp581_odr_to_ms(data->regs[BMP5_REG_ODR_CONFIG]));

                        bmp581_emul_worker_item.data = data;
                        k_work_init_delayable(&bmp581_emul_worker_item.dwork, bmp581_emul_worker);
                        k_work_schedule(&bmp581_emul_worker_item.dwork, K_MSEC(bmp581_odr_to_ms(data->regs[BMP5_REG_ODR_CONFIG])));
                    }

                    break;
                }
                /* Forced mode */
                case BMP5_POWERMODE_FORCED:
                {
                    LOG_INF("Forced mode");
                    if (!k_work_delayable_is_pending(&bmp581_emul_worker_item.dwork)) {
                        LOG_DBG("Schedule new work. Delay: %u ms", bmp581_odr_to_ms(data->regs[BMP5_REG_ODR_CONFIG]));

                        bmp581_emul_worker_item.data = data;
                        k_work_init_delayable(&bmp581_emul_worker_item.dwork, bmp581_emul_worker);
                        k_work_schedule(&bmp581_emul_worker_item.dwork, K_MSEC(bmp581_odr_to_ms(data->regs[BMP5_REG_ODR_CONFIG])));
                    }

                    break;
                }
            }

            break;
        }
    };

    return 0;
}

static int bmp581_emul_transfer_i2c(const struct emul *target, struct i2c_msg *msgs, int num_msgs,
                    int addr)
{
    struct bmp581_emul_data *data = target->data;

    LOG_DBG("Number of messages: %u", num_msgs);
    for (int i = 0; i < num_msgs; i++) {
        struct i2c_msg *msg = msgs++;

        if (!(msg->flags & I2C_MSG_READ)) {
            LOG_DBG(" Message length %u", msg->len);
            for(uint8_t i = 0; i < msg->len; i++) {
                LOG_DBG(" Byte %u: 0x%02X", i, msg->buf[i]);
            }
        }

        if (msg->flags & I2C_MSG_READ) {
            bmp581_emul_reg_read(target, data->current_register, msg->buf, msg->len);
        } else {
            if (i == 0) {
                data->current_register = msg->buf[0];
                LOG_DBG("Set current register to 0x%02X", msg->buf[0]);
                continue;
            }

            bmp581_emul_reg_write(target, data->current_register, &msg->buf[0], msg->len);
        }
    }

    return 0;
};

static int bmp581_emul_set_channel(const struct emul *target, struct sensor_chan_spec ch,
                       const q31_t *value, int8_t shift)
{
    struct bmp581_emul_data *data = target->data;

    if (!target || !target->data) {
        return -EINVAL;
    }

    switch (ch.chan_type) {
    case SENSOR_CHAN_AMBIENT_TEMP:
        break;
    case SENSOR_CHAN_PRESS:
        break;
    default:
        return -ENOTSUP;
    }

    return 0;
}

static int bmp581_emul_set_attribute(const struct emul *target, struct sensor_chan_spec ch,
                 enum sensor_attribute attribute, const void *value)
{
    return 0;
}

static int bmp581_emul_init(const struct emul *target, const struct device *parent)
{
    struct bmp581_emul_data *data = target->data;

    /* Apply the reset values from the datasheet */
    data->regs[BMP5_REG_CHIP_ID] = BMP5_CHIP_ID_VALUE;
    data->regs[BMP5_REG_REV_ID] = 0x32;
    data->regs[BMP5_REG_INT_STATUS] = BMP5_INT_ASSERTED_POR_SOFTRESET_COMPLETE;
    data->regs[BMP5_REG_ODR_CONFIG] = 0x70;

    LOG_INF("Initialization done");

    return 0;
}

static struct i2c_emul_api bmp581_emul_api_i2c = {
    .transfer = bmp581_emul_transfer_i2c,
};

static const struct emul_sensor_driver_api bmp581_emul_sensor_driver_api = {
    .set_channel = bmp581_emul_set_channel,
    .set_attribute = bmp581_emul_set_attribute,
};

#define BMP581(inst)                                                                             \
    static struct bmp581_emul_data bmp581_emul_data_##inst = {};                            \
    static const struct bmp581_emul_cfg bmp581_emul_cfg_##inst = {};                        \
    EMUL_DT_INST_DEFINE(inst, bmp581_emul_init, &bmp581_emul_data_##inst, &bmp581_emul_cfg_##inst,      \
                &bmp581_emul_api_i2c, &bmp581_emul_sensor_driver_api);

DT_INST_FOREACH_STATUS_OKAY(BMP581)
