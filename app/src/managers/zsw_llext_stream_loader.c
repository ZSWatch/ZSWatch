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
 * @file zsw_llext_stream_loader.c
 * @brief Streaming ELF XIP loader — loads LLEXT apps directly to XIP + data pool.
 *
 * This replaces the two-pass approach (llext_load → zsw_llext_xip_install)
 * with a single-pass streaming load.  Peak RAM usage is bounded by the
 * scratch buffer size (typically 20-40 KB) regardless of how large the
 * app's .text or .rodata sections are.
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

#include "managers/zsw_llext_stream_loader.h"
#include "managers/zsw_llext_xip.h"

LOG_MODULE_REGISTER(llext_stream, LOG_LEVEL_INF);

/* --------------------------------------------------------------------------
 * Configuration
 * -------------------------------------------------------------------------- */

#define MAX_ELF_SECTIONS    20
#define STAGING_BUF_SIZE    4096    /* Must match XIP sector size */
#define SYM_CACHE_SIZE      16      /* LRU symbol resolution cache */
#define REL_BATCH_SIZE      64      /* Reloc entries read per batch */
#define MAX_STRTAB_SIZE     4096    /* Max symbol string table we buffer */
#define MAX_SHSTRTAB_SIZE   256     /* Max section name string table */

/* --------------------------------------------------------------------------
 * Scratch Arena — simple bump allocator within the caller-provided buffer
 * -------------------------------------------------------------------------- */

struct scratch_arena {
    uint8_t *buf;
    size_t size;
    size_t offset;
};

static void *arena_alloc(struct scratch_arena *a, size_t align, size_t size)
{
    size_t off = ROUND_UP(a->offset, align);

    if (off + size > a->size) {
        LOG_ERR("Scratch arena exhausted: need %zu at offset %zu (total %zu)",
                size, off, a->size);
        return NULL;
    }
    void *ptr = a->buf + off;

    a->offset = off + size;
    return ptr;
}

/* --------------------------------------------------------------------------
 * ELF Section Index Tracking
 * -------------------------------------------------------------------------- */

struct elf_section_indices {
    int text;               /* .text (PROGBITS, AX) */
    int data;               /* .data (PROGBITS, WA) */
    int bss;                /* .bss  (NOBITS, WA) */
    int rodata;             /* .rodata (PROGBITS, A) */
    int symtab;             /* .symtab */
    int strtab;             /* .strtab */
    int shstrtab;           /* .shstrtab (from ELF header) */
    int exported_sym;       /* .exported_sym */
    int rel_text;           /* .rel.text */
    int rel_data;           /* .rel.data */
    int rel_rodata;         /* .rel.rodata */
    int rel_exported_sym;   /* .rel.exported_sym */
};

/* --------------------------------------------------------------------------
 * Symbol Resolution Cache
 * -------------------------------------------------------------------------- */

struct sym_cache_entry {
    uint32_t sym_idx;
    uintptr_t resolved_addr;
};

/* --------------------------------------------------------------------------
 * Helpers — read from ELF file at a given offset
 * -------------------------------------------------------------------------- */

