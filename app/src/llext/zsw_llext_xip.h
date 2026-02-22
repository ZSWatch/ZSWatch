/*
 * Copyright (c) 2026 ZSWatch Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <zephyr/llext/llext.h>
#include <zephyr/llext/loader.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Context struct for the XIP pre-copy hook.
 *
 * Pass a pointer to this as pre_copy_hook_user_data. The hook fills in the
 * GOT offset so the caller can compute the runtime GOT base address after
 * loading (needed for -msingle-pic-base R9 setup on ARM).
 */
struct zsw_llext_xip_context {
    size_t got_offset;      /**< Offset of .got section within LLEXT_MEM_DATA region */
    size_t got_size;        /**< Size of .got section in bytes */
    uintptr_t text_base_vma; /**< Original VMA of the TEXT region start */
    bool got_found;         /**< True if .got section was found in the ELF */
};

/**
 * @brief XIP support for PIC LLEXT apps.
 *
 * Two modes of operation:
 *
 * **Streaming mode (recommended)**: Use zsw_llext_xip_pre_copy_hook as a
 * pre_copy_hook in llext_load_param. This streams .text/.rodata from the ELF
 * file directly to XIP flash during loading — the full .text section is NEVER
 * allocated on heap. This allows loading apps whose .text section is larger
 * than the available LLEXT heap.
 *
 * **Post-load mode (legacy)**: Call zsw_llext_xip_install() after llext_load().
 * Copies .text/.rodata from heap to XIP flash and frees heap copies.
 * Requires the LLEXT heap to be large enough to hold the full .text section
 * temporarily.
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
 * @brief Pre-copy hook: stream .text/.rodata directly from ELF to XIP flash.
 *
 * Intended for use as llext_load_param.pre_copy_hook. Reads the TEXT and
 * RODATA region info from ldr->sects[], streams the data from the ELF loader
 * to XIP flash in small chunks, and sets ext->mem[] so that llext_copy_regions
 * skips these regions entirely.
 *
 * This avoids allocating the (potentially large) .text section on heap.
 *
 * Additionally, if user_data is non-NULL and points to a
 * struct zsw_llext_xip_context, the hook scans section headers to locate the
 * .got section and records its offset within the DATA region. This is needed
 * for -msingle-pic-base builds where R9 must be set to the GOT base address.
 *
 * @param ldr       ELF loader handle (used for llext_seek/llext_read)
 * @param ext       Extension being loaded
 * @param user_data Optional pointer to struct zsw_llext_xip_context (or NULL)
 * @return 0 on success, negative errno on failure
 */
int zsw_llext_xip_pre_copy_hook(struct llext_loader *ldr, struct llext *ext,
                                void *user_data);

/**
 * @brief Reset the XIP allocator.
 *
 * Resets the flash offset to 0, allowing the space to be reused by the next
 * app. Call this after llext_unload() when done with an app.
 */
void zsw_llext_xip_reset(void);
