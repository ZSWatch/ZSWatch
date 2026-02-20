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
 * @file zsw_llext_iflash.c
 * @brief Post-load copy of .text.iflash sections to internal flash + GOT patching.
 *
 * After an LLEXT app is loaded (with .text/.rodata in XIP flash), this module
 * finds sections named ".text.iflash", copies their contents from XIP to the
 * internal flash partition (llext_core_partition), and patches the GOT entries
 * so all callers use the internal flash address instead of XIP.
 *
 * This makes tagged functions safe to execute when XIP is disabled (screen off).
 *
 * On nRF5340, internal flash CPU address == flash offset (0x0 base).
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/cache.h>
#include <zephyr/llext/llext.h>
#include <string.h>

#include "managers/zsw_llext_iflash.h"

LOG_MODULE_REGISTER(llext_iflash, CONFIG_ZSW_LLEXT_XIP_LOG_LEVEL);

/* --------------------------------------------------------------------------
 * Internal Flash Partition Configuration
 * -------------------------------------------------------------------------- */

/** Flash sector size for erase alignment (nRF5340 internal flash = 4 KB pages) */
#define IFLASH_SECTOR_SIZE         4096

#define IFLASH_PARTITION_ID        FIXED_PARTITION_ID(llext_core_partition)
#define IFLASH_PARTITION_OFFSET    FIXED_PARTITION_OFFSET(llext_core_partition)

/**
 * On nRF5340, internal flash is mapped starting at CPU address 0x00000000.
 * The CPU address of a byte in internal flash == its flash offset.
 */
#define IFLASH_CPU_ADDR(offset)    (offset)

#define SECTOR_ALIGN(x)            ROUND_UP((x), IFLASH_SECTOR_SIZE)

/** Section name for functions that must survive XIP-off */
#define IFLASH_SECTION_NAME        ".text.iflash"

#ifdef CONFIG_ARM
/**
 * ARM Thumb2 R9-restoring trampoline (16 bytes).
 * Sets R9 to the LLEXT's GOT base before jumping to the real iflash function.
 * This allows iflash callbacks (e.g. zbus listeners) to run correctly on
 * threads that don't have R9 set (sysworkq, timer ISR, etc.).
 *
 * Layout:
 *   +0: ldr r9, [pc, #4]   ; 0xF8DF 0x9004 — load GOT base from +8
 *   +4: ldr pc, [pc, #4]   ; 0xF8DF 0xF004 — load target from +12 & branch
 *   +8: .word GOT_BASE
 *  +12: .word TARGET_ADDR  ; with thumb bit set
 */
#define TRAMPOLINE_SIZE  16
static const uint8_t trampoline_code[8] = {
    0xDF, 0xF8, 0x04, 0x90,   /* ldr r9, [pc, #4] (little-endian Thumb2) */
    0xDF, 0xF8, 0x04, 0xF0,   /* ldr pc, [pc, #4] (little-endian Thumb2) */
};
#endif /* CONFIG_ARM */

/* --------------------------------------------------------------------------
 * Linear Allocator
 * -------------------------------------------------------------------------- */

static uint32_t iflash_next_offset;
static uint32_t iflash_partition_size;

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

int zsw_llext_iflash_init(void)
{
    const struct flash_area *fa;
    int ret;

    ret = flash_area_open(IFLASH_PARTITION_ID, &fa);
    if (ret < 0) {
        LOG_ERR("Failed to open internal flash partition: %d", ret);
        return ret;
    }

    iflash_partition_size = fa->fa_size;
    flash_area_close(fa);

    iflash_next_offset = 0;

    LOG_INF("Internal flash init: partition at 0x%x, CPU 0x%08x, size %u KB",
            IFLASH_PARTITION_OFFSET,
            IFLASH_CPU_ADDR(IFLASH_PARTITION_OFFSET),
            iflash_partition_size / 1024);

    return 0;
}

/* --------------------------------------------------------------------------
 * Post-load install: copy .text.iflash from XIP -> internal flash, patch GOT
 * -------------------------------------------------------------------------- */

/**
 * Write data to internal flash with 4-byte alignment padding. Does NOT erase.
 */
static int flash_write_aligned(const struct flash_area *fa, uint32_t offset,
                               const void *data, size_t size)
{
    int ret;
    size_t write_size = ROUND_UP(size, 4);

    if (write_size == size) {
        ret = flash_area_write(fa, offset, data, write_size);
    } else {
        size_t aligned_part = size & ~3U;

        if (aligned_part > 0) {
            ret = flash_area_write(fa, offset, data, aligned_part);
            if (ret < 0) {
                return ret;
            }
        }

        size_t remainder = size - aligned_part;

        if (remainder > 0) {
            uint8_t pad_buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

            memcpy(pad_buf, (const uint8_t *)data + aligned_part, remainder);
            ret = flash_area_write(fa, offset + aligned_part, pad_buf, 4);
        }
    }

    if (ret < 0) {
        LOG_ERR("Internal flash write failed at 0x%x: %d", offset, ret);
    }

    return ret;
}

