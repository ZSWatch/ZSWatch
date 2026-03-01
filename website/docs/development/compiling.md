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

## Building

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
  <TabItem value="vscode" label="VS Code" default>

1. **Open VS Code** with the nRF Connect extension.
2. In the **nRF Connect** sidebar, under **Application**, select `app`.
3. Click **Add build configuration**.

   - **Target:** Select your board (e.g. `watchdk@1/nrf5340/cpuapp`).
   - **Extra Kconfig fragments:** Add the config files, e.g.: `debug.conf`, `log_on_uart.conf`
   - **Extra Devicetree overlays:** Add if needed, e.g.: `log_on_uart.overlay`
   - **Build Directory:** Optionally name it, e.g. `build_devkit`.

   **For the native simulator** (`native_sim/native/64`), you also need to add under **Extra CMake arguments**:
   ```
   -DSB_CONF_FILE=sysbuild_no_mcuboot_no_xip.conf
   ```

4. Click **Generate and Build**.

  </TabItem>
  <TabItem value="cli" label="Command Line">

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

  </TabItem>
</Tabs>

---

## Flashing

<Tabs>
  <TabItem value="vscode" label="VS Code" default>

Use the nRF Connect extension: **Actions → Flash**.

When hovering the Flash button, an icon to the right will appear **"Erase and Flash to Board"**. Use it as it will be much faster.

  </TabItem>
  <TabItem value="cli" label="Command Line">

```bash
west flash --build-dir app/build_dbg_dk
```

  </TabItem>
  <TabItem value="nodebugger" label="Without a Debugger">

You can flash a locally built firmware over USB without any debugger hardware, using the MCUboot serial bootloader built into ZSWatch.

#### Enter MCUboot Mode

1. Connect the watch to your computer via **USB-C**.
2. **Hold down the bottom-right button** and **press the reset button**. This enters MCUboot serial recovery mode.

#### Firmware Images

The `dfu_application.zip` (found in your build directory or a GitHub release) contains three firmware images:

| File | Image | Description |
|------|-------|-------------|
| `app.internal.bin` | App Internal | Main application (app core) |
| `ipc_radio.bin` | Net Core | BLE radio firmware (network core) |
| `app.external.bin` | App External (XIP) | Code/data stored on external QSPI flash |

<Tabs>
  <TabItem value="webupdater" label="Web Updater" default>

1. Go to [zswatch.dev/update](https://zswatch.dev/update).
2. Choose **USB Serial** as connection method and click **Connect via Serial**.
3. Select the USB serial port (appears as `ZSWatch BOOT` or similar).
4. Under **Manual Upload**, select `dfu_application.zip` from your build directory (`app/<build_dir>/dfu_application.zip`) or from a GitHub release.
5. Click **Upload Firmware**. The updater will upload all three images in sequence.

  </TabItem>
  <TabItem value="mcumgr" label="mcumgr CLI">

Use the `mcumgr` command-line tool. There are [several MCUmgr client tools available](https://docs.zephyrproject.org/latest/services/device_mgmt/mcumgr.html#tools-libraries), any of them work with similar parameters. The example below uses the Go-based `mcumgr`:

```bash
# Install mcumgr (and Go if needed first)
go install github.com/apache/mynewt-mcumgr-cli/mcumgr@latest

# Extract the firmware zip
unzip app/build_dir/dfu_application.zip -d /tmp/dfu

# Verify connection (use the USB serial port, e.g. /dev/ttyACM0)
mcumgr --conntype serial --connstring dev=/dev/ttyACM0 image list
mcumgr --conntype serial --connstring dev=/dev/ttyACM0,mtu=4096 image upload -e -n 1 /tmp/dfu/app.internal.bin
mcumgr --conntype serial --connstring dev=/dev/ttyACM0,mtu=4096 image upload -e -n 5 /tmp/dfu/app.external.bin

# Upload of the net core image the device needs ~30 seconds to internally
# copy it after upload, so wait before resetting
mcumgr --conntype serial --connstring dev=/dev/ttyACM0,mtu=4096 image upload -e -n 3 /tmp/dfu/ipc_radio.bin
sleep 30

# Reset to boot into the new firmware
mcumgr --conntype serial --connstring dev=/dev/ttyACM0 reset
```

  </TabItem>
</Tabs>

  </TabItem>
</Tabs>

---

## Debugging

For information on debugging, see the [Debugging](./debugging.md) guide.