/*
 * Copyright (c) 2026 ZSWatch Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef CONFIG_ZSW_LLEXT_APPS

#include <zephyr/llext/llext.h>
#include "managers/zsw_app_manager.h"

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
 * @brief Wrap all application_t function pointers with R9-restoring trampolines.
 *
 * Call this in app_entry() after populating the application_t struct.
 * Each non-NULL callback is replaced with a trampoline that restores R9
 * before jumping to the original function, so the firmware can invoke
 * LLEXT callbacks safely from any context.
 *
 * @param app_ptr  Pointer to the application_t to patch
 */
#define LLEXT_TRAMPOLINE_APP_FUNCS(app_ptr) do { \
    application_t *_a = (app_ptr); \
    if (_a->start_func) \
        _a->start_func = (void *)zsw_llext_create_trampoline((void *)_a->start_func); \
    if (_a->stop_func) \
        _a->stop_func = (void *)zsw_llext_create_trampoline((void *)_a->stop_func); \
    if (_a->back_func) \
        _a->back_func = (void *)zsw_llext_create_trampoline((void *)_a->back_func); \
    if (_a->ui_unavailable_func) \
        _a->ui_unavailable_func = (void *)zsw_llext_create_trampoline((void *)_a->ui_unavailable_func); \
    if (_a->ui_available_func) \
        _a->ui_available_func = (void *)zsw_llext_create_trampoline((void *)_a->ui_available_func); \
} while (0)

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

/**
 * @brief Create an R9-restoring trampoline for a function pointer at runtime.
 *
 * LLEXT apps must call this when passing function pointers to firmware APIs
 * that will store and call them later on a context where R9 is not set
 * (e.g., k_work_init, k_timer_init, k_thread_create, zbus_chan_add_obs).
 *
 * The returned pointer wraps the original function with a small stub that
 * sets R9 (GOT base) before jumping to the real function, ensuring the
 * LLEXT app's global variables are accessible when the callback executes.
 *
 * Must be called from LLEXT context (R9 must hold the correct GOT base).
 * The trampoline is allocated in internal flash and persists until
 * zsw_llext_iflash_reset() is called.
 *
 * On non-ARM platforms, returns the function pointer unchanged.
 *
 * Example usage from an LLEXT app:
 * @code
 * static void my_work_handler(struct k_work *work) { ... }
 *
 * // In app_entry() or start_func():
 * k_work_init(&my_work,
 *             (k_work_handler_t)zsw_llext_create_trampoline((void *)my_work_handler));
 * @endcode
 *
 * @param func  Original function pointer (may be XIP or iflash address)
 * @return Trampoline function pointer (internal flash), or NULL on failure
 */
void *zsw_llext_create_trampoline(void *func);

#else /* !CONFIG_ZSW_LLEXT_APPS */

/** @brief No-op when LLEXT is disabled. */
#define LLEXT_IFLASH

/** @brief No-op when LLEXT is disabled. */
#define LLEXT_TRAMPOLINE_APP_FUNCS(app_ptr) do { (void)(app_ptr); } while (0)

/** @brief Identity (no-op) when LLEXT is disabled — returns func unchanged. */
#define zsw_llext_create_trampoline(func) (func)

#endif /* CONFIG_ZSW_LLEXT_APPS */
