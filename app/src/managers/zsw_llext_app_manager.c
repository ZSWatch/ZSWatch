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
 * @brief LLEXT app manager with deferred (on-demand) loading.
 *
 * At boot, the filesystem is scanned for LLEXT app directories. Each app
 * directory must contain an app.llext file. A lightweight proxy application_t
 * is registered with the app manager for each discovered app. The actual
 * LLEXT shared library is NOT loaded at boot.
 *
 * When the user opens an LLEXT app, the proxy's start_func loads the ELF from
 * the filesystem, calls the extension's app_entry to obtain the real
 * application_t, and then invokes the real start_func. When the user closes
 * the app, the proxy's stop_func calls the real stop_func and then unloads
 * the entire LLEXT, freeing all heap memory.
 *
 * This ensures only ONE LLEXT is loaded at a time, keeping heap usage minimal.
 * The 45 KB LLEXT heap is sufficient for any single extension.
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
#include <lvgl.h>

LOG_MODULE_REGISTER(llext_app_mgr, LOG_LEVEL_INF);

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
#define LLEXT_CALL_START(got, fn, root, grp) do { llext_set_r9(got); (fn)(root, grp); } while (0)
#define LLEXT_CALL_STOP(got, fn) do { llext_set_r9(got); (fn)(); } while (0)
#define LLEXT_CALL_BACK(got, fn, result) do { llext_set_r9(got); (result) = (fn)(); } while (0)
#define LLEXT_CALL_VOID(got, fn) do { llext_set_r9(got); (fn)(); } while (0)

#else
/* Non-ARM: direct calls */
#define LLEXT_CALL_ENTRY(got, fn, result) do { (void)(got); (result) = (fn)(); } while (0)
#define LLEXT_CALL_START(got, fn, root, grp) do { (void)(got); (fn)(root, grp); } while (0)
#define LLEXT_CALL_STOP(got, fn) do { (void)(got); (fn)(); } while (0)
#define LLEXT_CALL_BACK(got, fn, result) do { (void)(got); (result) = (fn)(); } while (0)
#define LLEXT_CALL_VOID(got, fn) do { (void)(got); (fn)(); } while (0)
#endif

/* Set to the name of the LLEXT app to auto-open at boot for debugging.
 * Set to NULL to disable. */
//#define ZSW_LLEXT_AUTO_OPEN_APP  "battery_real_ext"
#define ZSW_LLEXT_AUTO_OPEN_DELAY_MS  5000

/* --------------------------------------------------------------------------
 * Configuration
 * -------------------------------------------------------------------------- */

#define ZSW_LLEXT_MAX_APPS          10
#define ZSW_LLEXT_APPS_BASE_PATH    "/lvgl_lfs/apps"
#define ZSW_LLEXT_ELF_NAME          "app.llext"
#define ZSW_LLEXT_ENTRY_SYMBOL      "app_entry"
#define ZSW_LLEXT_MAX_PATH_LEN      80
#define ZSW_LLEXT_MAX_NAME_LEN      32
#define ZSW_LLEXT_HEAP_SIZE         (45 * 1024)

/* --------------------------------------------------------------------------
 * Types
 * -------------------------------------------------------------------------- */

typedef application_t *(*llext_app_entry_fn)(void);

