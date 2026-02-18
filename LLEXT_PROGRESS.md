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

### Remaining Steps (ET_REL phase)

- [ ] Investigate first-boot crash (stale XIP data after firmware flash causes data bus error, auto-recovers on reboot)
- [ ] Convert more built-in apps to LLEXT (compass, music control, etc.)
- [ ] OTA LLEXT app upload flow (upload individual apps without reflashing firmware)
- [ ] App lifecycle improvements (unload/reload individual LLEXT apps)

---

## Phase 2: PIC (`-fPIC`) + ET_DYN Migration

### Motivation

The current ET_REL streaming loader works but has fundamental limitations:
1. **500+ lines of relocation patching** directly into flash, supporting only `R_ARM_ABS32` and `R_ARM_THM_CALL`
2. **Not upstreamable** — the relocation logic duplicates and diverges from Zephyr's `arch_elf_relocate()`
3. **Fragile** — adding new relocation types requires encoding/decoding ARM instructions in a streaming context
4. **Custom ELF parser** (916 lines) instead of using Zephyr's built-in `llext_load()`

With **PIC (`-fPIC`) + ET_DYN**, the compiler emits position-independent code that accesses all globals/functions through a GOT (Global Offset Table). The GOT is small (~100 bytes), lives in RAM, and is the *only* thing that needs relocation. `.text` and `.rodata` can be copied **verbatim** to XIP flash — no instruction patching whatsoever.

### Current Status: NOT STARTED (code changes)

Research complete. All gaps identified. Ready to implement.

### Architecture Comparison

**Current (ET_REL + custom relocation in flash):**
```
ELF (.o, ET_REL)
  → Custom parser: read shdrs, symtab, strtab
  → Stream .text/.rodata in 4KB chunks:
      read chunk → scan relocs → patch R_ARM_ABS32/THM_CALL in buffer → write to flash
  → Copy .data to RAM pool, apply relocs in-place
  → Zero .bss in RAM pool
  → Scan symtab for entry function
```

**Target (ET_DYN + PIC, no instruction patching):**
```
ELF (.so, ET_DYN via -fPIC -shared)
  → Stream .text/.rodata to XIP flash VERBATIM (no patching!)
  → Allocate .got + .data + .bss in RAM
  → Zephyr's llext_link() fills GOT entries + R_ARM_RELATIVE
  → Entry symbol via llext_find_sym()
```

### Detailed Research Findings

#### 1. Zephyr LLEXT Already Supports ET_DYN

`zephyr/subsys/llext/llext_load.c` has full ET_DYN support:
- Parses program headers (PT_LOAD segments)
- Validates VMAs, handles `.dynsym` / `.dynstr`
- Merges regions by permission (RX → TEXT, RW → DATA, R → RODATA)
- Computes `load_bias = ext->mem[LLEXT_MEM_TEXT]`
- Originally built for Xtensa but architecture-generic

#### 2. Kconfig Gate Blocks ARM

```kconfig
# zephyr/subsys/llext/Kconfig line 82-90
config LLEXT_BUILD_PIC
    bool "Use -fPIC when building LLEXT"
    depends on XTENSA          # <-- BLOCKS ARM
    default y if LLEXT_TYPE_ELF_SHAREDLIB
```

**Fix**: Change `depends on XTENSA` → `depends on XTENSA || ARM`

#### 3. ARM CMake Missing `-fPIC`

```cmake
# zephyr/cmake/compiler/gcc/target_arm.cmake lines 54-66
set(LLEXT_REMOVE_FLAGS
  -fno-pic -fno-pie           # ✅ Already removes anti-PIC flags
  -ffunction-sections -fdata-sections -Os
)
set(LLEXT_APPEND_FLAGS
  -mlong-calls -mthumb        # ❌ Missing -fPIC
)
```

**Fix**: Add `-fPIC` to `LLEXT_APPEND_FLAGS` (conditionally on `CONFIG_LLEXT_BUILD_PIC`)

#### 4. ARM Relocation Handler — What's There vs. What's Missing

`zephyr/arch/arm/core/elf.c` switch statement (line 356):

| Relocation | Type | Status | Formula | Used For |
|------------|------|--------|---------|----------|
| `R_ARM_RELATIVE` | 23 | ✅ Handled | `*loc += load_bias` | Self-referencing pointers |
| `R_ARM_GLOB_DAT` | 21 | ✅ Handled | `*loc = S` | GOT entries (external symbols) |
| `R_ARM_JUMP_SLOT` | 22 | ✅ Handled | `*loc = S` | PLT entries |
| `R_ARM_GOT_BREL` | 26 | ❌ **MISSING** | `*loc = S - GOT_ORG` | GOT-relative offset |
| `R_ARM_GOT_PREL` | 96 | ❌ **MISSING** | `*loc = GOT(S) - P` | PC-relative GOT access |

