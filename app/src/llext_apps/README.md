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
relies on linker sections not available in dynamically loaded code.

Instead, include `zsw_llext_log.h` which provides drop-in `LOG_ERR()`,
`LOG_WRN()`, `LOG_INF()`, `LOG_DBG()` macros that route through a
firmware-side log source. The header also provides a no-op
`LOG_MODULE_REGISTER()` so existing code compiles unchanged:

```c
#ifdef CONFIG_ZSW_LLEXT_APPS
#include "zsw_llext_log.h"
#else
#include <zephyr/logging/log.h>
#endif

LOG_MODULE_REGISTER(my_app, LOG_LEVEL_INF);
```

**Note:** `LOG_*` calls in `LLEXT_IFLASH` functions are safe — `zsw_llext_log()`
detects when the format string is in XIP flash and XIP is disabled, and
silently skips the log instead of faulting.

### No spawning kernel threads
LLEXT apps **cannot create new kernel threads** (`k_thread_create()`). The
LLEXT code is compiled as PIC (position-independent code) using R9 as the GOT
(Global Offset Table) base register. All global variable accesses go through
the GOT, so R9 must be set correctly.

When `k_thread_create()` spawns a new thread, the thread starts with a fresh
register context where R9 = 0. The thread entry function — which is LLEXT
code — immediately tries to access globals via R9, causing a BUS FAULT
(dereferencing address `0 + GOT_offset`).

This is a chicken-and-egg problem: to restore R9, the thread entry would need
to read `saved_got_base` from the GOT, but reading the GOT requires R9 to
already be set. Inline assembly to set R9 also fails because the compiler
rejects modifying the PIC register in PIC-compiled code
(`"PIC register clobbered by 'r9' in 'asm'"`).

**Workaround**: If your app needs background processing, use `k_work_submit()`
or `k_work_schedule()` to run work items on the system workqueue. These execute
in the system workqueue thread context where R9 is not relevant (the work
handler is called via a function pointer the kernel already resolved).

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

### LLEXT_IFLASH — running code when XIP is off

Some LLEXT functions (e.g. zbus callbacks, background work items) may execute
when the screen — and therefore XIP flash — is disabled. Marking such functions
with `LLEXT_IFLASH` causes their `.text` to be copied to **internal** flash
(the `llext_core_partition` at `0xE4000`, 48 KB) so they remain callable
regardless of XIP state.

```c
#include "managers/zsw_llext_iflash.h"

LLEXT_IFLASH
static void my_zbus_callback(const struct zbus_channel *chan)
{
    /* This runs from internal flash — safe even when XIP is off */
}
```

The trampoline created by `zsw_llext_create_trampoline()` restores the GOT
base register (R9) before jumping to the internal-flash copy, so PIC code
works correctly.

**Known limitation — .rodata stays in XIP:**
The `LLEXT_IFLASH` attribute only covers the function's `.text` (machine code).
String literals, format strings, and `const` arrays used by the function remain
in `.rodata` on XIP flash. If XIP is off when the function runs, accessing
those read-only data causes a **bus fault**.

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

Image pixel data (LVGL C arrays) would need to be placed in a separate
`.rodata.extflash` section to keep large image data in XIP while still
copying small string/const data to internal flash.

See the `LLEXT_IFLASH` audit notes in the codebase for details on affected
apps and section sizes.

Image .c files would tag their arrays with a section attribute (e.g., .rodata.extflash), keeping them in XIP. The linker keeps that section separate via --unique=.rodata.extflash. The install routine copies .rodata (strings, small consts) to internal flash but skips .rodata.extflash.

Pro: Regular app code needs zero changes — all string literals, format strings, etc. are automatically safe
Pro: Only image .c files need one attribute on their array declaration (a well-defined, small set)
Con: Image files need to follow a convention (but they already need special handling anyway)

## Architecture Reference

### Manager files
| File | Purpose |
|------|---------|
| `src/managers/zsw_llext_app_manager.c` | Discovery, proxy registration, load/unload lifecycle |
| `src/managers/zsw_llext_app_manager.h` | Public API (`zsw_llext_app_manager_init()`) |
| `src/managers/zsw_llext_xip.c` | XIP flash installer (erase/write/pointer fixup) |
| `src/managers/zsw_llext_xip.h` | XIP API (`zsw_llext_xip_init/install/reset`) |
| `src/managers/zsw_llext_iflash.c` | Internal-flash installer for `LLEXT_IFLASH` functions |
| `src/managers/zsw_llext_iflash.h` | `LLEXT_IFLASH` macro, trampoline API |

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
