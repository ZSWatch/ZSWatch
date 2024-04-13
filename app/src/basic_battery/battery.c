/*
 * Copyright (c) 2018-2019 Peter Bigot Consulting, LLC
 * Copyright (c) 2019-2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "events/battery_event.h"

LOG_MODULE_REGISTER(BATTERY, LOG_LEVEL_WRN);

#define VBATT DT_PATH(vbatt)

#define BATTERY_SAMPLE_INTETRVAL_MINUTES    5

struct battery_level_point {
    /** Remaining life at #lvl_mV. */
    uint16_t lvl_pptt;

    /** Battery voltage at #lvl_pptt remaining life. */
    uint16_t lvl_mV;
};

static void handle_battery_sample_timeout(struct k_work *item);

K_WORK_DELAYABLE_DEFINE(battery_sample_work, handle_battery_sample_timeout);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);

#if DT_IO_CHANNELS_INPUT(VBATT)
/* This board uses a divider that reduces max voltage to
 * reference voltage (600 mV).
 */
#define BATTERY_ADC_GAIN ADC_GAIN_1

struct io_channel_config {
    uint8_t channel;
};

struct divider_config {
    struct io_channel_config io_channel;
    struct gpio_dt_spec power_gpios;
    /* output_ohm is used as a flag value: if it is nonzero then
     * the battery is measured through a voltage divider;
     * otherwise it is assumed to be directly connected to Vdd.
     */
    uint32_t output_ohm;
    uint32_t full_ohm;
};

static const struct divider_config divider_config = {
    .io_channel = {
        DT_IO_CHANNELS_INPUT(VBATT),
    },
    .power_gpios = GPIO_DT_SPEC_GET_OR(VBATT, power_gpios, {}),
    .output_ohm = DT_PROP(VBATT, output_ohms),
    .full_ohm = DT_PROP(VBATT, full_ohms),
};

struct divider_data {
    const struct device *adc;
    struct adc_channel_cfg adc_cfg;
    struct adc_sequence adc_seq;
    int16_t raw;
};

static struct divider_data divider_data = {
    .adc = DEVICE_DT_GET_OR_NULL(DT_IO_CHANNELS_CTLR(VBATT)),
};

static bool battery_ok;

static int divider_setup(void)
{
    const struct divider_config *cfg = &divider_config;
    const struct io_channel_config *iocp = &cfg->io_channel;
    const struct gpio_dt_spec *gcp = &cfg->power_gpios;
    struct divider_data *ddp = &divider_data;
    struct adc_sequence *asp = &ddp->adc_seq;
    struct adc_channel_cfg *accp = &ddp->adc_cfg;
    int rc;

    if (!device_is_ready(ddp->adc)) {
        LOG_ERR("ADC device is not ready %s", ddp->adc->name);
        return -ENOENT;
    }

    if (gcp->port) {
        if (!device_is_ready(gcp->port)) {
            LOG_ERR("%s: device not ready", gcp->port->name);
            return -ENOENT;
        }
        rc = gpio_pin_configure_dt(gcp, GPIO_OUTPUT_INACTIVE);
        if (rc != 0) {
            LOG_ERR("Failed to control feed %s.%u: %d",
                    gcp->port->name, gcp->pin, rc);
            return rc;
        }
    }

    *asp = (struct adc_sequence) {
        .channels = BIT(0),
        .buffer = &ddp->raw,
        .buffer_size = sizeof(ddp->raw),
        .oversampling = 4,
        .calibrate = true,
    };

#ifdef CONFIG_ADC_NRFX_SAADC
    *accp = (struct adc_channel_cfg) {
        .gain = BATTERY_ADC_GAIN,
        .reference = ADC_REF_INTERNAL,
        .acquisition_time = ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 40),
    };

    if (cfg->output_ohm != 0) {
        accp->input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0
                               + iocp->channel;
    } else {
        accp->input_positive = SAADC_CH_PSELP_PSELP_VDD;
    }

    asp->resolution = 14;
#else /* CONFIG_ADC_var */
#error Unsupported ADC
#endif /* CONFIG_ADC_var */

    rc = adc_channel_setup(ddp->adc, accp);
    LOG_INF("Setup AIN%u got %d", iocp->channel, rc);

    return rc;
}

static int battery_setup(void)
{
    int rc = divider_setup();

    battery_ok = (rc == 0);
    LOG_INF("Battery setup: %d %d", rc, battery_ok);
    return rc;
}

