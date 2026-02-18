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
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>
#include <zephyr/llext/llext.h>
#include <zephyr/llext/buf_loader.h>
#include "cJSON.h"
#include <string.h>

#include "managers/zsw_app_manager.h"
#include "managers/zsw_llext_app_manager.h"
#include "managers/zsw_llext_stream_loader.h"
#include "managers/zsw_llext_xip.h"
#include <lvgl.h>

LOG_MODULE_REGISTER(llext_app_mgr, LOG_LEVEL_INF);

#define ZSW_LLEXT_MAX_APPS          16
#define ZSW_LLEXT_APPS_BASE_PATH    "/lvgl_lfs/apps"
#define ZSW_LLEXT_MANIFEST_NAME     "manifest.json"
#define ZSW_LLEXT_ELF_NAME          "app.llext"
#define ZSW_LLEXT_MAX_PATH_LEN      80
#define ZSW_LLEXT_MAX_NAME_LEN      32
#define ZSW_LLEXT_MANIFEST_BUF_SIZE 256
#define ZSW_LLEXT_HEAP_SIZE         (40 * 1024) /* 40 KB for LLEXT heap */

/* Embedded test LLEXT binary (generated at build time) */
static const uint8_t embedded_llext_buf[] __aligned(4) = {
#include "about_ext.inc"
};

/* Heap buffer for LLEXT dynamic allocations */
static uint8_t llext_heap_buf[ZSW_LLEXT_HEAP_SIZE] __aligned(8);

/* Auto-start delayed work */
#define LLEXT_AUTO_START_DELAY_MS   5000
static void llext_auto_start_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(llext_auto_start_work, llext_auto_start_work_handler);

typedef application_t *(*llext_app_entry_fn)(void);

typedef struct {
    char name[ZSW_LLEXT_MAX_NAME_LEN];
    char dir_path[ZSW_LLEXT_MAX_PATH_LEN];
    bool loaded;
    struct llext *ext;
    application_t *app;
} zsw_llext_app_t;

/* Manifest JSON fields we care about */
struct llext_manifest {
    const char *name;
    const char *version;
    const char *entry_symbol;
};

static zsw_llext_app_t llext_apps[ZSW_LLEXT_MAX_APPS];
static int num_llext_apps;

static int read_manifest(const char *dir_path, struct llext_manifest *manifest, char *buf, size_t buf_size)
{
    char path[ZSW_LLEXT_MAX_PATH_LEN];
    struct fs_file_t file;
    ssize_t bytes_read;
    int ret;

    snprintk(path, sizeof(path), "%s/%s", dir_path, ZSW_LLEXT_MANIFEST_NAME);

    fs_file_t_init(&file);
    ret = fs_open(&file, path, FS_O_READ);
    if (ret < 0) {
        LOG_WRN("Failed to open manifest %s: %d", path, ret);
        return ret;
    }

    bytes_read = fs_read(&file, buf, buf_size - 1);
    fs_close(&file);

    if (bytes_read <= 0) {
        LOG_WRN("Failed to read manifest %s: %zd", path, bytes_read);
        return -EIO;
    }
    buf[bytes_read] = '\0';

    cJSON *root = cJSON_Parse(buf);
    if (root == NULL) {
        LOG_WRN("Failed to parse manifest %s", path);
        return -EINVAL;
    }

    cJSON *name_obj = cJSON_GetObjectItem(root, "name");
    cJSON *version_obj = cJSON_GetObjectItem(root, "version");
    cJSON *entry_obj = cJSON_GetObjectItem(root, "entry_symbol");

    if (!cJSON_IsString(name_obj) || !cJSON_IsString(entry_obj)) {
        LOG_WRN("Manifest %s missing required fields", path);
        cJSON_Delete(root);
        return -EINVAL;
    }

    /* Copy strings since cJSON memory will be freed */
    static char name_buf[ZSW_LLEXT_MAX_NAME_LEN];
    static char version_buf[ZSW_LLEXT_MAX_NAME_LEN];
    static char entry_buf[ZSW_LLEXT_MAX_NAME_LEN];

    strncpy(name_buf, name_obj->valuestring, sizeof(name_buf) - 1);
    manifest->name = name_buf;

    if (cJSON_IsString(version_obj)) {
        strncpy(version_buf, version_obj->valuestring, sizeof(version_buf) - 1);
        manifest->version = version_buf;
    }

    strncpy(entry_buf, entry_obj->valuestring, sizeof(entry_buf) - 1);
    manifest->entry_symbol = entry_buf;

    cJSON_Delete(root);
    return 0;
}

