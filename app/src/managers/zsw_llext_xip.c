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

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/flash/nrf_qspi_nor.h>
#include <zephyr/llext/llext.h>
#include <zephyr/llext/elf.h>
#include <zephyr/cache.h>
#include <string.h>

#include "managers/zsw_llext_xip.h"

LOG_MODULE_REGISTER(llext_xip, LOG_LEVEL_INF);

/*
 * Access the LLEXT heap directly for freeing relocated sections.
 * llext_free() is a static inline in a private header (llext_priv.h),
 * so we reference the heap symbol directly.  On non-Harvard ARM (nRF5340),
 * llext_data_heap and llext_instr_heap are both aliases for llext_heap.
 */
extern struct k_heap llext_heap;

static inline void xip_llext_free(void *ptr)
{
    k_heap_free(&llext_heap, ptr);
}

/* --------------------------------------------------------------------------
 * XIP Partition Configuration
 * -------------------------------------------------------------------------- */

#define XIP_PARTITION_ID    FIXED_PARTITION_ID(llext_xip_partition)
#define XIP_PARTITION_OFFSET FIXED_PARTITION_OFFSET(llext_xip_partition)

/* CPU address corresponding to the start of the XIP partition */
#define XIP_PARTITION_CPU_ADDR  (ZSW_XIP_BASE_ADDR + XIP_PARTITION_OFFSET)

/* Round up to sector boundary */
#define SECTOR_ALIGN(x) ROUND_UP((x), ZSW_XIP_SECTOR_SIZE)

/* --------------------------------------------------------------------------
 * Linear Allocator State (rebuilt each boot, not persisted)
 * -------------------------------------------------------------------------- */

struct xip_alloc_entry {
    char name[16];
    uint32_t offset;        /* Offset within XIP partition */
    uint32_t text_size;     /* Sector-aligned size of .text */
    uint32_t rodata_size;   /* Sector-aligned size of .rodata */
    uint32_t total_size;    /* text_size + rodata_size */
};

static struct xip_alloc_entry xip_allocs[ZSW_XIP_MAX_APPS];
static int xip_alloc_count;
static uint32_t xip_next_offset;    /* Next free offset in XIP partition */
static uint32_t xip_partition_size; /* Total partition size (from flash_area) */

/* QSPI flash device for XIP enable/disable */
static const struct device *qspi_dev;

/* --------------------------------------------------------------------------
 * ELF Section-to-LLEXT Memory Region Mapping
 *
 * Maps ELF section indices to LLEXT memory regions. Built during relocation
 * adjustment by reading the ELF section headers and matching them to the
 * regions LLEXT would have assigned.
 * -------------------------------------------------------------------------- */

/** Map ELF section index to LLEXT_MEM region (or -1 if not mapped) */
#define MAX_ELF_SECTIONS 20

/* --------------------------------------------------------------------------
 * Forward declarations
 * -------------------------------------------------------------------------- */

static int xip_alloc_space(const char *name, size_t text_size, size_t rodata_size,
                           uint32_t *out_text_offset, uint32_t *out_rodata_offset);
static int adjust_relocations(struct llext *ext, const char *elf_path,
                              uintptr_t xip_text, uintptr_t xip_rodata);
static int write_section_to_xip(const struct flash_area *fa, uint32_t offset,
                                const void *data, size_t size);

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

int zsw_llext_xip_init(void)
{
    const struct flash_area *fa;
    int ret;

    qspi_dev = DEVICE_DT_GET_OR_NULL(DT_CHOSEN(nordic_pm_ext_flash));

    ret = flash_area_open(XIP_PARTITION_ID, &fa);
    if (ret < 0) {
        LOG_ERR("Failed to open XIP partition: %d", ret);
        return ret;
    }

    xip_partition_size = fa->fa_size;
    flash_area_close(fa);

    xip_alloc_count = 0;
    xip_next_offset = 0;

    LOG_INF("XIP allocator init: partition at flash 0x%x, CPU 0x%08x, size %u KB",
            XIP_PARTITION_OFFSET, XIP_PARTITION_CPU_ADDR, xip_partition_size / 1024);

    return 0;
}

