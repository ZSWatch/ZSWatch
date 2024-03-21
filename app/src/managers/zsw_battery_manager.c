/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>

#include "battery/battery.h"
#include "events/battery_event.h"
#include "managers/zsw_battery_manager.h"

LOG_MODULE_REGISTER(zsw_battery_manager, LOG_LEVEL_WRN);

#define BATTERY_SAMPLE_INTETRVAL_MINUTES    5

static void handle_battery_sample_timeout(struct k_work *item);

K_WORK_DELAYABLE_DEFINE(battery_sample_work, handle_battery_sample_timeout);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);

/** A discharge curve specific to the power source. */
static const struct battery_level_point levels[] = {
    /*
    Battery supervisor cuts power at 3500mA so treat that as 0%
    TODO analyze more to get a better curve.
    */
    { 10000, 4150 },
    { 0, 3500 },
};

int zsw_battery_manager_sample_battery(int *mV, int *percent)
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

    rc = zsw_battery_manager_sample_battery(&evt.mV, &evt.percent);
    if (rc == 0) {
        zbus_chan_pub(&battery_sample_data_chan, &evt, K_MSEC(5));
    }
    k_work_schedule(&battery_sample_work, K_MINUTES(BATTERY_SAMPLE_INTETRVAL_MINUTES));
}

static int zsw_battery_manager_init(void)
{
    k_work_schedule(&battery_sample_work, K_SECONDS(1));
    return 0;
}

//SYS_INIT(zsw_battery_manager_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);