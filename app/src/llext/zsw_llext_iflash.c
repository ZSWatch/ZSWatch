/*
 * Copyright (c) 2026 ZSWatch Project
 * SPDX-License-Identifier: Apache-2.0
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

#include "llext/zsw_llext_iflash.h"

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

/* Runtime trampoline sector allocator.
 * Erases one 4 KB sector on first use, then packs up to
 * IFLASH_SECTOR_SIZE / TRAMPOLINE_SIZE = 256 trampolines per sector. */
static uint32_t runtime_tramp_sector = UINT32_MAX; /* partition-relative offset */
static uint32_t runtime_tramp_used;                 /* bytes used in current sector */

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
    int ret = 0;
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

#ifdef CONFIG_ARM
/**
 * Core trampoline creation logic. Writes a 16-byte R9-restoring trampoline
 * to internal flash, packing multiple trampolines into shared 4 KB sectors.
 *
 * Used by both:
 * - zsw_llext_iflash_install() at load time (patches static DATA entries)
 * - zsw_llext_create_trampoline() at runtime (wraps dynamic callbacks)
 *
 * @param func      Target function address (with Thumb bit if applicable)
 * @param got_base  GOT base address for this LLEXT
 * @return Trampoline address (Thumb-bit set), or NULL on failure
 */
static void *create_trampoline_with_got(void *func, void *got_base)
{
    const struct flash_area *fa;
    int ret = flash_area_open(IFLASH_PARTITION_ID, &fa);

    if (ret < 0) {
        LOG_ERR("Failed to open flash for trampoline: %d", ret);
        return NULL;
    }

    /* Allocate and erase a new sector if the current one is full or unset */
    if (runtime_tramp_sector == UINT32_MAX ||
        runtime_tramp_used + TRAMPOLINE_SIZE > IFLASH_SECTOR_SIZE) {

        if (iflash_next_offset + IFLASH_SECTOR_SIZE > iflash_partition_size) {
            LOG_ERR("No iflash space for trampoline sector");
            flash_area_close(fa);
            return NULL;
        }

        ret = flash_area_erase(fa, iflash_next_offset, IFLASH_SECTOR_SIZE);
        if (ret < 0) {
            LOG_ERR("Failed to erase trampoline sector: %d", ret);
            flash_area_close(fa);
            return NULL;
        }

        runtime_tramp_sector = iflash_next_offset;
        runtime_tramp_used = 0;
        iflash_next_offset += IFLASH_SECTOR_SIZE;

        LOG_INF("Allocated trampoline sector at 0x%x",
                IFLASH_PARTITION_OFFSET + runtime_tramp_sector);
    }

    /* Build 16-byte trampoline: set R9 to GOT base, then jump to func */
    uint8_t tramp[TRAMPOLINE_SIZE];
    uint32_t gb = (uint32_t)(uintptr_t)got_base;
    uint32_t ta = (uint32_t)(uintptr_t)func;

    memcpy(tramp, trampoline_code, sizeof(trampoline_code));
    memcpy(tramp + 8, &gb, 4);
    memcpy(tramp + 12, &ta, 4);

    /* Write trampoline into the pre-erased sector */
    uint32_t write_offset = runtime_tramp_sector + runtime_tramp_used;

    ret = flash_write_aligned(fa, write_offset, tramp, TRAMPOLINE_SIZE);
    flash_area_close(fa);

    if (ret < 0) {
        LOG_ERR("Failed to write trampoline: %d", ret);
        return NULL;
    }

    uintptr_t tramp_cpu = IFLASH_CPU_ADDR(IFLASH_PARTITION_OFFSET + write_offset);

    runtime_tramp_used += TRAMPOLINE_SIZE;

    sys_cache_instr_invd_all();

    LOG_DBG("Trampoline: func %p -> tramp 0x%08lx (GOT %p)",
            func, (unsigned long)(tramp_cpu | 1), got_base);

    return (void *)(tramp_cpu | 1); /* Thumb bit set */
}
#endif /* CONFIG_ARM */

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
        /* Create R9-restoring trampolines for each DATA entry referencing
         * the iflash section. Uses the shared sector-packing allocator. */
        for (size_t d = 0; d < data_entries; d++) {
            uintptr_t addr = data[d] & ~1UL;

            if (addr >= xip_addr && addr < xip_addr + sect_size) {
                uintptr_t old_val = data[d];
                uintptr_t thumb_bit = old_val & 1UL;
                uintptr_t iflash_func = iflash_addr + (addr - xip_addr) + thumb_bit;

                void *tramp = create_trampoline_with_got(
                                  (void *)iflash_func, got_base);
                if (tramp == NULL) {
                    LOG_ERR("Failed to create trampoline for DATA[%zu]", d);
                    return -ENOMEM;
                }

                data[d] = (uintptr_t)tramp;
                patched++;
                LOG_DBG("DATA[%zu]: 0x%08lx -> tramp %p -> func 0x%08lx",
                        d, (unsigned long)old_val, tramp,
                        (unsigned long)iflash_func);
            }
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
    runtime_tramp_sector = UINT32_MAX;
    runtime_tramp_used = 0;
    LOG_DBG("Internal flash allocator reset");
}

/* --------------------------------------------------------------------------
 * Runtime trampoline creation (callable from LLEXT apps)
 * -------------------------------------------------------------------------- */

#ifdef CONFIG_ARM
void *zsw_llext_create_trampoline(void *func)
{
    if (func == NULL) {
        return NULL;
    }

    /* Read R9 — valid because caller is LLEXT code with R9 = GOT base.
     * Firmware is compiled with -ffixed-r9 so R9 is preserved through
     * the call into this firmware function. */
    void *got_base;

    __asm__ volatile("mov %0, r9" : "=r"(got_base));

    return create_trampoline_with_got(func, got_base);
}

#else
void *zsw_llext_create_trampoline(void *func)
{
    /* Non-ARM: no R9/PIC trampoline needed */
    return func;
}
#endif /* CONFIG_ARM */