static int elf_read_at(struct fs_file_t *f, off_t offset, void *buf, size_t len)
{
    int ret = fs_seek(f, offset, FS_SEEK_SET);

    if (ret < 0) {
        return ret;
    }

    ssize_t n = fs_read(f, buf, len);

    if (n < 0) {
        return (int)n;
    }
    if ((size_t)n != len) {
        return -EIO;
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * Section Classification
 * -------------------------------------------------------------------------- */

static void classify_sections(const elf_shdr_t *shdrs, uint16_t shnum,
                              const char *shstrtab, size_t shstrtab_size,
                              struct elf_section_indices *si)
{
    memset(si, -1, sizeof(*si));

    for (int i = 0; i < shnum; i++) {
        const char *name = NULL;

        if (shdrs[i].sh_name < shstrtab_size) {
            name = &shstrtab[shdrs[i].sh_name];
        }
        if (!name) {
            continue;
        }

        if (strcmp(name, ".text") == 0) {
            si->text = i;
        } else if (strcmp(name, ".data") == 0) {
            si->data = i;
        } else if (strcmp(name, ".bss") == 0) {
            si->bss = i;
        } else if (strcmp(name, ".rodata") == 0) {
            si->rodata = i;
        } else if (strcmp(name, ".symtab") == 0) {
            si->symtab = i;
        } else if (strcmp(name, ".strtab") == 0) {
            si->strtab = i;
        } else if (strcmp(name, ".exported_sym") == 0) {
            si->exported_sym = i;
        } else if (strcmp(name, ".rel.text") == 0) {
            si->rel_text = i;
        } else if (strcmp(name, ".rel.data") == 0) {
            si->rel_data = i;
        } else if (strcmp(name, ".rel.rodata") == 0) {
            si->rel_rodata = i;
        } else if (strcmp(name, ".rel.exported_sym") == 0) {
            si->rel_exported_sym = i;
        }
    }
}

/* --------------------------------------------------------------------------
 * Symbol Resolution
 *
 * Resolves an ELF symbol to its final runtime address.
 * - UND symbols → kernel export table (llext_find_sym)
 * - Defined symbols → section base + st_value
 * -------------------------------------------------------------------------- */

static int resolve_symbol(struct fs_file_t *f, const elf_shdr_t *shdrs,
                          const struct elf_section_indices *si,
                          const char *strtab, size_t strtab_size,
                          const uintptr_t *sect_base, uint16_t shnum,
                          uint32_t sym_idx,
                          struct sym_cache_entry *cache, int *cache_count,
                          uintptr_t *out_addr)
{
    /* Check cache first */
    for (int c = 0; c < *cache_count; c++) {
        if (cache[c].sym_idx == sym_idx) {
            *out_addr = cache[c].resolved_addr;
            return 0;
        }
    }

    /* Read symbol from ELF */
    elf_sym_t sym;
    off_t sym_off = shdrs[si->symtab].sh_offset + (off_t)sym_idx * sizeof(elf_sym_t);
    int ret = elf_read_at(f, sym_off, &sym, sizeof(sym));

    if (ret < 0) {
        LOG_ERR("Failed to read symbol %u: %d", sym_idx, ret);
        return ret;
    }

    uintptr_t addr = 0;

    if (sym.st_shndx == SHN_UNDEF) {
        /* Undefined symbol — look up in kernel export table */
        if (sym.st_name >= strtab_size) {
            LOG_ERR("Symbol %u name index %u out of range (%zu)",
                    sym_idx, sym.st_name, strtab_size);
            return -ENOEXEC;
        }
        const char *name = &strtab[sym.st_name];
        const void *found = llext_find_sym(NULL, name);

        if (!found) {
            LOG_ERR("Undefined symbol '%s' not found in kernel exports", name);
            return -ENODATA;
        }
        addr = (uintptr_t)found;
    } else if (sym.st_shndx == SHN_ABS) {
        addr = sym.st_value;
    } else if (sym.st_shndx < shnum) {
        /* Defined in a section — use the section's final base address */
        addr = sect_base[sym.st_shndx] + sym.st_value;
    } else {
        LOG_ERR("Symbol %u has invalid section index %u", sym_idx, sym.st_shndx);
        return -ENOEXEC;
    }

    /* Add to cache (circular replacement) */
    if (*cache_count < SYM_CACHE_SIZE) {
        cache[*cache_count].sym_idx = sym_idx;
        cache[*cache_count].resolved_addr = addr;
        (*cache_count)++;
    } else {
        int slot = sym_idx % SYM_CACHE_SIZE;

        cache[slot].sym_idx = sym_idx;
        cache[slot].resolved_addr = addr;
    }

    *out_addr = addr;
    return 0;
}

/* --------------------------------------------------------------------------
 * Stream a section to XIP flash, applying R_ARM_ABS32 relocations.
 *
 * Reads the section data in STAGING_BUF_SIZE chunks, applies any relocations
 * that target offsets within the current chunk, and writes the patched chunk
 * to XIP flash.
 * -------------------------------------------------------------------------- */

static int stream_section_to_xip(struct fs_file_t *f, const elf_shdr_t *shdrs,
                                 const struct elf_section_indices *si,
                                 const char *strtab, size_t strtab_size,
                                 const uintptr_t *sect_base, uint16_t shnum,
                                 int section_idx, int rel_section_idx,
                                 const struct flash_area *fa,
                                 uint32_t xip_partition_offset,
                                 uint8_t *staging_buf)
{
    const elf_shdr_t *sect = &shdrs[section_idx];
    size_t section_size = sect->sh_size;
    off_t section_file_off = sect->sh_offset;
    int ret;

    /* Pre-read ALL reloc entries for this section (if any).
     * We process them per-chunk, so we need them sorted or scannable.
     * For simplicity, we scan the full reloc table per chunk.
     * With ~450 relocs and ~5 chunks, this is ~2250 iterations — fast.
     */
    uint32_t rel_count = 0;
    off_t rel_file_off = 0;

    if (rel_section_idx >= 0) {
        rel_count = shdrs[rel_section_idx].sh_size / sizeof(elf_rel_t);
        rel_file_off = shdrs[rel_section_idx].sh_offset;
    }

    LOG_INF("Streaming section %d (%zu bytes, %u relocs) to XIP offset 0x%x",
            section_idx, section_size, rel_count, xip_partition_offset);

    /* Symbol resolution cache for this section */
    struct sym_cache_entry sym_cache[SYM_CACHE_SIZE];
    int sym_cache_count = 0;

    for (size_t chunk_start = 0; chunk_start < section_size;
         chunk_start += STAGING_BUF_SIZE) {
        size_t chunk_size = MIN(section_size - chunk_start, STAGING_BUF_SIZE);

        /* Read chunk from ELF */
        ret = elf_read_at(f, section_file_off + chunk_start,
                          staging_buf, chunk_size);
        if (ret < 0) {
            LOG_ERR("Failed to read section chunk at offset %zu: %d",
                    chunk_start, ret);
            return ret;
        }

        /* Pad remainder with 0xFF (flash-friendly) for write alignment */
        if (chunk_size < STAGING_BUF_SIZE) {
            memset(staging_buf + chunk_size, 0xFF, STAGING_BUF_SIZE - chunk_size);
        }

        /* Apply relocations targeting this chunk */
        if (rel_count > 0) {
            elf_rel_t rels[REL_BATCH_SIZE];

            for (uint32_t batch_start = 0; batch_start < rel_count;
                 batch_start += REL_BATCH_SIZE) {
                uint32_t batch_cnt = MIN(rel_count - batch_start, REL_BATCH_SIZE);

                ret = elf_read_at(f, rel_file_off + batch_start * sizeof(elf_rel_t),
                                  rels, batch_cnt * sizeof(elf_rel_t));
                if (ret < 0) {
                    LOG_ERR("Failed to read relocs: %d", ret);
                    return ret;
                }

                for (uint32_t r = 0; r < batch_cnt; r++) {
                    uint32_t r_offset = rels[r].r_offset;
                    uint32_t rel_type = ELF32_R_TYPE(rels[r].r_info);

                    /* Supported: R_ARM_ABS32 (2), R_ARM_THM_CALL (10) */
                    if (rel_type != 2 && rel_type != 10) {
                        LOG_WRN("Unsupported reloc type %u at offset 0x%x",
                                rel_type, r_offset);
                        continue;
                    }

                    /* Check if this reloc targets the current chunk */
                    if (r_offset < chunk_start ||
                        r_offset + 4 > chunk_start + chunk_size) {
                        continue;
                    }

                    /* Resolve the symbol */
                    uint32_t sym_idx = ELF32_R_SYM(rels[r].r_info);
                    uintptr_t sym_addr;

                    ret = resolve_symbol(f, shdrs, si, strtab, strtab_size,
                                         sect_base, shnum, sym_idx,
                                         sym_cache, &sym_cache_count,
                                         &sym_addr);
                    if (ret < 0) {
                        return ret;
                    }

                    if (rel_type == 2) {
                        /* R_ARM_ABS32: *(uint32_t*)loc += sym_addr */
                        uint32_t *patch = (uint32_t *)(staging_buf + r_offset - chunk_start);
                        uint32_t old_val = *patch;

                        *patch = old_val + (uint32_t)sym_addr;

                        LOG_DBG("  ABS32: off=0x%x, sym=%u, 0x%08x -> 0x%08x",
                                r_offset, sym_idx, old_val, *patch);
                    } else {
                        /* R_ARM_THM_CALL: Thumb BL/BLX instruction (2x16-bit) */
                        uint16_t *instr = (uint16_t *)(staging_buf + r_offset - chunk_start);
                        uint16_t hi = instr[0];
                        uint16_t lo = instr[1];

                        /* Decode addend from current BL encoding */
                        uint32_t sign = (hi >> 10) & 1;
                        uint32_t j1 = (lo >> 13) & 1;
                        uint32_t j2 = (lo >> 11) & 1;
                        uint32_t i1 = !(j1 ^ sign);
                        uint32_t i2 = !(j2 ^ sign);
                        int32_t addend = (int32_t)((sign << 24) | (i1 << 23) |
                                                   (i2 << 22) |
                                                   ((hi & 0x3FF) << 12) |
                                                   ((lo & 0x7FF) << 1));
                        if (addend & BIT(24)) {
                            addend |= (int32_t)0xFE000000;
                        }

                        /* P = runtime address of instruction in XIP */
                        uintptr_t place = sect_base[section_idx] + r_offset;

                        /* ARM ELF ABI: result = (S + A) - P */
                        int32_t result = (int32_t)sym_addr + addend - (int32_t)place;

                        /* BL can reach ±16 MB */
                        if (result > 0x00FFFFFF || result < (int32_t)0xFF000000) {
                            LOG_ERR("THM_CALL out of range at 0x%x: delta=0x%08x",
                                    r_offset, result);
                            return -ERANGE;
                        }

                        /* Encode result into BL instruction */
                        uint32_t ns = (result >> 24) & 1;
                        uint32_t ni1 = (result >> 23) & 1;
                        uint32_t ni2 = (result >> 22) & 1;
                        uint32_t nj1 = !(ni1 ^ ns);
                        uint32_t nj2 = !(ni2 ^ ns);

                        instr[0] = (hi & 0xF800) | (ns << 10) |
                                   ((result >> 12) & 0x3FF);
                        instr[1] = (lo & 0xD000) | (nj1 << 13) |
                                   (nj2 << 11) | ((result >> 1) & 0x7FF);

                        LOG_DBG("  THM_CALL: off=0x%x, sym=%u, S=0x%lx P=0x%lx result=0x%08x",
                                r_offset, sym_idx, (unsigned long)sym_addr,
                                (unsigned long)place, result);
                    }
                }
            }
        }

        /* Write patched chunk to XIP flash */
        uint32_t flash_off = xip_partition_offset + chunk_start;
        size_t write_size = ROUND_UP(chunk_size, 4);
        uint32_t erase_size = ROUND_UP(chunk_size, ZSW_XIP_SECTOR_SIZE);

        ret = flash_area_erase(fa, flash_off, erase_size);
        if (ret < 0) {
            LOG_ERR("Flash erase at 0x%x failed: %d", flash_off, ret);
            return ret;
        }

        ret = flash_area_write(fa, flash_off, staging_buf, write_size);
        if (ret < 0) {
            LOG_ERR("Flash write at 0x%x failed: %d", flash_off, ret);
            return ret;
        }
    }

    return 0;
}

/* --------------------------------------------------------------------------
 * Apply relocations to an in-memory buffer (for .data, .exported_sym)
 * -------------------------------------------------------------------------- */

static int apply_relocs_to_buffer(struct fs_file_t *f, const elf_shdr_t *shdrs,
                                  const struct elf_section_indices *si,
                                  const char *strtab, size_t strtab_size,
                                  const uintptr_t *sect_base, uint16_t shnum,
                                  int rel_section_idx,
                                  void *buf, size_t buf_size)
{
    if (rel_section_idx < 0) {
        return 0;  /* No relocs for this section */
    }

    uint32_t rel_count = shdrs[rel_section_idx].sh_size / sizeof(elf_rel_t);
    off_t rel_file_off = shdrs[rel_section_idx].sh_offset;
    int ret;

    struct sym_cache_entry sym_cache[SYM_CACHE_SIZE];
    int sym_cache_count = 0;

    elf_rel_t rels[REL_BATCH_SIZE];

    for (uint32_t batch_start = 0; batch_start < rel_count;
         batch_start += REL_BATCH_SIZE) {
        uint32_t batch_cnt = MIN(rel_count - batch_start, REL_BATCH_SIZE);

        ret = elf_read_at(f, rel_file_off + batch_start * sizeof(elf_rel_t),
                          rels, batch_cnt * sizeof(elf_rel_t));
        if (ret < 0) {
            return ret;
        }

        for (uint32_t r = 0; r < batch_cnt; r++) {
            uint32_t r_offset = rels[r].r_offset;
            uint32_t rel_type = ELF32_R_TYPE(rels[r].r_info);

            if (rel_type != 2 && rel_type != 10) {
                LOG_WRN("Unsupported reloc type %u in buffer", rel_type);
                continue;
            }

            if (r_offset + 4 > buf_size) {
                LOG_WRN("Reloc offset 0x%x out of range (%zu)", r_offset, buf_size);
                continue;
            }

            uint32_t sym_idx = ELF32_R_SYM(rels[r].r_info);
            uintptr_t sym_addr;

            ret = resolve_symbol(f, shdrs, si, strtab, strtab_size,
                                 sect_base, shnum, sym_idx,
                                 sym_cache, &sym_cache_count, &sym_addr);
            if (ret < 0) {
                return ret;
            }

            if (rel_type == 2) {
                /* R_ARM_ABS32 */
                uint32_t *patch = (uint32_t *)((uint8_t *)buf + r_offset);

                *patch += (uint32_t)sym_addr;
            } else {
                /* R_ARM_THM_CALL: Thumb BL/BLX in data/exported_sym section */
                uint16_t *instr = (uint16_t *)((uint8_t *)buf + r_offset);
                uint16_t hi = instr[0];
                uint16_t lo = instr[1];

                uint32_t sign = (hi >> 10) & 1;
                uint32_t j1 = (lo >> 13) & 1;
                uint32_t j2 = (lo >> 11) & 1;
                uint32_t i1 = !(j1 ^ sign);
                uint32_t i2 = !(j2 ^ sign);
                int32_t addend = (int32_t)((sign << 24) | (i1 << 23) |
                                           (i2 << 22) |
                                           ((hi & 0x3FF) << 12) |
                                           ((lo & 0x7FF) << 1));
                if (addend & BIT(24)) {
                    addend |= (int32_t)0xFE000000;
                }

                uint32_t target_sect = shdrs[rel_section_idx].sh_info;
                uintptr_t place = sect_base[target_sect] + r_offset;
                int32_t result = (int32_t)sym_addr + addend - (int32_t)place;

                uint32_t ns = (result >> 24) & 1;
                uint32_t ni1 = (result >> 23) & 1;
                uint32_t ni2 = (result >> 22) & 1;
                uint32_t nj1 = !(ni1 ^ ns);
                uint32_t nj2 = !(ni2 ^ ns);

                instr[0] = (hi & 0xF800) | (ns << 10) |
                           ((result >> 12) & 0x3FF);
                instr[1] = (lo & 0xD000) | (nj1 << 13) |
                           (nj2 << 11) | ((result >> 1) & 0x7FF);
            }
        }
    }

    return 0;
}

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

int zsw_llext_stream_load(const char *elf_path, const char *entry_symbol,
                          void *scratch, size_t scratch_size,
                          struct zsw_stream_load_result *result)
{
    struct scratch_arena arena = {
        .buf = (uint8_t *)scratch,
        .size = scratch_size,
        .offset = 0,
    };
    struct fs_file_t file;
    elf_ehdr_t ehdr;
    int ret;

    if (scratch_size < ZSW_STREAM_SCRATCH_MIN) {
        LOG_ERR("Scratch buffer too small: %zu < %d", scratch_size, ZSW_STREAM_SCRATCH_MIN);
        return -EINVAL;
    }

    result->entry_fn = NULL;

    /* ----------------------------------------------------------------
     * Phase 1: Parse ELF metadata
     * ---------------------------------------------------------------- */

    fs_file_t_init(&file);
    ret = fs_open(&file, elf_path, FS_O_READ);
    if (ret < 0) {
        LOG_ERR("Failed to open ELF %s: %d", elf_path, ret);
        return ret;
    }

    /* Read ELF header */
    ret = elf_read_at(&file, 0, &ehdr, sizeof(ehdr));
    if (ret < 0) {
        LOG_ERR("Failed to read ELF header: %d", ret);
        goto out;
    }

    /* Basic validation */
    if (ehdr.e_type != ET_REL) {
        LOG_ERR("ELF is not ET_REL (type %u)", ehdr.e_type);
        ret = -ENOEXEC;
        goto out;
    }

    uint16_t shnum = ehdr.e_shnum;

    if (shnum > MAX_ELF_SECTIONS) {
        LOG_ERR("Too many sections: %u (max %d)", shnum, MAX_ELF_SECTIONS);
        ret = -E2BIG;
        goto out;
    }

    /* Read section headers */
    elf_shdr_t *shdrs = arena_alloc(&arena, 4, shnum * sizeof(elf_shdr_t));

    if (!shdrs) {
        ret = -ENOMEM;
        goto out;
    }

    ret = elf_read_at(&file, ehdr.e_shoff, shdrs, shnum * sizeof(elf_shdr_t));
    if (ret < 0) {
        LOG_ERR("Failed to read section headers: %d", ret);
        goto out;
    }

    /* Read section header string table */
    uint16_t shstrndx = ehdr.e_shstrndx;

    if (shstrndx >= shnum) {
        LOG_ERR("Invalid shstrndx %u", shstrndx);
        ret = -ENOEXEC;
        goto out;
    }

    size_t shstrtab_size = MIN(shdrs[shstrndx].sh_size, MAX_SHSTRTAB_SIZE);
    char *shstrtab = arena_alloc(&arena, 1, shstrtab_size);

    if (!shstrtab) {
        ret = -ENOMEM;
        goto out;
    }

    ret = elf_read_at(&file, shdrs[shstrndx].sh_offset, shstrtab, shstrtab_size);
    if (ret < 0) {
        LOG_ERR("Failed to read shstrtab: %d", ret);
        goto out;
    }

    /* Classify sections by name */
    struct elf_section_indices si;

    classify_sections(shdrs, shnum, shstrtab, shstrtab_size, &si);

    if (si.text < 0 || si.symtab < 0 || si.strtab < 0) {
        LOG_ERR("ELF missing required sections (.text=%d, .symtab=%d, .strtab=%d)",
                si.text, si.symtab, si.strtab);
        ret = -ENOEXEC;
        goto out;
    }

    /* Read symbol string table */
    size_t strtab_size = MIN(shdrs[si.strtab].sh_size, MAX_STRTAB_SIZE);
    char *strtab = arena_alloc(&arena, 1, strtab_size);

    if (!strtab) {
        ret = -ENOMEM;
        goto out;
    }

    ret = elf_read_at(&file, shdrs[si.strtab].sh_offset, strtab, strtab_size);
    if (ret < 0) {
        LOG_ERR("Failed to read strtab: %d", ret);
        goto out;
    }

    /* ----------------------------------------------------------------
     * Phase 2: Allocate destination addresses
     * ---------------------------------------------------------------- */

    size_t text_size = shdrs[si.text].sh_size;
    size_t rodata_size = (si.rodata >= 0) ? shdrs[si.rodata].sh_size : 0;
    size_t data_size = (si.data >= 0) ? shdrs[si.data].sh_size : 0;
    size_t bss_size = (si.bss >= 0) ? shdrs[si.bss].sh_size : 0;

    LOG_INF("Stream load '%s': .text=%zu .rodata=%zu .data=%zu .bss=%zu",
            elf_path, text_size, rodata_size, data_size, bss_size);

    /* XIP space for .text and .rodata */
    uint32_t text_xip_off, rodata_xip_off;

    ret = zsw_llext_xip_alloc(elf_path, text_size, rodata_size,
                              &text_xip_off, &rodata_xip_off);
    if (ret < 0) {
        LOG_ERR("XIP allocation failed: %d", ret);
        goto out;
    }

    uintptr_t text_base = zsw_llext_xip_cpu_addr(text_xip_off);
    uintptr_t rodata_base = (rodata_size > 0) ?
        zsw_llext_xip_cpu_addr(rodata_xip_off) : 0;

    /* Data pool for .data and .bss */
    uintptr_t data_base = 0;
    uintptr_t bss_base = 0;

    if (data_size > 0) {
        void *p = zsw_llext_data_pool_alloc(sizeof(void *), data_size);

        if (!p) {
            ret = -ENOMEM;
            goto out;
        }
        data_base = (uintptr_t)p;
    }

    if (bss_size > 0) {
        void *p = zsw_llext_data_pool_alloc(sizeof(void *), bss_size);

        if (!p) {
            ret = -ENOMEM;
            goto out;
        }
        bss_base = (uintptr_t)p;
    }

    LOG_INF("Final addresses: .text=0x%08lx .rodata=0x%08lx .data=0x%08lx .bss=0x%08lx",
            (unsigned long)text_base, (unsigned long)rodata_base,
            (unsigned long)data_base, (unsigned long)bss_base);

    /* ----------------------------------------------------------------
     * Phase 3: Build section-index → final-base-address mapping
     * ---------------------------------------------------------------- */

    uintptr_t *sect_base = arena_alloc(&arena, 4, shnum * sizeof(uintptr_t));

    if (!sect_base) {
        ret = -ENOMEM;
        goto out;
    }

    memset(sect_base, 0, shnum * sizeof(uintptr_t));

    if (si.text >= 0) {
        sect_base[si.text] = text_base;
    }
    if (si.rodata >= 0) {
        sect_base[si.rodata] = rodata_base;
    }
    if (si.data >= 0) {
        sect_base[si.data] = data_base;
    }
    if (si.bss >= 0) {
        sect_base[si.bss] = bss_base;
    }

    /* ----------------------------------------------------------------
     * Phase 4: Stream .text and .rodata to XIP flash
     * ---------------------------------------------------------------- */

    /* Allocate staging buffer from scratch arena */
    uint8_t *staging_buf = arena_alloc(&arena, 4, STAGING_BUF_SIZE);

    if (!staging_buf) {
        ret = -ENOMEM;
        goto out;
    }

    /* Open flash area */
    const struct flash_area *fa;

    ret = flash_area_open(FIXED_PARTITION_ID(llext_xip_partition), &fa);
    if (ret < 0) {
        LOG_ERR("Failed to open XIP partition: %d", ret);
        goto out;
    }

    /* Disable XIP for flash operations */
    const struct device *qspi_dev = DEVICE_DT_GET_OR_NULL(DT_CHOSEN(nordic_pm_ext_flash));

    if (qspi_dev && device_is_ready(qspi_dev)) {
        nrf_qspi_nor_xip_enable(qspi_dev, false);
    }

    /* Stream .text */
    ret = stream_section_to_xip(&file, shdrs, &si, strtab, strtab_size,
                                sect_base, shnum,
                                si.text, si.rel_text,
                                fa, text_xip_off, staging_buf);
    if (ret < 0) {
        LOG_ERR("Failed to stream .text: %d", ret);
        goto xip_restore;
    }

    /* Stream .rodata */
    if (si.rodata >= 0 && rodata_size > 0) {
        ret = stream_section_to_xip(&file, shdrs, &si, strtab, strtab_size,
                                    sect_base, shnum,
                                    si.rodata, si.rel_rodata,
                                    fa, rodata_xip_off, staging_buf);
        if (ret < 0) {
            LOG_ERR("Failed to stream .rodata: %d", ret);
            goto xip_restore;
        }
    }

xip_restore:
    /* Re-enable XIP */
    if (qspi_dev && device_is_ready(qspi_dev)) {
        nrf_qspi_nor_xip_enable(qspi_dev, true);
    }

    sys_cache_instr_invd_all();
    flash_area_close(fa);

    if (ret < 0) {
        goto out;
    }

    /* ----------------------------------------------------------------
     * Phase 5: Copy .data to pool (with relocs applied) and zero .bss
     * ---------------------------------------------------------------- */

    if (data_size > 0 && si.data >= 0) {
        /* Read .data into pool buffer */
        ret = elf_read_at(&file, shdrs[si.data].sh_offset,
                          (void *)data_base, data_size);
        if (ret < 0) {
            LOG_ERR("Failed to read .data: %d", ret);
            goto out;
        }

        /* Apply relocations in-place */
        ret = apply_relocs_to_buffer(&file, shdrs, &si, strtab, strtab_size,
                                     sect_base, shnum, si.rel_data,
                                     (void *)data_base, data_size);
        if (ret < 0) {
            LOG_ERR("Failed to apply .data relocs: %d", ret);
            goto out;
        }

        LOG_INF("Stream: .data loaded to pool (%zu bytes)", data_size);
    }

    if (bss_size > 0) {
        memset((void *)bss_base, 0, bss_size);
        LOG_INF("Stream: .bss zeroed in pool (%zu bytes)", bss_size);
    }

    /* ----------------------------------------------------------------
     * Phase 6: Find the entry symbol
     *
     * Scan the ELF symtab for a global defined symbol matching
     * entry_symbol.  Its address = sect_base[st_shndx] + st_value.
     * ---------------------------------------------------------------- */

    {
        uint32_t sym_count = shdrs[si.symtab].sh_size / sizeof(elf_sym_t);
        size_t sym_ent_size = shdrs[si.symtab].sh_entsize;

        if (sym_ent_size == 0) {
            sym_ent_size = sizeof(elf_sym_t);
        }

        elf_sym_t sym;
        bool found = false;

        for (uint32_t i = 1; i < sym_count; i++) {
            off_t sym_off = shdrs[si.symtab].sh_offset + (off_t)i * sym_ent_size;

            ret = elf_read_at(&file, sym_off, &sym, sizeof(sym));
            if (ret < 0) {
                LOG_ERR("Failed to read symbol %u: %d", i, ret);
                goto out;
            }

            if (ELF_ST_BIND(sym.st_info) != STB_GLOBAL) {
                continue;
            }
            if (sym.st_shndx == SHN_UNDEF) {
                continue;
            }
            if (sym.st_name >= strtab_size) {
                continue;
            }

            const char *name = &strtab[sym.st_name];

            if (strcmp(name, entry_symbol) == 0) {
                if (sym.st_shndx < shnum && sect_base[sym.st_shndx] != 0) {
                    result->entry_fn = (void *)(sect_base[sym.st_shndx] + sym.st_value);

                    /* Thumb function: set bit 0 for BLX interworking */
                    if (ELF_ST_TYPE(sym.st_info) == STT_FUNC) {
                        result->entry_fn = (void *)((uintptr_t)result->entry_fn | 1);
                    }

                    LOG_INF("Entry symbol '%s' resolved to %p",
                            entry_symbol, result->entry_fn);
                    found = true;
                    break;
                }
                LOG_ERR("Entry symbol '%s' in unmapped section %u",
                        entry_symbol, sym.st_shndx);
                ret = -ENOENT;
                goto out;
            }
        }

        if (!found) {
            LOG_ERR("Entry symbol '%s' not found", entry_symbol);
            ret = -ENOENT;
            goto out;
        }
    }

    LOG_INF("Stream load complete: .text=%zu→XIP .rodata=%zu→XIP "
            ".data=%zu→pool .bss=%zu→pool (scratch used=%zu/%zu)",
            text_size, rodata_size, data_size, bss_size,
            arena.offset, arena.size);

    ret = 0;

out:
    fs_close(&file);
    return ret;
}
