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

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Streaming ELF XIP Loader for LLEXT apps.
 *
 * Loads LLEXT ELF apps directly to XIP flash + static data pool without
 * buffering entire sections in RAM.  Peak RAM is ~8-16KB of scratch space
 * regardless of app size (code, images, etc. can be arbitrarily large).
 *
 * Algorithm:
 *   1. Parse ELF header + section headers (small, on scratch)
 *   2. Read symbol/string tables, resolve all symbols
 *   3. Allocate XIP space (.text/.rodata) and data pool (.data/.bss)
 *   4. Stream each section in 4KB chunks: read → apply R_ARM_ABS32 relocs → write to XIP
 *   5. Copy .data to pool (with relocs applied), zero .bss
 *   6. Resolve entry function from the symbol table
 *
 * Only R_ARM_ABS32 relocations are supported (this is all that ELF object
 * files from add_llext_target generate for ARM Thumb-2 targets).
 */

/** Minimum scratch buffer size (bytes) for the streaming loader. */
#define ZSW_STREAM_SCRATCH_MIN      (20 * 1024)

/** Result from a successful streaming load. */
struct zsw_stream_load_result {
    void *entry_fn;         /**< Resolved entry function pointer (in XIP) */
};

/**
 * @brief Stream-load an LLEXT ELF app to XIP flash + data pool.
 *
 * The ELF file is read incrementally from the filesystem.  .text and .rodata
 * are written directly to XIP flash (external QSPI flash, memory-mapped).
 * .data and .bss are placed in a persistent static RAM pool.  No LLEXT heap
 * memory is consumed — the caller provides a scratch buffer that is used
 * only during loading and can be reused afterwards.
 *
 * @param elf_path      Filesystem path to the .llext ELF file
 * @param entry_symbol  Name of the entry function (e.g. "app_entry")
 * @param scratch       Scratch buffer (>= ZSW_STREAM_SCRATCH_MIN bytes)
 * @param scratch_size  Size of the scratch buffer in bytes
 * @param result        Output: resolved entry function pointer
 * @return 0 on success, negative errno on failure
 */
int zsw_llext_stream_load(const char *elf_path, const char *entry_symbol,
                          void *scratch, size_t scratch_size,
                          struct zsw_stream_load_result *result);
