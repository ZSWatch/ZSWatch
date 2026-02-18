# LLEXT ARM PIC + Flash XIP — Upstream Zephyr Implementation Plan

## Problem Statement

Loading LLEXT extensions on memory-constrained ARM Cortex-M devices (e.g. nRF5340 with 512 KB RAM) currently requires one of two unsatisfying options:

1. **Load everything to RAM** — The default `llext_load()` copies `.text`, `.rodata`, `.data`, `.bss` all into heap. A 200 KB app with images eats 200+ KB of scarce SRAM. Not viable for real apps.

2. **Custom out-of-tree loaders** — ZSWatch maintains ~1650 lines of custom ELF parsing, relocation, and flash-writing code (`zsw_llext_stream_loader.c`, `zsw_llext_xip.c`) to stream `.text`/`.rodata` directly to XIP flash. This works, but:
   - Reimplements ELF parsing and relocation logic, duplicating Zephyr's — and inevitably falls behind (e.g. initially only handled `R_ARM_ABS32`, missed `R_ARM_THM_CALL`, `R_ARM_THM_JUMP24`, Thumb MOV relocations)
   - Uses `ET_REL` (relocatable object), so `.text` must be **patched in-place** after writing to flash — the hardest and most bug-prone part of the loader
   - Requires `CONFIG_ARM_MPU=n` because relocated code ends up at runtime-determined addresses
   - Is tightly coupled to nRF5340 QSPI XIP specifics
   - Cannot benefit from upstream improvements

**Goal**: Enable ARM LLEXT extensions to be compiled as **position-independent shared libraries** (`-fPIC`, `ET_DYN`), so `.text`/`.rodata` can be written to XIP flash **verbatim** (no instruction patching), with only the GOT (a small pointer table) resolved in RAM at load time.

---

## Why PIC Eliminates the Hard Problem

The fundamental difficulty with `ET_REL` + flash is that `.text` contains unresolved references baked into instruction encodings. After writing `.text` to flash, every call/branch/load instruction targeting an external or cross-section symbol must be **patched in place** — requiring read-modify-write of flash pages, page buffering, handling relocations that span page boundaries, and supporting ~6 different Thumb2 instruction relocation encodings. This is 60%+ of the ZSWatch custom code complexity.

With `-fPIC` (`ET_DYN`), the compiler generates **position-independent code** where:
- Internal function calls use **PC-relative addressing** — no patching needed
- External symbol references go through a **Global Offset Table (GOT)** — a small array of pointers in `.got` (classified as writable data, lives in RAM)
- At load time, only the GOT entries need to be filled with resolved addresses — **simple pointer writes in RAM**

| Concern | `ET_REL` (current) | `ET_DYN` + PIC (this proposal) |
|---------|---------------------|-------------------------------|
| Writing `.text` to flash | Write, then **patch** individual instructions in-place | Write **as-is** — code is position-independent |
| External symbol resolution | Encode into instruction bits at scattered offsets across `.text` | Fill GOT entries (pointer table in **RAM**) |
| Internal calls | May need R_ARM_THM_CALL patches | PC-relative, **no patches needed** |
| Flash write pattern | Random read-modify-write across pages | **Linear streaming**, no going back |
| Relocation types to handle in flash | ~6 Thumb2 instruction encodings (THM_CALL, THM_JUMP24, THM_MOVW, THM_MOVT, ABS32, ...) | None in flash — GOT fills are RAM pointer writes |
| Page buffering logic | Required (accumulate, flush, re-read for multi-page relocations) | **Not needed** |
| Code size overhead | None | ~5-15% larger (GOT indirection, `-mlong-calls`) |
| Runtime overhead | None (direct calls) | GOT indirection per external symbol access |

**The loading flow becomes almost trivially simple:**

```
1. Stream .text/.rodata → flash (no patching!)
2. Allocate .got + .data + .bss in RAM (small — typically < 2 KB total)
3. Walk GOT entries: write resolved symbol addresses (pointer writes in RAM)
4. Apply R_ARM_RELATIVE: add load bias to data pointers (RAM)
5. Done — execute from flash
```