int zsw_llext_xip_install(struct llext *ext, const char *elf_path)
{
    const struct flash_area *fa;
    int ret;

    /* 1. Check that .text and .rodata are currently in heap (RAM) */
    if (!ext->mem[LLEXT_MEM_TEXT] || !ext->mem_on_heap[LLEXT_MEM_TEXT]) {
        LOG_ERR("XIP install: .text not in heap");
        return -EINVAL;
    }

    size_t text_size = ext->mem_size[LLEXT_MEM_TEXT];
    size_t rodata_size = ext->mem_size[LLEXT_MEM_RODATA];
    void *ram_text = ext->mem[LLEXT_MEM_TEXT];
    void *ram_rodata = ext->mem[LLEXT_MEM_RODATA];

    LOG_INF("XIP install '%s': .text=%zu bytes @ %p, .rodata=%zu bytes @ %p",
            ext->name, text_size, ram_text, rodata_size, ram_rodata);

    /* 2. Allocate XIP space */
    uint32_t text_xip_offset, rodata_xip_offset;
    ret = xip_alloc_space(ext->name, text_size, rodata_size,
                          &text_xip_offset, &rodata_xip_offset);
    if (ret < 0) {
        return ret;
    }

    uintptr_t xip_text_addr = XIP_PARTITION_CPU_ADDR + text_xip_offset;
    uintptr_t xip_rodata_addr = (rodata_size > 0) ?
        (XIP_PARTITION_CPU_ADDR + rodata_xip_offset) : 0;

    LOG_INF("XIP addresses: .text=0x%08lx, .rodata=0x%08lx",
            (unsigned long)xip_text_addr,
            (unsigned long)xip_rodata_addr);

    /* 3. Adjust relocations in RAM copies for XIP target addresses */
    ret = adjust_relocations(ext, elf_path, xip_text_addr, xip_rodata_addr);
    if (ret < 0) {
        LOG_ERR("Failed to adjust relocations: %d", ret);
        return ret;
    }

    /* 4. Write sections to XIP flash */
    ret = flash_area_open(XIP_PARTITION_ID, &fa);
    if (ret < 0) {
        LOG_ERR("Failed to open XIP partition for write: %d", ret);
        return ret;
    }

    /* Temporarily disable XIP for flash write operations */
    if (qspi_dev && device_is_ready(qspi_dev)) {
        nrf_qspi_nor_xip_enable(qspi_dev, false);
    }

    ret = write_section_to_xip(fa, text_xip_offset, ram_text, text_size);
    if (ret < 0) {
        LOG_ERR("Failed to write .text to XIP: %d", ret);
        goto xip_restore;
    }

    if (rodata_size > 0 && ram_rodata) {
        ret = write_section_to_xip(fa, rodata_xip_offset, ram_rodata, rodata_size);
        if (ret < 0) {
            LOG_ERR("Failed to write .rodata to XIP: %d", ret);
            goto xip_restore;
        }
    }

xip_restore:
    /* Re-enable XIP */
    if (qspi_dev && device_is_ready(qspi_dev)) {
        nrf_qspi_nor_xip_enable(qspi_dev, true);
    }

    /* Ensure instruction cache sees the new XIP content */
    sys_cache_instr_invd_all();

    flash_area_close(fa);

    if (ret < 0) {
        return ret;
    }

    /* 5. Update LLEXT struct to point to XIP, free RAM copies */
    void *old_text = ext->mem[LLEXT_MEM_TEXT];
    ext->mem[LLEXT_MEM_TEXT] = (void *)xip_text_addr;
    ext->mem_on_heap[LLEXT_MEM_TEXT] = false;
    ext->alloc_size -= ROUND_UP(text_size, sizeof(void *));

    LOG_INF("XIP: .text moved %p -> 0x%08lx", old_text, (unsigned long)xip_text_addr);
    xip_llext_free(old_text);

    if (rodata_size > 0 && ram_rodata) {
        void *old_rodata = ext->mem[LLEXT_MEM_RODATA];
        ext->mem[LLEXT_MEM_RODATA] = (void *)xip_rodata_addr;
        ext->mem_on_heap[LLEXT_MEM_RODATA] = false;
        ext->alloc_size -= ROUND_UP(rodata_size, sizeof(void *));

        LOG_INF("XIP: .rodata moved %p -> 0x%08lx",
                old_rodata, (unsigned long)xip_rodata_addr);
        xip_llext_free(old_rodata);
    }

    /* 6. Adjust symbol table entries that point into moved sections.
     * Both the address (addr) AND name pointer need adjustment if
     * they reside in the moved regions.
     */
    uintptr_t ram_text_base = (uintptr_t)old_text;
    uintptr_t ram_rodata_base = (uintptr_t)ram_rodata;
    intptr_t text_delta = (intptr_t)xip_text_addr - (intptr_t)ram_text_base;
    intptr_t rodata_delta = (rodata_size > 0) ?
        ((intptr_t)xip_rodata_addr - (intptr_t)ram_rodata_base) : 0;

    /* Helper macro to adjust a pointer if it falls in a moved range */
    #define ADJUST_PTR(ptr, type) do { \
        uintptr_t _a = (uintptr_t)(ptr); \
        if (_a >= ram_text_base && _a < ram_text_base + text_size) { \
            (ptr) = (type)(void *)(_a + text_delta); \
        } else if (rodata_size > 0 && \
                   _a >= ram_rodata_base && _a < ram_rodata_base + rodata_size) { \
            (ptr) = (type)(void *)(_a + rodata_delta); \
        } \
    } while (0)

    /* Adjust export table */
    for (size_t i = 0; i < ext->exp_tab.sym_cnt; i++) {
        ADJUST_PTR(ext->exp_tab.syms[i].addr, void *);
        ADJUST_PTR(ext->exp_tab.syms[i].name, const char *);
        LOG_DBG("XIP: exp_tab[%zu] '%s' addr=%p",
                i, ext->exp_tab.syms[i].name, ext->exp_tab.syms[i].addr);
    }

    /* Adjust global symbol table */
    for (size_t i = 0; i < ext->sym_tab.sym_cnt; i++) {
        ADJUST_PTR(ext->sym_tab.syms[i].addr, void *);
        ADJUST_PTR(ext->sym_tab.syms[i].name, const char *);
    }

    #undef ADJUST_PTR

    LOG_INF("XIP install '%s' complete: freed ~%zu bytes from heap",
            ext->name, text_size + rodata_size);

    return 0;
}

