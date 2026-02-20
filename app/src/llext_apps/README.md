# LLEXT Dynamic Apps

ZSWatch supports dynamically loaded applications using Zephyr's
[LLEXT](https://docs.zephyrproject.org/latest/services/llext/index.html)
subsystem. LLEXT apps are compiled as position-independent shared libraries
(`ET_DYN` ELF), stored on the LittleFS filesystem, and loaded at boot (or
hot-loaded at runtime) when discovered.

## How It Works

```
Boot                         User Opens App            User Closes App
─────                        ──────────────            ───────────────
Scan /lvgl_lfs/apps/    ──►  start_func() called  ──►  stop_func() called
Load ALL .llext files        (app is already loaded)    (LLEXT stays loaded)
Call app_entry() for each
  → self-registers via
    zsw_app_manager_add_application()
```

Key properties:
- **Eager loading** — all LLEXTs are loaded at boot and remain loaded for the
  lifetime of the system. This mirrors the lifecycle of built-in `SYS_INIT`
  apps: `app_entry()` runs once at boot and the app stays registered.
- **XIP flash** — `.text` and `.rodata` sections are streamed directly from
  the ELF file to QSPI XIP flash during loading (never allocated on heap).
  Only `.got`, `.data`, and `.bss` remain in RAM.
- **No manifest required** — the directory name under `/lvgl_lfs/apps/` is
  used as the app name. The entry symbol is always `app_entry`.
- **Hot-loading** — apps can also be installed at runtime via
  `zsw_llext_app_manager_load_app()` without a reboot (e.g. uploaded over BLE).
- **Dual-mode sources** — LLEXT app sources live alongside their built-in
  counterparts in `src/applications/<app>/`. The same `.c` files compile as
  either a built-in app or an LLEXT app depending on `CONFIG_ZSW_LLEXT_APPS`.

## Directory Structure

```
app/src/applications/
├── about/
│   ├── CMakeLists.txt          # Builds as LLEXT or built-in based on Kconfig
│   ├── about_app.c             # App logic (has both SYS_INIT and app_entry paths)
│   └── about_ui.c
├── battery/
│   ├── CMakeLists.txt
│   ├── battery_app.c
│   ├── battery_ui.c
│   └── battery_ui.h
├── compass/                    # More complex example with sensors + zbus
├── weather/                    # Example with HTTP, zbus, delayed work
├── trivia/                     # Example with HTTP + cJSON
├── calculator/                 # Example with k_thread, SMF, msgq
├── qr_code/                    # Example with display brightness control
└── CMakeLists.txt              # Defines add_zsw_llext_app() helper

app/src/llext_apps/
├── CMakeLists.txt              # Compiles export table + log helper
├── zsw_llext_exports.c         # EXPORT_SYMBOL table (firmware → LLEXT)
├── zsw_llext_log.h             # Drop-in LOG_ERR/WRN/INF/DBG macros for LLEXT
├── zsw_llext_log.c             # Firmware-side log router
└── README.md                   # This file

app/src/managers/
├── zsw_llext_app_manager.c     # Discovery, loading, R9 setup, lifecycle
├── zsw_llext_app_manager.h     # Public API (init, hot-load, prepare, remove)
├── zsw_llext_xip.c             # XIP flash streaming installer (pre-copy hook)
├── zsw_llext_xip.h             # XIP API + zsw_llext_xip_context struct
├── zsw_llext_iflash.c          # Internal flash installer for LLEXT_IFLASH + trampolines
└── zsw_llext_iflash.h          # LLEXT_IFLASH macro, LLEXT_TRAMPOLINE_APP_FUNCS, trampoline API

app/src/images/binaries/lvgl_lfs/apps/
├── about_ext/
│   └── app.llext               # Built ELF (auto-copied by CMake POST_BUILD)
├── battery_ext/
│   └── app.llext
├── compass_ext/
│   └── app.llext
├── weather_ext/
│   └── app.llext
├── trivia_ext/
│   └── app.llext
├── calculator_ext/
│   └── app.llext
└── qr_code_ext/
    └── app.llext
```

## Creating a New LLEXT App

### 1. Create the app source

LLEXT apps use the **same source files and `application_t` pattern** as
built-in apps. The code conditionally compiles as either built-in (with
`SYS_INIT`) or LLEXT (with `app_entry()`).

Create your app in `app/src/applications/my_app/`:

```c
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <lvgl.h>
#include "managers/zsw_app_manager.h"
#include "managers/zsw_llext_iflash.h"
#include "ui/utils/zsw_ui_utils.h"

#ifdef CONFIG_ZSW_LLEXT_APPS
#include <zephyr/llext/symbol.h>
#include "zsw_llext_log.h"
#else
#include <zephyr/logging/log.h>
#endif

LOG_MODULE_REGISTER(my_app, LOG_LEVEL_INF);

static void my_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void my_app_stop(void *user_data);

static application_t app = {
    .name = "My App",
    .icon = ZSW_LV_IMG_USE(my_app_icon),
    .start_func = my_app_start,
    .stop_func = my_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

static void my_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    /* Create LVGL UI under root */
}

static void my_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    /* Clean up all LVGL objects and timers */
}

static int my_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

#ifdef CONFIG_ZSW_LLEXT_APPS
application_t *app_entry(void)
{
    LLEXT_TRAMPOLINE_APP_FUNCS(&app);
    my_app_add();
    return &app;
}
EXPORT_SYMBOL(app_entry);
#else
SYS_INIT(my_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
#endif
```

**Key things in `app_entry()`:**
- `LLEXT_TRAMPOLINE_APP_FUNCS(&app)` wraps all `application_t` function
  pointers (`start_func`, `stop_func`, `back_func`, etc.) with R9-restoring
  trampolines so the firmware can safely call into LLEXT code from any context.
- Call your init / registration function (e.g. `my_app_add()`).
- Return `&app` — the manager uses this pointer for the app's lifetime.

### 2. Register in CMakeLists.txt

Create `app/src/applications/my_app/CMakeLists.txt`:

```cmake
# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ZSW_LLEXT_APPS)
    add_zsw_llext_app(my_app_ext
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/my_app.c
        # EXTRA_INCLUDES <dirs>   # optional extra include paths
    )
else()
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/my_app.c
    )
    set(RELOCATE_FILES ${RELOCATE_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/my_app.c
        PARENT_SCOPE)
endif()
```

The `add_zsw_llext_app()` function (defined in `app/src/applications/CMakeLists.txt`) handles:
- Creating the LLEXT shared library target via `add_llext_target()`
- Adding standard include directories (`src/`, `src/managers/`, `src/llext_apps/`)
- Setting `-mlong-calls` (required for XIP, see below)
- Keeping `.text.iflash` as a separate section (`--unique=.text.iflash`)
- Copying the built `.llext` to the LFS source directory
- Making it a build dependency of the main firmware

### 3. Handle missing symbol exports

If your app calls firmware functions not already exported, `llext_load()` will
fail and **log every missing symbol by name**:

```
[WRN] llext: PLT: cannot find idx 42 name my_missing_function
```

Add the missing symbols to `app/src/llext_apps/zsw_llext_exports.c`:

```c
#include "my_module.h"
EXPORT_SYMBOL(my_missing_function);
```

The error messages tell you exactly which symbols need to be added. After
adding them, rebuild the firmware.

### 4. Build and upload

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

## Trampolines (R9 / GOT)

LLEXT apps are compiled with `-fPIC -msingle-pic-base -mpic-register=r9`.
All global variable accesses go through a GOT (Global Offset Table) indexed
by the R9 register. The firmware is compiled with `-ffixed-r9` so it never
clobbers R9.

When the firmware calls into LLEXT code (e.g. `start_func`, a zbus callback,
or a work handler), R9 must hold the correct GOT base address. Trampolines
handle this automatically:

### `LLEXT_TRAMPOLINE_APP_FUNCS(&app)`
Wraps all non-NULL `application_t` callbacks (`start_func`, `stop_func`,
`back_func`, `ui_unavailable_func`, `ui_available_func`) with R9-restoring
trampolines. **Must be called in `app_entry()` before returning.**

### `zsw_llext_create_trampoline(func)`
Creates an individual R9-restoring trampoline for a function pointer. Use
this when passing LLEXT callbacks to firmware APIs that store and invoke them
later (where R9 won't be set):

```c
/* Work queues */
k_work_init(&my_work,
            (k_work_handler_t)zsw_llext_create_trampoline((void *)my_handler));

/* Delayed work */
k_work_init_delayable(&my_dwork,
                      (k_work_handler_t)zsw_llext_create_trampoline(
                          (void *)my_delayed_handler));

/* Zbus observers */
my_listener.callback = zsw_llext_create_trampoline((void *)my_zbus_callback);
zbus_chan_add_obs(&my_channel, &my_listener, K_MSEC(100));

/* Any callback stored by firmware code */
http_rsp_cb_wrapped = (ble_http_callback)zsw_llext_create_trampoline(
                           (void *)my_http_callback);
```

Trampolines are 16-byte stubs written to internal flash. They persist until
the system is reset.

## Logging

Zephyr's `LOG_MODULE_REGISTER()` uses `STRUCT_SECTION_ITERABLE` (linker
sections) which doesn't work in dynamically loaded code. Instead, use
`zsw_llext_log.h` which provides drop-in `LOG_ERR()`, `LOG_WRN()`,
`LOG_INF()`, `LOG_DBG()` macros that route through a firmware-side log source.

The header also provides a no-op `LOG_MODULE_REGISTER()` macro so existing
code compiles unchanged with both build paths:

```c
#ifdef CONFIG_ZSW_LLEXT_APPS
#include "zsw_llext_log.h"
#else
#include <zephyr/logging/log.h>
#endif

LOG_MODULE_REGISTER(my_app, LOG_LEVEL_INF);
```

All LLEXT log messages appear under the `llext_app` log module. The log level
is controlled by `CONFIG_ZSW_LLEXT_LOG_LEVEL`.

**XIP safety:** `zsw_llext_log()` detects when the format string is in XIP
flash and XIP is disabled, and silently skips the log instead of faulting.

## LLEXT_IFLASH — Running Code When XIP Is Off

Some LLEXT functions (e.g. zbus callbacks, work handlers) may execute when the
screen — and therefore XIP flash — is disabled. Marking such functions with
`LLEXT_IFLASH` places them in a `.text.iflash` section that gets copied to
**internal flash** at load time, so they remain callable regardless of XIP
state.

```c
#include "managers/zsw_llext_iflash.h"

LLEXT_IFLASH
static void my_zbus_callback(const struct zbus_channel *chan)
{
    /* This runs from internal flash — safe even when XIP is off */
}
```

The internal-flash installer (`zsw_llext_iflash_install()`) automatically
patches GOT entries so all callers use the internal flash address. Trampolines
created with `zsw_llext_create_trampoline()` also work with iflash functions.

**Known limitation — .rodata stays in XIP:** The `LLEXT_IFLASH` attribute only
covers the function's `.text` (machine code). String literals, format strings,
and `const` arrays used by the function remain in `.rodata` on XIP flash. If
XIP is off when the function runs, accessing those causes a bus fault.

Current mitigations:
- `LOG_*` macros (via `zsw_llext_log()`) include a runtime XIP guard — if
  the format string points into XIP and XIP is disabled, the log call is
  silently skipped instead of faulting.
- Other `.rodata` accesses in `LLEXT_IFLASH` functions (e.g. string literals
  passed to helper functions, `const` lookup tables) are **not yet protected**
  and should be avoided.

**Future plan — copy .rodata to internal flash:**
The long-term fix is to extend `zsw_llext_iflash_install()` to also copy the
LLEXT's `.rodata` section to internal flash (alongside `.text.iflash`) and
patch all GOT/DATA references. The `.rodata` sections are small for most apps
(~0.3–1 KB), making this feasible within the 48 KB partition budget.

Image pixel data (LVGL C arrays) would be placed in a separate
`.rodata.extflash` section to keep large image data in XIP while copying
small string/const data to internal flash. The linker keeps that section
separate via `--unique=.rodata.extflash`. Image `.c` files would tag their
arrays with a section attribute (e.g. `.rodata.extflash`).

This approach means regular app code needs zero changes — all string literals
and format strings are automatically safe. Only image `.c` files need one
attribute on their array declaration.

## Zbus in LLEXT Apps

Compile-time zbus macros (`ZBUS_LISTENER_DEFINE()`, `ZBUS_CHAN_ADD_OBS()`)
don't work in LLEXT because they use linker sections. Use the runtime API
instead:

```c
ZBUS_CHAN_DECLARE(battery_sample_data_chan);

static struct zbus_observer_data my_obs_data = { .enabled = true };
static struct zbus_observer my_listener;

LLEXT_IFLASH
static void my_zbus_callback(const struct zbus_channel *chan)
{
    const struct battery_sample_event *event =
        (const struct battery_sample_event *)chan->message;
    /* Handle event — runs from internal flash */
}

/* In app_entry(): */
my_listener.name = "my_app_listener";
my_listener.type = ZBUS_OBSERVER_LISTENER_TYPE;
my_listener.data = &my_obs_data;
my_listener.callback = zsw_llext_create_trampoline((void *)my_zbus_callback);
zbus_chan_add_obs(&battery_sample_data_chan, &my_listener, K_MSEC(100));
```

Mark zbus callbacks with `LLEXT_IFLASH` since they may fire when the screen
is off.

## Limitations & Considerations

### No spawning kernel threads (safely)
LLEXT code is PIC: all global accesses go through R9. When `k_thread_create()`
spawns a new thread, R9 starts at 0, causing bus faults when the thread entry
tries to access globals.

**Workaround**: Wrap the thread entry with `zsw_llext_create_trampoline()`:
```c
void *tramp_entry = zsw_llext_create_trampoline((void *)my_thread_func);
k_thread_create(&my_thread, stack, stack_size,
                (k_thread_entry_t)tramp_entry, ...);
```

Or preferably, use `k_work_submit()` / `k_work_schedule()` which run on the
system workqueue.

### `k_uptime_get()` workaround
`k_uptime_get()` is an inline wrapper that PIC may compile into a
problematic GOT-indirect call. Use `z_impl_k_uptime_ticks()` directly:
```c
extern int64_t z_impl_k_uptime_ticks(void);
int64_t now_ticks = z_impl_k_uptime_ticks();
```

### Symbol export requirement
Every firmware function or global variable called by an LLEXT app must be
listed in `zsw_llext_exports.c` with `EXPORT_SYMBOL()`. Missing exports
cause `llext_load()` to fail with clear error messages:

```
[WRN] llext: PLT: cannot find idx 5 name zsw_some_function
[ERR] llext: failed to link, ret -2
```

Common categories already exported:
- Zephyr kernel (`k_msleep`, `k_malloc`, `printk`, `k_work_*`, …)
- LVGL (labels, charts, images, styles, tileviews, buttons, timers, …)
- Zbus runtime API (`zbus_chan_read`, `zbus_chan_add_obs`, `zbus_chan_rm_obs`, …)
- Zbus channels (`battery_sample_data_chan`, `ble_comm_data_chan`, …)
- ZSWatch managers (`zsw_app_manager_*`)
- ZSWatch UI utils, history, filesystem, popup, sensor fusion, display control
- BLE HTTP, cJSON, settings, clock
- ARM EABI runtime helpers (float/double math)
- C library (`snprintf`, `strlen`, `memset`, `memcpy`, `strtod`, …)
- SMF (state machine framework)

### `-mlong-calls` required
LLEXT `.text` lives in XIP flash (address `0x10E20000+`) which is far from
firmware `.text` (address `0x00000000+`). ARM branch instructions have limited
range, so `-mlong-calls` forces the compiler to use indirect calls via the GOT.
This is handled automatically by `add_zsw_llext_app()`.

### Image assets
Image `.c` files can be `#include`d or compiled into the LLEXT source. They
become part of `.rodata` and are moved to XIP flash at load time, costing
zero RAM. Use the same image format as built-in apps (LVGL v9 C arrays).

### Memory budget
- **LLEXT heap**: 36 KB (`ZSW_LLEXT_HEAP_SIZE`) — used during `llext_load()`
  for ELF parsing, section allocation, and symbol resolution.
- **After XIP streaming**: `.text` and `.rodata` are written directly to XIP
  flash (never on heap). Only `.got`, `.data`, `.bss`, and ELF metadata
  remain in RAM.
- **XIP partition**: `llext_xip_partition` — multi-MB region for LLEXT code
  and read-only data. Each app gets a sequential allocation.
- **Internal flash partition**: `llext_core_partition` at `0xE4000` (48 KB) —
  used for `.text.iflash` sections and R9 trampolines.

### Kconfig
Enable LLEXT apps in `prj.conf`:
```
CONFIG_ZSW_LLEXT_APPS=y
CONFIG_LLEXT_HEAP_DYNAMIC=y
CONFIG_LLEXT_LOG_LEVEL_WRN=y
CONFIG_LLEXT_TYPE_ELF_SHAREDLIB=y
CONFIG_LLEXT_BUILD_PIC=y
CONFIG_ARM_MPU=n
```

## Existing LLEXT Apps

| App | Source | Demonstrates |
|-----|--------|-------------|
| About | `applications/about/` | Simple app, filesystem info |
| Battery | `applications/battery/` | Zbus listener, `LLEXT_IFLASH`, settings, history |
| Compass | `applications/compass/` | Sensor fusion, periodic timer |
| Weather | `applications/weather/` | HTTP requests, delayed work, zbus, cJSON |
| Trivia | `applications/trivia/` | HTTP requests, cJSON, dynamic UI |
| Calculator | `applications/calculator/` | SMF, threads, message queues, multi-file |
| QR Code | `applications/qr_code/` | Display brightness control |

## Architecture Reference

### Manager files
| File | Purpose |
|------|---------|
| `src/managers/zsw_llext_app_manager.c` | Discovery, boot loading, hot-loading, R9 setup |
| `src/managers/zsw_llext_app_manager.h` | Public API (`init`, `load_app`, `prepare_app_dir`, `remove_app`) |
| `src/managers/zsw_llext_xip.c` | XIP flash streaming installer (pre-copy hook) |
| `src/managers/zsw_llext_xip.h` | XIP API and `zsw_llext_xip_context` struct |
| `src/managers/zsw_llext_iflash.c` | Internal flash installer for `.text.iflash` + trampoline creation |
| `src/managers/zsw_llext_iflash.h` | `LLEXT_IFLASH` macro, `LLEXT_TRAMPOLINE_APP_FUNCS`, `zsw_llext_create_trampoline` |
| `src/llext_apps/zsw_llext_exports.c` | Central `EXPORT_SYMBOL` table |
| `src/llext_apps/zsw_llext_log.h` | Drop-in `LOG_*` macros for LLEXT apps |
| `src/llext_apps/zsw_llext_log.c` | Firmware-side log router with XIP safety |

### Loading flow (boot)
1. `zsw_llext_app_manager_init()` is called from `main.c` after LittleFS is mounted.
2. It scans `/lvgl_lfs/apps/` for subdirectories containing `app.llext`.
3. For each app:
   - `llext_load()` is called with a pre-copy hook that streams `.text`/`.rodata`
     directly to XIP flash (no heap allocation for code).
   - The GOT base address is computed from the `.got` section offset within
     the DATA region.
   - `.text.iflash` sections are copied to internal flash and GOT entries
     patched.
   - R9 is set to the GOT base, then `app_entry()` is called.
   - `app_entry()` wraps callbacks with trampolines, does initialization
     (settings, zbus registration, etc.), calls
     `zsw_app_manager_add_application()`, and returns `&app`.
4. The LLEXT stays loaded permanently — no unloading on app close.

### Related Zephyr modifications
The following local patches to Zephyr are required for ARM PIC LLEXT support:
- `zephyr/subsys/llext/Kconfig` — `LLEXT_BUILD_PIC` depends on `XTENSA || ARM`
  (upstream only supports Xtensa for PIC)
- `zephyr/cmake/compiler/gcc/target_arm.cmake` — adds `-fPIC -nostdlib
  -nodefaultlibs -msingle-pic-base -mno-pic-data-is-text-relative
  -mpic-register=r9` for LLEXT ELF builds
