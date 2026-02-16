# ZSWatch LLEXT Dynamic Apps — Implementation Plan

## Overview

Enable runtime-loadable apps on ZSWatch using Zephyr's LLEXT (Linkable Loadable Extensions) subsystem. Users will be able to install apps from a phone, transferred over BLE to LittleFS, and loaded at boot time. The LLEXT ELF file is uploaded to LittleFS first, then at boot the loader processes it: `.text`/`.rodata` are written to a dedicated XIP flash region, and `.data`/`.bss` are allocated in RAM.

### Architecture: Approach D — Hybrid Stub + XIP

Each LLEXT app consists of a **single ELF file** that the watch-side loader splits into:

| Component | Placement | RAM Cost | When Accessible |
|-----------|-----------|----------|-----------------|
| Stub code (lifecycle, zbus callbacks, background logic) | RAM heap | ~2-4 KB / app | Always |
| UI code (.text for LVGL, .rodata, images) | Dedicated XIP region (ext. flash) | 0 | Screen on only |
| Mutable data (.data, .bss) | RAM heap | ~100-500 bytes / app | Always |

### Key Decisions Made

- **Phase 1 app**: `about_app` — simple foreground app with images, good for validating the full pipeline. Created as a parallel copy; the original built-in about_app is kept during development
- **Background support**: From day one — apps set up background work (zbus observers, timers) in their `app_entry()` function, same as built-in apps do in `SYS_INIT`. The `_ui.c` files go to XIP, the `_app.c` logic stays in RAM — mirroring the existing CMake `RELOCATE_FILES` pattern where `*_ui.c` is marked for XIP
- **BLE transfer**: MCUmgr `fs_mgmt` over SMP BLE (already have SMP for DFU)
- **XIP safety**: Existing `ui_unavailable_func` / `ui_available_func` callbacks in `application_t` handle screen off/on transitions — LLEXT apps use these the same way built-in apps do (e.g. `music_control_app.c` uses `ui_available_func`)
- **App manifest**: Small JSON file per app in LFS at `/lfs/apps/<name>/manifest.json` — loader scans `/lfs/apps/*/manifest.json` at boot to discover installed apps
- **Stubs**: RAM-only initially (internal flash partition can be added later if RAM gets tight — not a big change)
- **Build**: Use Zephyr's `CONFIG_LLEXT_EDK` to generate an Extension Development Kit, then build LLEXT apps against it. In-repo first (`app/src/llext_apps/`), separate repo later. Avoid custom build tooling — use Zephyr's `add_llext_target()` CMake function or EDK mechanism
- **MAX_APPS**: Some built-in apps will eventually be converted to LLEXT, freeing slots. Short-term: bump `MAX_APPS` to 35
- **Images**: LLEXT apps use standard `LV_IMG_DECLARE` / `&var_name` — images compile into ELF `.rodata` and go to XIP alongside code. No `ZSW_LV_IMG_` macros. App icon for the launcher is also in `.rodata` — no separate `icon.bin` file needed
- **Development loading**: Reuse `west upload_fs` to push `.llext` files + manifest to LittleFS via J-Link. No phone app needed initially
- **Install flow**: Apps are **always installed at boot**. Upload ELF to LFS → reboot → loader discovers new/changed apps, processes them (writes XIP sections to flash partition), and registers them. No hot-install during runtime — keeps complexity down
- **Partition constraint**: Existing partitions **must not be changed** (not moved, not resized). The LLEXT XIP partition is placed in the ~50 MB of free space after `settings_storage` (external flash is 64 MB, only ~14 MB used)
- **native_sim**: LLEXT does **NOT** support native_sim/posix — the `posix` arch lacks an ELF relocation handler. All development and testing must be done on real hardware (or QEMU ARM if needed)

---

## XIP — How It Works on nRF5340

The nRF5340 QSPI peripheral maps the **entire external flash** into the CPU address space starting at `0x10000000`. The DTS defines the XIP memory-mapped window as 256 MB:

```dts
qspi: qspi@2b000 {
    reg = <0x2b000 0x1000>, <0x10000000 0x10000000>;
    reg-names = "qspi", "qspi_mm";
};
```

This means **any byte at flash offset N is readable at CPU address `0x10000000 + N`** when XIP is enabled. There is no fixed "XIP region" — the whole flash is XIP-addressable. The LLEXT XIP partition does NOT need to be at any special address; it just needs to exist somewhere in external flash.

XIP is enabled/disabled via `nrf_qspi_nor_xip_enable()`. In ZSWatch, this is tied to the display: XIP ON when screen wakes, XIP OFF when screen sleeps (in `zsw_display_control.c`). The `XIPOFFSET` register defaults to 0.

**Implication**: We can place the `llext_xip_partition` anywhere in external flash and it will be XIP-executable at `0x10000000 + partition_address`.

---

## External Flash Layout

### Current Layout (MCUboot enabled — `pm_static_watchdk_nrf5340_cpuapp_1.yml`)

MCUboot is **always enabled** in ZSWatch. The external flash chip is **64 MB** (Macronix MX25U51245G). Current layout:

