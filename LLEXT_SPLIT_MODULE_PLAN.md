# LLEXT Internal Flash Plan: Background Callbacks Safe from XIP-Off

## Problem

When the screen goes idle, `zsw_display_control_sleep_ctrl()` calls `zsw_xip_disable()`,
which drops the QSPI XIP reference count to zero and disables the QSPI XIP window.
Any LLEXT app whose `.text`/`.rodata` was streamed to XIP flash (currently **all**
LLEXT apps) will suffer an `IBUSERR` bus fault the next time the CPU tries to fetch
an instruction from that code — for example when a zbus battery callback fires on
the system workqueue.

## Root Insight

| Code class | Where it runs | XIP dependency |
|-----------|--------------|---------------|
| Background logic (zbus callbacks, timers, step counters, …) | Must survive screen-off | **None** |
| UI code (LVGL screen creation, widget updates, image data) | Only while screen is on | **Required** |

This split already exists in ZSWatch's built-in apps: `*_app.c` files hold logic,
`*_ui.c` files hold LVGL code. The CMake `zephyr_code_relocate(…EXTFLASH_TEXT NOCOPY)`
macro moves built-in `*_ui.c` to XIP the same way.

## Why Two-ELF Failed

The original plan was to split each app into `core.llext` (internal flash) and
`ui.llext` (XIP). This failed due to an **R9 / PIC conflict**:

LLEXT code is compiled with `-msingle-pic-base -mpic-register=r9`. All data/function
references go through the GOT addressed by R9. Each ELF has its own GOT → its own R9
value. For `core.llext` to call a function in `ui.llext`, it would need to switch R9
to `ui.llext`'s GOT base before the call. But GCC **forbids** clobbering R9 in inline
asm when compiling with `-mpic-register=r9`:

```
error: PIC register clobbered by 'r9' in 'asm'
```

Only firmware code (compiled with `-ffixed-r9`) can manipulate R9 via inline asm.
LLEXT code cannot. This makes cross-ELF calls impossible without firmware trampolines
for every UI function — impractical.

## Proposed Architecture: Single ELF with Post-Load Internal Flash Copy

Keep one ELF per app (all `*_app.c` + `*_ui.c` sources linked together). One GOT,
one R9 — all intra-app function calls work normally through GOT-indirect branches.

At load time:

1. Load the entire ELF normally — all `.text` and `.rodata` → XIP via existing
   `zsw_llext_xip_pre_copy_hook`. Relocations are applied. GOT entries are correct.
2. **Post-load**: identify specific background functions that must survive XIP-off.
3. Copy those function bodies from XIP to the `llext_core_partition` in internal flash.
4. Patch the GOT entries for those functions: replace XIP address → internal flash address.
5. Done. All calls to those functions now use internal flash. UI calls still use XIP.

### How it works at runtime

```
Screen ON (XIP enabled):
  - UI functions execute from XIP ✓
  - Background callbacks execute from internal flash ✓ (GOT points there)

Screen OFF (XIP disabled):
  - Background callbacks execute from internal flash ✓ (GOT points there)
  - UI functions: never called (ZSW_APP_STATE_UI_VISIBLE == false)
  - No IBUSERR ✓
```

### Lifecycle

| Event | Action | XIP state |
|-------|--------|-----------|
| Boot | QSPI driver init | XIP on |
| Discovery | Load app.llext → TEXT/RODATA to XIP | XIP on |
| Post-load | Copy tagged functions → internal flash, patch GOT | XIP on |
| Screen wakes | `zsw_xip_enable()` | XIP on |
| App opened | call `start_func()` → UI code (XIP) | XIP on |
| Screen sleeps | `zsw_xip_disable()` | XIP **off** |
| zbus cb fires | executes from **internal flash** — safe ✓ | XIP off |
| Screen wakes | `zsw_xip_enable()` | XIP on |

---

## Identifying Background Functions

Apps are compiled with `-ffunction-sections`, so each function gets its own
`.text.<function_name>` ELF section. After load, `ext->sect_hdrs[]` still contains
all section headers with names accessible via the shstrtab.

