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

int _zsw_xip_enable(const char *requester);
int _zsw_xip_disable(const char *requester);

/** Check whether QSPI XIP is currently enabled.
 *  Safe to call from any context (lock-free read of a boolean). */
bool zsw_xip_is_enabled(void);

#ifdef CONFIG_ZSW_XIP
#define zsw_xip_enable() _zsw_xip_enable(__FUNCTION__)
#define zsw_xip_disable() _zsw_xip_disable(__FUNCTION__)
#else
#define zsw_xip_enable() (0)
#define zsw_xip_disable() (0)
#endif
