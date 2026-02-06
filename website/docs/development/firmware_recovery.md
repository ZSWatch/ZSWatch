---
sidebar_position: 7
---

# Firmware recovery without a debugger

If the firmware is not working on ZSWatch, firmware updates are not possible over BLE. To flash new firmware you need to enter the bootloader.
This is done by holding down the two right buttons for ~20 seconds. After this the watch will enter MCUBoot and accept firmware over USB.

Now go to https://zswatch.dev/update and perform FW update over USB.