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

#pragma once

#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/slist.h>

#include "../../drivers/sensor/bmp581/zsw_bosch_bmp581.h"

/** @brief Caller-owned request; zero-initialize and keep alive until released. */
struct zsw_pressure_fast_request {
    sys_snode_t node;
    bool active;
};

/** @brief Initialize the requested pressure profile and periodic publication. */
int zsw_pressure_sensor_init(void);

/**
 * @brief Request or release fast sampling. Thread-only and idempotent.
 *
 * Fast sampling lasts until all requests are released. Do not copy or modify req.
 * Hardware updates are asynchronous; failures are logged and retried.
 * @return 0 if accepted; negative errno leaves the request unchanged.
 */
int zsw_pressure_sensor_request_fast(struct zsw_pressure_fast_request *req, bool enable);

/** @brief Get the latest pressure (Pa) and temperature (Celsius). */
int zsw_pressure_sensor_get_sample(float *pressure, float *temperature);

/** @brief Get the latest pressure in Pa. */
int zsw_pressure_sensor_get_pressure(float *pressure);

/** @brief Get the latest temperature in Celsius. */
int zsw_pressure_sensor_get_temperature(float *temperature);