The app developer marks background-critical functions with a section attribute
macro:

```c
#include "zsw_llext_iflash.h"

/* This function must survive XIP-off — runs from internal flash */
LLEXT_IFLASH
static void zbus_battery_callback(const struct zbus_channel *chan)
{
    /* store data … */
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        battery_ui_update(…);  /* direct call through GOT — linker resolves */
    }
}
```

Where:

```c
/* zsw_llext_iflash.h */
#define LLEXT_IFLASH __attribute__((section(".iflash.text"), noinline, used))
```

The post-load step scans `ext->sect_hdrs[]` for sections named `.iflash.text`
(or matching `.iflash.text.*` with `-ffunction-sections`). For each match:

1. Compute loaded address: `ext->mem[LLEXT_MEM_TEXT] + sect_map[i].offset`
2. Read size: `ext->sect_hdrs[i].sh_size`
3. Copy function body from XIP to `llext_core_partition` via `flash_area_write()`
4. Scan GOT (in `ext->mem[LLEXT_MEM_DATA]`) for entries matching the XIP address
5. Replace with internal flash address

Since `-mlong-calls` is used, ALL calls go through GOT (`LDR rN, [R9, #off]; BLX rN`).
Patching the GOT entry is sufficient to redirect all callers.

### GOT scanning

The GOT is a contiguous array of `uintptr_t` values within `ext->mem[LLEXT_MEM_DATA]`.
Its location and size are found via the `.got` section header (already parsed in the
existing `zsw_llext_xip_pre_copy_hook` for GOT-base computation). A linear scan for
matching XIP addresses is O(n) where n = number of GOT entries (typically < 200).

```c
/* Post-load: patch GOT entries for iflash functions */
uintptr_t *got = (uintptr_t *)((uint8_t *)ext->mem[LLEXT_MEM_DATA] + got_offset);
size_t got_entries = got_size / sizeof(uintptr_t);

for (size_t i = 0; i < got_entries; i++) {
    if (got[i] == xip_func_addr) {
        got[i] = iflash_func_addr;
        LOG_DBG("GOT[%zu]: patched 0x%08lx → 0x%08lx",
                i, (unsigned long)xip_func_addr, (unsigned long)iflash_func_addr);
    }
}
```

False positive risk is negligible — the chance of a random `.data` value exactly
matching a specific XIP address (0x10Exxxxx range) is vanishingly small.

---

## Internal Flash Partition

A new `llext_core_partition` goes in the 48 KB gap between the app image and
`settings_not_storage`:

```yaml
# pm_static_watchdk_nrf5340_cpuapp_1.yml AND pm_static_nrf5340dk_nrf5340_cpuapp.yml
llext_core_partition:
  address: 0xe4000
  end_address: 0xec000
  region: flash_primary
  size: 0x8000          # 32 KB
```

nRF5340 NVMC allows writing internal flash from a different page at runtime.
Zephyr `flash_area_write()` targeting `flash_controller` works. No special
cache/bus tricks needed — I-Bus and D-Bus are separate on Cortex-M33.

### CRC32 skip-write optimisation

To avoid unnecessary erase cycles (10,000 cycle spec per 4 KB page), a small
8-byte header `{uint32_t crc32, uint32_t size}` is stored at the start of the
partition. On boot:

1. Compare CRC32 of app.llext on LittleFS against stored header
2. Match → skip erase+write, load directly from partition
3. Mismatch → erase, write, then load

`west flash --erase` clears internal flash but not QSPI (LittleFS). The CRC
check will detect the blank partition and re-stream on next boot.

---

## App Developer Experience

Compared to the current monolithic approach, the only change for an app developer
with background work is adding `LLEXT_IFLASH` to their zbus/timer callbacks:

```c
/* Before (crashes on screen-off): */
static void zbus_battery_callback(const struct zbus_channel *chan) { … }

/* After (survives screen-off): */
LLEXT_IFLASH
static void zbus_battery_callback(const struct zbus_channel *chan) { … }
```

