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

/**
 * @file zsw_llext_log.h
 * @brief Logging for LLEXT dynamic apps via the Zephyr log subsystem.
 *
 * All messages are routed through a single Zephyr log module registered in
 * the firmware ("llext_app").  The log level is controlled by one Kconfig:
 * CONFIG_ZSW_LLEXT_LOG_LEVEL.
 *
 * Usage — in each LLEXT source file:
 *
 *     #include "zsw_llext_log.h"
 *     LOG_MODULE_REGISTER(my_module, LOG_LEVEL_INF);
 */

#pragma once

/* ---- Zephyr log-level constants (mirror <zephyr/logging/log.h>) ---- */
#ifndef LOG_LEVEL_NONE
#define LOG_LEVEL_NONE 0
#endif
#ifndef LOG_LEVEL_ERR
#define LOG_LEVEL_ERR  1
#endif
#ifndef LOG_LEVEL_WRN
#define LOG_LEVEL_WRN  2
#endif
#ifndef LOG_LEVEL_INF
#define LOG_LEVEL_INF  3
#endif
#ifndef LOG_LEVEL_DBG
#define LOG_LEVEL_DBG  4
#endif

/**
 * @brief Log a message through the Zephyr logging subsystem.
 *
 * Implemented in zsw_llext_log.c (firmware side).  Uses a pre-registered
 * Zephyr log source so messages appear in the normal log output.
 * Level filtering is done by CONFIG_ZSW_LLEXT_LOG_LEVEL.
 *
 * @param level  Log level (LOG_LEVEL_ERR .. LOG_LEVEL_DBG).
 * @param fmt    printf-style format string.
 */
void zsw_llext_log(uint8_t level, const char *fmt, ...);

/* ---------- Drop-in replacements for Zephyr logging macros ---------- */

/**
 * Replaces Zephyr's LOG_MODULE_REGISTER(name, level).
 * The level argument is accepted but ignored — filtering is done by
 * CONFIG_ZSW_LLEXT_LOG_LEVEL in the firmware.
 */
#define LOG_MODULE_REGISTER(...)

#define LOG_ERR(fmt, ...)  zsw_llext_log(LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)
#define LOG_WRN(fmt, ...)  zsw_llext_log(LOG_LEVEL_WRN, fmt, ##__VA_ARGS__)
#define LOG_INF(fmt, ...)  zsw_llext_log(LOG_LEVEL_INF, fmt, ##__VA_ARGS__)
#define LOG_DBG(fmt, ...)  zsw_llext_log(LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)