/* --------------------------------------------------------------------------
 * XIP Linear Allocator
 * -------------------------------------------------------------------------- */

static int xip_alloc_space(const char *name, size_t text_size, size_t rodata_size,
                           uint32_t *out_text_offset, uint32_t *out_rodata_offset)
{
    if (xip_alloc_count >= ZSW_XIP_MAX_APPS) {
        LOG_ERR("XIP allocator: max apps reached");
        return -ENOMEM;
    }

    uint32_t aligned_text = SECTOR_ALIGN(text_size);
    uint32_t aligned_rodata = SECTOR_ALIGN(rodata_size);
    uint32_t total = aligned_text + aligned_rodata;

    if (xip_next_offset + total > xip_partition_size) {
        LOG_ERR("XIP allocator: not enough space (need %u, have %u)",
                total, xip_partition_size - xip_next_offset);
        return -ENOSPC;
    }

    *out_text_offset = xip_next_offset;
    *out_rodata_offset = xip_next_offset + aligned_text;

    struct xip_alloc_entry *e = &xip_allocs[xip_alloc_count++];
    strncpy(e->name, name, sizeof(e->name) - 1);
    e->offset = xip_next_offset;
    e->text_size = aligned_text;
    e->rodata_size = aligned_rodata;
    e->total_size = total;

    xip_next_offset += total;

    LOG_INF("XIP alloc '%s': offset=0x%x, text=%u, rodata=%u, total=%u",
            name, e->offset, aligned_text, aligned_rodata, total);

    return 0;
}

/* --------------------------------------------------------------------------
 * Flash Write Helper
 * -------------------------------------------------------------------------- */