| Region | Address | Size | Purpose |
|--------|---------|------|---------|
| mcuboot_secondary | 0x000000 | 848 KB | MCUboot secondary slot |
| mcuboot_secondary_1 | 0x0D4000 | 256 KB | MCUboot net core secondary |
| EMPTY_1 | 0x114000 | 48 KB | Unused |
| mcuboot_primary_2 | 0x120000 | 1 MB | XIP code (main FW, ext flash part) |
| mcuboot_secondary_2 | 0x220000 | 1 MB | MCUboot XIP secondary |
| littlefs_storage | 0x320000 | 2 MB | LittleFS filesystem |
| lvgl_raw_partition | 0x520000 | 8 MB | Raw LVGL image data |
| settings_storage | 0xD20000 | 1 MB | Settings |
| *(free)* | 0xE20000 | **~49.9 MB** | Unused |

**Total**: ~14.1 MB used out of 64 MB. Nearly 50 MB is completely free.

### Proposed Layout (add LLEXT XIP region)

Place `llext_xip_partition` in the free space **after** `settings_storage`. **No existing partitions are changed** — nothing is moved, resized, or modified:

| Region | Address | Size | Purpose | Changed? |
|--------|---------|------|---------|----------|
| *(all existing partitions)* | 0x000000 – 0xE20000 | 14.1 MB | *(unchanged)* | **No** |
| **llext_xip_partition** | **0xE20000** | **4 MB** | **XIP-executable LLEXT app data** | **New** |
| *(free)* | 0x1220000 | ~45.9 MB | Available for future use | — |

The XIP base address for LLEXT apps: `0x10000000 + 0xE20000 = 0x10E20000`

4 MB provides generous room. With a typical app being 50-200 KB (including images), this fits 20+ apps comfortably. Apps with many images can be larger without issue — the allocator is flexible (see below).

> **Note**: This change must be applied to ALL `pm_static_*` variant files that are in active use. The `pm_static.yml` (non-MCUboot) is no longer used and can be ignored. The partition size can be increased later if needed — there is ~46 MB of free space remaining.

---

## App Storage & Install Flow

### Two-Stage Process

**Stage 1 — Upload (via BLE or J-Link):**
The `.llext` ELF file and `manifest.json` are uploaded to LittleFS:
```
/lfs/apps/<name>/manifest.json
/lfs/apps/<name>/app.llext
```

**Stage 2 — Install (at boot):**
On every boot, the LLEXT app manager scans `/lfs/apps/*/manifest.json`:
1. If a new/changed app is found (not yet installed, or ELF hash changed):
   - Read the ELF from LFS
   - Allocate an XIP slot in `llext_xip_partition`
   - Use `llext_load()` — LLEXT processes the ELF, allocates `.text`/`.rodata` in the XIP slot, `.data`/`.bss` in RAM
   - Record the XIP slot assignment in the manifest
2. If an already-installed app is found:
   - Load it using the stored XIP slot info (fast path — XIP sections already in flash)
3. Call `app_entry()` → get `application_t*` → register with `zsw_app_manager`
4. For background apps: the `app_entry()` function also sets up zbus observers, timers, etc.

**Why not install on BLE receive?** The install process needs XIP enabled (to write to the XIP partition), which ties into display/power state. Boot-time install is simpler and avoids race conditions. A reboot after upload is acceptable.

### How the LLEXT Loader Knows Where to Put Sections

The LLEXT subsystem handles this. When using `llext_load()` with the standard heap or with `llext_heap_init()`:
- LLEXT allocates memory regions for each ELF section from its configured heap
- For XIP placement: we use `llext_heap_init()` to point the LLEXT instruction heap at the XIP flash region, so `.text`/`.rodata` allocations come from XIP flash
- Alternatively, use the `section_detached` callback or `pre_located` mode for explicit section placement

### XIP Partition Allocator — Linear (Arena) Allocator

Instead of fixed-size slots (which waste space for small apps and limit large apps), the `llext_xip_partition` uses a **linear allocator**:

- Apps are packed sequentially into the partition, each getting exactly the space it needs (rounded up to flash erase-block alignment, typically 4 KB)
- An **allocation table** stored in LFS (`/lfs/llext_alloc.json`) tracks each app's placement:
  ```json
  {
    "entries": [
      { "name": "about_ext",   "offset": 0,      "size": 32768  },
      { "name": "weather_ext", "offset": 32768,   "size": 204800 },
      { "name": "game_ext",    "offset": 237568,  "size": 65536  }
    ]
  }
  ```
- **Install**: Find the first contiguous gap that fits the app's XIP data, write it there, update the table
- **Uninstall**: Remove the entry from the table, erase the flash region — the space becomes a free gap
- **Compaction**: If no single gap is large enough but total free space suffices, compact by copying all live entries to close gaps. This is rare in practice with 4 MB available
- **Benefits**: A 30 KB app uses 32 KB (aligned). A 500 KB app with many images uses 500 KB. No artificial cap per app, no wasted space in oversized slots

The allocation table also stores each app's XIP base address, which the loader needs to set up `pre_located` mode or to configure the LLEXT heap.

### LittleFS Directory Structure

```
/lfs/
  apps/
    about_ext/
      manifest.json       ← app metadata
      app.llext            ← ELF file (raw ELF kept for re-install after FW update)
    weather_ext/
      manifest.json
      app.llext
```