static int load_llext_app(const char *dir_path, const struct llext_manifest *manifest)
{
    char elf_path[ZSW_LLEXT_MAX_PATH_LEN];
    int ret;

    if (num_llext_apps >= ZSW_LLEXT_MAX_APPS) {
        LOG_ERR("Maximum LLEXT apps reached (%d)", ZSW_LLEXT_MAX_APPS);
        return -ENOMEM;
    }

    snprintk(elf_path, sizeof(elf_path), "%s/%s", dir_path, ZSW_LLEXT_ELF_NAME);

    /* Verify ELF file exists */
    struct fs_dirent entry;
    ret = fs_stat(elf_path, &entry);
    if (ret < 0) {
        LOG_WRN("ELF not found at %s: %d", elf_path, ret);
        return ret;
    }

    LOG_INF("Loading LLEXT app '%s' from %s (%zu bytes)", manifest->name, elf_path, entry.size);

    /* Use the streaming loader: ELF is parsed incrementally, .text/.rodata
     * go directly to XIP flash, .data/.bss go to the static data pool.
     * No LLEXT heap is consumed — the heap buffer serves as scratch space.
     */
    struct zsw_stream_load_result stream_result;

    ret = zsw_llext_stream_load(elf_path, manifest->entry_symbol,
                                llext_heap_buf, sizeof(llext_heap_buf),
                                &stream_result);
    if (ret != 0) {
        LOG_ERR("Stream load failed for '%s': %d", manifest->name, ret);
        return ret;
    }

    /* Call app_entry() to get the application_t pointer */
    llext_app_entry_fn entry_fn = (llext_app_entry_fn)stream_result.entry_fn;
    application_t *app = entry_fn();

    if (app == NULL) {
        LOG_ERR("app_entry() returned NULL for LLEXT '%s'", manifest->name);
        return -EINVAL;
    }

    /* Register with the main app manager */
    zsw_app_manager_add_application(app);

    /* Track this LLEXT app */
    zsw_llext_app_t *llext_app = &llext_apps[num_llext_apps];
    strncpy(llext_app->name, manifest->name, sizeof(llext_app->name) - 1);
    strncpy(llext_app->dir_path, dir_path, sizeof(llext_app->dir_path) - 1);
    llext_app->loaded = true;
    llext_app->ext = NULL;  /* streaming loader does not create an llext struct */
    llext_app->app = app;
    num_llext_apps++;

    LOG_INF("LLEXT app '%s' registered successfully (total: %d)", manifest->name, num_llext_apps);
    return 0;
}

static int load_embedded_llext_app(void)
{
    struct llext_load_param ldr_parm = LLEXT_LOAD_PARAM_DEFAULT;
    struct llext *ext = NULL;
    int ret;

    /* Initialize the LLEXT heap — only needed for the embedded app path
     * which uses llext_load(). The streaming loader uses the same buffer
     * as scratch space, so heap init must happen here (after scan completes).
     */
    ret = llext_heap_init(llext_heap_buf, sizeof(llext_heap_buf));
    if (ret != 0) {
        LOG_ERR("Failed to initialize LLEXT heap: %d", ret);
        return ret;
    }

    size_t buf_len = sizeof(embedded_llext_buf);
    LOG_INF("Loading embedded LLEXT app (%zu bytes)", buf_len);

    struct llext_buf_loader buf_loader = LLEXT_BUF_LOADER(
        (const uint8_t *)embedded_llext_buf, buf_len);

    ret = llext_load(&buf_loader.loader, "about_ext_emb", &ext, &ldr_parm);
    if (ret != 0) {
        LOG_ERR("Failed to load embedded LLEXT: %d", ret);
        return ret;
    }

    LOG_INF("Embedded LLEXT loaded, finding entry symbol 'app_entry'");

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

    zsw_app_manager_add_application(app);

    zsw_llext_app_t *llext_app = &llext_apps[num_llext_apps];
    strncpy(llext_app->name, "about_ext_emb", sizeof(llext_app->name) - 1);
    strncpy(llext_app->dir_path, "(embedded)", sizeof(llext_app->dir_path) - 1);
    llext_app->loaded = true;
    llext_app->ext = ext;
    llext_app->app = app;
    num_llext_apps++;

    LOG_INF("Embedded LLEXT app '%s' registered (total: %d)", app->name, num_llext_apps);
    return 0;
}