---

## Current Zephyr State (What Already Works)

Zephyr's LLEXT subsystem already has substantial `ET_DYN` support — it was built for Xtensa shared libraries. The ARM gap is narrow.

### What's Already Implemented

| Component | Status | Details |
|-----------|--------|---------|
| `ET_DYN` loader parsing | **Done** | `llext_load.c` handles `ET_DYN` ELF type: VMA validation, section merging, symbol table selection (`SHT_DYNSYM`), pre-padding, overlap detection |
| `ET_DYN` symbol resolution | **Done** | `llext_load.c` lines 724–748: correctly adjusts `st_value` for `ET_DYN` (subtracts section VMA to get offset, adds runtime base) |
| `R_ARM_RELATIVE` relocation | **Done** | `arch/arm/core/elf.c` line 404: `*(uint32_t *)loc += load_bias` |
| `R_ARM_GLOB_DAT` relocation | **Done** | `arch/arm/core/elf.c` line 408: `*(uint32_t *)loc = sym_base_addr` |
| `R_ARM_JUMP_SLOT` relocation | **Done** | Same case as `R_ARM_GLOB_DAT` |
| `load_bias` calculation | **Done** | `arch/arm/core/elf.c` line 331: `const uintptr_t load_bias = (uintptr_t)ext->mem[LLEXT_MEM_TEXT]` |
| Shared library CMake build | **Done** | `extensions.cmake` uses CMake `SHARED` library type when `CONFIG_LLEXT_TYPE_ELF_SHAREDLIB`, adds `-shared` to linker |
| `.got` section handling | **Done** | `.got` has `SHF_WRITE | SHF_ALLOC` → classified as `LLEXT_MEM_DATA` → loaded to heap (RAM). This is correct behavior |
| ARM flag removal | **Partial** | `target_arm.cmake` already removes `-fno-pic` and `-fno-pie` from LLEXT builds |

### What's Missing (The Gaps)

| Gap | Location | Fix |
|-----|----------|-----|
| **`LLEXT_BUILD_PIC` gated to Xtensa** | `zephyr/subsys/llext/Kconfig` line 83: `depends on XTENSA` | Remove the `depends on XTENSA` gate, or broaden to `depends on XTENSA \|\| ARM` |
| **`-fPIC` never added for ARM** | `zephyr/cmake/compiler/gcc/target_arm.cmake`: `LLEXT_APPEND_FLAGS` only has `-mlong-calls -mthumb` | Add `-fPIC` to `LLEXT_APPEND_FLAGS` when `CONFIG_LLEXT_BUILD_PIC` is set |
| **`R_ARM_GOT_BREL` (type 26) not handled** | `zephyr/arch/arm/core/elf.c`: not defined, falls through to `default: unknown relocation` | Add handler: `*(uint32_t *)loc += sym_base_addr + got_offset` (GOT-relative reference — the compiler generates this for PIC GOT access) |
| **No ARM PIC end-to-end test** | No test exists for ARM `ET_DYN` LLEXT | Add test building a PIC extension and loading it |
| **Flash XIP write path** | LLEXT only copies to heap | Add `flash_ops` callbacks for writing `.text`/`.rodata` to flash (see design below) |

### What Does NOT Need to Change

- `llext_load.c` — `ET_DYN` parsing is already correct for ARM
- `llext_link.c` — relocation iteration works for both `ET_REL` and `ET_DYN`; ARM uses the generic `llext_link()` path (not the Xtensa-specific `llext_link_plt()`)
- `llext_mem.c` — section classification correctly puts `.got` in `LLEXT_MEM_DATA` (RAM)
- `arch_elf_relocate()` — `R_ARM_RELATIVE`, `R_ARM_GLOB_DAT`, `R_ARM_JUMP_SLOT` all work. Only `R_ARM_GOT_BREL` needs adding
- Symbol lookup, string tables, export tables — all ELF-type-agnostic

---

## Design

### Part 1: Enable ARM PIC Build + Load (In-RAM First)