### manifest.json Schema

```json
{
  "name": "About",
  "version": "1.0.0",
  "fw_compat": "3.3.0",
  "entry_symbol": "app_entry",
  "installed": false
}
```

- `fw_compat`: Minimum firmware version required. Loader checks this at boot and skips incompatible apps
- `entry_symbol`: The exported function name in the LLEXT ELF that returns the `application_t*`
- `installed`: Set to `true` by the loader after successful install (XIP data written, entry in allocation table). On next boot, if `true`, the loader uses the allocation table for the fast path (XIP sections already in flash). After a firmware update, all manifests should have `installed` reset to `false` to force re-install (relocations may have changed)

**Fields NOT in the manifest** (handled elsewhere):
- `category` — already in `application_t` struct, set by the app code itself
- `background` — determined by the app's behavior in `app_entry()`, not a manifest flag. If the app sets up zbus observers or timers in `app_entry()`, it runs in the background. This mirrors how built-in apps work
- `icon` — compiled into the app's `.rodata` as a standard `lv_img_dsc_t`. The `application_t.icon` field points to it, just like built-in apps. When the app is loaded (XIP enabled, screen on — which is when the user browses apps), the icon is accessible

---

## Background App Pattern

The background pattern for LLEXT apps mirrors exactly how built-in apps work today, just with runtime API calls instead of compile-time macros.

### Built-in app (today):
```c
/* battery_app.c */
ZBUS_LISTENER_DEFINE(battery_app_battery_event, zbus_battery_sample_data_callback);
ZBUS_CHAN_ADD_OBS(battery_sample_data_chan, battery_app_battery_event, 1);

static int battery_app_add(void) {
    zsw_app_manager_add_application(&app);
    zsw_history_init(...);
    return 0;
}
SYS_INIT(battery_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
```

### LLEXT app (equivalent):
```c
/* battery_ext_app.c */
static struct zbus_observer battery_ext_listener;

application_t *app_entry(void) {
    /* Runtime equivalents of the compile-time macros */
    zbus_obs_init(&battery_ext_listener, "battery_ext_lis",
                  zbus_battery_sample_data_callback);
    zbus_chan_add_obs(&battery_sample_data_chan, &battery_ext_listener, K_MSEC(100));

    zsw_history_init(...);
    return &app;  /* app manager registration happens in the loader */
}
EXPORT_SYMBOL(app_entry);
```

