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
 * @file zsw_llext_xip.c
 * @brief Post-load XIP installer for PIC (-fPIC / ET_DYN) LLEXT apps.
 *
 * Because PIC code uses GOT indirection for all external references,
 * .text and .rodata can be written to XIP flash VERBATIM — no relocation
 * patching of instructions is needed. The GOT (part of .data) stays in RAM
 * and is filled by llext_load() during the normal linking phase.
 *
 * Flow:
 *   1. llext_load() loads everything to RAM heap (standard path)
 *   2. zsw_llext_xip_install() writes .text/.rodata to flash, frees heap copies
 *   3. App executes .text from XIP flash, GOT/.data/.bss in RAM
 *   4. On app close: llext_unload() frees .data/.bss, zsw_llext_xip_reset()
 *      reclaims flash space
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/cache.h>
#include <zephyr/llext/llext.h>
#include <string.h>

#include "managers/zsw_llext_xip.h"

LOG_MODULE_REGISTER(llext_xip, LOG_LEVEL_INF);

/*
 * Access the LLEXT heap directly for freeing relocated sections.
 * On non-Harvard ARM (nRF5340), llext_data_heap and llext_instr_heap
 * are both aliases for llext_heap.
 */
extern struct k_heap llext_heap;

/* --------------------------------------------------------------------------
 * XIP Partition Configuration
 * -------------------------------------------------------------------------- */

/** XIP base CPU address (external flash mapped at 0x10000000 on nRF5340) */
#define XIP_BASE_ADDR           0x10000000

/** Flash sector size for erase alignment */
#define XIP_SECTOR_SIZE         4096

#define XIP_PARTITION_ID        FIXED_PARTITION_ID(llext_xip_partition)
#define XIP_PARTITION_OFFSET    FIXED_PARTITION_OFFSET(llext_xip_partition)

/** CPU address corresponding to the start of the XIP partition */
#define XIP_PARTITION_CPU_ADDR  (XIP_BASE_ADDR + XIP_PARTITION_OFFSET)

#define SECTOR_ALIGN(x)         ROUND_UP((x), XIP_SECTOR_SIZE)

/* --------------------------------------------------------------------------
 * Linear Allocator (only one app at a time, reset on unload)
 * -------------------------------------------------------------------------- */

static uint32_t xip_next_offset;
static uint32_t xip_partition_size;

/* --------------------------------------------------------------------------
 * Flash Write Helper
 * -------------------------------------------------------------------------- */

static int write_to_flash(const struct flash_area *fa, uint32_t offset,
                          const void *data, size_t size)
{
    uint32_t erase_size = SECTOR_ALIGN(size);
    int ret;

    LOG_INF("XIP flash: erase %u bytes at offset 0x%x", erase_size, offset);
    ret = flash_area_erase(fa, offset, erase_size);
    if (ret < 0) {
        LOG_ERR("Flash erase failed at 0x%x: %d", offset, ret);
        return ret;
    }

    /* flash_area_write requires 4-byte aligned writes */
    size_t write_size = ROUND_UP(size, 4);

    LOG_INF("XIP flash: write %zu bytes at offset 0x%x", size, offset);

    if (write_size == size) {
        ret = flash_area_write(fa, offset, data, write_size);
    } else {
        /* Write aligned portion directly */
        size_t aligned_part = size & ~3U;

        if (aligned_part > 0) {
            ret = flash_area_write(fa, offset, data, aligned_part);
            if (ret < 0) {
                return ret;
            }
        }

        /* Pad remaining 1-3 bytes to 4 */
        size_t remainder = size - aligned_part;

        if (remainder > 0) {
            uint8_t pad_buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

            memcpy(pad_buf, (const uint8_t *)data + aligned_part, remainder);
            ret = flash_area_write(fa, offset + aligned_part, pad_buf, 4);
        }
    }

    if (ret < 0) {
        LOG_ERR("Flash write failed at 0x%x: %d", offset, ret);
    }

    return ret;
}

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

int zsw_llext_xip_init(void)
{
    const struct flash_area *fa;
    int ret;

    ret = flash_area_open(XIP_PARTITION_ID, &fa);
    if (ret < 0) {
        LOG_ERR("Failed to open XIP partition: %d", ret);
        return ret;
    }

    xip_partition_size = fa->fa_size;
    flash_area_close(fa);

    xip_next_offset = 0;

    LOG_INF("XIP init: partition at flash 0x%x, CPU 0x%08x, size %u KB",
            XIP_PARTITION_OFFSET, XIP_PARTITION_CPU_ADDR,
            xip_partition_size / 1024);

    return 0;
}

