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
 * @file zsw_llext_app_manager.c
 * @brief LLEXT app manager — loads all extensions at boot.
 *
 * At boot, the filesystem is scanned for LLEXT app directories. Each app
 * directory must contain an app.llext file. Each LLEXT is loaded and its
 * app_entry() is called — mirroring the SYS_INIT pattern used by built-in
 * apps. app_entry() performs initialization (settings, zbus observers, etc.)
 * and self-registers with the app manager via zsw_app_manager_add_application().
 *
 * After app_entry() returns, the manager wraps the registered application_t's
 * function pointers with R9-aware trampolines so the GOT base is set
 * correctly before any LLEXT code executes.
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

#include "managers/zsw_app_manager.h"
#include "managers/zsw_llext_app_manager.h"
#include "managers/zsw_llext_xip.h"
#include "managers/zsw_llext_iflash.h"
#include "managers/zsw_xip_manager.h"
#include "ui/popup/zsw_popup_window.h"
#include <lvgl.h>

LOG_MODULE_REGISTER(llext_app_mgr, CONFIG_ZSW_LLEXT_APP_MANAGER_LOG_LEVEL);

/*
 * ARM PIC LLEXT apps are compiled with -msingle-pic-base -mpic-register=r9.
 * R9 must hold the GOT base address whenever LLEXT code runs.
 *
 * The firmware is compiled with -ffixed-r9 so it never uses R9 as a scratch
 * register. This ensures R9 is preserved across calls from LLEXT to firmware.
 *
 * We still initialize R9 before the first call into LLEXT code.
 */
#ifdef CONFIG_ARM
static __always_inline void llext_set_r9(void *got_base)
{
    __asm__ volatile("mov r9, %0" : : "r"(got_base) : "r9");
}
#define LLEXT_CALL_ENTRY(got, fn, result) do { llext_set_r9(got); (result) = (fn)(); } while (0)
#else
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

typedef application_t *(*llext_app_entry_fn)(void);

typedef struct {
    char name[ZSW_LLEXT_MAX_NAME_LEN];
    char dir_path[ZSW_LLEXT_MAX_PATH_LEN];

    /* Runtime state — populated when the LLEXT is loaded at boot */
    struct llext *ext;
    application_t *real_app;  /* Points into LLEXT .data, valid for lifetime */
    void *got_base;           /* GOT base address — baked into trampolines */
    bool loaded;
} zsw_llext_app_t;

static void show_app_installed_popup_work_handler(struct k_work *work);

/* --------------------------------------------------------------------------
 * Static Data
 * -------------------------------------------------------------------------- */

static zsw_llext_app_t llext_apps[ZSW_LLEXT_MAX_APPS];
static int num_llext_apps;

/* Heap buffer for LLEXT dynamic allocations */
// TODO: Might change this for a common heap
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
    strncpy(la->dir_path, dir_path, sizeof(la->dir_path) - 1);
    strncpy(la->name, dir_name, sizeof(la->name) - 1);

    /* Verify the ELF file exists */
    char elf_path[ZSW_LLEXT_MAX_PATH_LEN];

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
    if (xip_ctx.got_found && la->ext->mem[LLEXT_MEM_DATA] != NULL) {
        la->got_base = (uint8_t *)la->ext->mem[LLEXT_MEM_DATA] + xip_ctx.got_offset;
        LOG_DBG("GOT base = %p (DATA %p + offset %zu)",
                la->got_base, la->ext->mem[LLEXT_MEM_DATA], xip_ctx.got_offset);
    } else {
        LOG_WRN("No .got found for '%s' — R9 will be NULL", la->name);
        la->got_base = NULL;
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

    /* Install iflash sections BEFORE calling app_entry() so that function
     * pointers resolved through GOT inside app_entry() (e.g. for trampolines)
     * already point to the internal-flash copies, making them safe for
     * background execution when XIP is disabled. */
    ret = zsw_llext_iflash_install(la->ext, xip_ctx.text_base_vma, la->got_base);
    if (ret < 0) {
        LOG_WRN("iflash install failed for '%s': %d", la->name, ret);
    }

    LLEXT_CALL_ENTRY(la->got_base, entry_fn, la->real_app);
    if (la->real_app == NULL) {
        LOG_ERR("app_entry() returned NULL for '%s'", la->name);
        llext_unload(&la->ext);
        la->ext = NULL;
        return -EINVAL;
    }

    la->loaded = true;
    num_llext_apps++;

    LOG_INF("Loaded LLEXT app '%s' (name='%s', icon=%p, slot %d)",
            la->name, la->real_app->name, la->real_app->icon, idx);
    if (la->real_app->icon) {
        const uint8_t *ibytes = (const uint8_t *)la->real_app->icon;
        LOG_INF("  icon bytes: %02x %02x %02x %02x '%c%c%c%c'",
                ibytes[0], ibytes[1], ibytes[2], ibytes[3],
                ibytes[0] > 0x20 ? ibytes[0] : '.', ibytes[1] > 0x20 ? ibytes[1] : '.',
                ibytes[2] > 0x20 ? ibytes[2] : '.', ibytes[3] > 0x20 ? ibytes[3] : '.');
    }

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

    /* Discover all LLEXT apps (load, call app_entry, wrap, keep loaded) */
    while (true) {
        ret = fs_readdir(&dir, &entry);
        if (ret < 0 || entry.name[0] == '\0') {
            break;
        }

        if (entry.type != FS_DIR_ENTRY_DIR) {
            continue;
        }

        char app_dir[ZSW_LLEXT_MAX_PATH_LEN];

        snprintk(app_dir, sizeof(app_dir), "%s/%s", ZSW_LLEXT_APPS_BASE_PATH, entry.name);

        ret = discover_llext_app(app_dir, entry.name);
        if (ret < 0) {
            LOG_WRN("Failed to discover LLEXT in %s: %d", entry.name, ret);
        }
    }

    fs_closedir(&dir);

    LOG_INF("LLEXT discovery complete: %d app(s) found", num_llext_apps);

    return 0;
}

static int zsw_llext_app_manager_sys_init(void)
{
    /* Delay-start: LittleFS needs to be mounted first.
     * The actual init is called from main.c after filesystem is ready.
     */
    return 0;
}

SYS_INIT(zsw_llext_app_manager_sys_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

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
    char elf_path[ZSW_LLEXT_MAX_PATH_LEN];
    char dir_path[ZSW_LLEXT_MAX_PATH_LEN];
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
    zsw_llext_app_t *la = &llext_apps[num_llext_apps - 1];
    if (la->real_app && la->real_app->name) {
        strncpy(installed_app_name, la->real_app->name, sizeof(installed_app_name) - 1);
    } else {
        strncpy(installed_app_name, app_id, sizeof(installed_app_name) - 1);
    }
    installed_app_name[sizeof(installed_app_name) - 1] = '\0';
    k_work_submit(&show_app_installed_popup_work);

    LOG_INF("llext: hot-loaded app '%s'", app_id);
    return 0;
}
