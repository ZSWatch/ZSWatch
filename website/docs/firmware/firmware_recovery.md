---
sidebar_position: 2
---

# Firmware Recovery

If the watch firmware is not working (crash loop, frozen screen, etc.), normal BLE firmware updates won't work. You can recover using the built-in MCUboot bootloader.

:::tip
For a full guide on initial setup and firmware updates, see the [WatchDK Quick Start](../getting-started/watchdk-quickstart.md).
:::

## Recovery Without a Debugger

1. **Connect USB**: Make sure the watch is connected to your computer via USB-C.
2. **Enter the bootloader**: Hold down the **two right buttons** simultaneously for approximately **20 seconds**. The display will go blank. This means MCUboot is active and waiting for a firmware image over USB.
3. **Flash via the web updater**: Go to [zswatch.dev/update](https://zswatch.dev/update) and perform a firmware update over **USB**.

## Recovery With a Debugger (SWD)

If you have a debugger (J-Link or nRF54L15 DK) connected via the 10-pin SWD cable:

Follow the steps in [Firmware Updates](./firmware_updates.md), using the SWD method.

In recovery scenarios, prefer a full erase and re-flash of the application core.