int zsw_llext_iflash_install(struct llext *ext, uintptr_t text_base_vma, void *got_base)
{
    const struct flash_area *fa;
    int ret;
    bool found_iflash = false;

    if (ext == NULL) {
        return -EINVAL;
    }

    const char *shstrtab = ext->mem[LLEXT_MEM_SHSTRTAB];

    if (shstrtab == NULL) {
        LOG_WRN("No shstrtab - cannot scan for iflash sections");
        return 0;
    }

    /* Scan section headers for .text.iflash */
    LOG_DBG("Scanning %u sections for '%s' (TEXT base at 0x%08lx)",
            ext->sect_cnt, IFLASH_SECTION_NAME, (unsigned long)text_base_vma);
    for (unsigned int i = 0; i < ext->sect_cnt; i++) {
        const char *name = shstrtab + ext->sect_hdrs[i].sh_name;

        if (strcmp(name, IFLASH_SECTION_NAME) != 0) {
            continue;
        }

        found_iflash = true;

        size_t sect_size = ext->sect_hdrs[i].sh_size;
        uintptr_t sect_vma = ext->sect_hdrs[i].sh_addr;

        if (sect_size == 0) {
            LOG_WRN("Empty %s section, skipping", IFLASH_SECTION_NAME);
            continue;
        }

        /* Compute the XIP address where this section was loaded.
         * XIP runtime addr = TEXT region base + (section VMA - TEXT base VMA) */
        uintptr_t text_runtime_base = (uintptr_t)ext->mem[LLEXT_MEM_TEXT];
        uintptr_t xip_addr = text_runtime_base + (sect_vma - text_base_vma);

        LOG_INF("%s: VMA 0x%08lx, size %zu, XIP addr 0x%08lx",
                IFLASH_SECTION_NAME,
                (unsigned long)sect_vma, sect_size,
                (unsigned long)xip_addr);

        /* Allocate internal flash space */
        uint32_t aligned_size = SECTOR_ALIGN(sect_size);

        if (iflash_next_offset + aligned_size > iflash_partition_size) {
            LOG_ERR("Internal flash: not enough space (%zu bytes, avail %u)",
                    sect_size, iflash_partition_size - iflash_next_offset);
            return -ENOMEM;
        }

        uint32_t flash_offset = iflash_next_offset;
        uintptr_t iflash_addr = IFLASH_CPU_ADDR(IFLASH_PARTITION_OFFSET + flash_offset);

        /* Open partition, erase, and copy from XIP */
        ret = flash_area_open(IFLASH_PARTITION_ID, &fa);
        if (ret < 0) {
            LOG_ERR("Failed to open internal flash partition: %d", ret);
            return ret;
        }

        ret = flash_area_erase(fa, flash_offset, aligned_size);
        if (ret < 0) {
            LOG_ERR("Internal flash erase failed at 0x%x: %d", flash_offset, ret);
            flash_area_close(fa);
            return ret;
        }

        /* Copy from XIP address (memory-mapped, directly readable) */
        ret = flash_write_aligned(fa, flash_offset, (const void *)xip_addr, sect_size);
        if (ret < 0) {
            flash_area_close(fa);
            return ret;
        }

        flash_area_close(fa);

        iflash_next_offset += aligned_size;

        LOG_INF("Copied %zu bytes: XIP 0x%08lx -> internal flash 0x%08lx",
                sect_size, (unsigned long)xip_addr, (unsigned long)iflash_addr);

        /* Patch ALL data entries in the LLEXT DATA region.
         * Function pointers are stored not only in .got but also in
         * .data.rel.ro (e.g., zbus observer callback fields). Scanning
         * the entire DATA region catches all references.
         * NOTE: ARM Thumb addresses have bit 0 set. Clear it for range check. */
        if (ext->mem[LLEXT_MEM_DATA] == NULL || ext->mem_size[LLEXT_MEM_DATA] == 0) {
            LOG_WRN("No DATA region available, skipping address patching");
            continue;
        }

        uintptr_t *data = (uintptr_t *)ext->mem[LLEXT_MEM_DATA];
        size_t data_size = ext->mem_size[LLEXT_MEM_DATA];
        size_t data_entries = data_size / sizeof(uintptr_t);
        int patched = 0;

        LOG_DBG("DATA base=%p, size=%zu, entries=%zu, searching [0x%08lx..0x%08lx)",
                (void *)data, data_size, data_entries,
                (unsigned long)xip_addr, (unsigned long)(xip_addr + sect_size));

#ifdef CONFIG_ARM
        /* Count how many DATA entries need trampolines */
        int num_tramps = 0;

        for (size_t d = 0; d < data_entries; d++) {
            uintptr_t addr = data[d] & ~1UL;

            if (addr >= xip_addr && addr < xip_addr + sect_size) {
                num_tramps++;
            }
        }

        if (num_tramps > 0) {
            /* Allocate iflash space for trampolines (after the code) */
            uint32_t tramp_total = ROUND_UP(num_tramps * TRAMPOLINE_SIZE, 4);
            uint32_t tramp_aligned = SECTOR_ALIGN(tramp_total);

            if (iflash_next_offset + tramp_aligned > iflash_partition_size) {
                LOG_ERR("Internal flash: not enough space for %d trampolines",
                        num_tramps);
                return -ENOMEM;
            }

            /* Open partition, erase trampoline sectors, write trampolines */
            ret = flash_area_open(IFLASH_PARTITION_ID, &fa);
            if (ret < 0) {
                LOG_ERR("Failed to open flash for trampolines: %d", ret);
                return ret;
            }

            ret = flash_area_erase(fa, iflash_next_offset, tramp_aligned);
            if (ret < 0) {
                LOG_ERR("Trampoline erase failed: %d", ret);
                flash_area_close(fa);
                return ret;
            }

            /* Generate R9-restoring trampolines and patch DATA entries */
            int tramp_idx = 0;

            for (size_t d = 0; d < data_entries; d++) {
                uintptr_t addr = data[d] & ~1UL;

                if (addr >= xip_addr && addr < xip_addr + sect_size) {
                    uintptr_t old_val = data[d];
                    uintptr_t thumb_bit = old_val & 1UL;
                    uintptr_t iflash_func = iflash_addr + (addr - xip_addr) + thumb_bit;

                    /* Build trampoline: code + GOT base + target */
                    uint32_t tramp_off = iflash_next_offset +
                                         tramp_idx * TRAMPOLINE_SIZE;
                    uintptr_t tramp_cpu = IFLASH_CPU_ADDR(
                        IFLASH_PARTITION_OFFSET + tramp_off);
                    uint8_t tramp[TRAMPOLINE_SIZE];
                    uint32_t gb = (uint32_t)(uintptr_t)got_base;
                    uint32_t ta = (uint32_t)iflash_func;

                    memcpy(tramp, trampoline_code, sizeof(trampoline_code));
                    memcpy(tramp + 8, &gb, 4);
                    memcpy(tramp + 12, &ta, 4);

                    ret = flash_write_aligned(fa, tramp_off,
                                              tramp, TRAMPOLINE_SIZE);
                    if (ret < 0) {
                        LOG_ERR("Trampoline write failed: %d", ret);
                        flash_area_close(fa);
                        return ret;
                    }

                    data[d] = tramp_cpu | 1; /* Thumb bit */
                    patched++;
                    tramp_idx++;
                    LOG_DBG("DATA[%zu]: 0x%08lx -> tramp 0x%08lx -> func 0x%08lx",
                            d, (unsigned long)old_val,
                            (unsigned long)(tramp_cpu | 1),
                            (unsigned long)iflash_func);
                }
            }

            flash_area_close(fa);
            iflash_next_offset += tramp_aligned;
        }
#else
        /* Non-ARM: direct remapping (no R9 needed) */
        for (size_t d = 0; d < data_entries; d++) {
            /* Clear Thumb bit for address comparison */
            uintptr_t addr = data[d] & ~1UL;
            if (addr >= xip_addr && addr < xip_addr + sect_size) {
                uintptr_t old_val = data[d];
                uintptr_t thumb_bit = old_val & 1UL;
                uintptr_t new_val = iflash_addr + (addr - xip_addr) + thumb_bit;

                data[d] = new_val;
                patched++;
                LOG_DBG("DATA[%zu] (@%p): 0x%08lx -> 0x%08lx",
                        d, (void *)&data[d],
                        (unsigned long)old_val, (unsigned long)new_val);
            }
        }
#endif /* CONFIG_ARM */

        LOG_INF("Patched %d DATA entries for %s", patched, IFLASH_SECTION_NAME);
    }

    if (!found_iflash) {
        LOG_DBG("No %s sections found - all code stays in XIP", IFLASH_SECTION_NAME);
    }

    /* Flush data cache to ensure patched addresses are visible */
    if (found_iflash && ext->mem[LLEXT_MEM_DATA] != NULL) {
        sys_cache_data_flush_range(
            ext->mem[LLEXT_MEM_DATA],
            ext->mem_size[LLEXT_MEM_DATA]);
    }

    /* Invalidate instruction cache for internal flash region */
    if (found_iflash) {
        sys_cache_instr_invd_all();
    }

    return 0;
}

void zsw_llext_iflash_reset(void)
{
    iflash_next_offset = 0;
    LOG_DBG("Internal flash allocator reset");
}