static int battery_measure_enable(bool enable)
{
    int rc = -ENOENT;

    if (battery_ok) {
        const struct gpio_dt_spec *gcp = &divider_config.power_gpios;

        rc = 0;
        if (gcp->port) {
            rc = gpio_pin_set_dt(gcp, enable);
        }
    }
    return rc;
}

static int battery_sample(void)
{
    int rc = -ENOENT;

    if (battery_ok) {
        struct divider_data *ddp = &divider_data;
        const struct divider_config *dcp = &divider_config;
        struct adc_sequence *sp = &ddp->adc_seq;

        rc = adc_read(ddp->adc, sp);
        sp->calibrate = true;
        if (rc == 0) {
            int32_t val = ddp->raw;

            adc_raw_to_millivolts(adc_ref_internal(ddp->adc),
                                  ddp->adc_cfg.gain,
                                  sp->resolution,
                                  &val);

            if (dcp->output_ohm != 0) {
                rc = val * (uint64_t)dcp->full_ohm
                     / dcp->output_ohm;
                LOG_INF("raw %u ~ %u mV => %d mV\n",
                        ddp->raw, val, rc);
            } else {
                rc = val;
                LOG_INF("raw %u ~ %u mV\n", ddp->raw, val);
            }
        }
    }

    return rc;
}

static unsigned int battery_level_pptt(unsigned int batt_mV,
                                       const struct battery_level_point *curve)
{
    const struct battery_level_point *pb = curve;

    if (batt_mV >= pb->lvl_mV) {
        /* Measured voltage above highest point, cap at maximum. */
        return pb->lvl_pptt;
    }
    /* Go down to the last point at or below the measured voltage. */
    while ((pb->lvl_pptt > 0)
           && (batt_mV < pb->lvl_mV)) {
        ++pb;
    }
    if (batt_mV < pb->lvl_mV) {
        /* Below lowest point, cap at minimum */
        return pb->lvl_pptt;
    }

    /* Linear interpolation between below and above points. */
    const struct battery_level_point *pa = pb - 1;

    return pb->lvl_pptt
           + ((pa->lvl_pptt - pb->lvl_pptt)
              * (batt_mV - pb->lvl_mV)
              / (pa->lvl_mV - pb->lvl_mV));
}

SYS_INIT(battery_setup, APPLICATION, CONFIG_ZSW_DRIVER_INIT_PRIORITY);

#else

static int battery_measure_enable(bool enable)
{
    return 0;
}

static int battery_sample(void)
{
    return 4000;
}

static unsigned int battery_level_pptt(unsigned int batt_mV, const struct battery_level_point *curve)
{
    return 10000;
}

#endif // VBATT

/** A discharge curve specific to the power source. */
static const struct battery_level_point levels[] = {
    /*
    Battery supervisor cuts power at 3500mA so treat that as 0%
    This is very basic and the percentage will not be exact.
    */
    { 10000, 4150 },
    { 0, 3500 },
};

static int get_battery_status(int *mV, int *percent)
{
    unsigned int batt_pptt;
    int rc = battery_measure_enable(true);
    if (rc != 0) {
        LOG_ERR("Failed initialize battery measurement: %d\n", rc);
        return -1;
    }
    // From https://github.com/zephyrproject-rtos/zephyr/blob/main/samples/boards/nrf/battery/src/main.c
    *mV = battery_sample();

    if (*mV < 0) {
        LOG_ERR("Failed to read battery voltage: %d\n", *mV);
        return -1;
    }

    batt_pptt = battery_level_pptt(*mV, levels);

    LOG_DBG("%d mV; %u pptt\n", *mV, batt_pptt);
    *percent = batt_pptt / 100;

    rc = battery_measure_enable(false);
    if (rc != 0) {
        LOG_ERR("Failed disable battery measurement: %d\n", rc);
        return -1;
    }
    return 0;
}

static void handle_battery_sample_timeout(struct k_work *item)
{
    int rc;
    struct battery_sample_event evt;

    rc = get_battery_status(&evt.mV, &evt.percent);
    if (rc == 0) {
        zbus_chan_pub(&battery_sample_data_chan, &evt, K_MSEC(5));
    }
    k_work_schedule(&battery_sample_work, K_MINUTES(BATTERY_SAMPLE_INTETRVAL_MINUTES));
}

static int battery_init(void)
{
    k_work_schedule(&battery_sample_work, K_SECONDS(1));
    return 0;
}

SYS_INIT(battery_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