Before adding flash XIP, get PIC extensions building and loading to RAM correctly. This validates the toolchain and relocation handling in isolation.

#### 1a. Kconfig Changes

```kconfig
config LLEXT_BUILD_PIC
    bool "Use -fPIC when building LLEXT"
-   depends on XTENSA
+   depends on XTENSA || ARM
    default y if LLEXT_TYPE_ELF_SHAREDLIB
    help
      By default LLEXT compilation is performed with -fno-pic -fno-pie
      compiler flags. Some platforms can benefit from using -fPIC instead,
      in which case most internal linking is performed by the linker at
      build time. Select "y" to make use of that advantage.
```

#### 1b. ARM CMake Flag Changes

In `zephyr/cmake/compiler/gcc/target_arm.cmake`:

```cmake
set(LLEXT_APPEND_FLAGS
  -mlong-calls
  -mthumb
)

# Add PIC flag if configured
if(CONFIG_LLEXT_BUILD_PIC)
  list(APPEND LLEXT_APPEND_FLAGS -fPIC)
endif()
```

#### 1c. Add `R_ARM_GOT_BREL` Relocation Handler

In `zephyr/arch/arm/core/elf.c`:

```c
#define R_ARM_GOT_BREL     26

// In the switch statement:
case R_ARM_GOT_BREL:
    /* GOT-relative offset: the linker has already placed the GOT entry,
     * we just need to adjust it by the load bias (GOT address in memory).
     * The GOT section is part of LLEXT_MEM_DATA which is loaded to heap.
     */
    *(uint32_t *)loc += (uintptr_t)ext->mem[LLEXT_MEM_DATA] - load_bias;
    break;
```

> **Note:** The exact semantics of `R_ARM_GOT_BREL` for Zephyr LLEXT need validation. In standard ELF, `R_ARM_GOT_BREL` resolves to `GOT(S) - GOT_ORG` — the offset of the symbol's GOT entry relative to the GOT base. Since LLEXT relocates the GOT (as part of `.data`) to a different address than the linked VMA, the offset may need adjustment. This requires testing with actual compiler output to determine the exact fix. The implementation above is a starting hypothesis.

#### 1d. Verification: PIC Extension Loads to RAM

At this point, a PIC-compiled ARM extension should load into RAM via the normal `llext_load()` path:
- `.text` → `LLEXT_MEM_TEXT` (heap) — contains PIC code
- `.rodata` → `LLEXT_MEM_RODATA` (heap) — read-only data
- `.got` + `.data` → `LLEXT_MEM_DATA` (heap) — GOT + initialized data
- `.bss` → `LLEXT_MEM_BSS` (heap)
- Relocations: `R_ARM_RELATIVE` adjusts pointers by `load_bias`, `R_ARM_GLOB_DAT` fills GOT entries with resolved symbol addresses

This validates the toolchain pipeline and relocation handling before adding flash complexity.

### Part 2: Flash XIP for PIC Extensions

Once PIC loading works in RAM, add flash write support. Because PIC code needs **no patching**, this is straightforward: write `.text`/`.rodata` to flash verbatim, allocate `.got`/`.data`/`.bss` in RAM, apply relocations only to RAM.

#### 2a. Flash Operations Callback

```c
/**
 * @brief Callbacks for writing LLEXT sections to flash for XIP execution.
 *
 * The user provides these to direct where .text/.rodata are installed
 * in flash and what CPU address corresponds to each flash offset.
 */
struct llext_flash_ops {
    /**
     * Allocate space in flash for a section.
     * @param size       Required size (bytes)
     * @param alignment  Required alignment
     * @param out_flash_addr  Output: flash device offset to write to
     * @param out_cpu_addr    Output: CPU address for XIP execution
     * @return 0 on success, negative errno on failure
     */
    int (*alloc)(size_t size, size_t alignment,
                 uintptr_t *out_flash_addr, uintptr_t *out_cpu_addr,
                 void *user_data);

    /**
     * Erase flash before writing.
     * @param flash_addr  Flash device offset
     * @param size        Size to erase
     */
    int (*erase)(uintptr_t flash_addr, size_t size, void *user_data);

    /**
     * Write data to flash.
     * @param flash_addr  Flash device offset
     * @param data        Data to write
     * @param size        Size in bytes
     */
    int (*write)(uintptr_t flash_addr, const void *data, size_t size,
                 void *user_data);

    /**
     * Free previously allocated flash space (for unload/error cleanup).
     */
    void (*free)(uintptr_t flash_addr, size_t size, void *user_data);

    /** Opaque user data passed to all callbacks */
    void *user_data;
};
```

