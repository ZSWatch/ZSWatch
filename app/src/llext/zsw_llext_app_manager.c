/*
 * Copyright (c) 2026 ZSWatch Project
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file zsw_llext_app_manager.c
 * @brief LLEXT app manager — loads all extensions at boot.
 *
 * At boot, the filesystem is scanned for LLEXT app directories. Each app
 * directory must contain an app.llext file. Each LLEXT is loaded and its
 * app_entry() is called — mirroring the SYS_INIT pattern used by built-in
 * apps. app_entry() performs initialization (settings, zbus observers, etc.)
 * and self-registers with the app manager via zsw_app_manager_add_application().
 *
 * After app_entry() returns, the application_t callbacks are already wrapped
 * with R9-restoring trampolines (via LLEXT_TRAMPOLINE_APP_FUNCS in app_entry()),
 * so no further wrapping is needed — the app manager can call them directly.
 *
 * LLEXTs remain loaded for the lifetime of the system — there is no unload
 * on app-close. This matches the lifecycle of built-in SYS_INIT apps.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>
#include <zephyr/llext/llext.h>
#include <zephyr/llext/fs_loader.h>
#include <string.h>

#include "llext/zsw_llext_app_manager.h"
#include "llext/zsw_llext_xip.h"
#include "llext/zsw_llext_iflash.h"
#include "managers/zsw_xip_manager.h"
#include "ui/popup/zsw_popup_window.h"

LOG_MODULE_REGISTER(llext_app_mgr, CONFIG_ZSW_LLEXT_APP_MANAGER_LOG_LEVEL);

/*
 * ARM PIC LLEXT apps are compiled with -msingle-pic-base -mpic-register=r9.
 * R9 must hold the GOT base address whenever LLEXT code runs.
 *
 * The firmware is compiled with -ffixed-r9 so it never uses R9 as a scratch
 * register. This ensures R9 is preserved across calls from LLEXT to firmware.
 *
 * R9 is set in two places:
 * 1. LLEXT_CALL_ENTRY — for the initial app_entry() call at boot.
 * 2. LLEXT_TRAMPOLINE_APP_FUNCS — called by app_entry() to wrap all
 *    application_t callbacks with R9-restoring trampolines, so any
 *    subsequent invocation (from app manager, zbus, etc.) is safe.
 */
#ifdef CONFIG_ARM

/* Helper to set R9 before entering LLEXT code */
static __always_inline void llext_set_r9(void *got_base)
{
    __asm__ volatile("mov r9, %0" : : "r"(got_base) : "r9");
}

#define LLEXT_CALL_ENTRY(got, fn, result) do { llext_set_r9(got); (result) = (fn)(); } while (0)

#else
/* Non-ARM: direct calls */
#define LLEXT_CALL_ENTRY(got, fn, result) do { (void)(got); (result) = (fn)(); } while (0)
#endif

/* --------------------------------------------------------------------------
 * Configuration
 * -------------------------------------------------------------------------- */

#define ZSW_LLEXT_MAX_APPS          10
#define ZSW_LLEXT_APPS_BASE_PATH    "/lvgl_lfs/apps"
#define ZSW_LLEXT_ELF_NAME          "app.llext"
#define ZSW_LLEXT_ENTRY_SYMBOL      "app_entry"
#define ZSW_LLEXT_MAX_PATH_LEN      80
#define ZSW_LLEXT_MAX_NAME_LEN      32
#define ZSW_LLEXT_HEAP_SIZE         (36 * 1024)

/* --------------------------------------------------------------------------
 * Types
 * -------------------------------------------------------------------------- */

typedef int (*llext_app_entry_fn)(void);

static void show_app_installed_popup_work_handler(struct k_work *work);

typedef struct {
    char name[ZSW_LLEXT_MAX_NAME_LEN];
    struct llext *ext;
} zsw_llext_app_t;

/* --------------------------------------------------------------------------
 * Static Data
 * -------------------------------------------------------------------------- */

static zsw_llext_app_t llext_apps[ZSW_LLEXT_MAX_APPS];
static int num_llext_apps;

/* Heap buffer for LLEXT dynamic allocations */
static uint8_t llext_heap_buf[ZSW_LLEXT_HEAP_SIZE] __aligned(8);
static bool heap_initialized;

static K_WORK_DEFINE(show_app_installed_popup_work, show_app_installed_popup_work_handler);
static char installed_app_name[ZSW_LLEXT_MAX_NAME_LEN];

/* --------------------------------------------------------------------------
 * Heap Management
 * -------------------------------------------------------------------------- */

static int ensure_heap_init(void)
{
    if (heap_initialized) {
        return 0;
    }

    int ret = llext_heap_init(llext_heap_buf, sizeof(llext_heap_buf));

    if (ret != 0) {
        LOG_ERR("Failed to initialize LLEXT heap: %d", ret);
        return ret;
    }

    heap_initialized = true;
    LOG_INF("LLEXT heap initialized (%d bytes)", ZSW_LLEXT_HEAP_SIZE);
    return 0;
}

