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

#include <zephyr/logging/log.h>
#include <stdarg.h>
#include <stdint.h>

#include "managers/zsw_xip_manager.h"

LOG_MODULE_REGISTER(llext_app, CONFIG_ZSW_LLEXT_LOG_LEVEL);

/* nRF5340 QSPI XIP address window */
#define XIP_ADDR_START  0x10000000UL
#define XIP_ADDR_END    0x20000000UL

void zsw_llext_log(uint8_t level, const char *fmt, ...)
{
    /* fmt lives in LLEXT .rodata which resides in XIP flash.
     * If XIP is currently disabled (screen off / power save),
     * dereferencing fmt would cause a bus fault.  Silently skip. */
    uintptr_t addr = (uintptr_t)fmt;

    if (addr >= XIP_ADDR_START && addr < XIP_ADDR_END && !zsw_xip_is_enabled()) {
        return;
    }

    va_list ap;

    va_start(ap, fmt);
    z_log_msg_runtime_vcreate(Z_LOG_LOCAL_DOMAIN_ID,
                              (const void *)Z_LOG_CURRENT_DATA(),
                              level, NULL, 0, 0, fmt, ap);
    va_end(ap);
}
