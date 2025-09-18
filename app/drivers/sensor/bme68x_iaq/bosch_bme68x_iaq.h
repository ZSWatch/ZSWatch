/* bosch_bme68x_iaq.h- Driver for Bosch BME68x encironmental sensor. */

/*
 * Copyright (c) 2025, ZSWatch Project
 * Copyright (c) 2023, Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <zephyr/drivers/sensor.h>

/** @brief IAQ sensor channel.
*/
#define SENSOR_CHAN_IAQ                 (SENSOR_CHAN_PRIV_START + 1)
