# LLEXT Implementation Progress

## Current Status: Step 6 — Convert Real Apps to LLEXT

### Completed Steps

#### Step 0: Infrastructure ✅
- `CONFIG_ZSW_LLEXT_APPS=y` in `app/prj.conf`
- `CONFIG_ARM_MPU=n` (required for LLEXT on ARM)
- `CONFIG_LLEXT_HEAP_DYNAMIC=y`, `CONFIG_LLEXT_LOG_LEVEL_INF=y`
- `CONFIG_CJSON_LIB=y` (for manifest parsing)
- Kconfig entry `ZSW_LLEXT_APPS` in `app/Kconfig` (selects `LLEXT`)
- Build confirmed working for `watchdk@1/nrf5340/cpuapp`

#### Step 1: Export Symbol Table ✅
- `app/src/llext_apps/zsw_llext_exports.c` — exports kernel, LVGL, zbus, app manager symbols
- Zbus channels exported (battery, BLE, activity, periodic, accel, magnetometer, pressure, light, music)
- ARM EABI float/double helpers exported (`__aeabi_i2d`, `__aeabi_f2d`, `__aeabi_ddiv`, `__aeabi_f2iz`, `__aeabi_ui2f`)
- Picolibc `__assert_no_args` exported (for C stdlib `assert()`)
- LVGL misc: `lv_pct`, `lv_log_add`, tileview, scale, LED, draw helpers, string/memory
- Zephyr: `z_impl_k_uptime_ticks`, `settings_subsys_init`, `settings_delete`
- ZSWatch: `zsw_history_*` (7 functions), `zsw_pmic_charger_status_str/error_str`, `zsw_ui_utils_seconds_to_day_hour_min`

#### Step 2: Build about_ext LLEXT App ✅
- `app/src/llext_apps/about_ext/about_ext_app.c` — simple "About LLEXT" app
- `app/src/llext_apps/CMakeLists.txt` — uses `add_llext_target()`, generates `.llext` + embedded `.inc`
- Build produces `app/build_dbg_dk/app/llext/about_ext.llext` (31108 bytes)

#### Step 3: LLEXT App Loader Manager ✅
- `app/src/managers/zsw_llext_app_manager.c` + `.h`
- Scans `/lvgl_lfs/apps/*/manifest.json` at boot
- Loads LLEXT apps from LFS using streaming XIP loader
- Also loads an **embedded** copy (compiled into firmware via `.inc`) as fallback
- Called from `main.c` after filesystem mount
- Auto-start via delayed work (5-second delay, creates root container and calls `start_func`)

#### Step 4: XIP Streaming Loader ✅
- `app/src/managers/zsw_llext_stream_loader.c` — custom streaming ELF loader
- Streams `.text` and `.rodata` directly to XIP flash (no RAM copy needed)
- `.data` and `.bss` allocated from static data pool in RAM
- Supports **R_ARM_ABS32** (type 2) and **R_ARM_THM_CALL** (type 10) relocations
- Symbol resolution cache for performance
- XIP allocator in `zsw_llext_xip.c` with 4096-byte data pool
- Partition `llext_xip_partition` at flash 0xe20000 (CPU 0x10e20000), 4096 KB

#### Step 5: Filesystem Upload ✅
- Pre-staged files in `app/src/images/binaries/lvgl_lfs/apps/` for `west upload_fs --type lfs`
- SMP BLE kept registered at boot (LLEXT dev mode in `update_app.c`)
- XIP forced on during dev mode
- `battery_ext` (simple battery chart) and `about_ext` load reliably from LittleFS

