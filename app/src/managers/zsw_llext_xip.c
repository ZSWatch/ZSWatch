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
#include <zephyr/llext/loader.h>
#include <string.h>

#include "managers/zsw_llext_xip.h"

LOG_MODULE_REGISTER(llext_xip, CONFIG_ZSW_LLEXT_XIP_LOG_LEVEL);

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
 * Flash Write Helpers
 * -------------------------------------------------------------------------- */

/**
 * Write data to flash with 4-byte alignment padding. Does NOT erase.
 * Caller must ensure the target area has been erased first.
 */
static int flash_write_aligned(const struct flash_area *fa, uint32_t offset,
                               const void *data, size_t size)
{
    int ret;

    /* flash_area_write requires 4-byte aligned writes */
    size_t write_size = ROUND_UP(size, 4);

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

/**
 * Erase + write data to flash.
 */
static int write_to_flash(const struct flash_area *fa, uint32_t offset,
                          const void *data, size_t size)
{
    uint32_t erase_size = SECTOR_ALIGN(size);
    int ret;

    LOG_DBG("XIP flash: erase %u bytes at offset 0x%x", erase_size, offset);
    ret = flash_area_erase(fa, offset, erase_size);
    if (ret < 0) {
        LOG_ERR("Flash erase failed at 0x%x: %d", offset, ret);
        return ret;
    }

    LOG_DBG("XIP flash: write %zu bytes at offset 0x%x", size, offset);
    return flash_write_aligned(fa, offset, data, size);
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

/* --------------------------------------------------------------------------
 * Streaming Pre-Copy Hook (no heap allocation for TEXT/RODATA)
 * -------------------------------------------------------------------------- */

#define XIP_STREAM_BUF_SIZE     512

/**
 * Stream a single region from the ELF loader to XIP flash.
 * Returns the XIP CPU address on success, or 0 on failure.
 */
static uintptr_t xip_stream_region(struct llext_loader *ldr,
                                   const elf_shdr_t *region,
                                   const char *region_name)
{
    const struct flash_area *fa;
    uint8_t stream_buf[XIP_STREAM_BUF_SIZE];
    int ret;

    size_t total_size = region->sh_size;
    size_t prepad = region->sh_info;
    size_t data_offset = region->sh_offset + prepad;
    size_t data_len = total_size - prepad;

    /* Allocate XIP flash space */
    uint32_t aligned_size = SECTOR_ALIGN(total_size);

    if (xip_next_offset + aligned_size > xip_partition_size) {
        LOG_ERR("XIP stream: not enough flash for %s (%zu bytes)", region_name, total_size);
        return 0;
    }

    uint32_t flash_offset = xip_next_offset;
    uintptr_t xip_addr = XIP_PARTITION_CPU_ADDR + flash_offset;

    ret = flash_area_open(XIP_PARTITION_ID, &fa);
    if (ret < 0) {
        LOG_ERR("XIP stream: failed to open partition: %d", ret);
        return 0;
    }

    /* Erase sectors */
    ret = flash_area_erase(fa, flash_offset, aligned_size);
    if (ret < 0) {
        LOG_ERR("XIP stream: erase failed: %d", ret);
        flash_area_close(fa);
        return 0;
    }

    /* Write prepad zeros if needed (maintains alignment of sections within region) */
    if (prepad > 0) {
        memset(stream_buf, 0, MIN(prepad, XIP_STREAM_BUF_SIZE));
        size_t remaining = prepad;
        uint32_t wr_off = flash_offset;

        while (remaining > 0) {
            size_t chunk = MIN(remaining, XIP_STREAM_BUF_SIZE);

            ret = flash_write_aligned(fa, wr_off, stream_buf, chunk);
            if (ret < 0) {
                flash_area_close(fa);
                return 0;
            }
            wr_off += chunk;
            remaining -= chunk;
        }
    }

    /* Stream data from ELF loader to flash */
    ret = llext_seek(ldr, data_offset);
    if (ret < 0) {
        LOG_ERR("XIP stream: seek failed: %d", ret);
        flash_area_close(fa);
        return 0;
    }

    size_t remaining = data_len;
    uint32_t wr_off = flash_offset + prepad;

    while (remaining > 0) {
        size_t chunk = MIN(remaining, XIP_STREAM_BUF_SIZE);

        ret = llext_read(ldr, stream_buf, chunk);
        if (ret < 0) {
            LOG_ERR("XIP stream: read failed: %d", ret);
            flash_area_close(fa);
            return 0;
        }

        ret = flash_write_aligned(fa, wr_off, stream_buf, chunk);
        if (ret < 0) {
            flash_area_close(fa);
            return 0;
        }

        wr_off += chunk;
        remaining -= chunk;
    }

    flash_area_close(fa);

    /* Advance allocator */
    xip_next_offset += aligned_size;

    LOG_DBG("XIP stream %s: %zu bytes -> 0x%08lx (prepad=%zu)",
            region_name, data_len, (unsigned long)xip_addr, prepad);

    return xip_addr;
}

int zsw_llext_xip_pre_copy_hook(struct llext_loader *ldr, struct llext *ext,
                                void *user_data)
{
    /* Stream .text to XIP flash */
    elf_shdr_t *text_region = &ldr->sects[LLEXT_MEM_TEXT];

    if (text_region->sh_type != SHT_NULL && text_region->sh_size > 0) {
        uintptr_t xip_addr = xip_stream_region(ldr, text_region, ".text");

        if (xip_addr == 0) {
            return -EIO;
        }

        ext->mem[LLEXT_MEM_TEXT] = (void *)xip_addr;
        ext->mem_on_heap[LLEXT_MEM_TEXT] = false;
        ext->mem_size[LLEXT_MEM_TEXT] = text_region->sh_size;
    }

    /* Stream .rodata to XIP flash */
    elf_shdr_t *rodata_region = &ldr->sects[LLEXT_MEM_RODATA];

    if (rodata_region->sh_type != SHT_NULL && rodata_region->sh_size > 0) {
        uintptr_t xip_addr = xip_stream_region(ldr, rodata_region, ".rodata");

        if (xip_addr == 0) {
            /* Rollback .text XIP allocation */
            xip_next_offset = 0;
            ext->mem[LLEXT_MEM_TEXT] = NULL;
            ext->mem_size[LLEXT_MEM_TEXT] = 0;
            return -EIO;
        }

        ext->mem[LLEXT_MEM_RODATA] = (void *)xip_addr;
        ext->mem_on_heap[LLEXT_MEM_RODATA] = false;
        ext->mem_size[LLEXT_MEM_RODATA] = rodata_region->sh_size;
    }

    sys_cache_instr_invd_all();

    /*
     * If the caller provided an xip_context, find the .got section in the
     * ELF and compute its byte offset within the LLEXT_MEM_DATA region.
     * This is needed for -msingle-pic-base (R9) GOT access on ARM:
     * got_base = ext->mem[LLEXT_MEM_DATA] + got_offset  (computed after loading)
     */
    if (user_data != NULL) {
        struct zsw_llext_xip_context *ctx = user_data;

        ctx->got_found = false;
        ctx->got_offset = 0;

        const char *shstrtab = ext->mem[LLEXT_MEM_SHSTRTAB];

        for (unsigned int i = 0; i < ext->sect_cnt; i++) {
            const char *name = shstrtab + ext->sect_hdrs[i].sh_name;

            if (strcmp(name, ".got") == 0) {
                uintptr_t got_vma = ext->sect_hdrs[i].sh_addr;
                uintptr_t data_vma = ldr->sects[LLEXT_MEM_DATA].sh_addr;

                ctx->got_offset = got_vma - data_vma;
                ctx->got_found = true;
                LOG_DBG(".got at VMA 0x%zx, DATA region offset %zu",
                        (size_t)got_vma, ctx->got_offset);
                break;
            }
        }

        if (!ctx->got_found) {
            LOG_WRN("No .got section found in ELF");
        }
    }

    return 0;
}


void zsw_llext_xip_reset(void)
{
    xip_next_offset = 0;
    LOG_DBG("XIP allocator reset");
}
