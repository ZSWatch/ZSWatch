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
#include "events/activity_event.h"
#include <zephyr/zbus/zbus.h>
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

/* Helper to set R9 before entering LLEXT code */
static __always_inline void llext_set_r9(void *got_base)
{
    __asm__ volatile("mov r9, %0" : : "r"(got_base) : "r9");
}

#define LLEXT_CALL_ENTRY(got, fn, result) do { llext_set_r9(got); (result) = (fn)(); } while (0)
#define LLEXT_CALL_START(got, fn, root, grp) do { llext_set_r9(got); (fn)(root, grp, NULL); } while (0)
#define LLEXT_CALL_STOP(got, fn) do { llext_set_r9(got); (fn)(NULL); } while (0)
#define LLEXT_CALL_BACK(got, fn, result) do { llext_set_r9(got); (result) = (fn)(); } while (0)
#define LLEXT_CALL_VOID(got, fn) do { llext_set_r9(got); (fn)(); } while (0)

#else
/* Non-ARM: direct calls */
#define LLEXT_CALL_ENTRY(got, fn, result) do { (void)(got); (result) = (fn)(); } while (0)
#define LLEXT_CALL_START(got, fn, root, grp) do { (void)(got); (fn)(root, grp, NULL); } while (0)
#define LLEXT_CALL_STOP(got, fn) do { (void)(got); (fn)(NULL); } while (0)
#define LLEXT_CALL_BACK(got, fn, result) do { (void)(got); (result) = (fn)(); } while (0)
#define LLEXT_CALL_VOID(got, fn) do { (void)(got); (fn)(); } while (0)
#endif

/* Set to the name of the LLEXT app to auto-open at boot for debugging.
 * Set to NULL (or comment out) to disable. */
// #define ZSW_LLEXT_AUTO_OPEN_APP  "calculator_ext"
#ifdef ZSW_LLEXT_AUTO_OPEN_APP
#define ZSW_LLEXT_AUTO_OPEN_DELAY_MS  5000
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
    void *got_base;           /* GOT base address — loaded into R9 before calling LLEXT */
    bool loaded;

    /* Original function pointers from the real app — saved before wrapping */
    void (*orig_start_func)(lv_obj_t *, lv_group_t *, void *);
    void (*orig_stop_func)(void *);
    bool (*orig_back_func)(void);
    void (*orig_ui_unavailable_func)(void);
    void (*orig_ui_available_func)(void);
} zsw_llext_app_t;

/* --------------------------------------------------------------------------
 * Static Data
 * -------------------------------------------------------------------------- */

static zsw_llext_app_t llext_apps[ZSW_LLEXT_MAX_APPS];
static int num_llext_apps;
static zsw_llext_app_t *active_llext_app;

/* Heap buffer for LLEXT dynamic allocations */
static uint8_t llext_heap_buf[ZSW_LLEXT_HEAP_SIZE] __aligned(8);
static bool heap_initialized;

#if defined(ZSW_LLEXT_AUTO_OPEN_APP)
static void auto_open_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(auto_open_work, auto_open_work_handler);
#endif

/* --------------------------------------------------------------------------
 * Activity State Tracking
 * --------------------------------------------------------------------------
 * Keep the active LLEXT app's real_app->current_state in sync with screen
 * state. This is essential for auto-opened apps (which bypass zsw_app_manager)
 * and as a safety net for normally launched apps.
 * -------------------------------------------------------------------------- */

ZBUS_CHAN_DECLARE(activity_state_data_chan);
static void llext_activity_state_cb(const struct zbus_channel *chan);
ZBUS_LISTENER_DEFINE(llext_activity_state_lis, llext_activity_state_cb);

static void llext_activity_state_cb(const struct zbus_channel *chan)
{
    const struct activity_state_event *event = zbus_chan_const_msg(chan);

    if (active_llext_app == NULL || active_llext_app->real_app == NULL) {
        return;
    }

    if (event->state == ZSW_ACTIVITY_STATE_ACTIVE) {
        if (active_llext_app->real_app->current_state == ZSW_APP_STATE_UI_HIDDEN) {
            active_llext_app->real_app->current_state = ZSW_APP_STATE_UI_VISIBLE;
            LOG_INF("LLEXT '%s' UI -> visible", active_llext_app->name);
            if (active_llext_app->orig_ui_available_func) {
                LLEXT_CALL_VOID(active_llext_app->got_base,
                                active_llext_app->orig_ui_available_func);
            }
        }
    } else {
        if (active_llext_app->real_app->current_state == ZSW_APP_STATE_UI_VISIBLE) {
            active_llext_app->real_app->current_state = ZSW_APP_STATE_UI_HIDDEN;
            LOG_INF("LLEXT '%s' UI -> hidden", active_llext_app->name);
            if (active_llext_app->orig_ui_unavailable_func) {
                LLEXT_CALL_VOID(active_llext_app->got_base,
                                active_llext_app->orig_ui_unavailable_func);
            }
        }
    }
}

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
 * R9-Aware Wrapper Functions
 * --------------------------------------------------------------------------
 * These replace the real app's function pointers after app_entry() so that
 * R9 (GOT base) is set correctly before LLEXT code executes.
 *
 * start_func / stop_func receive user_data which carries the zsw_llext_app_t*.
 * back_func / ui_*_func take no arguments — they use active_llext_app which
 * is valid because these are only called on the currently running app.
 * -------------------------------------------------------------------------- */

static void llext_wrapped_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    zsw_llext_app_t *la = (zsw_llext_app_t *)user_data;

    active_llext_app = la;
    la->real_app->current_state = ZSW_APP_STATE_UI_VISIBLE;
    LLEXT_CALL_START(la->got_base, la->orig_start_func, root, group);
}

