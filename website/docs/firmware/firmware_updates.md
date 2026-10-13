---
sidebar_position: 1
---

# Firmware Updates

This page describes how to update ZSWatch firmware on WatchDK.

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
| `watchdk@1_nrf5340_cpuapp_debug.hex` | Firmware image (hex, for SWD flashing) |
| `zswatch_nrf5340_CPUNET.hex` | Network core image (hex, for SWD flashing) |
| `lvgl_resources_raw.bin` | Image resources (icons, graphics) for USB/BLE upload |
| `lvgl_resources_raw.hex` | RAW filesystem image for `nrfjprog` flashing |
| `qspi_mx25u51245.ini` | QSPI flash configuration for `nrfjprog` (required for `--qspiini`) |

After updating firmware, you typically also need to upload the matching **image resources** so icons and graphics display correctly. See [Image Resources](../development/image_resources.md) for details.

## Method A: Web Updater (USB or BLE)

1. For USB, enable **USB** in **Apps → Update** on the watch. For a first Bluetooth connection, enable **Pairable** in **Settings → Bluetooth**.
2. Open [zswatch.dev/update](https://zswatch.dev/update), choose USB or Bluetooth, and connect. Bluetooth update mode is enabled automatically, and the watch's reported hardware selects the matching firmware filter.
3. Choose a release or CI build and download the matching firmware package. Select the complete ZIP in **Firmware Package**; no manual extraction is needed.
4. Use **Update watch** to upload the included image resources and firmware together. **Advanced → Update firmware only** skips the resources. The watch restarts automatically after the update.

### Firmware file naming

The WatchDK firmware build is named:

- `watchdk@1_nrf5340_cpuapp_debug`

## Method B: Flash via Debugger (SWD)

Use this method if you have a debugger connected (for example J-Link, or Nordic nRF54L15 DK) and a 10-pin 1.27mm SWD cable.

### Option B1: Flash a pre-built firmware (nrfjprog)

1. Download the latest firmware hex from one of:
   - https://github.com/ZSWatch/ZSWatch/releases
   - https://github.com/ZSWatch/ZSWatch/actions
2. Connect the debugger to the WatchDK SWD header.
3. Flash the application core (and external flash configuration):

```bash
nrfjprog -f nrf53 \
  --program watchdk@1_nrf5340_cpuapp_debug.hex \
  --chiperase --qspisectorerase --verify --reset \
  --qspiini qspi_mx25u51245.ini \
  --coprocessor CP_APPLICATION
```

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
