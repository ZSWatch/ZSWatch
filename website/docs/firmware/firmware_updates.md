---
sidebar_position: 1
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

# Firmware Updates

This page describes how to update the firmware on the ZSWatch and the WatchDK.

## Enable Updates on the Watch

The **ZSWatch companion app** and [web updater](https://zswatch.dev/update)
enable Bluetooth firmware updates automatically. You do not need to open
**Apps → Update** or turn on **BLE** when using them.

For your first Bluetooth connection to a computer, go to **Settings → Bluetooth**
on the watch, enable **Pairable**, then connect in the web updater and accept
pairing on your computer.

For **USB updates**, go to **Apps → Update** on the watch and turn on **USB**.
For **nRF Device Manager**, turn on **BLE** there manually. Older firmware that
does not support automatic Bluetooth setup may also require this manual step.

## Firmware Download Contents

When you download a firmware package from [GitHub Releases](https://github.com/ZSWatch/ZSWatch/releases) or [GitHub Actions](https://github.com/ZSWatch/ZSWatch/actions), the zip contains both the firmware and the image resources:

:::note GitHub Actions downloads
GitHub requires you to be logged in to download **GitHub Actions** artifacts. If the link redirects you to sign in, log in and try again.
:::

| File | Description |
|------|-------------|
| `dfu_application.zip` | Firmware images (zip, for USB/BLE update) |
| `<board>_nrf5340_cpuapp_debug.hex` | Firmware image (hex, for SWD flashing) |
| `zswatch_nrf5340_CPUNET.hex` | Network core image (hex, for SWD flashing) |
| `lvgl_resources_raw.bin` | Image resources (icons, graphics) for USB/BLE upload |
| `lvgl_resources_raw.hex` | RAW filesystem image for `nrfjprog` flashing |
| `qspi_mx25u51245.ini` | QSPI flash configuration for `nrfjprog` (required for `--qspiini`) |

Image resources need to match the firmware's icons and graphics. You can keep the installed resources when they are unchanged; otherwise upload the matching resources from the same package. See [Image Resources](../development/image_resources.md) for details.

## WatchDK With an Older Bootloader

<details>
<summary>Only WatchDK units whose bootloader was flashed before 2026-10-19</summary>

Newer firmware uses 32-bit flash addressing; older WatchDK bootloaders use
24-bit. Updates and normal restarts work. After a reset from a debugger or
the hardware watchdog, an older bootloader can stop in serial recovery
(`Image in the primary slot is not valid!`): remove all power for a few
seconds. Flashing the full hex with a debugger
([Method B1](#option-b1-flash-a-pre-built-firmware-nrfjprog)) updates the
bootloader and removes the issue.

</details>

## Method A: Web Updater (USB or BLE)

1. For USB, enable **USB** in **Apps → Update** on the watch. For a first Bluetooth connection, enable **Pairable** in **Settings → Bluetooth**.
2. Open [zswatch.dev/update](https://zswatch.dev/update), choose USB or Bluetooth, and connect. Bluetooth update mode is enabled automatically, and the watch's reported hardware selects the matching firmware filter.
3. Under **Choose firmware**, select a release or download a CI build. You can also open a complete ZIP in **Firmware package**; no manual extraction is needed.
4. Choose **Update firmware** for a faster update that keeps the current image resources. Choose **Update firmware + resources** when the new version changes icons or graphics. The combined action appears when the ZIP includes resources and requires application mode. The watch restarts automatically after the update.

For Bluetooth connections, **Name filter** filters the browser's device list by name prefix. For example, `ZSWatch` includes names such as `ZSWatch AA:BB:CC`. Matching is case-sensitive. Clear the field to show all devices. **Connect watch** always opens the filtered device picker so you can choose which watch to connect.

The updater and Watch Tools share the same connection toolbar. **Device details** is collapsed by default; expand it to see hardware, firmware and battery information. Installed firmware slots and shell commands are under **Installed images & diagnostics**.

### Firmware file naming

Firmware builds are named after the board:

| Hardware | Firmware build |
|----------|----------------|
| ZSWatch | `zswatch@1_nrf5340_cpuapp_debug` |
| WatchDK | `watchdk@1_nrf5340_cpuapp_debug` |

Builds ending in `_rotated` are for a display mounted rotated (`clockwise_rotation.overlay`).

## Method B: Flash via Debugger (SWD)

Use this method if you have a debugger connected (for example J-Link, or Nordic nRF54L15 DK) and a 10-pin 1.27mm SWD cable. The ZSWatch has no SWD header of its own; connect the debugger through the Minimal Dock or a USB-C breakout board, see [Debugger Setup](../getting-started/quickstart.md#optional-debugger-setup).

### Option B1: Flash a pre-built firmware (nrfjprog)

1. Download the latest firmware hex from one of:
   - https://github.com/ZSWatch/ZSWatch/releases
   - https://github.com/ZSWatch/ZSWatch/actions
2. Connect the debugger to the WatchDK SWD header, or to the Minimal Dock with the ZSWatch in it.
3. Flash the application core (and external flash configuration):

<Tabs groupId="hardware" queryString>
  <TabItem value="zswatch" label="ZSWatch" default>

```bash
nrfjprog -f nrf53 \
  --program zswatch@1_nrf5340_cpuapp_debug.hex \
  --chiperase --qspisectorerase --verify --reset \
  --qspiini qspi_mx25u51245.ini \
  --coprocessor CP_APPLICATION
```

  </TabItem>
  <TabItem value="watchdk" label="WatchDK">

```bash
nrfjprog -f nrf53 \
  --program watchdk@1_nrf5340_cpuapp_debug.hex \
  --chiperase --qspisectorerase --verify --reset \
  --qspiini qspi_mx25u51245.ini \
  --coprocessor CP_APPLICATION
```

  </TabItem>
</Tabs>

The `--qspiini` flag expects the `qspi_mx25u51245.ini` file included in the downloaded zip.

4. If you also need to re-flash the network core, flash CPUNET:

```bash
nrfjprog -f nrf53 \
  --program zswatch_nrf5340_CPUNET.hex \
  --chiperase --verify --reset \
  --coprocessor CP_NETWORK
```

### Option B2: Build from source and flash

If you are building the firmware yourself, see [Compiling the Software](../development/compiling.md) for build and flash instructions using the nRF Connect VS Code extension or `west`.

## Program external filesystems with nrfjprog

If you want to program the filesystem images using only `nrfjprog`, use the generated `.hex` files:

```bash
nrfjprog -f nrf53 \
  --program lvgl_resources_raw.hex \
  --qspisectorerase --verify --reset \
  --qspiini qspi_mx25u51245.ini \
  --coprocessor CP_APPLICATION
```

If you only have source assets and need to generate those `.hex` files locally first:

```bash
west upload_fs --type raw --generate_only
```
