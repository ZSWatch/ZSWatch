---
sidebar_position: 1.6
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

# Heart Rate Extension

The heart rate extension board is an optional add-on that measures heart rate from the back of the watch. It is built around the Maxim **MAX32664C** sensor hub with a **MAX86141** optical front end and a **LIS2DS12** accelerometer, and connects to the extension connector on the ZSWatch PCB or the WatchDK.

:::info
We are still fine-tuning the heart rate extension, so the hardware, case parts and firmware may change.
:::

## Hardware

- **PCB:** design files are in the [Extension-HW](https://github.com/ZSWatch/Extension-HW) repository.
- **Case (ZSWatch):** print `bottom_hr.stl` and `bottom_hr_center.stl` from [Watch-3D](https://github.com/ZSWatch/Watch-3D) instead of `bottom.stl`. See [Step 10 of the assembly guide](./assembly.md#step-10---heart-rate-extension-only-glue-the-cover) for gluing the cover.

## Building the Firmware

The pre-built firmware does not include heart rate support. Build it yourself and add the heart rate config and overlay for your board (see [Compiling the Software](../development/compiling.md) for setup):

<Tabs groupId="hardware" queryString>
  <TabItem value="zswatch" label="ZSWatch" default>

```bash
west build --build-dir app/build_hr app \
  --board zswatch@1/nrf5340/cpuapp -- \
  -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf;boards/zswatch/zswatch/zswatch_nrf5340_cpuapp_hr.conf" \
  -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay;boards/zswatch/zswatch/zswatch_nrf5340_cpuapp_hr.overlay"
```

  </TabItem>
  <TabItem value="watchdk" label="WatchDK">

```bash
west build --build-dir app/build_hr app \
  --board watchdk@1/nrf5340/cpuapp -- \
  -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf;boards/zswatch/watchdk/watchdk_nrf5340_cpuapp_hr.conf" \
  -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay;boards/zswatch/watchdk/watchdk_nrf5340_cpuapp_hr.overlay"
```

  </TabItem>
</Tabs>

In VS Code, add the `_hr.conf` file under **Extra Kconfig fragments** and the `_hr.overlay` file under **Extra Devicetree overlays**.

With the extension enabled, the **Heart rate** app shows up in the app picker. The `hr` shell command (`hr status`, `hr measure`, ...) is useful for testing.
