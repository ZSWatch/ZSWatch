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
 * At boot, the filesystem is scanned for LLEXT app directories. Each app's
 * manifest.json is read to extract metadata (name, entry symbol). A lightweight
 * proxy application_t is registered with the app manager for each discovered
 * app. The actual LLEXT shared library is NOT loaded at boot.
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
#include <zephyr/llext/buf_loader.h>
#include <zephyr/llext/fs_loader.h>
#include "cJSON.h"
#include <string.h>

#include "managers/zsw_app_manager.h"
#include "managers/zsw_llext_app_manager.h"
#include <lvgl.h>

LOG_MODULE_REGISTER(llext_app_mgr, LOG_LEVEL_INF);

/* --------------------------------------------------------------------------
 * Configuration
 * -------------------------------------------------------------------------- */

#define ZSW_LLEXT_MAX_APPS          10
#define ZSW_LLEXT_APPS_BASE_PATH    "/lvgl_lfs/apps"
#define ZSW_LLEXT_MANIFEST_NAME     "manifest.json"
#define ZSW_LLEXT_ELF_NAME          "app.llext"
#define ZSW_LLEXT_MAX_PATH_LEN      80
#define ZSW_LLEXT_MAX_NAME_LEN      32
#define ZSW_LLEXT_MANIFEST_BUF_SIZE 256
#define ZSW_LLEXT_HEAP_SIZE         (45 * 1024)

/* Embedded test LLEXT binary (generated at build time) */
static const uint8_t embedded_llext_buf[] __aligned(4) = {
#include "about_ext.inc"
};

/* --------------------------------------------------------------------------
 * Types
 * -------------------------------------------------------------------------- */

typedef application_t *(*llext_app_entry_fn)(void);

typedef struct {
    char name[ZSW_LLEXT_MAX_NAME_LEN];
    char dir_path[ZSW_LLEXT_MAX_PATH_LEN];
    char entry_symbol[ZSW_LLEXT_MAX_NAME_LEN];

    /* Proxy app registered with the main app manager at discovery time.
     * Its start/stop functions are trampolines that trigger deferred loading.
     */
    application_t proxy_app;

    /* Runtime state — populated when the LLEXT is actually loaded */
    struct llext *ext;
    application_t *real_app;  /* Points into LLEXT memory, valid only while loaded */
    bool loaded;
    bool is_embedded;
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

/* Auto-start delayed work for testing */
#define LLEXT_AUTO_START_DELAY_MS   5000
static void llext_auto_start_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(llext_auto_start_work, llext_auto_start_work_handler);

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
        return active_llext_app->real_app->back_func();
    }
    return false;
}

static void llext_proxy_ui_unavailable(void)
{
    if (active_llext_app && active_llext_app->real_app &&
        active_llext_app->real_app->ui_unavailable_func) {
        active_llext_app->real_app->ui_unavailable_func();
    }
}

static void llext_proxy_ui_available(void)
{
    if (active_llext_app && active_llext_app->real_app &&
        active_llext_app->real_app->ui_available_func) {
        active_llext_app->real_app->ui_available_func();
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
        la->real_app->start_func(root, group);
        return;
    }

    LOG_INF("Deferred loading LLEXT '%s' from %s", la->name, la->dir_path);

    ret = ensure_heap_init();
    if (ret != 0) {
        LOG_ERR("Heap init failed: %d", ret);
        return;
    }

    /* Load the ELF from filesystem */
    char elf_path[ZSW_LLEXT_MAX_PATH_LEN];

    snprintk(elf_path, sizeof(elf_path), "%s/%s", la->dir_path, ZSW_LLEXT_ELF_NAME);

    struct llext_fs_loader fs_loader = LLEXT_FS_LOADER(elf_path);
    struct llext_load_param ldr_parm = LLEXT_LOAD_PARAM_DEFAULT;

    ret = llext_load(&fs_loader.loader, la->name, &la->ext, &ldr_parm);
    if (ret != 0) {
        LOG_ERR("llext_load failed for '%s': %d", la->name, ret);
        return;
    }

    LOG_INF("LLEXT '%s' loaded, finding entry symbol '%s'", la->name, la->entry_symbol);

    /* Find and call the extension's app_entry to get the application_t */
    llext_app_entry_fn entry_fn = llext_find_sym(&la->ext->exp_tab, la->entry_symbol);

    if (entry_fn == NULL) {
        LOG_ERR("Entry symbol '%s' not found in LLEXT '%s'", la->entry_symbol, la->name);
        llext_unload(&la->ext);
        la->ext = NULL;
        return;
    }

    la->real_app = entry_fn();
    if (la->real_app == NULL) {
        LOG_ERR("app_entry() returned NULL for LLEXT '%s'", la->name);
        llext_unload(&la->ext);
        la->ext = NULL;
        return;
    }

    la->loaded = true;
    active_llext_app = la;

    LOG_INF("LLEXT '%s' ready: real_name='%s', category=%d",
            la->name, la->real_app->name, la->real_app->category);

    /* Start the real app UI */
    la->real_app->current_state = ZSW_APP_STATE_UI_VISIBLE;
    la->real_app->start_func(root, group);

    LOG_INF("LLEXT '%s' start_func completed", la->name);
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
        la->real_app->stop_func();
    }

    la->real_app = NULL;

    /* Unload the extension and free all heap memory */
    if (la->ext) {
        LOG_INF("Unloading LLEXT '%s'", la->name);
        llext_unload(&la->ext);
        la->ext = NULL;
    }

    la->loaded = false;

    if (active_llext_app == la) {
        active_llext_app = NULL;
    }

    LOG_INF("LLEXT '%s' unloaded, heap freed", la->name);
}