#### 2b. Extended Load Parameters

```c
struct llext_load_param {
    /* ... existing fields ... */

    /* Flash-backed loading for PIC .text/.rodata */
    const struct llext_flash_ops *flash_ops;  /* NULL = normal RAM loading */
};
```

#### 2c. Loading Flow (PIC + Flash XIP)

```
Phase 1 — Metadata (same as today):
    1. Read ELF header, section headers
    2. Classify sections, build region map
    3. Read string tables, symbol table

Phase 2 — Allocate all regions:
    For .text and .rodata:
        → flash_ops->alloc() → get flash_addr + cpu_addr
        → Set ext->mem[region] = cpu_addr  (XIP address)
        → ext->mem_on_heap[region] = false
    For .got + .data:
        → Normal heap allocation (small — typically < 1-2 KB)
    For .bss:
        → Normal heap allocation + zero

Phase 3 — Copy data:
    For .text and .rodata:
        → Stream from ELF source → flash_ops->erase() → flash_ops->write()
        → NO relocation patching needed (PIC code is position-independent)
    For .got + .data:
        → Normal copy to heap (includes GOT with linker-generated entries)

Phase 4 — Link (relocations applied to RAM only):
    → R_ARM_RELATIVE: adjust pointers in .data/.got by load_bias
    → R_ARM_GLOB_DAT: fill GOT entries with resolved symbol addresses
    → R_ARM_JUMP_SLOT: fill PLT GOT entries (if present)
    → All writes go to LLEXT_MEM_DATA (heap) — no flash writes needed!

Phase 5 — Ready:
    → .text executes from XIP flash via GOT for external references
    → GOT lives in RAM alongside .data
```

**The critical simplification**: Phase 4 (linking) only writes to RAM. No flash read-modify-write, no page buffering, no streaming relocation chunking. This is why PIC eliminates the hard problem.

#### 2d. Integration Points

| Existing Component | Change |
|---------------------|--------|
| `llext_copy_regions()` in `llext_mem.c` | When `flash_ops != NULL`, write `.text`/`.rodata` to flash via streaming, set `ext->mem[]` to XIP CPU address |
| `llext_link()` in `llext_link.c` | No change needed — relocations target `.got`/`.data` (in RAM heap), which works as-is |
| `arch_elf_relocate()` | Add `R_ARM_GOT_BREL` handler (see 1c above) |
| `llext_unload()` | Call `flash_ops->free()` for flash-backed regions |
| `struct llext` | Store `flash_ops` pointer for unload |
| Kconfig | Add `CONFIG_LLEXT_FLASH_XIP` to gate flash write path |

**Key difference from `ET_REL` flash approach**: with PIC, `llext_link()` doesn't need any modification. All relocations target writable sections (`.got`, `.data`) which are in RAM. The loader doesn't need to "skip" any relocations or redirect write destinations.

#### 2e. Streaming Copy to Flash

Since PIC `.text`/`.rodata` need no patching, the flash copy is a simple linear stream:

