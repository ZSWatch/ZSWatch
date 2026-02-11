---
sidebar_position: 2
---

# Compiling the Software

:::info Prerequisite
Make sure the [Toolchain is set up](./toolchain.md) as described.
:::

## Build Configurations

ZSWatch supports different **board targets** and **logging transports**. You combine them when creating a build.

### Board Targets

| Target | Board string | Description |
|--------|-------------|-------------|
| **WatchDK** | `watchdk@1/nrf5340/cpuapp` | Current development kit |
| **ZSWatch Legacy v5** | `zswatch_legacy@5/nrf5340/cpuapp` | Legacy ZSWatch v5 hardware |
| **ZSWatch Legacy v4** | `zswatch_legacy@4/nrf5340/cpuapp` | Legacy ZSWatch v4 hardware |
| **Native simulator** | `native_sim/native/64` | Linux simulator, see [Native Simulator](./linux_development.md) |

### Debug Logging

For debug builds, you need a base config (`debug.conf`) plus a **log transport**:

| Transport | Kconfig fragment | Overlay needed | Notes |
|-----------|-----------------|----------------|-------|
| **UART** | `debug.conf`, `log_on_uart.conf` | `log_on_uart.overlay` | Logs via serial |
| **RTT** | `debug.conf`, `log_on_rtt.conf` | *(none)* | Logs via SEGGER RTT (needs J-Link) |
| **USB** | `debug.conf`, `log_on_usb.conf` | `log_on_usb.overlay` | Logs via USB CDC ACM |

For **release builds** (no logging, optimized), use `release.conf` instead of `debug.conf`.

---

## Option A: Build from VS Code (nRF Connect Extension)

1. **Open VS Code** with the nRF Connect extension.
2. In the **nRF Connect** sidebar, under **Application**, select `app`.
3. Click **Add build configuration**.

   - **Target:** Select your board (e.g. `watchdk@1/nrf5340/cpuapp`).
   - **Extra Kconfig fragments:** Add the config files, e.g.: `debug.conf`, `log_on_uart.conf`
   - **Extra Devicetree overlays:** Add if needed, e.g.: `log_on_uart.overlay`
   - **Build Directory:** Optionally name it, e.g. `build_devkit`.

4. Click **Generate and Build**.

## Option B: Build from Command Line

Open an **nRF Connect Terminal** in VS Code (`Ctrl+Shift+P` → `nRF Connect: Create Shell Terminal`) and run:

```bash
# For example WatchDK with UART debug logging
west build --build-dir app/build_dbg_dk app \
  --board watchdk@1/nrf5340/cpuapp -- \
  -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf" \
  -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay"
```

<details>
<summary>More build examples</summary>

```bash
# WatchDK with RTT debug (no overlay needed)
west build --build-dir app/build_dbg_dk app \
  --board watchdk@1/nrf5340/cpuapp -- \
  -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_rtt.conf"

# Legacy ZSWatch v5 with RTT debug
west build --build-dir app/build_dbg_leg_v5 app \
  --board zswatch_legacy@5/nrf5340/cpuapp -- \
  -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_rtt.conf"

# Native simulator (Linux)
west build --build-dir app/build app \
  --board native_sim/native/64 \
  -DSB_CONF_FILE="sysbuild_no_mcuboot_no_xip.conf"
```

</details>


---

## Flashing

### From VS Code

Use the nRF Connect extension: **Actions → Flash**.

When hovering the Flash button, an icon to the right will appear **"Erase and Flash to Board"**. Use it as it will be much faster.

### From Command Line

```bash
west flash --build-dir app/build_dbg_dk
```

:::note TODO
Document how to flash a locally built firmware image without a debugger, using MCUboot and MCUmgr tools only (USB or BLE).
:::

---

## Debugging

For information on debugging, see the [Debugging](./debugging.md) guide.