int zsw_llext_xip_install(struct llext *ext)
{
    const struct flash_area *fa;
    int ret;

    void *ram_text = ext->mem[LLEXT_MEM_TEXT];
    void *ram_rodata = ext->mem[LLEXT_MEM_RODATA];
    size_t text_size = ext->mem_size[LLEXT_MEM_TEXT];
    size_t rodata_size = ext->mem_size[LLEXT_MEM_RODATA];

    if (!ram_text || !ext->mem_on_heap[LLEXT_MEM_TEXT]) {
        LOG_ERR("XIP install: .text not on heap");
        return -EINVAL;
    }

    /* Calculate space needed (sector-aligned) */
    uint32_t aligned_text = SECTOR_ALIGN(text_size);
    uint32_t aligned_rodata = (rodata_size > 0) ? SECTOR_ALIGN(rodata_size) : 0;
    uint32_t total = aligned_text + aligned_rodata;

    if (xip_next_offset + total > xip_partition_size) {
        LOG_ERR("XIP: not enough flash (need %u, have %u)",
                total, xip_partition_size - xip_next_offset);
        return -ENOSPC;
    }

    uint32_t text_offset = xip_next_offset;
    uint32_t rodata_offset = xip_next_offset + aligned_text;

    uintptr_t xip_text_addr = XIP_PARTITION_CPU_ADDR + text_offset;
    uintptr_t xip_rodata_addr = (rodata_size > 0) ?
        (XIP_PARTITION_CPU_ADDR + rodata_offset) : 0;

    LOG_INF("XIP install '%s': .text=%zu->0x%08lx, .rodata=%zu->0x%08lx",
            ext->name, text_size, (unsigned long)xip_text_addr,
            rodata_size, (unsigned long)xip_rodata_addr);

    /* Open partition and write sections */
    ret = flash_area_open(XIP_PARTITION_ID, &fa);
    if (ret < 0) {
        LOG_ERR("Failed to open XIP partition: %d", ret);
        return ret;
    }

    ret = write_to_flash(fa, text_offset, ram_text, text_size);
    if (ret < 0) {
        flash_area_close(fa);
        return ret;
    }

    if (rodata_size > 0 && ram_rodata) {
        ret = write_to_flash(fa, rodata_offset, ram_rodata, rodata_size);
        if (ret < 0) {
            flash_area_close(fa);
            return ret;
        }
    }

    flash_area_close(fa);

    /* Invalidate instruction cache so CPU sees new flash content */
    sys_cache_instr_invd_all();

    /* Advance allocator */
    xip_next_offset += total;

    /* Save old RAM addresses for sym/exp table pointer fixup */
    uintptr_t old_text = (uintptr_t)ram_text;
    uintptr_t old_rodata = (uintptr_t)ram_rodata;
    size_t old_text_size = text_size;
    size_t old_rodata_size = rodata_size;

    /* Free heap copies and redirect to XIP addresses */
    k_heap_free(&llext_heap, ram_text);
    ext->mem[LLEXT_MEM_TEXT] = (void *)xip_text_addr;
    ext->mem_on_heap[LLEXT_MEM_TEXT] = false;

    if (rodata_size > 0 && ram_rodata) {
        k_heap_free(&llext_heap, ram_rodata);
        ext->mem[LLEXT_MEM_RODATA] = (void *)xip_rodata_addr;
        ext->mem_on_heap[LLEXT_MEM_RODATA] = false;
    }

    /* Fix sym_tab and exp_tab pointers that reference moved regions */
    intptr_t text_delta = (intptr_t)xip_text_addr - (intptr_t)old_text;
    intptr_t rodata_delta = (rodata_size > 0) ?
        ((intptr_t)xip_rodata_addr - (intptr_t)old_rodata) : 0;

    #define ADJUST_PTR(ptr, type) do { \
        uintptr_t _a = (uintptr_t)(ptr); \
        if (_a >= old_text && _a < old_text + old_text_size) { \
            (ptr) = (type)(void *)(_a + text_delta); \
        } else if (old_rodata_size > 0 && \
                   _a >= old_rodata && _a < old_rodata + old_rodata_size) { \
            (ptr) = (type)(void *)(_a + rodata_delta); \
        } \
    } while (0)

    for (size_t i = 0; i < ext->exp_tab.sym_cnt; i++) {
        ADJUST_PTR(ext->exp_tab.syms[i].addr, void *);
        ADJUST_PTR(ext->exp_tab.syms[i].name, const char *);
    }

    for (size_t i = 0; i < ext->sym_tab.sym_cnt; i++) {
        ADJUST_PTR(ext->sym_tab.syms[i].addr, void *);
        ADJUST_PTR(ext->sym_tab.syms[i].name, const char *);
    }

    #undef ADJUST_PTR

    LOG_INF("XIP install '%s' complete: freed %zu bytes from heap, "
            ".text+.rodata now in flash",
            ext->name, text_size + rodata_size);

    return 0;
}

void zsw_llext_xip_reset(void)
{
    xip_next_offset = 0;
    LOG_INF("XIP allocator reset");
}
