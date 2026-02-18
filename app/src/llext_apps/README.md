# LLEXT Dynamic Apps

ZSWatch supports dynamically loaded applications using Zephyr's
[LLEXT](https://docs.zephyrproject.org/latest/services/llext/index.html)
subsystem. LLEXT apps are compiled as position-independent shared libraries
(`ET_DYN` ELF), stored on the LittleFS filesystem, and loaded on-demand when
the user opens them from the app picker.

## How It Works

```
Boot                         User Opens App            User Closes App
─────                        ──────────────            ───────────────
Scan /lvgl_lfs/apps/    ──►  Load .llext from FS  ──►  Call real stop_func
Register proxy app           Install .text/.rodata     Unload LLEXT
  in app picker                to XIP flash            Reset XIP allocator
(no ELF loading)             Call app_entry()           (free all resources)
                             Call real start_func
```

Key properties:
- **Deferred loading** — only one LLEXT is loaded at a time, keeping RAM usage
  minimal (~600 bytes per loaded app vs ~33 KB without XIP).
- **XIP flash** — after loading, `.text` and `.rodata` sections are copied to
  the QSPI XIP flash partition (`llext_xip_partition` at `0x10E20000`). Since
  the code is PIC (`-fPIC`), no relocation patching is needed — the GOT in
  RAM provides all address indirection.
- **No manifest required** — the directory name under `/lvgl_lfs/apps/` is used
  as the app name. The entry symbol is always `app_entry`.
- **Proxy pattern** — a lightweight `application_t` with trampoline functions
  is registered at discovery time. The real `application_t` (inside the LLEXT)
  is only obtained when the user opens the app.

## Directory Structure

```
app/src/llext_apps/
├── CMakeLists.txt           # Build rules (add_zsw_llext_app helper)
├── zsw_llext_exports.c      # EXPORT_SYMBOL table (firmware → LLEXT)
├── README.md                # This file
├── about_ext/               # Example: simple "About" LLEXT app
│   └── about_ext_app.c
└── battery_real_ext/        # Example: full Battery app as LLEXT
    ├── battery_real_ext_app.c
    ├── battery_ui.c
    └── battery_ui.h

app/src/images/binaries/lvgl_lfs/apps/
├── about_ext/
│   └── app.llext            # Built ELF (auto-copied by CMake POST_BUILD)
└── battery_real_ext/
    └── app.llext
```

## Creating a New LLEXT App

### 1. Create the app directory

```
app/src/llext_apps/my_app/
├── my_app.c
└── my_app_ui.c   (optional — multiple source files are supported)
```

### 2. Write the app source

LLEXT apps follow the same `application_t` pattern as built-in apps, but with
a few differences:

```c
#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <lvgl.h>
#include "managers/zsw_app_manager.h"

static lv_obj_t *root_page;

static void my_app_start(lv_obj_t *root, lv_group_t *group)
{
    LV_UNUSED(group);
    root_page = lv_obj_create(root);
    /* ... create LVGL UI ... */
}

static void my_app_stop(void)
{
    lv_obj_delete(root_page);
    root_page = NULL;
}

static application_t app = {
    .name = "My App",
    .icon = NULL,
    .start_func = my_app_start,
    .stop_func = my_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

/* Entry point — called once when the LLEXT is loaded.
 * Must always be named app_entry and exported with EXPORT_SYMBOL. */
application_t *app_entry(void)
{
    printk("my_app loaded\n");
    return &app;
}
EXPORT_SYMBOL(app_entry);
```

**Key differences from a built-in app:**
- No `SYS_INIT` registration — the LLEXT manager calls `app_entry()` at load
  time and uses the returned `application_t`.
- `EXPORT_SYMBOL(app_entry)` — makes the entry point discoverable by the loader.
- Use `printk()` for debug output. Zephyr's `LOG_MODULE_REGISTER()` uses
  `STRUCT_SECTION_ITERABLE` (linker sections) which doesn't work in LLEXT.
- Zbus observers must be registered at runtime with `zbus_chan_add_obs()`
  instead of the compile-time `ZBUS_CHAN_ADD_OBS()` macro.

### 3. Register in CMakeLists.txt

Add to `app/src/llext_apps/CMakeLists.txt`:

```cmake
add_zsw_llext_app(my_app
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/my_app/my_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/my_app/my_app_ui.c   # optional extra files
    # EXTRA_INCLUDES <dirs>   # optional, if your app needs additional headers
)
```

The `add_zsw_llext_app()` function handles:
- Creating the LLEXT shared library target (supports multiple source files)
- Adding standard include directories (`src/`, `src/managers/`)
- Setting `-mlong-calls` (required for XIP)
- Copying the built `.llext` to the LFS source directory
- Adding a build dependency so it's built with the firmware

### 4. Handle missing symbol exports

If your app calls firmware functions not already exported, `llext_load()` will
fail and **log every missing symbol by name**:

```
[WRN] llext: PLT: cannot find idx 42 name my_missing_function
```

Add the missing symbols to `zsw_llext_exports.c`:

```c
#include "my_module.h"
EXPORT_SYMBOL(my_missing_function);
```

The error messages tell you exactly which symbols need to be added. After
adding them, rebuild the firmware.

### 5. Build and upload

```bash
# Build firmware (includes all LLEXT apps)
west build --build-dir app/build_dbg_dk app \
    --board watchdk@1/nrf5340/cpuapp -- \
    -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf" \
    -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay"

# Upload the LFS image containing app.llext files
west upload_fs --type lfs \
    --ini_file app/boards/zswatch/watchdk/support/qspi_mx25u51245.ini
```

## Limitations & Considerations

### No compile-time zbus observers
`ZBUS_CHAN_ADD_OBS()` and `ZBUS_LISTENER_DEFINE()` with static registration
don't work in LLEXT because they use linker sections. Use the runtime API:

```c
/* Define observer data and struct manually */
static struct zbus_observer_data my_obs_data = { .enabled = true };
static struct zbus_observer my_listener = {
    .type = ZBUS_OBSERVER_LISTENER_TYPE,
    .data = &my_obs_data,
    .callback = my_callback,
};

/* Register at load time (e.g. in app_entry or start_func) */
zbus_chan_add_obs(&my_channel, &my_listener, K_MSEC(100));

/* Unregister in stop_func */
zbus_chan_rm_obs(&my_channel, &my_listener);
```

### No LOG_MODULE_REGISTER
Zephyr's `LOG_MODULE_REGISTER()` macro uses `STRUCT_SECTION_ITERABLE` which
relies on linker sections not available in dynamically loaded code. Use
`printk()` for debug output instead.

### Symbol export requirement
Every firmware function or global variable called by an LLEXT app must be
explicitly listed in `zsw_llext_exports.c` with `EXPORT_SYMBOL()`. Missing
exports cause `llext_load()` to fail with clear error messages naming each
unresolved symbol:

```
[WRN] llext: PLT: cannot find idx 5 name zsw_some_function
[ERR] llext: failed to link, ret -2
```

Common categories already exported:
- Zephyr kernel (`k_msleep`, `k_malloc`, `printk`, …)
- LVGL (labels, charts, images, styles, tileviews, …)
- Zbus runtime API (`zbus_chan_read`, `zbus_chan_add_obs`, …)
- Zbus channels (`battery_sample_data_chan`, `ble_comm_data_chan`, …)
- ZSWatch managers (`zsw_app_manager_*`)
- ARM EABI runtime helpers (float/double math)

### Memory budget
- **LLEXT heap**: 45 KB (`ZSW_LLEXT_HEAP_SIZE`) — used during `llext_load()`
  for ELF parsing, section allocation, and symbol tables.
- **After XIP install**: `.text` and `.rodata` are moved to flash; only
  `.got`, `.data`, `.bss`, and ELF metadata remain in RAM (~600 bytes typical).
- **Only one LLEXT loaded at a time** — the heap is fully reclaimed on unload.

### XIP flash partition
The `llext_xip_partition` provides 4 MB of XIP-mapped flash for LLEXT code
and read-only data. The linear allocator starts at offset 0 and resets each
time an app is unloaded. XIP is always enabled on ZSWatch (managed by
`zsw_xip_manager.c`).

### `-mlong-calls` required
LLEXT `.text` lives in XIP flash (address `0x10E20000+`) which is far from
firmware `.text` (address `0x00000000+`). ARM branch instructions have limited
range, so `-mlong-calls` forces the compiler to use indirect calls via the GOT,
which works at any distance.

### Image assets
Image `.c` files can be `#include`d into the LLEXT source. They become part of
`.rodata` and are moved to XIP flash at load time, costing zero RAM. Use the
same image format as built-in apps (LVGL v9 C arrays).

## Architecture Reference

### Manager files
| File | Purpose |
|------|---------|
| `src/managers/zsw_llext_app_manager.c` | Discovery, proxy registration, load/unload lifecycle |
| `src/managers/zsw_llext_app_manager.h` | Public API (`zsw_llext_app_manager_init()`) |
| `src/managers/zsw_llext_xip.c` | XIP flash installer (erase/write/pointer fixup) |
| `src/managers/zsw_llext_xip.h` | XIP API (`zsw_llext_xip_init/install/reset`) |

### App discovery convention
The LLEXT app manager scans `/lvgl_lfs/apps/` at boot. Each subdirectory
containing an `app.llext` file is registered as a dynamic app. The directory
name is used as the display name, and the entry symbol is always `app_entry`.

### Kconfig
Enable LLEXT apps in `prj.conf`:
```
CONFIG_ZSW_LLEXT_APPS=y
CONFIG_LLEXT_HEAP_DYNAMIC=y
CONFIG_LLEXT_LOG_LEVEL_INF=y
CONFIG_LLEXT_TYPE_ELF_SHAREDLIB=y
CONFIG_LLEXT_BUILD_PIC=y
CONFIG_ARM_MPU=n
```

### Related Zephyr modifications
- `zephyr/subsys/llext/Kconfig` — `LLEXT_BUILD_PIC` depends on `XTENSA || ARM`
  (upstream only supports Xtensa for PIC)
- `zephyr/cmake/compiler/gcc/target_arm.cmake` — adds `-fPIC -nostdlib
  -nodefaultlibs` for LLEXT ELF builds
