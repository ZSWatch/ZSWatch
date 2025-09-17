/* Copyright (c) 2025 Daniel Kampert
 * Author: Daniel Kampert <DanielKampert@kampis-elektroecke.de>
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

#define APDS9306_REGISTER_MAIN_CTRL       0x00
#define APDS9306_REGISTER_ALS_MEAS_RATE   0x04
#define APDS9306_REGISTER_ALS_GAIN        0x05
#define APDS9306_REGISTER_PART_ID         0x06
#define APDS9306_REGISTER_MAIN_STATUS     0x07
#define APDS9306_REGISTER_CLEAR_DATA_0    0x0A
#define APDS9306_REGISTER_CLEAR_DATA_1    0x0B
#define APDS9306_REGISTER_CLEAR_DATA_2    0x0C
#define APDS9306_REGISTER_ALS_DATA_0      0x0D
#define APDS9306_REGISTER_ALS_DATA_1      0x0E
#define APDS9306_REGISTER_ALS_DATA_2      0x0F
#define APDS9306_REGISTER_INT_CFG         0x19
#define APDS9306_REGISTER_INT_PERSISTENCE 0x1A
#define APDS9306_REGISTER_ALS_THRES_UP_0  0x21
#define APDS9306_REGISTER_ALS_THRES_UP_1  0x22
#define APDS9306_REGISTER_ALS_THRES_UP_2  0x23
#define APDS9306_REGISTER_ALS_THRES_LOW_0 0x24
#define APDS9306_REGISTER_ALS_THRES_LOW_1 0x25
#define APDS9306_REGISTER_ALS_THRES_LOW_2 0x26
#define APDS9306_REGISTER_ALS_THRES_VAR   0x27

#define ADPS9306_BIT_ALS_EN               BIT(0x01)
#define ADPS9306_BIT_ALS_DATA_STATUS      BIT(0x03)
#define APDS9306_BIT_SW_RESET             BIT(0x04)
#define ADPS9306_BIT_ALS_INTERRUPT_STATUS BIT(0x03)
#define APDS9306_BIT_POWER_ON_STATUS      BIT(0x05)

#define APDS_9306_065_CHIP_ID 0xB3

/* See datasheet for the values. Aligned with avago,apds9306.yaml */
static const uint8_t avago_apds9306_gain[] = {1, 3, 6, 9, 18};

/* These values represent the gain based on the integration time. */
/* A gain of 1 is used for a time of 3.125 ms (13 bits). */
/* This results in a gain of 8 (2^3) for a time if 25 ms (16 bits), etc. */
static const uint16_t avago_apds9306_integration_time_gain[] = {128, 64, 32, 16, 8, 1};

static const uint16_t avago_apds9306_measurement_rate[] = {25, 50, 100, 200, 500, 1000, 2000};

struct apds9306_emul_data {
    uint8_t regs[40];
    uint32_t measurement_rate;
    uint8_t current_register;
};

struct apds9306_emul_cfg {
};

struct apds9306_worker_item_t {
    struct k_work_delayable dwork;
    struct apds9306_emul_data *data;
} apds9306_emul_worker_item;

#define DT_DRV_COMPAT avago_apds9306

LOG_MODULE_REGISTER(avago_apds9306_emul, CONFIG_SENSOR_LOG_LEVEL);

static void apds9306_emul_worker(struct k_work *p_work)
{
    struct k_work_delayable *dwork = k_work_delayable_from_work(p_work);
    struct apds9306_worker_item_t *item =
        CONTAINER_OF(dwork, struct apds9306_worker_item_t, dwork);
    struct apds9306_emul_data *data = item->data;

    uint32_t raw;
    uint32_t lux;
    uint32_t factor = 16;
    uint32_t gain = avago_apds9306_gain[data->regs[APDS9306_REGISTER_ALS_GAIN] & 0x07];
    uint32_t integration_time = avago_apds9306_integration_time_gain[data->regs[APDS9306_REGISTER_ALS_MEAS_RATE] & 0x07];

    if (data->regs[APDS9306_REGISTER_PART_ID] == APDS_9306_065_CHIP_ID) {
        factor = 14;
    }

    /* Get a random LUX value in the range of 0 to 1000 */
    lux = sys_rand32_get() % 1000;

    /* Use the inverted formula from the driver to get the data value */
    raw = lux * gain * integration_time / factor;

    data->regs[APDS9306_REGISTER_ALS_DATA_0] = raw & 0xFF;
    data->regs[APDS9306_REGISTER_ALS_DATA_1] = (raw >> 8) & 0xFF;
    data->regs[APDS9306_REGISTER_ALS_DATA_2] = (raw >> 16) & 0xFF;

    LOG_INF("Gain: %u", gain);
    LOG_INF("Integration Time: %u", integration_time);
    LOG_INF("New simulated LUX value: %u", lux);
    LOG_INF("New simulated raw value: %u", raw);
    LOG_INF("   Data 0: 0x%02X", data->regs[APDS9306_REGISTER_ALS_DATA_0]);
    LOG_INF("   Data 1: 0x%02X", data->regs[APDS9306_REGISTER_ALS_DATA_1]);
    LOG_INF("   Data 2: 0x%02X", data->regs[APDS9306_REGISTER_ALS_DATA_2]);
}

