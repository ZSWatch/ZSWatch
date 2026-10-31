/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

#include <stdbool.h>

/**
 * @brief Announce SMP traffic. The service is always registered; this only
 * asks for the short BLE connection interval for a while.
 *
 * @param auto_disable Ignored, kept for compatibility.
 * @return 0 on success, negative error code on failure.
 */
int zsw_smp_manager_enable(bool auto_disable);

/**
 * @brief No-op, kept for compatibility. SMP stays registered.
 *
 * @return 0, or -ENOTSUP on native_sim.
 */
int zsw_smp_manager_disable(void);

/**
 * @brief Check if SMP is available.
 *
 * @return true when SMP over BLE is available.
 */
bool zsw_smp_manager_is_enabled(void);