```c
int llext_flash_copy_region(struct llext_loader *ldr, struct llext *ext,
                            enum llext_mem mem_idx,
                            const struct llext_load_param *ldr_parm)
{
    const struct llext_flash_ops *ops = ldr_parm->flash_ops;
    size_t section_size = ext->mem_size[mem_idx];
    uintptr_t flash_addr, cpu_addr;
    uint8_t buf[CONFIG_LLEXT_FLASH_COPY_CHUNK_SIZE];  /* e.g. 4 KB */

    /* Allocate flash space */
    int ret = ops->alloc(section_size, /* alignment */, &flash_addr, &cpu_addr,
                         ops->user_data);

    /* Erase destination */
    ret = ops->erase(flash_addr, ROUND_UP(section_size, erase_size), ops->user_data);

    /* Stream copy: read from ELF, write to flash */
    size_t offset = 0;
    while (offset < section_size) {
        size_t chunk = MIN(sizeof(buf), section_size - offset);
        llext_seek(ldr, section_file_offset + offset);
        llext_read(ldr, buf, chunk);
        ops->write(flash_addr + offset, buf, chunk, ops->user_data);
        offset += chunk;
    }

    ext->mem[mem_idx] = (void *)cpu_addr;
    ext->mem_on_heap[mem_idx] = false;
    return 0;
}
```

Compare this to the `ET_REL` streaming approach which would need ~300-500 lines for the same function (chunk-level relocation filtering, staging buffer management, seek contention avoidance, cross-chunk relocation handling).

---

## RAM Analysis

### Current Zephyr `llext_load()` (all RAM, ET_REL)

For a 200 KB extension (150 KB .text, 45 KB .rodata, 4 KB .data, 1 KB .bss):

| Allocation | Size | Lifetime |
|-----------|------|----------|
| .text heap | 150 KB | Permanent |
| .rodata heap | 45 KB | Permanent |
| .data heap | 4 KB | Permanent |
| .bss heap | 1 KB | Permanent |
| Section headers | ~400 B | Load only |
| Symbol table | ~2 KB | Load only |
| String table | ~1 KB | Permanent |
| **Total RAM** | **~203 KB** | |

### Proposed PIC + Flash XIP

Same 200 KB extension (now compiled with `-fPIC`, ~5-10% larger `.text`):

| Allocation | Size | Lifetime |
|-----------|------|----------|
| Streaming buffer | 4 KB | Load only (freed after) |
| Section headers | ~400 B | Load only |
| Symbol table (dynsym) | ~1 KB | Load only |
| String table (dynstr) | ~512 B | Load only |
| .got (GOT entries) | ~200-800 B | Permanent (in .data region) |
| .data heap | 4 KB | Permanent |
| .bss heap | 1 KB | Permanent |
| sym_tab (export) | ~200 B | Permanent |
| **Peak RAM during load** | **~12 KB** | |
| **Steady-state RAM** | **~6 KB** | |

**RAM reduction: ~94% during load, ~97% steady-state.**

The GOT adds a small permanent cost (~4 bytes per external symbol), but this is negligible compared to the 195 KB saved on `.text`/`.rodata`.

---

## Implementation Steps

### Step 1: Enable ARM PIC Build

**Modified files:**
- `zephyr/subsys/llext/Kconfig` — Remove `depends on XTENSA` from `LLEXT_BUILD_PIC`
- `zephyr/cmake/compiler/gcc/target_arm.cmake` — Add `-fPIC` to `LLEXT_APPEND_FLAGS` when `CONFIG_LLEXT_BUILD_PIC`

**Validation:** Build a simple ARM LLEXT extension with `CONFIG_LLEXT_TYPE_ELF_SHAREDLIB=y` + `CONFIG_LLEXT_BUILD_PIC=y`. Inspect the output ELF:
```bash
arm-zephyr-eabi-readelf -d extension.so    # check NEEDED, GOT entries
arm-zephyr-eabi-readelf -r extension.so    # check relocation types
arm-zephyr-eabi-objdump -d extension.so    # check PIC codegen (GOT-relative loads)
```

### Step 2: Add Missing Relocation Handler

**Modified file:**
- `zephyr/arch/arm/core/elf.c` — Add `R_ARM_GOT_BREL` (and any other PIC-specific types the compiler generates)

**Approach:**
1. Build a non-trivial PIC extension (multiple external calls, data references)
2. List all relocation types: `readelf -r extension.so`
3. Implement handlers for any unhandled types
4. Likely candidates: `R_ARM_GOT_BREL` (26), possibly `R_ARM_GOT_ABS` (95), `R_ARM_GOT_PREL` (96)