static void llext_auto_start_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (num_llext_apps == 0) {
        LOG_WRN("No LLEXT apps loaded, cannot auto-start");
        return;
    }

    LOG_INF("=== LLEXT Auto-Start Verification ===");
    for (int i = 0; i < num_llext_apps; i++) {
        zsw_llext_app_t *la = &llext_apps[i];
        LOG_INF("  App[%d]: name='%s', loaded=%d, path=%s", i, la->name, la->loaded, la->dir_path);
        if (la->app) {
            LOG_INF("  App[%d]: app_name='%s', category=%d, start_func=%p, stop_func=%p",
                    i, la->app->name, la->app->category,
                    (void *)la->app->start_func, (void *)la->app->stop_func);
        }
    }
    LOG_INF("=== LLEXT verification complete: %d app(s) ready ===", num_llext_apps);

    /* Auto-open the Battery LLEXT app for testing via delayed work.
     * Create a full-screen root and call start_func directly. */
    for (int i = 0; i < num_llext_apps; i++) {
        if (llext_apps[i].app && strcmp(llext_apps[i].app->name, "Battery") == 0) {
            application_t *bat_app = llext_apps[i].app;

            LOG_INF("Auto-opening '%s' via delayed work", bat_app->name);

            lv_obj_t *root = lv_obj_create(lv_scr_act());
            lv_obj_set_size(root, 240, 240);
            lv_obj_set_style_pad_all(root, 0, LV_PART_MAIN);
            lv_obj_set_style_border_width(root, 0, LV_PART_MAIN);
            lv_obj_set_style_radius(root, 0, LV_PART_MAIN);
            lv_obj_align(root, LV_ALIGN_CENTER, 0, 0);

            bat_app->current_state = ZSW_APP_STATE_UI_VISIBLE;
            bat_app->start_func(root, NULL);
            LOG_INF("'%s' start_func returned OK", bat_app->name);
            break;
        }
    }
}

int zsw_llext_app_manager_init(void)
{
    struct fs_dir_t dir;
    struct fs_dirent entry;
    int ret;
    char manifest_buf[ZSW_LLEXT_MANIFEST_BUF_SIZE];

    /* Initialize the XIP allocator for external flash installation */
    ret = zsw_llext_xip_init();
    if (ret != 0) {
        LOG_WRN("XIP allocator init failed: %d (XIP install disabled)", ret);
        /* Non-fatal: apps will load to RAM only */
    }

    /* NOTE: SMP BLE transport is NOT registered here because MCUmgr code
     * is relocated to external flash (XIP). When XIP is disabled (screen off),
     * the SMP thread would crash with a BUS FAULT. Instead, SMP BLE is
     * registered dynamically by the update app (which keeps XIP enabled).
     * Use the update app or 'mcumgr' over USB CDC for LLEXT uploads.
     */

    /* Ensure the apps directory exists for mcumgr uploads */
    ret = fs_mkdir(ZSW_LLEXT_APPS_BASE_PATH);
    if (ret < 0 && ret != -EEXIST) {
        LOG_WRN("Failed to create apps directory %s: %d (continuing anyway)",
                ZSW_LLEXT_APPS_BASE_PATH, ret);
    }

    LOG_INF("Scanning for LLEXT apps in %s", ZSW_LLEXT_APPS_BASE_PATH);

    fs_dir_t_init(&dir);
    ret = fs_opendir(&dir, ZSW_LLEXT_APPS_BASE_PATH);
    if (ret < 0) {
        if (ret == -ENOENT) {
            LOG_INF("No LLEXT apps directory found (%s), loading embedded test app",
                    ZSW_LLEXT_APPS_BASE_PATH);
            ret = load_embedded_llext_app();
            if (ret == 0) {
                LOG_INF("Scheduling auto-start in %d ms", LLEXT_AUTO_START_DELAY_MS);
                k_work_schedule(&llext_auto_start_work, K_MSEC(LLEXT_AUTO_START_DELAY_MS));
            }
            return ret;
        }
        LOG_ERR("Failed to open apps directory %s: %d", ZSW_LLEXT_APPS_BASE_PATH, ret);
        return ret;
    }

    while (true) {
        ret = fs_readdir(&dir, &entry);
        if (ret < 0) {
            LOG_ERR("Failed to read apps directory: %d", ret);
            break;
        }

        /* End of directory */
        if (entry.name[0] == '\0') {
            break;
        }

        /* Only interested in directories */
        if (entry.type != FS_DIR_ENTRY_DIR) {
            continue;
        }

        char app_dir[ZSW_LLEXT_MAX_PATH_LEN];
        snprintk(app_dir, sizeof(app_dir), "%s/%s", ZSW_LLEXT_APPS_BASE_PATH, entry.name);

        struct llext_manifest manifest = {0};
        ret = read_manifest(app_dir, &manifest, manifest_buf, sizeof(manifest_buf));
        if (ret < 0) {
            LOG_WRN("Skipping %s (no valid manifest)", entry.name);
            continue;
        }

        ret = load_llext_app(app_dir, &manifest);
        if (ret < 0) {
            LOG_ERR("Failed to load LLEXT app from %s: %d", app_dir, ret);
            /* Continue to try other apps */
        }
    }

    fs_closedir(&dir);

    LOG_INF("LLEXT app scan complete: %d filesystem app(s) loaded", num_llext_apps);

    /* Load embedded test app only when no filesystem apps were found.
     * The embedded app stays fully in RAM (no XIP) so it wastes heap space;
     * skip it when real filesystem apps are available.
     */
    if (num_llext_apps == 0) {
        ret = load_embedded_llext_app();
        if (ret != 0) {
            LOG_WRN("Failed to load embedded LLEXT app: %d", ret);
        }
    } else {
        LOG_INF("Filesystem apps loaded, skipping embedded LLEXT");
    }

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