/* --------------------------------------------------------------------------
 * App Discovery — load LLEXT, call app_entry(), keep loaded
 * -------------------------------------------------------------------------- */

static int discover_llext_app(const char *dir_path, const char *dir_name)
{
    int idx;
    int ret;
    zsw_llext_app_t *la;

    if (num_llext_apps >= ZSW_LLEXT_MAX_APPS) {
        LOG_ERR("Maximum LLEXT apps reached (%d)", ZSW_LLEXT_MAX_APPS);
        return -ENOMEM;
    }

    idx = num_llext_apps;
    la = &llext_apps[idx];
    memset(la, 0, sizeof(*la));
    strncpy(la->name, dir_name, sizeof(la->name) - 1);

    /* Verify the ELF file exists */
    char elf_path[ZSW_LLEXT_MAX_PATH_LEN + sizeof(ZSW_LLEXT_ELF_NAME)];

    snprintk(elf_path, sizeof(elf_path), "%s/%s", dir_path, ZSW_LLEXT_ELF_NAME);

    struct fs_dirent entry;

    ret = fs_stat(elf_path, &entry);
    if (ret < 0) {
        LOG_WRN("No ELF file at %s, skipping", elf_path);
        return ret;
    }

    /* Load the LLEXT */
    ret = ensure_heap_init();
    if (ret != 0) {
        return ret;
    }

    struct llext_fs_loader fs_loader = LLEXT_FS_LOADER(elf_path);
    struct llext_load_param ldr_parm = LLEXT_LOAD_PARAM_DEFAULT;
    struct zsw_llext_xip_context xip_ctx = {0};

    ldr_parm.pre_copy_hook = zsw_llext_xip_pre_copy_hook;
    ldr_parm.pre_copy_hook_user_data = &xip_ctx;

    ret = llext_load(&fs_loader.loader, la->name, &la->ext, &ldr_parm);
    if (ret != 0) {
        LOG_ERR("llext_load failed for '%s': %d", la->name, ret);
        return ret;
    }

    /* Compute GOT base address for R9 register */
    void *got_base = NULL;

    if (xip_ctx.got_found && la->ext->mem[LLEXT_MEM_DATA] != NULL) {
        got_base = (uint8_t *)la->ext->mem[LLEXT_MEM_DATA] + xip_ctx.got_offset;
        LOG_DBG("GOT base = %p (DATA %p + offset %zu)",
                got_base, la->ext->mem[LLEXT_MEM_DATA], xip_ctx.got_offset);
    } else {
        LOG_WRN("No .got found for '%s' — R9 will be NULL", la->name);
    }

    /* Find and call app_entry() — this mirrors SYS_INIT for built-in apps.
     * app_entry() does initialization (settings, zbus, etc.) and calls
     * zsw_app_manager_add_application() to self-register. */
    llext_app_entry_fn entry_fn = llext_find_sym(&la->ext->exp_tab, ZSW_LLEXT_ENTRY_SYMBOL);

    if (entry_fn == NULL) {
        LOG_ERR("Entry symbol '%s' not found in '%s'", ZSW_LLEXT_ENTRY_SYMBOL, la->name);
        llext_unload(&la->ext);
        la->ext = NULL;
        return -ENOENT;
    }

    /* Install iflash sections BEFORE app_entry() so that trampolines
     * created in app_entry() (via LLEXT_TRAMPOLINE_APP_FUNCS and
     * zsw_llext_create_trampoline) capture the internal flash addresses,
     * not the XIP addresses that become invalid when the screen is off. */
    ret = zsw_llext_iflash_install(la->ext, xip_ctx.text_base_vma, got_base);
    if (ret < 0) {
        LOG_WRN("iflash install failed for '%s': %d", la->name, ret);
    }

    int entry_ret;

    LLEXT_CALL_ENTRY(got_base, entry_fn, entry_ret);
    if (entry_ret != 0) {
        LOG_ERR("app_entry() failed for '%s': %d", la->name, entry_ret);
        llext_unload(&la->ext);
        la->ext = NULL;
        return entry_ret;
    }

    num_llext_apps++;

    LOG_INF("Loaded LLEXT app '%s' (slot %d)", la->name, idx);

    return 0;
}

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