### Step 3: Verify PIC Load to RAM

**New test:**
- `zephyr/tests/subsys/llext/arm_pic/` — Build and load a PIC ARM extension to heap

Test should verify:
- Extension builds as `ET_DYN` with `-fPIC`
- `llext_load()` succeeds (all relocation types handled)
- Extension function calls work (GOT-mediated external calls)
- Extension data references work (GOT-mediated global access)
- Load + unload cycle works

### Step 4: Add Flash XIP Write Path

**New files:**
- `zephyr/include/zephyr/llext/flash.h` — `struct llext_flash_ops`
- `zephyr/subsys/llext/llext_flash.c` — `llext_flash_copy_region()` (linear streaming, ~80 lines)

**Modified files:**
- `zephyr/include/zephyr/llext/llext.h` — Add `flash_ops` to `struct llext_load_param` and `struct llext`
- `zephyr/subsys/llext/llext_mem.c` — Route `.text`/`.rodata` through flash path when `flash_ops != NULL`
- `zephyr/subsys/llext/llext.c` — Call `flash_ops->free()` on unload
- `zephyr/subsys/llext/Kconfig` — Add `CONFIG_LLEXT_FLASH_XIP`

**Kconfig:**
```kconfig
config LLEXT_FLASH_XIP
    bool "Flash-backed XIP loading for LLEXT"
    help
      When enabled, LLEXT can load .text and .rodata sections directly
      to flash memory for execute-in-place (XIP) instead of copying
      them to RAM. The user provides flash operation callbacks via
      llext_load_param.flash_ops.

      When combined with CONFIG_LLEXT_BUILD_PIC, this enables a very
      efficient loading path: PIC code is written to flash verbatim
      (no instruction patching), and only the GOT (a small pointer
      table) is resolved in RAM.

      Requires the target flash to be memory-mapped (XIP-capable).

config LLEXT_FLASH_COPY_CHUNK_SIZE
    int "Flash copy chunk size in bytes"
    depends on LLEXT_FLASH_XIP
    default 4096
    help
      Size of the temporary buffer used to stream sections to flash.
      Larger values are faster but consume more stack/heap during loading.
```

### Step 5: Reference Implementation for nRF5340

**New sample:**
- `zephyr/samples/subsys/llext/flash_xip/` — PIC extension loaded to QSPI XIP flash on nRF5340

```c
struct nrf_xip_ctx {
    const struct flash_area *fa;
    uint32_t next_offset;
    uint32_t partition_base;   /* XIP CPU address base (0x10000000 + partition offset) */
};

static const struct llext_flash_ops nrf_xip_ops = {
    .alloc = nrf_xip_alloc,
    .erase = nrf_xip_erase,
    .write = nrf_xip_write,
    .free  = nrf_xip_free,
    .user_data = &xip_ctx,
};

struct llext_load_param params = LLEXT_LOAD_PARAM_DEFAULT;
params.flash_ops = &nrf_xip_ops;

struct llext *ext;
llext_load(&loader, "my_app", &ext, &params);
/* .text/.rodata now execute from QSPI XIP flash */
/* .got/.data/.bss in RAM (~5 KB) */
```

### Step 6: ARM MPU Integration

With PIC + flash XIP, MPU compatibility improves significantly:

- `.text` in XIP flash → configure MPU region as **RX** (read + execute)
- `.got` + `.data` in RAM → normal **RW** data MPU region
- `.bss` in RAM → normal **RW** data MPU region
- The XIP flash region is a **fixed, known address range** (e.g. `0x10000000–0x13FFFFFF` on nRF5340) — can be configured as a static MPU region

Since the `.text` address range is predictable (allocated by `flash_ops->alloc()`), LLEXT can set up `k_mem_partition` entries for `CONFIG_USERSPACE` builds.

**This removes the current requirement for `CONFIG_ARM_MPU=n`** when using LLEXT on ARM.

---

## Challenges and Mitigations

### Challenge 1: ARM PIC Toolchain Validation

**Risk:** `arm-zephyr-eabi-gcc` may generate relocation types or code patterns not handled by Zephyr's relocation handler.