/* --------------------------------------------------------------------------
 * Manifest Reading
 * -------------------------------------------------------------------------- */

static int read_manifest(const char *dir_path, zsw_llext_app_t *app)
{
    char path[ZSW_LLEXT_MAX_PATH_LEN];
    char buf[ZSW_LLEXT_MANIFEST_BUF_SIZE];
    struct fs_file_t file;
    ssize_t bytes_read;
    int ret;

    snprintk(path, sizeof(path), "%s/%s", dir_path, ZSW_LLEXT_MANIFEST_NAME);

    fs_file_t_init(&file);
    ret = fs_open(&file, path, FS_O_READ);
    if (ret < 0) {
        return ret;
    }

    bytes_read = fs_read(&file, buf, sizeof(buf) - 1);
    fs_close(&file);

    if (bytes_read <= 0) {
        return -EIO;
    }
    buf[bytes_read] = '\0';

    cJSON *root = cJSON_Parse(buf);

    if (root == NULL) {
        LOG_WRN("Failed to parse manifest %s", path);
        return -EINVAL;
    }

    cJSON *name_obj = cJSON_GetObjectItem(root, "name");
    cJSON *entry_obj = cJSON_GetObjectItem(root, "entry_symbol");

    if (cJSON_IsString(name_obj)) {
        strncpy(app->name, name_obj->valuestring, sizeof(app->name) - 1);
    }

    if (cJSON_IsString(entry_obj)) {
        strncpy(app->entry_symbol, entry_obj->valuestring, sizeof(app->entry_symbol) - 1);
    } else {
        strncpy(app->entry_symbol, "app_entry", sizeof(app->entry_symbol) - 1);
    }

    cJSON_Delete(root);
    return 0;
}

/* --------------------------------------------------------------------------
 * App Discovery (no loading, just filesystem scan + proxy registration)
 * -------------------------------------------------------------------------- */