int zsw_llext_app_manager_init(void)
{
    struct fs_dir_t dir;
    struct fs_dirent entry;
    int ret;

    /* Ensure the apps base directory exists */
    ret = fs_mkdir(ZSW_LLEXT_APPS_BASE_PATH);
    if (ret < 0 && ret != -EEXIST) {
        LOG_WRN("Failed to create apps directory: %d", ret);
    }

    /* Initialize XIP flash allocator */
    ret = zsw_llext_xip_init();
    if (ret < 0) {
        LOG_WRN("XIP init failed: %d (continuing without XIP)", ret);
    }

    /* Initialize internal flash allocator for iflash sections */
    ret = zsw_llext_iflash_init();
    if (ret < 0) {
        LOG_WRN("Internal flash init failed: %d (continuing without iflash)", ret);
    }

    LOG_INF("Scanning for LLEXT apps in %s", ZSW_LLEXT_APPS_BASE_PATH);

    fs_dir_t_init(&dir);
    ret = fs_opendir(&dir, ZSW_LLEXT_APPS_BASE_PATH);
    if (ret < 0) {
        LOG_WRN("No apps directory found (%d), no LLEXT apps available", ret);
        return 0;
    }

    /* Discover all LLEXT apps (load, call app_entry, keep loaded) */
    while (true) {
        ret = fs_readdir(&dir, &entry);
        if (ret < 0 || entry.name[0] == '\0') {
            break;
        }

        if (entry.type != FS_DIR_ENTRY_DIR) {
            continue;
        }

        char app_dir[ZSW_LLEXT_MAX_PATH_LEN];
        char name_buf[ZSW_LLEXT_MAX_NAME_LEN];

        if (snprintk(name_buf, sizeof(name_buf), "%s", entry.name) >= (int)sizeof(name_buf)) {
            LOG_WRN("LLEXT app name too long, skipping: %s", entry.name);
            continue;
        }

        snprintk(app_dir, sizeof(app_dir), "%s/%s", ZSW_LLEXT_APPS_BASE_PATH, name_buf);

        ret = discover_llext_app(app_dir, name_buf);
        if (ret < 0) {
            LOG_WRN("Failed to discover LLEXT in %s: %d", entry.name, ret);
        }
    }

    fs_closedir(&dir);

    LOG_INF("LLEXT discovery complete: %d app(s) found", num_llext_apps);

    return 0;
}

int zsw_llext_app_manager_prepare_app_dir(const char *app_id)
{
    char dir_path[ZSW_LLEXT_MAX_PATH_LEN];
    int ret;

    snprintk(dir_path, sizeof(dir_path), "%s/%s", ZSW_LLEXT_APPS_BASE_PATH, app_id);

    ret = fs_mkdir(dir_path);
    if (ret < 0 && ret != -EEXIST) {
        LOG_WRN("llext: mkdir %s: %d", dir_path, ret);
        return ret;
    }

    LOG_INF("llext: app dir ready: %s", dir_path);
    return 0;
}

static void show_app_installed_popup_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    char popup_body[64];
    snprintk(popup_body, sizeof(popup_body), "'%s' installed", installed_app_name);
    zsw_popup_show("App Ready", popup_body, NULL, 3, false);
}

int zsw_llext_app_manager_remove_app(const char *app_id)
{
    char dir_path[ZSW_LLEXT_MAX_PATH_LEN];
    char elf_path[ZSW_LLEXT_MAX_PATH_LEN + sizeof(ZSW_LLEXT_ELF_NAME)];
    int ret;

    snprintk(dir_path, sizeof(dir_path), "%s/%s", ZSW_LLEXT_APPS_BASE_PATH, app_id);
    snprintk(elf_path, sizeof(elf_path), "%s/%s", dir_path, ZSW_LLEXT_ELF_NAME);

    ret = fs_unlink(elf_path);
    if (ret < 0 && ret != -ENOENT) {
        LOG_WRN("llext: unlink %s: %d", elf_path, ret);
    }

    ret = fs_unlink(dir_path);
    if (ret < 0 && ret != -ENOENT) {
        LOG_WRN("llext: rmdir %s: %d", dir_path, ret);
    }

    LOG_INF("llext: removed app '%s'", app_id);
    return 0;
}

/* --------------------------------------------------------------------------
 * Hot-load: Load and register an app at runtime
 * -------------------------------------------------------------------------- */

int zsw_llext_app_manager_load_app(const char *app_id)
{
    char dir_path[ZSW_LLEXT_MAX_PATH_LEN];
    int ret;

    /* Check if already loaded */
    for (int i = 0; i < num_llext_apps; i++) {
        if (strcmp(llext_apps[i].name, app_id) == 0) {
            LOG_WRN("llext: app '%s' already loaded", app_id);
            return -EALREADY;
        }
    }

    snprintk(dir_path, sizeof(dir_path), "%s/%s", ZSW_LLEXT_APPS_BASE_PATH, app_id);

    ret = discover_llext_app(dir_path, app_id);
    if (ret < 0) {
        LOG_ERR("llext: failed to load app '%s': %d", app_id, ret);
        return ret;
    }

    /* Show popup and refresh picker from LVGL thread context */
    strncpy(installed_app_name, app_id, sizeof(installed_app_name) - 1);
    installed_app_name[sizeof(installed_app_name) - 1] = '\0';
    k_work_submit(&show_app_installed_popup_work);

    LOG_INF("llext: hot-loaded app '%s'", app_id);
    return 0;
}