**Mitigation:** Build several representative extensions (simple functions, LVGL UI code, callback-heavy code) with `-fPIC` and catalog all relocation types. Add handlers incrementally. The GCC ARM backend has mature PIC support — this is unlikely to be fundamentally broken, but edge cases may exist.

### Challenge 2: `R_ARM_GOT_BREL` Semantics

**Risk:** The exact semantics of `R_ARM_GOT_BREL` in the context of LLEXT (where GOT is relocated separately from `.text`) need validation.

**Mitigation:** Standard ELF spec defines `R_ARM_GOT_BREL` as `GOT(S) - GOT_ORG`. In LLEXT, both `GOT(S)` and `GOT_ORG` are VMA values that the linker computed; at runtime the GOT has moved to `ext->mem[LLEXT_MEM_DATA]`. Test with real compiler output and adjust. May need to add the GOT base as explicit metadata in `struct llext`.

### Challenge 3: Code Size Increase

**Impact:** PIC codegen is ~5-15% larger than non-PIC due to GOT indirection instructions.

**Mitigation:** For flash-backed extensions, code size increase doesn't matter — flash is cheap. The trade-off is explicit: slightly larger flash footprint vs. dramatically reduced RAM. On nRF5340 with 64 MB external flash, this is a clear win.

### Challenge 4: Flash Erase/Write Timing

**Impact:** Loading a 200 KB extension requires erasing ~50 × 4 KB sectors (~5 seconds on QSPI NOR).

**Mitigation:** This is a load-time cost only. With PIC, the flash write is a **single linear pass** — simpler and potentially faster than `ET_REL` which may need multiple passes over the same pages for scattered relocations.

### Challenge 5: GOT Size for Large Extensions

**Impact:** Each external symbol reference adds 4 bytes to `.got`. An extension calling 100 Zephyr APIs has a ~400 byte GOT.

**Mitigation:** This is negligible. Even with 500 external symbols, the GOT is 2 KB — vs. 200 KB saved on `.text`/`.rodata`. The GOT must live in RAM, but `.got` is already classified as `LLEXT_MEM_DATA` by the standard section classifier, so it shares the `.data` heap allocation naturally.

### Challenge 6: XIP Disable During Flash Write

**Impact:** On nRF5340, XIP must be disabled while writing to QSPI flash. Any code currently executing from the same flash will crash.

**Mitigation:** This is the user's responsibility (documented in `flash_ops` contract):
- Disable XIP before calling `llext_load()` with `flash_ops`
- Ensure no other code runs from the same flash partition during load
- Re-enable XIP after load completes
- This is identical to any firmware update scenario

---

## Scope and Non-Goals

### In Scope
- Remove `depends on XTENSA` from `LLEXT_BUILD_PIC`
- Add `-fPIC` to ARM LLEXT build flags
- Add `R_ARM_GOT_BREL` (and any other needed PIC relocation) handler
- `struct llext_flash_ops` callback API for flash-backed loading
- Linear streaming copy of PIC `.text`/`.rodata` to flash
- Integration with `llext_load()` / `llext_unload()` flow
- Kconfig gating (`CONFIG_LLEXT_FLASH_XIP`, `CONFIG_LLEXT_BUILD_PIC` on ARM)
- Reference nRF5340 sample
- ARM MPU compatibility guidance

### Not In Scope
- Automatic flash partition discovery — user provides callbacks
- Persistent install tracking (manifest, allocation table) — application-level concern
- BLE upload protocol — application-level
- Cache/XIP enable/disable management — platform-specific, user's responsibility in callbacks
- Wear leveling — user's `flash_ops` can implement if needed
- Inter-extension symbol sharing — future work
- `ET_REL` streaming relocation (the old Phase 1) — superseded by this PIC approach

---

## Relationship to Existing LLEXT Features