static int apds9306_emul_reg_read(const struct emul *target, uint8_t reg, uint8_t *out, uint8_t length)
{
    struct apds9306_emul_data *data = target->data;

    /* Clear status bits after reading data registers */
    if (reg == APDS9306_REGISTER_ALS_DATA_2) {
        data->regs[APDS9306_REGISTER_ALS_DATA_2] &= ~0x08;
    }

    memcpy(out, data->regs + reg, length);

    LOG_DBG("Read register %u with length %u", reg, length);
    for (uint8_t i = 0; i < length; i++) {
        LOG_DBG(" Value[%u]: 0x%02X", i, out[i]);
    }

    return 0;
}

static int apds9306_emul_reg_write(const struct emul *target, uint8_t reg, uint8_t val)
{
    struct apds9306_emul_data *data = target->data;

    data->regs[reg] = val;

    LOG_DBG("Write register %u: 0x%02X", reg, data->regs[reg]);

    switch (reg)
    {
        case APDS9306_REGISTER_MAIN_CTRL:
        {
            if (data->regs[APDS9306_REGISTER_MAIN_CTRL] & ADPS9306_BIT_ALS_EN) {
                if (!k_work_delayable_is_pending(&apds9306_emul_worker_item.dwork)) {
                    LOG_DBG("Schedule new work. Delay: %u ms", avago_apds9306_measurement_rate[data->regs[APDS9306_REGISTER_ALS_MEAS_RATE] & 0x07]);

                    apds9306_emul_worker_item.data = data;
                    k_work_init_delayable(&apds9306_emul_worker_item.dwork, apds9306_emul_worker);
                    k_work_schedule(&apds9306_emul_worker_item.dwork, K_MSEC(avago_apds9306_measurement_rate[data->regs[APDS9306_REGISTER_ALS_MEAS_RATE] & 0x07] - 5));
                } else {
                    LOG_DBG("Work pending. Wait for completion.");
                }
            } else {
                LOG_DBG("ALS disabled. Cancel work.");
            }
            break;
        }
    };

    return 0;
}

static int apds9306_emul_transfer_i2c(const struct emul *target, struct i2c_msg *msgs, int num_msgs,
                    int addr)
{
    struct apds9306_emul_data *data = target->data;

    LOG_DBG("Number of messages: %u", num_msgs);
    for (int i = 0; i < num_msgs; i++) {
        struct i2c_msg *msg = msgs++;

        if (msg->flags & I2C_MSG_READ) {
            apds9306_emul_reg_read(target, data->current_register, msg->buf, msg->len);
        } else {
            if ((num_msgs == 1) || (msg->len == 1)) {
                data->current_register = msg->buf[0];

                if (num_msgs > 1) {
                    continue;
                }
            }

            apds9306_emul_reg_write(target, data->current_register, msg->buf[1]);
        }
    }

    return 0;
};

static int apds9306_emul_set_channel(const struct emul *target, struct sensor_chan_spec ch,
                       const q31_t *value, int8_t shift)
{
    struct apds9306_emul_data *data = target->data;

    if (!target || !target->data) {
        return -EINVAL;
    }

    switch (ch.chan_type) {
    case SENSOR_CHAN_LIGHT:
        break;
    default:
        return -ENOTSUP;
    }

    return 0;
}

static int apds9306_emul_set_attribute(const struct emul *target, struct sensor_chan_spec ch,
                 enum sensor_attribute attribute, const void *value)
{
    return 0;
}

static int apds9306_emul_init(const struct emul *target, const struct device *parent)
{
    struct apds9306_emul_data *data = target->data;

    data->regs[APDS9306_REGISTER_MAIN_CTRL] = 0;
    data->regs[APDS9306_REGISTER_ALS_MEAS_RATE] = 0x22;
    data->regs[APDS9306_REGISTER_ALS_GAIN] = 0x01;
    data->regs[APDS9306_REGISTER_PART_ID] = APDS_9306_065_CHIP_ID;
    data->regs[APDS9306_REGISTER_MAIN_STATUS] = 0x08;
    data->regs[APDS9306_REGISTER_ALS_DATA_0] = 0x01;
    data->regs[APDS9306_REGISTER_ALS_DATA_1] = 0x02;
    data->regs[APDS9306_REGISTER_ALS_DATA_2] = 0x03;

    LOG_INF("Initialization done");

    return 0;
}

static struct i2c_emul_api apds9306_emul_api_i2c = {
    .transfer = apds9306_emul_transfer_i2c,
};

static const struct emul_sensor_driver_api apds9306_emul_sensor_driver_api = {
    .set_channel = apds9306_emul_set_channel,
    .set_attribute = apds9306_emul_set_attribute,
};

#define APDS9306(inst)                                                                             \
    static struct apds9306_emul_data apds9306_emul_data_##inst = {};                            \
    static const struct apds9306_emul_cfg apds9306_emul_cfg_##inst = {};                        \
    EMUL_DT_INST_DEFINE(inst, apds9306_emul_init, &apds9306_emul_data_##inst, &apds9306_emul_cfg_##inst,      \
                &apds9306_emul_api_i2c, &apds9306_emul_sensor_driver_api);

DT_INST_FOREACH_STATUS_OKAY(APDS9306)
