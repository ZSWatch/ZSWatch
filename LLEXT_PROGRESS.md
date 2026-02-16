# LLEXT Implementation Progress

## Current Status: Step 5 — BLE/USB Upload of LLEXT Apps

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

#### Step 2: Build about_ext LLEXT App ✅
- `app/src/llext_apps/about_ext/about_ext_app.c` — simple "About LLEXT" app
- `app/src/llext_apps/CMakeLists.txt` — uses `add_llext_target()`, generates `.llext` + embedded `.inc`
- Build produces `app/build_dbg_dk/app/llext/about_ext.llext` (27880 bytes)

#### Step 3: LLEXT App Loader Manager ✅
- `app/src/managers/zsw_llext_app_manager.c` + `.h`
- Scans `/lvgl_lfs/apps/*/manifest.json` at boot
- Loads LLEXT apps from LFS using `LLEXT_FS_LOADER` (all RAM, no XIP yet)
- Also loads an **embedded** copy (compiled into firmware via `.inc`) as fallback
- Called from `main.c` after filesystem mount
- LLEXT heap: 20 KB static buffer
- Auto-start verification via delayed work (5 seconds)

#### Step 4: XIP Install Pipeline ⏳ (deferred — RAM-only for now)
- Phase 1 runs everything in RAM (no XIP partition used yet)
- Partition `llext_xip_partition` added to `pm_static_watchdk_nrf5340_cpuapp_1.yml` but not used yet

#### Step 5: Upload LLEXT App (IN PROGRESS) 🔄
- Created `app/scripts/upload_llext_app.py` — Python script using bleak + smpclient for BLE upload
- Pre-staged files in `app/src/images/binaries/lvgl_lfs/apps/about_ext/` (for `west upload_fs`)
- **BLOCKER**: SMP BLE transport is disabled by default (unregistered at boot in `update_app.c`)
- **BLOCKER**: XIP must be enabled for SMP to work (MCUmgr code is in XIP)
- **NEXT**: Hack firmware to auto-enable SMP+XIP at boot for dev testing
- **ALTERNATIVE**: Use USB CDC for mcumgr instead of BLE

### Files Created/Modified

**New files:**
- `LLEXT_IMPLEMENTATION_PLAN.md` — full implementation plan
- `app/src/llext_apps/CMakeLists.txt` — LLEXT build rules
- `app/src/llext_apps/zsw_llext_exports.c` — symbol export table
- `app/src/llext_apps/about_ext/about_ext_app.c` — test LLEXT app
- `app/src/managers/zsw_llext_app_manager.c` — LLEXT app loader
- `app/src/managers/zsw_llext_app_manager.h` — LLEXT app loader header
- `app/scripts/upload_llext_app.py` — BLE upload script
- `app/src/images/binaries/lvgl_lfs/apps/about_ext/app.llext` — pre-staged LLEXT binary
- `app/src/images/binaries/lvgl_lfs/apps/about_ext/manifest.json` — pre-staged manifest

**Modified files:**
- `app/CMakeLists.txt` — added llext_apps subdirectory
- `app/Kconfig` — added `ZSW_LLEXT_APPS` config
- `app/prj.conf` — LLEXT + cJSON + ARM_MPU=n
- `app/pm_static_watchdk_nrf5340_cpuapp_1.yml` — added llext_xip_partition
- `app/src/main.c` — call `zsw_llext_app_manager_init()`
- `app/src/managers/CMakeLists.txt` — build llext_app_manager
- `app/src/managers/zsw_app_manager.c` — bumped MAX_APPS

### Current Task: Enable SMP+XIP at Boot for Dev Testing

The problem: SMP BLE transport is explicitly unregistered at boot in `update_app.c` line 311.
The user needs to manually enable it via the Update app UI. For dev iteration, we need
SMP + XIP enabled automatically at boot.

Plan:
1. In `update_app.c` `update_app_add()` (SYS_INIT): Instead of unregistering SMP, keep it registered
2. Call `zsw_xip_enable()` to keep XIP on (needed because MCUmgr code is in XIP)
3. Also need `CONFIG_MCUMGR_GRP_FS=y` in prj.conf for filesystem upload support
4. Alternative: enable USB CDC + mcumgr over USB

### Remaining Steps

- [ ] Step 5: Get mcumgr upload working (BLE or USB)
- [ ] Step 5: Upload about_ext.llext + manifest.json to `/lvgl_lfs/apps/about_ext/`
- [ ] Step 5: Reboot → verify LLEXT app loads from LFS and appears in launcher
- [ ] Step 4: XIP install pipeline (move .text/.rodata to XIP flash) — future
- [ ] Step 6: Convert more built-in apps — future