static int write_section_to_xip(const struct flash_area *fa, uint32_t offset,
                                const void *data, size_t size)
{
    int ret;
    uint32_t erase_size = SECTOR_ALIGN(size);

    /* Flash write size must be aligned to write block size (4 bytes for QSPI NOR) */
    size_t write_size = ROUND_UP(size, 4);

    LOG_INF("XIP flash: erasing %u bytes at partition offset 0x%x", erase_size, offset);
    ret = flash_area_erase(fa, offset, erase_size);
    if (ret < 0) {
        LOG_ERR("Flash erase failed at offset 0x%x: %d", offset, ret);
        return ret;
    }

    LOG_INF("XIP flash: writing %zu bytes (padded to %zu) at partition offset 0x%x",
            size, write_size, offset);

    if (write_size == size) {
        /* Size is already aligned, write directly */
        ret = flash_area_write(fa, offset, data, write_size);
    } else {
        /* Need to pad to write block alignment. Since the erase fills with 0xFF,
         * we write the actual data and let the padding be 0xFF from erase. But
         * flash_area_write requires the buffer to contain all bytes to write.
         * Use a small stack buffer for the trailing partial word. */
        size_t aligned_part = size & ~3U;

        if (aligned_part > 0) {
            ret = flash_area_write(fa, offset, data, aligned_part);
            if (ret < 0) {
                LOG_ERR("Flash write (aligned) failed: %d", ret);
                return ret;
            }
        }

        /* Write the remaining 1-3 bytes padded to 4 */
        size_t remainder = size - aligned_part;
        if (remainder > 0) {
            uint8_t pad_buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
            memcpy(pad_buf, (const uint8_t *)data + aligned_part, remainder);
            ret = flash_area_write(fa, offset + aligned_part, pad_buf, 4);
        }
    }

    if (ret < 0) {
        LOG_ERR("Flash write failed at offset 0x%x: %d", offset, ret);
        return ret;
    }

    return 0;
}

/* --------------------------------------------------------------------------
 * ELF Relocation Adjustment
 *
 * Re-reads the ELF file to find relocation entries. For each R_ARM_ABS32
 * relocation whose target symbol belongs to a section being moved to XIP,
 * adjusts the 4-byte value in the RAM copy of the source section.
 * -------------------------------------------------------------------------- */

/**
 * Map an ELF section index to an LLEXT memory region.
 *
 * Uses the section header flags to determine the equivalent LLEXT_MEM_* region,
 * matching the logic in llext_load.c's section classification.
 */
static int section_to_mem_idx(const elf_shdr_t *shdr)
{
    switch (shdr->sh_type) {
    case SHT_NOBITS:
        return LLEXT_MEM_BSS;
    case SHT_PROGBITS:
        if (shdr->sh_flags & SHF_EXECINSTR) {
            return LLEXT_MEM_TEXT;
        } else if (shdr->sh_flags & SHF_WRITE) {
            return LLEXT_MEM_DATA;
        } else {
            return LLEXT_MEM_RODATA;
        }
    default:
        return -1;
    }
}

/**
 * Read bytes from an ELF file at a given offset.
 */
static int elf_read_at(struct fs_file_t *file, off_t offset, void *buf, size_t len)
{
    int ret = fs_seek(file, offset, FS_SEEK_SET);

    if (ret < 0) {
        return ret;
    }
    ssize_t n = fs_read(file, buf, len);

    if (n < 0) {
        return (int)n;
    }
    if ((size_t)n != len) {
        return -EIO;
    }
    return 0;
}