**Note**: Need to build with `-fPIC` first and inspect `readelf -r` output to see which relocation types the compiler actually emits for Cortex-M Thumb-2. The GOT_BREL handler may not be needed if the compiler uses GLOB_DAT + RELATIVE only.

#### 5. `.got` Section → RAM (Correct)

`.got` has flags `SHF_WRITE | SHF_ALLOC` → `llext_mem.c` maps to `LLEXT_MEM_DATA` → allocated in LLEXT heap (RAM). This is correct — GOT must be writable during load, readable at runtime.

#### 6. `load_bias` Computation (Correct for XIP)

```c
// zephyr/arch/arm/core/elf.c line 331
const uintptr_t load_bias = (uintptr_t)ext->mem[LLEXT_MEM_TEXT];
```

For XIP, `ext->mem[LLEXT_MEM_TEXT]` = XIP CPU address (e.g., `0x10E20000 + offset`). `R_ARM_RELATIVE` adds this to adjust pointers linked at VMA 0.

#### 7. XIP Memory Map

```
External QSPI Flash (64 MB total):
  llext_xip_partition:
    Physical offset: 0xE20000
    CPU address:     0x10E20000 (memory-mapped via QSPI XIP)
    Size:            4 MB (0x400000)

  Linear allocator: next_offset starts at 0
    CPU addr = 0x10E20000 + offset
    Sector size: 4096 bytes
```

#### 8. Extension Build Pipeline

```cmake
add_llext_target(battery_real_ext ...)
```
When `CONFIG_LLEXT_TYPE_ELF_SHAREDLIB=y`:
- `add_llext_target()` uses CMake `SHARED` library type
- Adds `-shared` to linker flags
- Combined with `-fPIC`, produces ET_DYN ELF
- `.llext` files copied to LFS source dir for `west upload_fs`

#### 9. `-mlong-calls` + `-fPIC` Interaction

Both flags are needed simultaneously:
- `-mlong-calls`: Required because XIP `.text` is at `0x10E20000`, far from firmware `.text` at `0x00000000`
- `-fPIC`: Makes code position-independent via GOT

With `-fPIC`, function calls to external symbols go through a PLT/GOT sequence instead of direct BL instructions. `-mlong-calls` ensures the PLT stubs can reach the GOT. These should be compatible.

#### 10. Key Concern: LVGL Symbol Count

The battery extension uses ~50 LVGL symbols. All must be exported from firmware. The current `zsw_llext_exports.c` already handles this. With ET_DYN, `llext_link()` resolves symbols by name from the export table — same mechanism.

### Implementation Plan

| Step | Description | Status |
|------|-------------|--------|
| 1 | Kconfig: `LLEXT_BUILD_PIC` allow ARM | Not started |
| 2 | cmake: Add `-fPIC` to ARM LLEXT flags | Not started |
| 3 | `arch/arm/core/elf.c`: Add `R_ARM_GOT_BREL` handler | Not started |
| 4 | `app/prj.conf`: Enable `LLEXT_TYPE_ELF_SHAREDLIB` + `LLEXT_BUILD_PIC` | Not started |
| 5 | Build and inspect generated ELF with `readelf -a` | Not started |
| 6 | Rewrite streaming loader (remove reloc patching) | Not started |
| 7 | Update app manager for new loader | Not started |
| 8 | Build → Flash → Upload LFS → Test | Not started |
| 9 | Debug and iterate | Not started |

### Risk Areas

1. **Unknown relocation types**: Compiler may emit relocations not yet handled in `arch_elf_relocate()`. Must inspect `readelf -r` output after first PIC build.
2. **`-mlong-calls` + `-fPIC`**: Untested combination on Cortex-M for Zephyr LLEXT. May produce unexpected PLT/veneer patterns.
3. **ET_DYN binary size**: Extra sections (`.dynamic`, `.dynsym`, `.dynstr`, `.got`, `.plt`, `.hash`) increase ELF size. Need to verify fit in LFS.
4. **XIP + GOT lifetime**: GOT in RAM must persist while app runs. If LLEXT heap is reused, GOT would be corrupted. Current data pool approach may need adaptation.
5. **First-boot crash**: Existing issue with stale XIP data. PIC may mitigate (no instruction patching) or exacerbate (different flash layout).
