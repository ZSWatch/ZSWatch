/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/mfd/npm1300.h>
#include <zephyr/drivers/sensor/npm1300_charger.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <events/activity_event.h>
#include <events/zsw_periodic_event.h>
#include <events/battery_event.h>
#include "nrf_fuel_gauge.h"
#include "zsw_pmic.h"

LOG_MODULE_REGISTER(zsw_pmic, LOG_LEVEL_DBG);

#define ZSW_NOT_WORN_STATIONARY_CURRENT 0.0005 // 50uA. TODO remeasure this value and make Kconfig

typedef enum {
    CHG_STATUS_BATTERYDETECTED = 1, // Battery is connected
    CHG_STATUS_COMPLETED = 2, // Charging completed (Battery Full)
    CHG_STATUS_TRICKLECHARGE = 4, // Trickle charge
    CHG_STATUS_CONSTANTCURRENT = 8, // Constant Current charging
    CHG_STATUS_CONSTANTVOLTAGE = 16, // Constant Voltage charging
    CHG_STATUS_RECHARGE = 32, // Battery re-charge is needed
    CHG_STATUS_DIETEMPHIGHCHGPAUSED = 64, // Charging stopped due Die Temp high.
    CHG_STATUS_SUPPLEMENTACTIVE = 128, // Supplement Mode Active
} npm1300_chg_status_t;

static void zbus_activity_event_callback(const struct zbus_channel *chan);
static void zbus_periodic_slow_10s_callback(const struct zbus_channel *chan);
static int read_sensors(const struct device *charger, float *voltage, float *current, float *temp, int *status,
                        int *error);

ZBUS_CHAN_DECLARE(activity_state_data_chan);
ZBUS_LISTENER_DEFINE(pmic_activity_state_event_lis, zbus_activity_event_callback);
ZBUS_CHAN_ADD_OBS(activity_state_data_chan, pmic_activity_state_event_lis, 1);

ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_LISTENER_DEFINE(zsw_pmic_slow_lis, zbus_periodic_slow_10s_callback);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);

static const struct device *pmic = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_pmic));
static const struct device *charger = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_charger));

static float max_charge_current;
static float term_charge_current;
static int64_t ref_time;

static const struct battery_model battery_model = {
#include "battery_model.inc"
};

static void zbus_activity_event_callback(const struct zbus_channel *chan)
{
    int ret;
    float voltage;
    float current;
    float temp;
    int status;
    int error;

    ret = read_sensors(charger, &voltage, &current, &temp, &status, &error);
    if (ret != 0) {
        LOG_ERR("Error: Could not read from charger device\n");
        return;
    }
    const struct activity_state_event *event = zbus_chan_const_msg(chan);
    switch (event->state) {
        case ZSW_ACTIVITY_STATE_NOT_WORN_STATIONARY:
            // TODO must be called in same contect as nrf_fuel_gauge_process
            //nrf_fuel_gauge_idle_set(voltage, temp, ZSW_NOT_WORN_STATIONARY_CURRENT);
            break;
        case ZSW_ACTIVITY_STATE_ACTIVE:
        case ZSW_ACTIVITY_STATE_INACTIVE:
            // TODO resume
            break;
    }
}

static bool is_charging_from_status(int status)
{
    return (status & CHG_STATUS_CONSTANTCURRENT) || (status & CHG_STATUS_CONSTANTVOLTAGE) ||
           (status & CHG_STATUS_TRICKLECHARGE);
}

static void zbus_periodic_slow_10s_callback(const struct zbus_channel *chan)
{
    int ret;
    struct battery_sample_event evt;

    ret = zsw_pmic_get_full_state(&evt);
    if (ret < 0) {
        LOG_ERR("Error: Could not read from charger device\n");
        return;
    }

    zbus_chan_pub(&battery_sample_data_chan, &evt, K_MSEC(50));
}

