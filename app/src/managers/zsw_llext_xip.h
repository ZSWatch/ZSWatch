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
#include <stddef.h>
#include <stdint.h>

/**
 * @brief XIP partition allocator and flash writer for LLEXT apps.
 *
 * Manages a linear allocator within the llext_xip_partition (external flash)
 * to store relocated .text and .rodata sections. After LLEXT loads an app
 * fully into RAM, this module:
 *   1. Allocates space in the XIP partition
 *   2. Re-parses ELF relocation entries to adjust absolute addresses
 *   3. Writes the adjusted sections to flash
 *   4. Updates the LLEXT struct to point to XIP addresses
 *   5. Frees the original RAM copies
 */

/** XIP base CPU address (external flash mapped at 0x10000000 on nRF5340) */
#define ZSW_XIP_BASE_ADDR       0x10000000

/** Flash sector size for erase alignment */
#define ZSW_XIP_SECTOR_SIZE     4096

/** Maximum number of XIP-installed apps */
#define ZSW_XIP_MAX_APPS        16

/**
 * @brief Initialize the XIP allocator.
 *
 * Must be called once before any install operations. Resets the allocator
 * and erases tracking state (allocations are rebuilt each boot).
 *
 * @return 0 on success, negative errno on failure
 */
int zsw_llext_xip_init(void);

/**
 * @brief Install an LLEXT app's .text and .rodata into XIP flash.
 *
 * After llext_load() has loaded the app fully into RAM with all relocations
 * applied, this function:
 *   1. Allocates space in the XIP partition for .text and .rodata
 *   2. Re-reads the ELF file to find relocation entries
 *   3. Adjusts absolute addresses that reference the moved sections
 *   4. Writes the adjusted sections to XIP flash
 *   5. Updates ext->mem[] pointers to XIP addresses
 *   6. Frees the original RAM heap copies
 *   7. Adjusts symbol table entries
 *
 * @param ext           Loaded LLEXT (from llext_load, all sections in RAM)
 * @param elf_path      Path to the .llext ELF file on filesystem
 * @return 0 on success, negative errno on failure.
 *         On failure, the LLEXT is left in its original RAM state.
 */
int zsw_llext_xip_install(struct llext *ext, const char *elf_path);

/**
 * @brief Allocate XIP partition space for .text and .rodata sections.
 *
 * @param name          App name (for logging)
 * @param text_size     Size of .text in bytes
 * @param rodata_size   Size of .rodata in bytes (0 if none)
 * @param out_text_offset   Output: partition offset for .text
 * @param out_rodata_offset Output: partition offset for .rodata
 * @return 0 on success, negative errno on failure
 */
int zsw_llext_xip_alloc(const char *name, size_t text_size, size_t rodata_size,
                        uint32_t *out_text_offset, uint32_t *out_rodata_offset);

/**
 * @brief Convert a partition offset to a CPU-visible XIP address.
 */
uintptr_t zsw_llext_xip_cpu_addr(uint32_t partition_offset);

/**
 * @brief Allocate space in the persistent static data pool.
 *
 * Used by the streaming loader to place .data/.bss outside the LLEXT heap.
 *
 * @param align  Alignment requirement
 * @param size   Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL if pool exhausted
 */
void *zsw_llext_data_pool_alloc(size_t align, size_t size);