static int adjust_relocations(struct llext *ext, const char *elf_path,
                              uintptr_t xip_text, uintptr_t xip_rodata)
{
    struct fs_file_t file;
    elf_ehdr_t ehdr;
    int ret;
    int adj_count = 0;

    uintptr_t ram_text = (uintptr_t)ext->mem[LLEXT_MEM_TEXT];
    uintptr_t ram_rodata = (uintptr_t)ext->mem[LLEXT_MEM_RODATA];
    size_t text_size = ext->mem_size[LLEXT_MEM_TEXT];
    size_t rodata_size = ext->mem_size[LLEXT_MEM_RODATA];

    intptr_t text_delta = (intptr_t)xip_text - (intptr_t)ram_text;
    intptr_t rodata_delta = (rodata_size > 0) ?
        ((intptr_t)xip_rodata - (intptr_t)ram_rodata) : 0;

    LOG_INF("Reloc adjust: text_delta=0x%lx, rodata_delta=0x%lx",
            (unsigned long)text_delta, (unsigned long)rodata_delta);

    /* Open ELF file */
    fs_file_t_init(&file);
    ret = fs_open(&file, elf_path, FS_O_READ);
    if (ret < 0) {
        LOG_ERR("Failed to open ELF for relocation: %d", ret);
        return ret;
    }

    /* Read ELF header */
    ret = elf_read_at(&file, 0, &ehdr, sizeof(ehdr));
    if (ret < 0) {
        LOG_ERR("Failed to read ELF header: %d", ret);
        goto out;
    }

    /* Read all section headers */
    uint16_t shnum = ehdr.e_shnum;
    if (shnum > MAX_ELF_SECTIONS) {
        LOG_ERR("Too many ELF sections: %u (max %d)", shnum, MAX_ELF_SECTIONS);
        ret = -E2BIG;
        goto out;
    }

    elf_shdr_t shdrs[MAX_ELF_SECTIONS];
    ret = elf_read_at(&file, ehdr.e_shoff, shdrs, shnum * sizeof(elf_shdr_t));
    if (ret < 0) {
        LOG_ERR("Failed to read section headers: %d", ret);
        goto out;
    }

    /* Read section header string table for name-based section identification */
    char shstrtab[256];
    uint16_t shstrndx = ehdr.e_shstrndx;
    size_t shstrtab_size = 0;

    if (shstrndx < shnum && shdrs[shstrndx].sh_size <= sizeof(shstrtab)) {
        shstrtab_size = shdrs[shstrndx].sh_size;
        ret = elf_read_at(&file, shdrs[shstrndx].sh_offset, shstrtab, shstrtab_size);
        if (ret < 0) {
            LOG_WRN("Failed to read .shstrtab, using flag-based mapping only");
            shstrtab_size = 0;
        }
    }

    /*
     * Build section-index → LLEXT mem_idx mapping.
     * We need precise mapping because some sections (like .exported_sym)
     * have the same ELF flags as .rodata but are classified differently by LLEXT.
     * Track which ELF section is the "primary" for each LLEXT region (first encountered).
     */
    int8_t sect_mem_map[MAX_ELF_SECTIONS];
    int primary_sect[LLEXT_MEM_COUNT];

    memset(sect_mem_map, -1, sizeof(sect_mem_map));
    memset(primary_sect, -1, sizeof(primary_sect));

    for (int i = 0; i < shnum; i++) {
        /* Check section name for special classifications */
        const char *sec_name = NULL;

        if (shstrtab_size > 0 && shdrs[i].sh_name < shstrtab_size) {
            sec_name = &shstrtab[shdrs[i].sh_name];
        }

        /* .exported_sym → LLEXT_MEM_EXPORT (special case) */
        if (sec_name && strcmp(sec_name, ".exported_sym") == 0) {
            sect_mem_map[i] = LLEXT_MEM_EXPORT;
            continue;
        }

        int mem_idx = section_to_mem_idx(&shdrs[i]);
        if (mem_idx < 0) {
            continue;
        }

        sect_mem_map[i] = (int8_t)mem_idx;

        /* Record the first (primary) section for each LLEXT region */
        if (primary_sect[mem_idx] == -1) {
            primary_sect[mem_idx] = i;
        }
    }

    /*
     * Only process relocations for sections that are the PRIMARY section
     * for their LLEXT memory region. This avoids misidentifying offsets
     * when multiple ELF sections map to the same region type.
     * We only adjust .text and .data — export table is adjusted separately in step 6.
     */
    for (int s = 0; s < shnum; s++) {
        if (shdrs[s].sh_type != SHT_REL) {
            continue;
        }

        /* sh_info = index of section these relocs apply to */
        uint32_t target_sect_idx = shdrs[s].sh_info;
        if (target_sect_idx >= shnum) {
            continue;
        }

        int source_mem_idx = sect_mem_map[target_sect_idx];

        /* Only process relocs for TEXT and DATA regions — these are
         * the sections where we need to patch absolute addresses in-place.
         * EXPORT table is handled separately via exp_tab/sym_tab adjustment. */
        if (source_mem_idx != LLEXT_MEM_TEXT && source_mem_idx != LLEXT_MEM_DATA) {
            LOG_DBG("Skipping relocs for section %d (mem_idx=%d)", target_sect_idx, source_mem_idx);
            continue;
        }

        /* Verify this is the primary section for its region */
        if (primary_sect[source_mem_idx] != (int)target_sect_idx) {
            LOG_DBG("Skipping non-primary section %d for region %d", target_sect_idx, source_mem_idx);
            continue;
        }

        void *source_base = ext->mem[source_mem_idx];
        if (!source_base) {
            continue;
        }

        /* sh_link = symbol table section index */
        uint32_t symtab_idx = shdrs[s].sh_link;
        if (symtab_idx >= shnum || shdrs[symtab_idx].sh_type != SHT_SYMTAB) {
            continue;
        }

        uint32_t rel_count = shdrs[s].sh_size / sizeof(elf_rel_t);
        uint32_t sym_count = shdrs[symtab_idx].sh_size / sizeof(elf_sym_t);

        LOG_INF("Processing %u relocs for section %d (mem_idx=%d)",
                rel_count, target_sect_idx, source_mem_idx);

        /* Process relocations in batches to limit stack usage */
        #define REL_BATCH_SIZE 32
        #define SYM_CACHE_SIZE 8

        /* Cache for recently looked-up symbols */
        struct {
            uint32_t idx;
            int8_t mem_idx;
        } sym_cache[SYM_CACHE_SIZE];
        int sym_cache_count = 0;

        for (uint32_t batch_start = 0; batch_start < rel_count; batch_start += REL_BATCH_SIZE) {
            uint32_t batch_count = MIN(rel_count - batch_start, REL_BATCH_SIZE);
            elf_rel_t rels[REL_BATCH_SIZE];

            ret = elf_read_at(&file,
                              shdrs[s].sh_offset + batch_start * sizeof(elf_rel_t),
                              rels, batch_count * sizeof(elf_rel_t));
            if (ret < 0) {
                LOG_ERR("Failed to read relocation batch: %d", ret);
                goto out;
            }

            for (uint32_t r = 0; r < batch_count; r++) {
                uint32_t sym_idx = ELF32_R_SYM(rels[r].r_info);
                uint32_t rel_type = ELF32_R_TYPE(rels[r].r_info);

                /* Only handle R_ARM_ABS32 */
                if (rel_type != 2 /* R_ARM_ABS32 */) {
                    continue;
                }

                if (sym_idx >= sym_count) {
                    continue;
                }

                /* Look up which section the target symbol belongs to */
                int8_t target_mem = -1;

                /* Check cache first */
                for (int c = 0; c < sym_cache_count; c++) {
                    if (sym_cache[c].idx == sym_idx) {
                        target_mem = sym_cache[c].mem_idx;
                        break;
                    }
                }

                if (target_mem == -1) {
                    /* Read symbol from ELF */
                    elf_sym_t sym;
                    ret = elf_read_at(&file,
                                      shdrs[symtab_idx].sh_offset + sym_idx * sizeof(elf_sym_t),
                                      &sym, sizeof(sym));
                    if (ret < 0) {
                        continue;
                    }

                    /* Map symbol's section to LLEXT mem region */
                    if (sym.st_shndx > 0 && sym.st_shndx < shnum) {
                        target_mem = sect_mem_map[sym.st_shndx];
                    }

                    /* Add to cache (circular) */
                    if (sym_cache_count < SYM_CACHE_SIZE) {
                        sym_cache[sym_cache_count].idx = sym_idx;
                        sym_cache[sym_cache_count].mem_idx = target_mem;
                        sym_cache_count++;
                    } else {
                        int slot = batch_start % SYM_CACHE_SIZE;
                        sym_cache[slot].idx = sym_idx;
                        sym_cache[slot].mem_idx = target_mem;
                    }
                }

                /* Determine adjustment delta based on target section */
                intptr_t delta = 0;
                if (target_mem == LLEXT_MEM_TEXT) {
                    delta = text_delta;
                } else if (target_mem == LLEXT_MEM_RODATA && rodata_size > 0) {
                    delta = rodata_delta;
                } else {
                    continue; /* Target not being moved */
                }

                /* Apply adjustment to the 4-byte value in the source section */
                uint32_t rel_offset = rels[r].r_offset;

                if (rel_offset + 4 > ext->mem_size[source_mem_idx]) {
                    LOG_WRN("Reloc offset 0x%x out of bounds for region %d (size %zu)",
                            rel_offset, source_mem_idx, ext->mem_size[source_mem_idx]);
                    continue;
                }

                uint32_t *patch_addr = (uint32_t *)((uint8_t *)source_base + rel_offset);
                uint32_t old_val = *patch_addr;
                *patch_addr = (uint32_t)((intptr_t)old_val + delta);

                adj_count++;
                LOG_DBG("  reloc[%u]: offset=0x%x, val 0x%08x -> 0x%08x (delta=0x%lx)",
                        batch_start + r, rel_offset, old_val, *patch_addr,
                        (unsigned long)delta);
            }
        }
    }

    LOG_INF("Relocation adjustment complete: %d values adjusted", adj_count);
    ret = 0;

out:
    fs_close(&file);
    return ret;
}
