---
sidebar_position: 1
---

# Firmware Updates

This page describes how to update ZSWatch firmware on WatchDK.

## Enable Updates on the Watch

Before any update (USB, BLE, or SWD), enable updates on the watch:

1. On the watch, go to **Apps → Update**.
2. Set **USB** and/or **BLE** to **ON** if you plan to use the web updater.

## Firmware Download Contents

When you download a firmware package from [GitHub Releases](https://github.com/ZSWatch/ZSWatch/releases) or [GitHub Actions](https://github.com/ZSWatch/ZSWatch/actions), the zip contains both the firmware and the image resources:

| File | Description |
|------|-------------|
| `dfu_application.zip` | Firmware images (zip, for USB/BLE update) |
| `watchdk@1_nrf5340_cpuapp_debug.hex` | Firmware image (hex, for SWD flashing) |
| `zswatch_nrf5340_CPUNET.hex` | Network core image (hex, for SWD flashing) |
| `lvgl_resources_raw.bin` | Image resources (icons, graphics) for USB/BLE upload |

After updating firmware, you typically also need to upload the matching **image resources** so icons and graphics display correctly. See [Image Resources](../development/image_resources.md) for details.

## Method A: Web Updater (USB or BLE)

1. Enable USB and/or BLE updates on the watch.
2. Open https://zswatch.dev/update.
3. Select a WatchDK firmware image.

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
  --qspiini /path/to/qspi_mx25u51245.ini \
  --coprocessor CP_APPLICATION
```

The `--qspiini` flag expects the `qspi_mx25u51245.ini` file from this repo. If you are flashing pre-built hex files, provide a local path to your copy of that file.

4. If you also need to re-flash the network core, flash CPUNET:

```bash
nrfjprog -f nrf53 \
  --program zswatch_nrf5340_CPUNET.hex \
  --chiperase --verify --reset \
  --coprocessor CP_NETWORK
```

### Option B2: Build from source and flash

If you are building the firmware yourself, use the nRF Connect VS Code extension or `west`.

See:
- [Toolchain Setup](../development/toolchain.md)
- [Compiling the Software](../development/compiling.md)
