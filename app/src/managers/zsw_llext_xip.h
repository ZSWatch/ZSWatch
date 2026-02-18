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

#include <zephyr/llext/llext.h>

/**
 * @brief Post-load XIP installer for PIC LLEXT apps.
 *
 * After llext_load() loads a PIC (ET_DYN / -fPIC) extension fully into RAM,
 * this module moves .text and .rodata to XIP flash verbatim (no relocation
 * patching needed — PIC code uses GOT indirection in RAM).
 *
 * Result: .text/.rodata execute from flash, .got/.data/.bss stay in RAM.
 * Typical RAM savings: 90-97% (a 200KB app uses ~6KB RAM instead of ~200KB).
 */

/**
 * @brief Initialize the XIP allocator.
 *
 * Opens the llext_xip_partition and records its size. Must be called once
 * before any install operations.
 *
 * @return 0 on success, negative errno on failure
 */
int zsw_llext_xip_init(void);

/**
 * @brief Install a PIC LLEXT's .text/.rodata into XIP flash.
 *
 * Writes .text and .rodata from the LLEXT heap to the XIP flash partition,
 * updates ext->mem[] to point to XIP CPU addresses, frees heap copies,
 * and adjusts symbol/export table pointers.
 *
 * Only one LLEXT should be installed at a time (call zsw_llext_xip_reset()
 * after unloading to reclaim flash space).
 *
 * @param ext  Loaded LLEXT (from llext_load())
 * @return 0 on success, negative errno on failure
 */
int zsw_llext_xip_install(struct llext *ext);

/**
 * @brief Reset the XIP allocator.
 *
 * Resets the flash offset to 0, allowing the space to be reused by the next
 * app. Call this after llext_unload() when done with an app.
 */
void zsw_llext_xip_reset(void);