static int discover_llext_app(const char *dir_path, const char *dir_name)
{
    int idx;
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

    /* Try to read manifest for app name and entry symbol */
    int ret = read_manifest(dir_path, la);

    if (ret < 0) {
        /* No manifest — use directory name and default entry symbol */
        strncpy(la->name, dir_name, sizeof(la->name) - 1);
        strncpy(la->entry_symbol, "app_entry", sizeof(la->entry_symbol) - 1);
        LOG_INF("No manifest for '%s', using dir name", dir_name);
    }

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
 * Embedded LLEXT (fallback when no filesystem apps exist)
 * -------------------------------------------------------------------------- */

static int load_embedded_llext_app(void)
{
    struct llext_load_param ldr_parm = LLEXT_LOAD_PARAM_DEFAULT;
    struct llext *ext = NULL;
    int ret;

    ret = ensure_heap_init();
    if (ret != 0) {
        return ret;
    }

    LOG_INF("Loading embedded LLEXT app (%zu bytes)", sizeof(embedded_llext_buf));

    struct llext_buf_loader buf_loader = LLEXT_BUF_LOADER(
        (const uint8_t *)embedded_llext_buf, sizeof(embedded_llext_buf));

    ret = llext_load(&buf_loader.loader, "about_ext_emb", &ext, &ldr_parm);
    if (ret != 0) {
        LOG_ERR("Failed to load embedded LLEXT: %d", ret);
        return ret;
    }

    llext_app_entry_fn entry_fn = llext_find_sym(&ext->exp_tab, "app_entry");

    if (entry_fn == NULL) {
        LOG_ERR("Entry symbol 'app_entry' not found in embedded LLEXT");
        llext_unload(&ext);
        return -ENOENT;
    }

    application_t *app = entry_fn();

    if (app == NULL) {
        LOG_ERR("app_entry() returned NULL for embedded LLEXT");
        llext_unload(&ext);
        return -EINVAL;
    }

    /* Register the real app directly (embedded stays loaded forever) */
    zsw_app_manager_add_application(app);

    /* Track it */
    zsw_llext_app_t *la = &llext_apps[num_llext_apps];

    memset(la, 0, sizeof(*la));
    strncpy(la->name, "about_ext_emb", sizeof(la->name) - 1);
    strncpy(la->dir_path, "(embedded)", sizeof(la->dir_path) - 1);
    la->loaded = true;
    la->is_embedded = true;
    la->ext = ext;
    la->real_app = app;
    num_llext_apps++;

    LOG_INF("Embedded LLEXT '%s' loaded and registered (total: %d)",
            app->name, num_llext_apps);

    return 0;
}

/* --------------------------------------------------------------------------
 * Auto-Start (for testing)
 * -------------------------------------------------------------------------- */

static void llext_auto_start_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (num_llext_apps == 0) {
        LOG_WRN("No LLEXT apps discovered, cannot auto-start");
        return;
    }

    LOG_INF("=== LLEXT Auto-Start Verification ===");
    for (int i = 0; i < num_llext_apps; i++) {
        zsw_llext_app_t *la = &llext_apps[i];

        LOG_INF("  App[%d]: '%s' at %s (loaded=%d, embedded=%d)",
                i, la->name, la->dir_path, la->loaded, la->is_embedded);
    }
    LOG_INF("=== %d app(s) discovered ===", num_llext_apps);

    /* Try to auto-start 'battery_real_ext' for testing, otherwise first app */
    zsw_llext_app_t *target = &llext_apps[0];

    for (int i = 0; i < num_llext_apps; i++) {
        if (strcmp(llext_apps[i].name, "battery_real_ext") == 0) {
            target = &llext_apps[i];
            break;
        }
    }

    LOG_INF("Auto-opening '%s' for testing (deferred load)", target->name);

    lv_obj_t *root = lv_obj_create(lv_scr_act());

    lv_obj_set_size(root, 240, 240);
    lv_obj_set_style_pad_all(root, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(root, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(root, 0, LV_PART_MAIN);
    lv_obj_align(root, LV_ALIGN_CENTER, 0, 0);

    target->proxy_app.current_state = ZSW_APP_STATE_UI_VISIBLE;
    target->proxy_app.start_func(root, NULL);
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

    LOG_INF("Scanning for LLEXT apps in %s", ZSW_LLEXT_APPS_BASE_PATH);

    fs_dir_t_init(&dir);
    ret = fs_opendir(&dir, ZSW_LLEXT_APPS_BASE_PATH);
    if (ret < 0) {
        if (ret == -ENOENT) {
            LOG_INF("No apps directory, loading embedded test app");
            load_embedded_llext_app();
        } else {
            LOG_ERR("Failed to open apps directory: %d", ret);
        }
        goto schedule;
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

    /* Load embedded app only when no filesystem apps exist */
    if (num_llext_apps == 0) {
        ret = load_embedded_llext_app();
        if (ret != 0) {
            LOG_WRN("Failed to load embedded LLEXT: %d", ret);
        }
    }

schedule:
    if (num_llext_apps > 0) {
        LOG_INF("Scheduling auto-start in %d ms", LLEXT_AUTO_START_DELAY_MS);
        k_work_schedule(&llext_auto_start_work, K_MSEC(LLEXT_AUTO_START_DELAY_MS));
    }

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