static void llext_wrapped_stop(void *user_data)
{
    zsw_llext_app_t *la = (zsw_llext_app_t *)user_data;

    if (la->orig_stop_func) {
        LLEXT_CALL_STOP(la->got_base, la->orig_stop_func);
    }

    if (active_llext_app == la) {
        active_llext_app = NULL;
    }
}

static bool llext_wrapped_back(void)
{
    if (active_llext_app && active_llext_app->orig_back_func) {
        bool consumed;

        LLEXT_CALL_BACK(active_llext_app->got_base,
                        active_llext_app->orig_back_func, consumed);
        return consumed;
    }
    return false;
}

static void llext_wrapped_ui_unavailable(void)
{
    if (active_llext_app && active_llext_app->real_app) {
        active_llext_app->real_app->current_state = ZSW_APP_STATE_UI_HIDDEN;

        if (active_llext_app->orig_ui_unavailable_func) {
            LLEXT_CALL_VOID(active_llext_app->got_base,
                            active_llext_app->orig_ui_unavailable_func);
        }
    }
}

static void llext_wrapped_ui_available(void)
{
    if (active_llext_app && active_llext_app->real_app) {
        active_llext_app->real_app->current_state = ZSW_APP_STATE_UI_VISIBLE;

        if (active_llext_app->orig_ui_available_func) {
            LLEXT_CALL_VOID(active_llext_app->got_base,
                            active_llext_app->orig_ui_available_func);
        }
    }
}

/* --------------------------------------------------------------------------
 * Wrap an application_t's function pointers with R9-aware trampolines
 * -------------------------------------------------------------------------- */

static void wrap_app_functions(zsw_llext_app_t *la)
{
    application_t *app = la->real_app;

    /* Save originals */
    la->orig_start_func = app->start_func;
    la->orig_stop_func = app->stop_func;
    la->orig_back_func = app->back_func;
    la->orig_ui_unavailable_func = app->ui_unavailable_func;
    la->orig_ui_available_func = app->ui_available_func;

    /* Replace with R9-aware wrappers */
    app->start_func = llext_wrapped_start;
    app->stop_func = llext_wrapped_stop;
    app->back_func = la->orig_back_func ? llext_wrapped_back : NULL;
    app->ui_unavailable_func = la->orig_ui_unavailable_func ? llext_wrapped_ui_unavailable : NULL;
    app->ui_available_func = la->orig_ui_available_func ? llext_wrapped_ui_available : NULL;
    app->user_data = la;
}

/* --------------------------------------------------------------------------
 * App Discovery — load LLEXT, call app_entry(), wrap, keep loaded
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

    LLEXT_CALL_ENTRY(la->got_base, entry_fn, la->real_app);
    if (la->real_app == NULL) {
        LOG_ERR("app_entry() returned NULL for '%s'", la->name);
        llext_unload(&la->ext);
        la->ext = NULL;
        return -EINVAL;
    }

    /* Install iflash sections for background callbacks */
    ret = zsw_llext_iflash_install(la->ext, xip_ctx.text_base_vma, la->got_base);
    if (ret < 0) {
        LOG_WRN("iflash install failed for '%s': %d", la->name, ret);
    }

    la->loaded = true;
    num_llext_apps++;

    /* Wrap the real app's function pointers with R9-aware trampolines */
    wrap_app_functions(la);

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

    /* Subscribe to activity state so we can sync real_app->current_state */
    ret = zbus_chan_add_obs(&activity_state_data_chan,
                           &llext_activity_state_lis, K_MSEC(100));
    if (ret < 0) {
        LOG_WRN("Failed to add activity state observer: %d", ret);
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

#if defined(ZSW_LLEXT_AUTO_OPEN_APP)
    /* Schedule auto-open for debugging */
    for (int i = 0; i < num_llext_apps; i++) {
        if (strcmp(llext_apps[i].name, ZSW_LLEXT_AUTO_OPEN_APP) == 0) {
            LOG_INF("Auto-open '%s' scheduled in %d ms",
                    ZSW_LLEXT_AUTO_OPEN_APP, ZSW_LLEXT_AUTO_OPEN_DELAY_MS);
            k_work_schedule(&auto_open_work, K_MSEC(ZSW_LLEXT_AUTO_OPEN_DELAY_MS));
            break;
        }
    }
#endif

    return 0;
}

/* --------------------------------------------------------------------------
 * Debug: Auto-Open App at Boot
 * -------------------------------------------------------------------------- */

#if defined(ZSW_LLEXT_AUTO_OPEN_APP)
static void auto_open_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    for (int i = 0; i < num_llext_apps; i++) {
        if (strcmp(llext_apps[i].name, ZSW_LLEXT_AUTO_OPEN_APP) == 0) {
            zsw_llext_app_t *la = &llext_apps[i];

            if (!la->loaded || !la->real_app) {
                LOG_WRN("Auto-open: '%s' not loaded", ZSW_LLEXT_AUTO_OPEN_APP);
                return;
            }
            LOG_INF("Auto-opening LLEXT app '%s'", ZSW_LLEXT_AUTO_OPEN_APP);
            lv_obj_t *root = lv_obj_create(lv_screen_active());

            lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
            /* Call the wrapped start (sets R9 and active_llext_app) */
            la->real_app->start_func(root, NULL, la->real_app->user_data);
            return;
        }
    }
    LOG_WRN("Auto-open: app '%s' not found", ZSW_LLEXT_AUTO_OPEN_APP);
}
#endif

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