typedef struct {
    char name[ZSW_LLEXT_MAX_NAME_LEN];
    char dir_path[ZSW_LLEXT_MAX_PATH_LEN];

    /* Proxy app registered with the main app manager at discovery time.
     * Its start/stop functions are trampolines that trigger deferred loading.
     */
    application_t proxy_app;

    /* Runtime state — populated when the LLEXT is actually loaded */
    struct llext *ext;
    application_t *real_app;  /* Points into LLEXT memory, valid only while loaded */
    void *got_base;           /* GOT base address — loaded into R9 before calling LLEXT */
    bool loaded;
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
 * Forward Declarations — Proxy Trampolines
 * -------------------------------------------------------------------------- */

static void proxy_start_common(int idx, lv_obj_t *root, lv_group_t *group);
static void proxy_stop_common(int idx);

/*
 * Each LLEXT app slot needs a unique function pointer for start/stop so the
 * app manager can distinguish them. We generate small trampolines per slot.
 */
#define DEFINE_PROXY(n) \
    static void proxy_start_##n(lv_obj_t *r, lv_group_t *g) { proxy_start_common(n, r, g); } \
    static void proxy_stop_##n(void) { proxy_stop_common(n); }

DEFINE_PROXY(0) DEFINE_PROXY(1) DEFINE_PROXY(2) DEFINE_PROXY(3) DEFINE_PROXY(4)
DEFINE_PROXY(5) DEFINE_PROXY(6) DEFINE_PROXY(7) DEFINE_PROXY(8) DEFINE_PROXY(9)

static const application_start_fn proxy_start_fns[ZSW_LLEXT_MAX_APPS] = {
    proxy_start_0, proxy_start_1, proxy_start_2, proxy_start_3, proxy_start_4,
    proxy_start_5, proxy_start_6, proxy_start_7, proxy_start_8, proxy_start_9,
};

static const application_stop_fn proxy_stop_fns[ZSW_LLEXT_MAX_APPS] = {
    proxy_stop_0, proxy_stop_1, proxy_stop_2, proxy_stop_3, proxy_stop_4,
    proxy_stop_5, proxy_stop_6, proxy_stop_7, proxy_stop_8, proxy_stop_9,
};

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
 * Active App Callbacks (shared by all proxies — only one LLEXT is active)
 * -------------------------------------------------------------------------- */

static bool llext_proxy_back(void)
{
    if (active_llext_app && active_llext_app->real_app &&
        active_llext_app->real_app->back_func) {
        bool consumed;

        LLEXT_CALL_BACK(active_llext_app->got_base,
                        active_llext_app->real_app->back_func, consumed);
        return consumed;
    }
    return false;
}

static void llext_proxy_ui_unavailable(void)
{
    if (active_llext_app && active_llext_app->real_app &&
        active_llext_app->real_app->ui_unavailable_func) {
        LLEXT_CALL_VOID(active_llext_app->got_base,
                        active_llext_app->real_app->ui_unavailable_func);
    }
}

static void llext_proxy_ui_available(void)
{
    if (active_llext_app && active_llext_app->real_app &&
        active_llext_app->real_app->ui_available_func) {
        LLEXT_CALL_VOID(active_llext_app->got_base,
                        active_llext_app->real_app->ui_available_func);
    }
}

/* --------------------------------------------------------------------------
 * Deferred Load / Unload
 * -------------------------------------------------------------------------- */

static void proxy_start_common(int idx, lv_obj_t *root, lv_group_t *group)
{
    zsw_llext_app_t *la = &llext_apps[idx];
    int ret;

    if (la->loaded && la->real_app) {
        LOG_INF("LLEXT '%s' already loaded, calling start_func", la->name);
        active_llext_app = la;
        la->real_app->current_state = ZSW_APP_STATE_UI_VISIBLE;
        LLEXT_CALL_START(la->got_base, la->real_app->start_func, root, group);
        return;
    }

    LOG_INF("Loading LLEXT '%s' from %s", la->name, la->dir_path);

    ret = ensure_heap_init();
    if (ret != 0) {
        LOG_ERR("Heap init failed: %d", ret);
        return;
    }

    /* Load the ELF from filesystem, streaming .text/.rodata directly to XIP flash */
    char elf_path[ZSW_LLEXT_MAX_PATH_LEN];

    snprintk(elf_path, sizeof(elf_path), "%s/%s", la->dir_path, ZSW_LLEXT_ELF_NAME);

    struct llext_fs_loader fs_loader = LLEXT_FS_LOADER(elf_path);
    struct llext_load_param ldr_parm = LLEXT_LOAD_PARAM_DEFAULT;
    struct zsw_llext_xip_context xip_ctx = {0};

    ldr_parm.pre_copy_hook = zsw_llext_xip_pre_copy_hook;
    ldr_parm.pre_copy_hook_user_data = &xip_ctx;

    ret = llext_load(&fs_loader.loader, la->name, &la->ext, &ldr_parm);
    if (ret != 0) {
        LOG_ERR("llext_load failed for '%s': %d", la->name, ret);
        zsw_llext_xip_reset();
        return;
    }

    /* Compute GOT base address for R9 register (ARM -msingle-pic-base) */
    if (xip_ctx.got_found && la->ext->mem[LLEXT_MEM_DATA] != NULL) {
        la->got_base = (uint8_t *)la->ext->mem[LLEXT_MEM_DATA] + xip_ctx.got_offset;
        LOG_INF("GOT base = %p (DATA %p + offset %zu)",
                la->got_base, la->ext->mem[LLEXT_MEM_DATA], xip_ctx.got_offset);
    } else {
        LOG_WRN("No .got found — R9 will be NULL (non-PIC or no GOT)");
        la->got_base = NULL;
    }

    LOG_INF("LLEXT '%s' loaded, finding entry '%s'", la->name, ZSW_LLEXT_ENTRY_SYMBOL);

    /* Find and call the extension's app_entry to get the application_t */
    llext_app_entry_fn entry_fn = llext_find_sym(&la->ext->exp_tab, ZSW_LLEXT_ENTRY_SYMBOL);

    if (entry_fn == NULL) {
        LOG_ERR("Entry symbol '%s' not found in LLEXT '%s'", ZSW_LLEXT_ENTRY_SYMBOL, la->name);
        llext_unload(&la->ext);
        la->ext = NULL;
        zsw_llext_xip_reset();
        return;
    }

    LLEXT_CALL_ENTRY(la->got_base, entry_fn, la->real_app);
    if (la->real_app == NULL) {
        LOG_ERR("app_entry() returned NULL for LLEXT '%s'", la->name);
        llext_unload(&la->ext);
        la->ext = NULL;
        zsw_llext_xip_reset();
        return;
    }

    la->loaded = true;
    active_llext_app = la;

    LOG_INF("LLEXT '%s' ready (name='%s', category=%d)",
            la->name, la->real_app->name, la->real_app->category);

    /* Start the real app UI */
    la->real_app->current_state = ZSW_APP_STATE_UI_VISIBLE;
    LLEXT_CALL_START(la->got_base, la->real_app->start_func, root, group);
}

static void proxy_stop_common(int idx)
{
    zsw_llext_app_t *la = &llext_apps[idx];

    if (!la->loaded) {
        LOG_WRN("LLEXT '%s' not loaded, nothing to stop", la->name);
        return;
    }

    LOG_INF("Stopping LLEXT '%s'", la->name);

    /* Call the real stop function */
    if (la->real_app && la->real_app->stop_func) {
        LLEXT_CALL_STOP(la->got_base, la->real_app->stop_func);
    }

    la->real_app = NULL;
    la->got_base = NULL;

    /* Unload the extension and free all heap memory */
    if (la->ext) {
        llext_unload(&la->ext);
        la->ext = NULL;
    }

    la->loaded = false;

    if (active_llext_app == la) {
        active_llext_app = NULL;
    }

    /* Reset XIP allocator so flash space can be reused by next app */
    zsw_llext_xip_reset();

    LOG_INF("LLEXT '%s' unloaded", la->name);
}

/* --------------------------------------------------------------------------
 * App Discovery (no loading, just filesystem scan + proxy registration)
 * -------------------------------------------------------------------------- */

static int discover_llext_app(const char *dir_path, const char *dir_name)
{
    int idx;
    int ret;
    zsw_llext_app_t *la;
    application_t *proxy;

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

    /* Set up the proxy application_t with trampoline functions */
    proxy = &la->proxy_app;
    proxy->name = la->name;
    proxy->icon = NULL;
    proxy->start_func = proxy_start_fns[idx];
    proxy->stop_func = proxy_stop_fns[idx];
    proxy->back_func = llext_proxy_back;
    proxy->ui_unavailable_func = llext_proxy_ui_unavailable;
    proxy->ui_available_func = llext_proxy_ui_available;
    proxy->category = ZSW_APP_CATEGORY_ROOT;
    proxy->hidden = false;

    /* Register the proxy with the main app manager */
    zsw_app_manager_add_application(proxy);

    num_llext_apps++;
    LOG_INF("Discovered LLEXT app '%s' at %s (%zu bytes, slot %d)",
            la->name, elf_path, entry.size, idx);

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

    /* Ensure the apps directory exists for mcumgr uploads */
    ret = fs_mkdir(ZSW_LLEXT_APPS_BASE_PATH);
    if (ret < 0 && ret != -EEXIST) {
        LOG_WRN("Failed to create apps directory: %d", ret);
    }

    /* Initialize XIP flash allocator */
    ret = zsw_llext_xip_init();
    if (ret < 0) {
        LOG_WRN("XIP init failed: %d (continuing without XIP)", ret);
    }

    LOG_INF("Scanning for LLEXT apps in %s", ZSW_LLEXT_APPS_BASE_PATH);

    fs_dir_t_init(&dir);
    ret = fs_opendir(&dir, ZSW_LLEXT_APPS_BASE_PATH);
    if (ret < 0) {
        LOG_WRN("No apps directory found (%d), no LLEXT apps available", ret);
        return 0;
    }

    /* Discover all LLEXT apps (scan dirs, register proxies, NO loading) */
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
            LOG_INF("Auto-opening LLEXT app '%s'", ZSW_LLEXT_AUTO_OPEN_APP);
            lv_obj_t *root = lv_obj_create(lv_screen_active());

            lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
            proxy_start_common(i, root, NULL);
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