#### Step 6: battery_real_ext — Full Battery App as LLEXT ✅
- `app/src/llext_apps/battery_real_ext/battery_real_ext_app.c` — complete battery app
- Includes full 3-page UI (`battery_ui.c` via `#include`): chart, charger info, regulator info
- Icon compiled into `.rodata` (lives in XIP flash alongside code)
- Runtime zbus observer registration (not compile-time)
- PMIC support via `#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED`
- No file writes required (history clear callback is no-op)
- 50104 bytes LLEXT binary, .text=27676, .rodata=3915, .data=56, .bss=1640
- Loads successfully from LittleFS, all 3 LLEXT apps registered (total: 3)
- Original `battery_app.c` SYS_INIT disabled (replaced by LLEXT version)

### Files Created/Modified

**New files:**
- `LLEXT_IMPLEMENTATION_PLAN.md` — full implementation plan
- `app/src/llext_apps/CMakeLists.txt` — LLEXT build rules (about_ext, battery_ext, battery_real_ext)
- `app/src/llext_apps/zsw_llext_exports.c` — symbol export table (~70 exports)
- `app/src/llext_apps/about_ext/about_ext_app.c` — test LLEXT app
- `app/src/llext_apps/battery_ext/battery_ext_app.c` — simple battery chart LLEXT app
- `app/src/llext_apps/battery_real_ext/battery_real_ext_app.c` — full battery app LLEXT
- `app/src/managers/zsw_llext_app_manager.c` — LLEXT app loader manager
- `app/src/managers/zsw_llext_app_manager.h` — LLEXT app loader header
- `app/src/managers/zsw_llext_stream_loader.c` — streaming XIP ELF loader
- `app/src/managers/zsw_llext_xip.c` — XIP flash allocator + data pool
- `app/scripts/upload_llext_app.py` — BLE upload script
- `app/src/images/binaries/lvgl_lfs/apps/*/manifest.json` — manifests for each LLEXT app
- `app/src/images/binaries/lvgl_lfs/apps/*/app.llext` — pre-staged LLEXT binaries

**Modified files:**
- `app/CMakeLists.txt` — added llext_apps subdirectory
- `app/Kconfig` — added `ZSW_LLEXT_APPS` config
- `app/prj.conf` — LLEXT + cJSON + ARM_MPU=n
- `app/pm_static_watchdk_nrf5340_cpuapp_1.yml` — added llext_xip_partition
- `app/src/main.c` — call `zsw_llext_app_manager_init()`
- `app/src/managers/CMakeLists.txt` — build llext_app_manager + stream loader + xip
- `app/src/managers/zsw_app_manager.c` — bumped MAX_APPS
- `app/src/applications/battery/battery_app.c` — disabled SYS_INIT (LLEXT replaces it)

### Key Technical Decisions

1. **R_ARM_THM_CALL relocation support**: The stream loader handles both R_ARM_ABS32 (type 2) for data references and R_ARM_THM_CALL (type 10) for Thumb BL/BLX function calls. THM_CALL encoding uses split 25-bit signed offset across two 16-bit instruction words.

2. **Single compilation unit**: Battery LLEXT uses `#include "battery_ui.c"` because `add_llext_target()` only supports a single source file. All UI code (763 lines, 3 pages) is merged into one translation unit.

3. **Data pool sizing**: Increased from 1024 to 4096 bytes to accommodate battery_real_ext's 1640 bytes of BSS (LVGL static variables from the UI).

4. **ARM EABI exports**: Floating-point operations in the chart/scale UI generate calls to compiler runtime helpers (`__aeabi_i2d`, `__aeabi_ddiv`, etc.) that must be exported from the firmware.

5. **Linker-pulled symbols**: The LLEXT build process (`add_llext_target`) links additional libraries, pulling in symbols like `zsw_history_*` and `settings_*` that aren't directly referenced by the app code. These must be exported even if unused at runtime.

### Remaining Steps

- [ ] Investigate first-boot crash (stale XIP data after firmware flash causes data bus error, auto-recovers on reboot)
- [ ] Convert more built-in apps to LLEXT (compass, music control, etc.)
- [ ] OTA LLEXT app upload flow (upload individual apps without reflashing firmware)
- [ ] App lifecycle improvements (unload/reload individual LLEXT apps)