| Feature | Relationship |
|---------|-------------|
| `CONFIG_LLEXT_TYPE_ELF_SHAREDLIB` | **Required**. This proposal enables it on ARM (currently Xtensa-only in practice). Extensions build as `ET_DYN` shared libraries |
| `CONFIG_LLEXT_BUILD_PIC` | **Required**. This proposal removes the Xtensa-only gate |
| `CONFIG_LLEXT_RODATA_NO_RELOC` | Complementary but less needed. With PIC, `.rodata` typically has no relocations anyway (references go through GOT). `RODATA_NO_RELOC` may still be useful for non-PIC builds |
| `pre_located` mode | Different use case. `pre_located` assumes sections are already at final addresses. Flash XIP *creates* the placement during loading |
| `section_detached` callback | Orthogonal — could be combined if needed |
| `LLEXT_STORAGE_WRITABLE` / `PERSISTENT` | Flash XIP works with any storage type. ELF is read sequentially; output goes to flash |
| Xtensa `llext_link_plt()` | ARM does NOT use this code path. ARM uses the generic `llext_link()` loop. The `.rela.plt`/`.rela.dyn` special-casing is Xtensa-only. ARM's `.rel.dyn` is processed by the standard relocation loop |
| Harvard heaps | Flash XIP replaces the instruction heap for flash-backed extensions. Data heap still used for `.got`/`.data`/`.bss` |

---

## Testing Plan

1. **Build test: PIC ARM extension** — Verify `arm-zephyr-eabi-gcc -fPIC -shared` produces valid `ET_DYN` ELF with expected relocation types
2. **Unit test: PIC load to RAM** — Load PIC ARM extension to heap, verify all relocation types handled, extension runs correctly
3. **Unit test: GOT resolution** — Extension calls multiple exported kernel/subsystem APIs, verify GOT entries filled correctly
4. **Unit test: Data references** — Extension accesses global variables, verify GOT-mediated access works
5. **Integration test: Flash XIP** — Load PIC extension to flash via `flash_ops`, execute from XIP, verify correct behavior
6. **Integration test: Load + unload cycle** — Load, run, unload, verify flash freed, load again
7. **Integration test: Multiple extensions** — Load two PIC extensions to flash, verify independent GOTs
8. **Stress test: Large extension** — Extension with large `.rodata` (e.g. 500 KB of image data), verify linear flash streaming works
9. **MPU test: Userspace** — Load PIC extension with `CONFIG_USERSPACE`, verify MPU regions configured correctly
10. **Regression: Existing tests pass** — All existing LLEXT tests (ET_REL, Xtensa PIC) must pass unchanged

---

## Migration Path for ZSWatch

Once this feature lands upstream:

1. Remove `zsw_llext_stream_loader.c` entirely (829 lines) — no longer needed
2. Remove `zsw_llext_xip.c` `adjust_relocations()` and `write_section_to_xip()` (~500 lines) — no longer needed
3. Simplify `zsw_llext_xip.c` allocator to implement `struct llext_flash_ops` (~100 lines wrapping nRF QSPI APIs)
4. Change extension build to use `CONFIG_LLEXT_TYPE_ELF_SHAREDLIB=y` + `CONFIG_LLEXT_BUILD_PIC=y`
5. Remove `CONFIG_ARM_MPU=n` workaround — PIC + flash XIP with proper MPU region
6. Pass `flash_ops` in `llext_load_param`

**Net result:** ~1500 lines of custom ELF parsing, streaming relocation, and in-place flash patching code replaced by ~100 lines of flash callback wrappers + Kconfig changes. All relocation handling maintained by Zephyr upstream.

---

## Upstream RFC Framing

**RFC title:** "ARM `-fPIC` + Flash XIP for Low-RAM LLEXT Loading"

**Key selling points for upstream review:**
1. **Small diff** — Kconfig gate removal, ~20 lines of cmake, one new relocation handler, ~100-line flash copy function, callback struct
2. **No new ELF parsing** — reuses existing `ET_DYN` loader path that's proven on Xtensa
3. **No architectural changes** — extends existing `llext_load_param` pattern
4. **Solves real problem** — 200 KB extension → 6 KB RAM (vs. 203 KB today)
5. **Production-validated use case** — ZSWatch smartwatch with nRF5340 + QSPI XIP
