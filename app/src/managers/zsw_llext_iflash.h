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
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Mark a function for internal flash execution.
 *
 * Functions marked with LLEXT_IFLASH will be copied from XIP flash to
 * internal flash after loading, and their GOT entries patched so all callers
 * use the internal flash copy. This makes them safe to execute when XIP is
 * disabled (screen off).
 *
 * Use this for zbus callbacks, timer handlers, and any code that must
 * survive screen-off in LLEXT apps.
 */
#define LLEXT_IFLASH __attribute__((section(".text.iflash"), noinline, used))

/**
 * @brief Initialize the internal flash allocator.
 *
 * Opens the llext_core_partition and records its size. Must be called once
 * before any install operations.
 *
 * @return 0 on success, negative errno on failure
 */
int zsw_llext_iflash_init(void);

/**
 * @brief Post-load: copy .text.iflash sections from XIP to internal flash and patch GOT.
 *
 * After llext_load() has streamed .text/.rodata to XIP flash and linked
 * everything, this function:
 *   1. Scans ext->sect_hdrs[] for sections named ".text.iflash"
 *   2. Copies those function bodies from their XIP address to internal flash
 *   3. Patches ALL data entries (DATA region) that reference the old XIP
 *      address so callers and data structures (e.g. zbus observer callbacks)
 *      use the internal flash copy instead.
 *
 * @param ext            Loaded LLEXT extension
 * @param text_base_vma  Original ELF VMA of the TEXT region start
 * @param got_base       GOT base address for this LLEXT (used for R9 trampolines)
 * @return 0 on success (also 0 if no .text.iflash sections found),
 *         negative errno on failure
 */
int zsw_llext_iflash_install(struct llext *ext, uintptr_t text_base_vma, void *got_base);

/**
 * @brief Reset the internal flash allocator.
 *
 * Resets the flash offset to 0, allowing the space to be reused.
 * Call this when the LLEXT module is unloaded.
 */
void zsw_llext_iflash_reset(void);