- No file splitting
- No vtable boilerplate
- No `EXPORT_SYMBOL` on UI functions
- No changes to `_ui.c` / `_ui.h` files
- No CMake changes beyond what exists today
- Direct `battery_ui_show()` / `battery_ui_update()` calls work normally through GOT

Apps without background work (like `about_ext`) need **zero changes** — all their
code stays in XIP and they only run while the screen is on.

---

## Implementation Requirements

### New files

| File | Purpose |
|------|---------|
| `app/src/managers/zsw_llext_iflash.c` | Internal flash partition management + post-load copy + GOT patching |
| `app/src/managers/zsw_llext_iflash.h` | `LLEXT_IFLASH` macro + API for post-load step |

### Modified files

| File | Change |
|------|--------|
| `app/src/managers/zsw_xip_manager.c` | Remove `first_xip_disable_done` double-call hack |
| `app/src/managers/zsw_llext_app_manager.c` | Call post-load iflash copy+patch after `llext_load()` |
| `app/src/managers/CMakeLists.txt` | Add `zsw_llext_iflash.c` |
| `app/pm_static_watchdk_nrf5340_cpuapp_1.yml` | Add `llext_core_partition` |
| `app/pm_static_nrf5340dk_nrf5340_cpuapp.yml` | Add `llext_core_partition` |
| `app/src/llext_apps/battery_real_ext/battery_real_ext_app.c` | Add `LLEXT_IFLASH` to zbus callback |

### Key implementation detail: section name with `-ffunction-sections`

With `-ffunction-sections`, a function marked `__attribute__((section(".iflash.text")))`
gets its section named `.iflash.text` (GCC places it exactly as specified, `-ffunction-sections`
does NOT append the function name when an explicit section attribute is used). Multiple
`LLEXT_IFLASH` functions in the same compilation unit will be merged into one
`.iflash.text` section by the linker. The post-load step scans for exactly this section
name.

If separate tracking per-function is needed, use `-ffunction-sections` without the
section attribute and instead provide a build-time list of function names to look
for in `.text.<name>` sections. The explicit section attribute approach is simpler.

---

## XIP Manager Fix

Remove the `first_xip_disable_done` workaround in `zsw_xip_manager.c` that calls
`nrf_qspi_nor_xip_enable(false)` twice on the first `zsw_xip_disable()`. The QSPI
driver's own `xip_users` refcount is the sole authority.

Current callers (LLEXT manager does NOT call these):

| Caller | Pattern |
|--------|---------|
| `zsw_display_control.c` | enable on wake, disable on sleep |
| `zsw_usb_manager.c` | enable during USB, disable after |
| `update_app.c` | enable around flash write, disable after |

---

## Open Questions

1. **`.iflash.text` section handling by LLEXT loader**: If GCC puts `LLEXT_IFLASH`
   functions into `.iflash.text`, the LLEXT loader may NOT classify this as
   `LLEXT_MEM_TEXT` (it checks for `.text` prefix). Need to verify whether the loader
   picks it up or ignores it. If ignored, the function body won't be in the loaded
   regions at all. May need the section named `.text.iflash` instead (still matches
   `.text*` pattern), or patch the loader's section classifier.

2. **GOT size discovery post-load**: The existing `zsw_llext_xip_pre_copy_hook`
   already finds the `.got` section offset in DATA. This offset (and the GOT size
   from `sh_size`) should be saved in the `zsw_llext_xip_context` so the post-load
   step can use it. Currently only `got_offset` is saved; `got_size` should be added.

3. **Instruction cache**: After patching GOT entries in RAM, `sys_cache_data_flush_range()`
   may be needed to ensure the D-cache writes are visible. I-cache invalidation is not
   needed since internal flash is already instruction-fetchable without caching.

4. **Multiple LLEXT_IFLASH functions**: If all tagged functions merge into one
   `.iflash.text` section, we can only copy the entire block, not individual functions.
   This is fine — the total size of background callbacks is small (~1-2 KB per app).
   The GOT scan must find and patch ALL entries pointing into the old XIP range
   `[old_addr, old_addr + section_size)`.