static int read_sensors(const struct device *charger, float *voltage, float *current, float *temp, int *status,
                        int *error)
{
    struct sensor_value value;
    int ret;

    ret = sensor_sample_fetch(charger);
    if (ret < 0) {
        return ret;
    }

    sensor_channel_get(charger, SENSOR_CHAN_GAUGE_VOLTAGE, &value);
    *voltage = (float)value.val1 + ((float)value.val2 / 1000000);

    sensor_channel_get(charger, SENSOR_CHAN_GAUGE_TEMP, &value);
    *temp = (float)value.val1 + ((float)value.val2 / 1000000);

    sensor_channel_get(charger, SENSOR_CHAN_GAUGE_AVG_CURRENT, &value);
    *current = (float)value.val1 + ((float)value.val2 / 1000000);

    if (status != NULL) {
        sensor_channel_get(charger, SENSOR_CHAN_NPM1300_CHARGER_STATUS, &value);
        *status = value.val1;
    }

    if (error != NULL) {
        sensor_channel_get(charger, SENSOR_CHAN_NPM1300_CHARGER_ERROR, &value);
        *error = value.val1;
    }

    return 0;
}

static int fuel_gauge_init(const struct device *charger)
{
    struct sensor_value value;
    struct nrf_fuel_gauge_init_parameters parameters = { .model = &battery_model };
    int ret;

    ret = read_sensors(charger, &parameters.v0, &parameters.i0, &parameters.t0, NULL, NULL);
    if (ret < 0) {
        return ret;
    }

    /* Store charge nominal and termination current, needed for ttf calculation */
    sensor_channel_get(charger, SENSOR_CHAN_GAUGE_DESIRED_CHARGING_CURRENT, &value);
    max_charge_current = (float)value.val1 + ((float)value.val2 / 1000000);
    term_charge_current = max_charge_current / 10.f;

    nrf_fuel_gauge_init(&parameters, NULL);

    ref_time = k_uptime_get();

    return 0;
}

static void event_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    LOG_DBG("Event detected\n");
    if (BIT(NPM1300_EVENT_CHG_COMPLETED) & pins) {
        LOG_DBG("Charging completed\n");
    }
    if (BIT(NPM1300_EVENT_VBUS_DETECTED) & pins) {
        LOG_DBG("VBUS detected\n");
    }
    if (BIT(NPM1300_EVENT_VBUS_REMOVED) & pins) {
        LOG_DBG("VBUS removed\n");
    }
    if (BIT(NPM1300_EVENT_CHG_ERROR) & pins) {
        LOG_ERR("Charging error\n");
    }
}

int zsw_pmic_get_full_state(struct battery_sample_event *sample)
{
    int ret;
    int status;
    int error;
    float voltage;
    float current;
    float temp;
    float soc;
    float tte;
    float ttf;
    float delta;

    ret = read_sensors(charger, &voltage, &current, &temp, &status, &error);
    if (ret < 0) {
        LOG_ERR("Error: Could not read from charger device\n");
        return ret;
    }

    delta = (float) k_uptime_delta(&ref_time) / 1000.f;

    soc = nrf_fuel_gauge_process(voltage, current, temp, delta, NULL);
    tte = nrf_fuel_gauge_tte_get();
    ttf = nrf_fuel_gauge_ttf_get(-max_charge_current, -term_charge_current);

    LOG_DBG("V: %.3f, I: %.3f, T: %.2f, ", voltage, current, temp);
    LOG_DBG("SoC: %.2f, TTE: %.0f, TTF: %.0f\n", soc, tte, ttf);
    LOG_DBG("Status: %d, Error: %d\n", status, error);

    sample->mV = voltage * 1000;
    sample->percent = soc;
    sample->avg_current = current;
    sample->temperature = temp;
    sample->tte = tte;
    sample->ttf = ttf;
    sample->status = status;
    sample->error = error;
    sample->is_charging = is_charging_from_status(status);

    return ret;
}

static int zsw_pmic_init(void)
{
    if (!device_is_ready(charger)) {
        LOG_ERR("Charger device not ready.\n");
        return 0;
    }

    if (fuel_gauge_init(charger) < 0) {
        LOG_ERR("Could not initialise fuel gauge.\n");
        return 0;
    }

    LOG_DBG("PMIC device ok\n");

    static struct gpio_callback event_cb;

    gpio_init_callback(&event_cb, event_callback,
                       BIT(NPM1300_EVENT_CHG_COMPLETED) | BIT(NPM1300_EVENT_VBUS_DETECTED) | BIT(NPM1300_EVENT_VBUS_REMOVED) | BIT(
                           NPM1300_EVENT_CHG_ERROR));

    mfd_npm1300_add_callback(pmic, &event_cb);

    zsw_periodic_chan_add_obs(&periodic_event_10s_chan, &zsw_pmic_slow_lis);
    return 0;
}

SYS_INIT(zsw_pmic_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
