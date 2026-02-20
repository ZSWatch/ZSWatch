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

#include <stdbool.h>

/**
 * @brief SMP Manager - Centralized BLE SMP/MCUmgr management
 *
 * This module manages the MCUmgr BLE transport (SMP) lifecycle including:
 * - Enabling/disabling SMP BT service
 * - XIP enable/disable (MCUmgr code resides in XIP)
 * - BLE parameter optimization (fast advertising, short connection interval)
 * - Auto-disable after inactivity timeout (detected via MCUmgr callbacks)
 */

/**
 * @brief Enable SMP BT transport with auto-disable timer.
 *
 * Enables XIP (required for MCUmgr code), registers the SMP BT service,
 * sets fast BLE advertising and short connection intervals, and starts
 * an inactivity timer that will auto-disable SMP after the configured
 * timeout (default 3 minutes).
 *
 * @param auto_disable If true, auto-disable after inactivity timeout.
 *                     If false, SMP remains enabled until explicitly disabled.
 * @return 0 on success, negative error code on failure.
 */
int zsw_smp_manager_enable(bool auto_disable);

/**
 * @brief Disable SMP BT transport.
 *
 * Unregisters the SMP BT service, restores default BLE advertising and
 * connection intervals, disables XIP, and cancels any pending auto-disable
 * timer.
 *
 * @return 0 on success, negative error code on failure.
 */
int zsw_smp_manager_disable(void);

/**
 * @brief Check if SMP is currently enabled.
 *
 * @return true if SMP is enabled, false otherwise.
 */
bool zsw_smp_manager_is_enabled(void);

/**
 * @brief Reset the auto-disable inactivity timer.
 *
 * Call this when SMP activity is detected to postpone auto-disable.
 * This is called automatically by the manager's internal MCUmgr callbacks
 * for IMG and FS operations.
 */
void zsw_smp_manager_reset_timeout(void);