The CMake for built-in apps already splits `_ui.c` → XIP and `_app.c` → internal flash. For LLEXT apps, the same convention applies naturally:
- Code in `_app.c` (background logic, zbus callbacks) stays in RAM (it's small)
- Code in `_ui.c` (LVGL widgets, images) goes to XIP `.rodata`/`.text`

The `application_t` struct has `ui_unavailable_func` and `ui_available_func` callbacks. LLEXT apps that need to update UI from background callbacks **must** check `app.current_state == ZSW_APP_STATE_UI_VISIBLE` before calling any UI/XIP code — exactly as built-in apps already do (see `battery_app.c`, `music_control_app.c`).

---

## Implementation Steps

### Step 0: Infrastructure — Enable LLEXT and Increase MAX_APPS

**Goal**: Get the LLEXT subsystem compiling into ZSWatch firmware.

**Prerequisite**: Before any LLEXT changes, verify the current firmware builds and runs correctly with `CONFIG_ARM_MPU=n`. LLEXT requires MPU disabled (or `CONFIG_USERSPACE=y`), so we must confirm no existing functionality regresses when MPU is turned off. Build for `watchdk@1/nrf5340/cpuapp`, flash, and test all built-in apps, BLE, display, sensors, etc. Fix any issues before proceeding.

**Changes**:

1. **prj.conf** — Add LLEXT Kconfig:
   ```
   CONFIG_LLEXT=y
   CONFIG_LLEXT_TYPE_ELF_RELOCATABLE=y
   CONFIG_LLEXT_HEAP_SIZE=16
   CONFIG_LLEXT_SHELL=y
   CONFIG_LLEXT_EDK=y
   ```
   - Use `ELF_RELOCATABLE` (partially linked `.o`) — default for ARM, no `-fPIC` needed
   - 16 KB heap for LLEXT metadata during load (temporary, freed after load)
   - Shell commands enabled for early debugging
   - EDK generation enabled — produces `llext-edk.tar.xz` with headers + compiler flags

2. **zsw_app_manager.c** — Increase `MAX_APPS` from 25 to 35

3. **pm_static_watchdk_nrf5340_cpuapp_1.yml** (and other active MCUboot variants) — Add `llext_xip_partition` in the free space after `settings_storage`:
   ```yaml
   llext_xip_partition:
     address: 0xE20000
     end_address: 0x1220000
     region: external_flash
     size: 0x400000
   ```
   **No existing partitions are changed** — this uses previously unused flash space.

4. **Kconfig** — Add new ZSWatch Kconfig:
   ```
   config ZSW_LLEXT_APPS
       bool "Enable LLEXT dynamic app loading"
       default n
       select LLEXT
       select LLEXT_TYPE_ELF_RELOCATABLE
   ```

**Verification**:
- Build ZSWatch with `CONFIG_ZSW_LLEXT_APPS=y` for `watchdk@1/nrf5340/cpuapp`
- Firmware boots, built-in apps work normally
- `llext` shell commands available (e.g. `llext list` shows no extensions)
- `west build -t llext-edk` produces the EDK archive
- Flash usage confirms LLEXT subsystem cost (~2-4 KB flash, ~16 KB heap configured)

---

### Step 1: Export Symbol Table

**Goal**: Base firmware exports the symbols that LLEXT apps need to call.

**Changes**:

1. Create `app/src/llext_apps/zsw_llext_exports.c` — Central file with all `EXPORT_SYMBOL()` calls:

   ```c
   #include <zephyr/llext/symbol.h>
   // ... include all headers for exported functions

   /* Zephyr kernel */
   EXPORT_SYMBOL(k_msleep);
   EXPORT_SYMBOL(k_malloc);
   EXPORT_SYMBOL(k_free);
   EXPORT_SYMBOL(printk);

   /* ZSWatch app manager */
   EXPORT_SYMBOL(zsw_app_manager_add_application);
   EXPORT_SYMBOL(zsw_app_manager_app_close_request);
   EXPORT_SYMBOL(zsw_app_manager_exit_app);
   EXPORT_SYMBOL(zsw_app_manager_get_num_apps);

   /* LVGL core (curated subset — add more as needed by apps) */
   EXPORT_SYMBOL(lv_obj_create);
   EXPORT_SYMBOL(lv_obj_del);
   EXPORT_SYMBOL(lv_obj_set_size);
   EXPORT_SYMBOL(lv_obj_set_style_bg_color);
   EXPORT_SYMBOL(lv_label_create);
   EXPORT_SYMBOL(lv_label_set_text);
   EXPORT_SYMBOL(lv_label_set_text_fmt);
   EXPORT_SYMBOL(lv_img_create);
   EXPORT_SYMBOL(lv_img_set_src);
   EXPORT_SYMBOL(lv_obj_set_flex_flow);
   EXPORT_SYMBOL(lv_obj_set_flex_align);
   EXPORT_SYMBOL(lv_obj_set_style_text_font);
   EXPORT_SYMBOL(lv_obj_set_style_text_color);
   EXPORT_SYMBOL(lv_obj_set_style_pad_all);
   EXPORT_SYMBOL(lv_obj_align);
   EXPORT_SYMBOL(lv_obj_set_scroll_dir);
   EXPORT_SYMBOL(lv_obj_remove_flag);
   // ... more LVGL functions as needed

   /* Zbus (runtime API) */
   EXPORT_SYMBOL(zbus_chan_read);
   EXPORT_SYMBOL(zbus_chan_add_obs);
   EXPORT_SYMBOL(zbus_chan_rm_obs);
   EXPORT_SYMBOL(zbus_obs_init);
   EXPORT_SYMBOL(zbus_chan_pub);

   /* Zbus channels (so LLEXT apps can observe/read them) */
   EXPORT_SYMBOL(battery_sample_data_chan);
   EXPORT_SYMBOL(ble_comm_data_chan);
   EXPORT_SYMBOL(activity_state_data_chan);
   EXPORT_SYMBOL(periodic_event_1s_chan);
   // ... add channels as needed

   /* Filesystem (for apps that need storage) */
   EXPORT_SYMBOL(zsw_filesystem_get_stats);

   /* Logging — LLEXT apps use printk() */
   EXPORT_SYMBOL(printk);
   ```

2. The exported symbols are automatically discovered by LLEXT at load time via the iterable section mechanism (Zephyr's `STRUCT_SECTION_ITERABLE`).

**Verification**:
- Build succeeds with the export file
- Build the EDK: `west build -t llext-edk` — ensures the EDK includes these exports
- Check `.map` file to confirm `llext_const_symbol` section exists and contains the exported names
- Use `llext` shell to verify the symbol table is populated

---

### Step 2: Build LLEXT about_ext App Using EDK

**Goal**: Create an LLEXT version of `about_app` and build it using Zephyr's EDK.

**New files**:
- `app/src/llext_apps/about_ext/about_ext_app.c`
- `app/src/llext_apps/about_ext/CMakeLists.txt` — uses Zephyr's `add_llext_target()` for in-tree build

**App structure**:

```c
/* about_ext_app.c — LLEXT version of about_app */
#include <zephyr/kernel.h>
#include <lvgl.h>
#include <zephyr/llext/symbol.h>
#include "managers/zsw_app_manager.h"

/* Images compiled into .rodata (will land in XIP) */
LV_IMG_DECLARE(ZSWatch_logo_small);
LV_IMG_DECLARE(zswatch_text);
LV_IMG_DECLARE(templates);  /* App icon — also in .rodata */

static void about_ext_start(lv_obj_t *root, lv_group_t *group);
static void about_ext_stop(void);

static application_t app = {
    .name = "About (ext)",
    .icon = &templates,  /* Icon is compiled into this ELF's .rodata */
    .start_func = about_ext_start,
    .stop_func = about_ext_stop,
    .category = ZSW_APP_CATEGORY_SYSTEM,
};

static lv_obj_t *main_page;

/* Entry point — called by LLEXT loader */
application_t *app_entry(void)
{
    /* No background work for about_app — just return the app struct */
    return &app;
}
EXPORT_SYMBOL(app_entry);

static void about_ext_start(lv_obj_t *root, lv_group_t *group)
{
    main_page = lv_obj_create(root);
    /* ... build UI using LVGL calls ... */
    lv_obj_t *logo = lv_img_create(main_page);
    lv_img_set_src(logo, &ZSWatch_logo_small);
    /* ... labels, layout, etc. */
}

static void about_ext_stop(void)
{
    lv_obj_del(main_page);
    main_page = NULL;
}
```

**Build system** (in-tree, using Zephyr CMake helpers):

```cmake
# app/src/llext_apps/about_ext/CMakeLists.txt
if(CONFIG_ZSW_LLEXT_APPS)
  add_llext_target(about_ext
    OUTPUT  ${PROJECT_BINARY_DIR}/llext/about_ext.llext
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/about_ext_app.c
            ${CMAKE_SOURCE_DIR}/src/images/ZSWatch_logo_small.c
            ${CMAKE_SOURCE_DIR}/src/images/zswatch_text.c
            ${CMAKE_SOURCE_DIR}/src/images/templates.c
  )
endif()
```

Zephyr's `add_llext_target()` handles:
- Compiling with correct ARM flags (from `LLEXT_APPEND_FLAGS`: `-mlong-calls`, `-mthumb`)
- Partial linking into a single relocatable ELF (`-r` flag for `ELF_RELOCATABLE`)
- Adding `-DLL_EXTENSION_BUILD` define
- Stripping unnecessary sections

**Verification**:
1. `west build` produces `build/llext/about_ext.llext` alongside the main firmware
2. Inspect the ELF: `arm-zephyr-eabi-readelf -S about_ext.llext` — shows `.text`, `.rodata`, `.data`, `.bss` sections
3. Verify `app_entry` is in the symbol table: `arm-zephyr-eabi-nm about_ext.llext | grep app_entry`
4. Verify image data is in `.rodata`: file size should be substantially larger than just code

---

### Step 3: LLEXT App Loader Manager

**Goal**: Create the watch-side manager that discovers, installs, loads, and unloads LLEXT apps.

**New files**:
- `app/src/managers/zsw_llext_app_manager.h`
- `app/src/managers/zsw_llext_app_manager.c`

**Boot-time flow**:

```
zsw_llext_app_manager_init()  [called via SYS_INIT at APPLICATION level]
  │
  ├── Scan /lfs/apps/*/manifest.json
  │
  ├── For each manifest:
  │     ├── Check fw_compat against running firmware version
  │     │     └── Skip if incompatible (log warning)
  │     │
  │     ├── If installed == true (already installed):
  │     │     └── Fast path: look up allocation table, map XIP sections, load .data/.bss to RAM
  │     │
  │     ├── If installed == false (new/needs install):
  │     │     ├── Read ELF from LFS
  │     │     ├── Allocate space in XIP partition (linear allocator)
  │     │     ├── llext_load() → .text/.rodata to XIP, .data/.bss to RAM
  │     │     └── Update allocation table + set manifest installed=true
  │     │
  │     ├── Call app_entry() → get application_t*
  │     │     (app_entry may also set up zbus observers, timers for background work)
  │     │
  │     └── zsw_app_manager_add_application(app)
  │
  └── Done — LLEXT apps appear in app list alongside built-in apps
```

**Phase 1 simplification**: Skip XIP entirely. Use `LLEXT_FS_LOADER` to load everything into RAM. This validates the full pipeline without XIP complexity:

```c
#include <zephyr/llext/llext.h>
#include <zephyr/llext/fs_loader.h>

struct llext_fs_loader fs_loader = LLEXT_FS_LOADER("/lfs/apps/about_ext/app.llext");
struct llext_load_param ldr_parm = LLEXT_LOAD_PARAM_DEFAULT;
struct llext *ext;

int ret = llext_load(&fs_loader.loader, "about_ext", &ext, &ldr_parm);
if (ret == 0) {
    application_t *(*entry)(void) = llext_find_sym(&ext->exp_tab, "app_entry");
    if (entry) {
        application_t *app = entry();
        zsw_app_manager_add_application(app);
    }
}
```

**Key Data Structures**:

```c
#define ZSW_LLEXT_MAX_APPS          16

typedef struct {
    char name[32];
    char path[64];              /* LFS path: /lfs/apps/<name>/ */
    bool loaded;
    bool installed;             /* true = XIP data written, in allocation table */
    uint32_t xip_offset;        /* Offset within llext_xip_partition */
    uint32_t xip_size;          /* Size of XIP data (flash-aligned) */
    struct llext *ext;          /* Zephyr LLEXT handle */
    application_t *app;         /* Pointer into LLEXT .data */
} zsw_llext_app_t;
```

**Verification (Phase 1 — all RAM, no XIP)**:
1. Place `about_ext.llext` + `manifest.json` in LFS (via `west upload_fs` with the files included in the LFS image source directory)
2. Boot → loader discovers `/lfs/apps/about_ext/manifest.json`
3. Loader calls `llext_load()` with filesystem loader → all sections in RAM
4. `app_entry()` called → `application_t*` returned → registered with app manager
5. "About (ext)" appears in app list with icon
6. Open app → UI renders with logos
7. Close app → clean teardown
8. Original built-in "About" app still works alongside

---

### Step 4: XIP Install Pipeline

**Goal**: Move `.text`/`.rodata` from RAM to XIP flash for production use.

**Changes to the loader**:

1. **XIP Linear Allocator**:
   - Tracks allocations in `/lfs/llext_alloc.json` (offset + size per app)
   - `zsw_llext_xip_alloc(size)` → finds first gap that fits, returns offset + XIP address
   - `zsw_llext_xip_free(name)` → removes entry, erases flash region
   - All sizes rounded up to flash erase-block alignment (4 KB)

2. **Install process** (replaces the Phase 1 all-RAM approach):
   - Use `llext_heap_init()` or `llext_heap_init_harvard()` to configure the LLEXT heap:
     - Instruction heap → points to the allocated XIP region (for `.text`/`.rodata`)
     - Data heap → regular RAM heap (for `.data`/`.bss`)
   - Or use the `section_detached` callback to keep certain sections out of the default allocation
   - Or use `LLEXT_PERSISTENT_BUF_LOADER` with the ELF already in XIP-addressable flash
   - **Research the best Zephyr-native approach before implementing** — avoid custom ELF parsing

3. **Flash write**: Use Zephyr's flash API (`flash_write()`, `flash_erase()`) to write relocated sections to the XIP partition

**Verification**:
1. Install about_ext → space allocated in XIP partition, sections written to flash
2. Boot → fast path: XIP sections already in flash, only .data/.bss loaded to RAM
3. RAM usage significantly lower than Phase 1
4. Screen off/on cycle: app works after XIP disable/enable
5. Uninstall app → space freed in allocator, flash erased

---

### Step 5: MCUmgr fs_mgmt for BLE Upload

**Goal**: Enable uploading LLEXT app files from phone to watch over BLE.

**Changes**:

1. **Kconfig** — Enable fs_mgmt in MCUmgr:
   ```
   CONFIG_MCUMGR_GRP_FS=y
   CONFIG_MCUMGR_GRP_FS_MAX_FILE_SIZE=262144
   CONFIG_MCUMGR_GRP_FS_PATH_LEN=128
   ```

2. **SMP availability**: Currently SMP is only registered when the update app is open (in `update_app.c`). For LLEXT uploads, SMP should be available more broadly:
   - **Recommendation**: Option A (always registered) for development, Option B (developer mode toggle) for production

3. **Install trigger**: After file upload completes → reboot → boot-time installer processes new app.

4. **Phone-side tool**: For development, use `mcumgr` CLI:
   ```bash
   mcumgr --conntype ble --connstring peer_name="ZSWatch" \
     fs upload about_ext.llext /lfs/apps/about_ext/app.llext
   mcumgr --conntype ble --connstring peer_name="ZSWatch" \
     fs upload manifest.json /lfs/apps/about_ext/manifest.json
   ```

**Verification**:
1. Enable fs_mgmt, build firmware
2. Use `mcumgr` CLI to upload `.llext` + `manifest.json` to LFS over BLE
3. Verify files appear in LFS (check via shell: `fs ls /lfs/apps/`)
4. Reboot → app appears and works
5. Full round-trip: build LLEXT → upload via BLE → reboot → app runs

---

### Step 6: Convert Built-in Apps to LLEXT (Ongoing)

**Goal**: Gradually move built-in apps to LLEXT format, freeing internal flash and validating the system.

**Priority order for conversion** (easiest/least dependencies first):
1. `about_app` (Phase 1 proof-of-concept — already done)
2. `qr_code_app` — simple, no background
3. `x_ray_app` — simple UI toy
4. `game_2048_app` — self-contained game
5. `trivia_app` — self-contained
6. `calculator_app` — self-contained
7. `flashlight_app` — very simple
8. `compass_app` — uses sensor data via zbus (tests zbus integration)
9. `weather_app` — uses BLE data (tests background + BLE)
10. `battery_app` — full background + UI split (validates Approach D fully)

**Each conversion follows the same pattern**:
1. Create LLEXT version in `app/src/llext_apps/<name>/`
2. Add any missing symbols to the export table
3. Build → upload to LFS → reboot → test
4. Once validated, remove the built-in version (frees a `MAX_APPS` slot)

---

## LLEXT Technical Details for the Implementing Agent

### Loading Mechanism

Zephyr provides these loaders out of the box:

| Loader | Header | Use Case |
|--------|--------|----------|
| `LLEXT_FS_LOADER(path)` | `llext/fs_loader.h` | Load from filesystem path |
| `LLEXT_BUF_LOADER(buf, len)` | `llext/buf_loader.h` | Load from RAM buffer |
| `LLEXT_PERSISTENT_BUF_LOADER(buf, len)` | `llext/buf_loader.h` | Load from persistent memory (XIP flash) |

**Phase 1 (Steps 0-3)**: Use `LLEXT_FS_LOADER` — reads ELF from LittleFS, LLEXT allocates all sections in RAM.

**Phase 2 (Step 4)**: Use `llext_heap_init_harvard()` to direct `.text`/`.rodata` allocations to the XIP flash region, and `.data`/`.bss` to RAM. Or use `pre_located = true` after flashing the sections manually.

### pre_located Mode

When `pre_located = true` in `llext_load_param`:
- LLEXT trusts that RODATA/TEXT are already at their final addresses
- Does NOT allocate heap memory for those sections
- Only allocates RAM for .data and .bss
- Relocations are still applied (patching addresses in writable sections)
- The ELF must have been linked with the correct target addresses

### EDK (Extension Development Kit)

The EDK is the **preferred way** to build LLEXT apps. Generated by `west build -t llext-edk`, it produces an archive containing:
- All Zephyr + NCS + LVGL + ZSWatch headers
- Correct compiler flags for the target (ARM Cortex-M33, FPU, etc.)
- `-DLL_EXTENSION_BUILD` define
- `-mlong-calls` and `-mthumb` flags
- `-nodefaultlibs`

For **in-tree builds** (our initial approach), use `add_llext_target()` CMake function instead — it handles all this automatically.

For **out-of-tree builds** (future, third-party apps):
1. Extract EDK: `tar -xf build/zephyr/llext-edk.tar.xz`
2. Build extension: `cmake -DLLEXT_EDK_INSTALL_DIR=<edk_path> -B build && make -C build`

### Key Kconfig for ZSWatch

```
# Core LLEXT
CONFIG_LLEXT=y
CONFIG_LLEXT_TYPE_ELF_RELOCATABLE=y
CONFIG_LLEXT_HEAP_SIZE=16
CONFIG_LLEXT_EDK=y

# ARM MPU must be disabled OR userspace enabled for LLEXT to work
# (LLEXT needs to execute code from dynamically allocated memory)
CONFIG_ARM_MPU=n
# Or alternatively: CONFIG_USERSPACE=y (but heavier)

# For development
CONFIG_LLEXT_SHELL=y

# MCUmgr file upload (Step 5)
CONFIG_MCUMGR_GRP_FS=y
CONFIG_MCUMGR_GRP_FS_MAX_FILE_SIZE=262144

# JSON parsing for manifests
CONFIG_JSON_LIBRARY=y
```

> **ARM MPU Note**: The Zephyr LLEXT `modules` sample documents that on ARM targets, either `CONFIG_ARM_MPU=n` or `CONFIG_USERSPACE=y` is required. Without this, the MPU blocks execution from dynamically allocated heap/flash regions. Disabling MPU is the simpler path; userspace adds significant overhead but provides memory isolation between extensions.

### Quarantine Warning

Nordic's CI quarantines some LLEXT tests on nRF5340 due to:
- Stack overflow issues (zephyr#74536) — ensure sufficient stack for the LLEXT loader thread
- NRFX-8497 — writable storage mode issues on nRF

**Mitigation**: Use a dedicated thread with large stack (4-8 KB) for LLEXT load operations. Avoid `LLEXT_STORAGE_WRITABLE` mode initially.

### native_sim Limitation

**LLEXT does NOT support the `posix` architecture** used by `native_sim`. The `posix` arch lacks an ELF relocation handler (`elf.c`). Only `arm`, `arm64`, `riscv`, `x86`, `xtensa`, and `arc` are supported. All LLEXT development and testing must be done on real hardware or QEMU ARM targets.

### Symbols That Won't Work in LLEXT

| Compile-time Mechanism | LLEXT Alternative |
|------------------------|-------------------|
| `SYS_INIT()` | Call setup code from `app_entry()` |
| `ZBUS_LISTENER_DEFINE()` | Use `zbus_obs_init()` + `zbus_chan_add_obs()` at runtime |
| `ZBUS_CHAN_ADD_OBS()` | Use `zbus_chan_add_obs()` at runtime |
| `LOG_MODULE_REGISTER()` | Use `printk()` |
| `ZSW_LV_IMG_DECLARE()` | Use standard `LV_IMG_DECLARE()` |
| `ZSW_LV_IMG_USE()` | Use `&image_var` directly |
| `K_SEM_DEFINE()` | Use `k_sem_init()` at runtime |
| `K_MUTEX_DEFINE()` | Use `k_mutex_init()` at runtime |
| `K_THREAD_DEFINE()` | Use `k_thread_create()` at runtime |
| `K_WORK_DEFINE()` | Use `k_work_init()` at runtime |

### Exported Symbol Guidelines

- **Only export stable APIs** — internal helpers should not be exported
- **LVGL**: Only export functions actually used by LLEXT apps. Start minimal, add as needed. Each export costs ~12 bytes
- **Zbus channels**: Export channel variables so LLEXT apps can observe/read them
- **Avoid custom wrappers** — export Zephyr/LVGL functions directly whenever possible

### Crash Isolation (from EDK sample)

The Zephyr LLEXT `edk` sample shows a pattern for handling crashing extensions by overriding `k_sys_fatal_error_handler()`. If an extension thread faults, the handler identifies which LLEXT owns the thread and unloads it, preventing a full system crash. This is a good pattern to adopt once multi-threaded LLEXT apps are supported.

### Reference: Zephyr LLEXT Samples (`zephyr/samples/subsys/llext/`)

Three samples are included in the Zephyr tree and should be studied before implementation:

| Sample | Key Takeaway |
|--------|--------------|
| `modules` | Simplest example. Shows `add_llext_target()` CMake, `LLEXT_BUF_LOADER`, `EXPORT_SYMBOL`, `llext_find_sym()`. Demonstrates Kconfig tristate (`y`/`m`) for built-in vs. loadable. Documents ARM MPU requirement. |
| `shell_loader` | Interactive shell loading. Shows `CONFIG_LLEXT_SHELL=y` commands (`llext list/load_hex/unload/list_symbols/call_fn`). Extension built with just `arm-zephyr-eabi-gcc -mlong-calls -mthumb -c`. |
| `edk` | **Most relevant**. Full pub/sub app with multiple independently-built extensions. Shows: EDK standalone build workflow, `LLEXT_EDK_INSTALL_DIR` cmake integration, custom app syscalls (`__syscall`), memory domain isolation per extension, crash handler that unloads faulting extensions, dynamic kernel objects (`k_object_alloc`), user vs. kernel mode extensions. |

**What the samples do NOT cover** (novel to our use case):
- No XIP / execute-in-place patterns — all samples load from RAM buffers
- No filesystem-based loading — extensions are embedded via `.inc` hex arrays at compile time
- No dynamic discovery / manifest system
- No `LLEXT_LOAD_PARAM` customization for section placement

---

## Development Workflow

### Quick Iteration Cycle

```bash
# 1. Build main firmware + LLEXT app (once, or when exports change)
west build -b watchdk@1/nrf5340/cpuapp -- -DCONFIG_ZSW_LLEXT_APPS=y
west flash

# 2. The LLEXT app is built as part of the main build (add_llext_target)
#    Output: build/llext/about_ext.llext

# 3. Upload LLEXT app to LFS
#    Include the .llext and manifest.json in the LFS image source dir
#    Then: west upload_fs
#    Or: use mcumgr CLI over BLE

# 4. Reboot watch → app appears in launcher

# 5. Iterate: edit LLEXT source → rebuild → re-upload → reboot → test
```

### Loading LLEXT via Shell (for debugging)

With `CONFIG_LLEXT_SHELL=y`:
```
uart:~$ llext list
uart:~$ llext load about_ext /lfs/apps/about_ext/app.llext
uart:~$ llext list
  about_ext
uart:~$ llext unload about_ext
```

### Loading via west upload_fs (J-Link, development)

The existing `west upload_fs` script generates a LittleFS image and programs it to flash via J-Link. To include LLEXT apps:
1. Add the `.llext` files and `manifest.json` to the LFS image source directory
2. Run `west upload_fs` — it programs the complete LFS image
3. On boot, the LLEXT manager discovers and loads the apps

---

## Phased Implementation Summary

| Phase | Scope | Loading Method | XIP | Verification |
|-------|-------|---------------|-----|------------|
| **0** | Infrastructure + Kconfig | N/A | N/A | FW builds with LLEXT enabled, EDK generated |
| **1** | Export symbols | Shell test only | No | `llext load` from shell works |
| **2** | Build about_ext | `add_llext_target()` | No | ELF produced, symbols verified |
| **3** | App loader manager | LFS FS loader (all RAM) | No | about_ext loads at boot, appears in launcher, UI works |
| **4** | XIP install pipeline | LFS + XIP slot | Yes | .text/.rodata in XIP, low RAM usage, survives screen off/on |
| **5** | BLE upload (MCUmgr) | fs_mgmt → LFS → reboot | Yes | Upload via BLE → reboot → app works |
| **6** | Convert more apps | Full pipeline | Yes | Multiple LLEXT apps coexist, background apps work |

Each phase builds on the previous and is independently testable. Complete and verify each phase before moving to the next.

---

## Open Items / Future Work

- **App versioning/compatibility**: `fw_compat` field in manifest. Loader checks at boot and skips incompatible apps. After a firmware update, all manifest `installed` flags should be reset to `false` to force re-installation (relocations may change)
- **Internal flash for stubs**: If RAM for background stubs becomes tight, add an internal flash partition (48 KB available in the gap at 0xE4000-0xF0000) and write pre-relocated stubs there
- **Phone app UI**: Build screens for browsing app catalog, downloading, installing, managing apps on the watch
- **App catalog/store**: GitHub-based repository of LLEXT apps with metadata, screenshots, compatibility info
- **Security**: Consider signing LLEXT ELFs to prevent loading unauthorized code
- **Hot install**: Currently requires reboot. Could add runtime install if complexity is acceptable
- **XIP defragmentation**: When many apps are installed/uninstalled, gaps may form in the linear allocator. A compaction pass can close gaps by copying live entries. Rarely needed with 4 MB available
- **Memory isolation**: The EDK sample demonstrates `k_mem_domain` per extension for userspace isolation. Not needed initially (we use `CONFIG_ARM_MPU=n`), but could be added later for untrusted third-party apps
- **Userspace extensions**: The EDK sample shows both `K_USER` (restricted) and kernel-mode extensions. Could adopt this for apps that don't need full kernel access
