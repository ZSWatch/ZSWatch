/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/npm1300_charger.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <events/activity_event.h>
#include "nrf_fuel_gauge.h"

LOG_MODULE_REGISTER(zsw_pmic, LOG_LEVEL_DBG);

#define UPDATE_INTERVAL 1000
#define ZSW_NOT_WORN_STATIONARY_CURRENT 0.0005 // 50uA. TODO remeasure this value

static void zbus_activity_event_callback(const struct zbus_channel *chan);
static int read_sensors(const struct device *charger, float *voltage, float *current, float *temp);

static const struct device *charger = DEVICE_DT_GET(DT_NODELABEL(npm1300_ek_charger));

ZBUS_CHAN_DECLARE(activity_state_data_chan);
ZBUS_LISTENER_DEFINE(pmic_activity_state_event, zbus_activity_event_callback);

static float max_charge_current;
static float term_charge_current;
static int64_t ref_time;

static struct k_work_delayable pmic_work;

static const struct battery_model battery_model = {
#include "battery_model.inc"
};

static void zbus_activity_event_callback(const struct zbus_channel *chan)
{
	int ret;
	float voltage;
	float current;
	float temp;

	ret = read_sensors(charger, &voltage, &current, &temp);
	if (ret != 0) {
		printk("Error: Could not read from charger device\n");
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

static int read_sensors(const struct device *charger, float *voltage, float *current, float *temp)
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

	return 0;
}

static int fuel_gauge_init(const struct device *charger)
{
	struct sensor_value value;
	struct nrf_fuel_gauge_init_parameters parameters = { .model = &battery_model };
	int ret;

	ret = read_sensors(charger, &parameters.v0, &parameters.i0, &parameters.t0);
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

static int fuel_gauge_update(const struct device *charger)
{
	float voltage;
	float current;
	float temp;
	float soc;
	float tte;
	float ttf;
	float delta;
	int ret;

	ret = read_sensors(charger, &voltage, &current, &temp);
	if (ret < 0) {
		printk("Error: Could not read from charger device\n");
		return ret;
	}

	delta = (float) k_uptime_delta(&ref_time) / 1000.f;

	soc = nrf_fuel_gauge_process(voltage, current, temp, delta, NULL);
	tte = nrf_fuel_gauge_tte_get();
	ttf = nrf_fuel_gauge_ttf_get(-max_charge_current, -term_charge_current);

	printk("V: %.3f, I: %.3f, T: %.2f, ", voltage, current, temp);
	printk("SoC: %.2f, TTE: %.0f, TTF: %.0f\n", soc, tte, ttf);

	return 0;
}

static void fuel_gauge_update_work(struct k_work *work)
{
	fuel_gauge_update(charger);
	k_work_reschedule(&pmic_work, K_MSEC(UPDATE_INTERVAL));
}

static int zsw_pmic_init(void)
{
	if (!device_is_ready(charger)) {
		printk("Charger device not ready.\n");
		return 0;
	}

	if (fuel_gauge_init(charger) < 0) {
		printk("Could not initialise fuel gauge.\n");
		return 0;
	}

	printk("PMIC device ok\n");

	k_work_init_delayable(&pmic_work, fuel_gauge_update_work);
	k_work_schedule(&pmic_work, K_NO_WAIT);
	return 0;
}

SYS_INIT(zsw_pmic_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
