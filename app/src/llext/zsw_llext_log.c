/*
 * Copyright (c) 2026 ZSWatch Project
 * SPDX-License-Identifier: Apache-2.0
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
