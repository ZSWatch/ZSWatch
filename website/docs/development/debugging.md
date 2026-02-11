---
sidebar_position: 9
---

# Debugging

## On-Target Debugging (WatchDK / ZSWatch)

On-target debugging is handled through the **nRF Connect VS Code extension**, which provides GDB integration with the J-Link debugger.

### Prerequisites

- A debugger connected via the **10-pin 1.27mm SWD cable** (J-Link or nRF54L15 DK)
- A build configuration created in VS Code (see [Compiling the Software](./compiling.md))

### Start Debugging

1. In VS Code, open the **nRF Connect** sidebar.
2. Under your build configuration, click **Actions → Debug**.
3. Use standard VS Code debugging controls: breakpoints, step over/into, variable inspection, call stack, etc.

### Reading Logs

Depending on your build configuration, logs are available through different transports:

| Transport | How to read |
|-----------|-------------|
| **UART** | Connect to the serial port (e.g. `/dev/ttyUSB0` on Linux) at 115200 baud. Use `minicom -b 115200 -D /dev/ttyUSB0` or `screen /dev/ttyUSB0 115200`. |
| **RTT** | Use SEGGER RTT Viewer or `JLinkRTTLogger` while the J-Link is connected. The nRF Connect extension also supports RTT output. |
| **USB** | Connect via USB and open the CDC ACM serial port that appears. |

See the [Compiling](./compiling.md#debug-logging) page for how to select a log transport in your build.

### Resetting the Watch
Use nRF Connect VSCode plugin -> CONNECTED DEVICES -> Reset button

OR:

```bash
nrfjprog --reset
```

---

## Native Simulator Debugging (Linux)

The native simulator compiles ZSWatch as a Linux executable, allowing you to debug with standard GDB, with full variable inspection, breakpoints, and LVGL UI interaction in a simulated display window.

For full setup instructions, see the [Native Simulator](./linux_development.md) page.

### Quick Start

1. Build for `native_sim/native/64` (see [Native Simulator - Build](./linux_development.md#2-build-the-project)).
2. In VS Code, go to **Run and Debug** (`Ctrl+Shift+D`).
3. Select **Debug Native (with sudo)** and press **F5**.
4. A simulated display window will open. Debug logs appear in the VS Code terminal.

:::tip
The native simulator is the fastest way to iterate on UI and application logic. No hardware or flash cycle needed.
:::
