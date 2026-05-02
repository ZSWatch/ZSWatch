---
sidebar_position: 9
---

# Debugging

## Debugger Hardware Setup (nRF54L15 DK)

The recommended debugger is the Nordic **nRF54L15 DK** (~$35), connected via a 10-pin 1.27mm SWD cable.

<div style={{padding: '0 20px'}}>

![nRF54L15 DK debugger connection](/img/dk_debugger_connection.jpg)

<div style={{textAlign: 'center', fontSize: '0.9em', marginBottom: '1em'}}>
  SWD cable and UART debug pin connection between the WatchDK and the nRF54L15 DK.
</div>

</div>

- **SWD cable**: Connect the 10-pin cable between the nRF54L15DK's Debug Out connector and the WatchDK's SWD header.
- **UART logs**: Connect the WatchDK's **UART TxD** pin to **P0.00** on the nRF54L15 DK. This routes log output to **VCOM0** on the nRF54L15 DK. All pre-built firmwares use UART logging, open a serial terminal on VCOM0 at **115200 baud**.
- **GND**: If no SWD cable is connected, also connect a GND wire between the two boards.

:::tip No hardware for UART logs?
If you don't have a debugger or serial adapter, compile the firmware with **RTT** enabled and view logs over the SWD connection instead. See [Compiling the Software](./compiling.md) for build options.
:::

---

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

---

## Coredump Debugging

ZSWatch has a built-in coredump system that captures crash information (registers, stack) when a fatal error or assertion occurs. The coredump is stored in retained RAM, then written to the filesystem (`/lvgl_lfs/coredump.txt`) on the next boot.

### Viewing a Coredump on the Watch

1. After a crash, the watch will reboot automatically.
2. Open the **Info** app on the watch.
3. If a coredump was captured, it will show the crash summary (file name, line number, timestamp).
4. From the Info app, you can **dump the coredump to the log output**, this prints the coredump data to whatever log transport you have configured (UART, RTT, or USB).

### Analyzing a Coredump

:::note
Run `west coredump` from the **nRF Connect shell** (e.g. via the nRF Connect VS Code extension terminal), where the required Python dependencies are installed.
:::

1. **Start `west coredump`** and let it wait for the coredump data. Use `--build_dir` to let west find the ELF and toolchain automatically.

   **Via UART**:
   ```bash
   west coredump --build_dir app/build_dbg_dk --serial_port /dev/ttyUSB0
   ```

   **Via RTT** (requires debugger connected):
   ```bash
   west coredump --build_dir app/build_dbg_dk
   ```

   Or specify the ELF and toolchain manually with `--elf` and `--toolchain` instead of `--build_dir`.

2. **On the watch**, open the **Info** app, then press the **download button** to output the coredump to the log. `west coredump` automatically captures it and loads the crash state into GDB.

3. You can now inspect variables, stack frames, and memory at the time of the crash.

:::caution
The `.elf` file must match the **exact firmware build** that was running when the crash occurred. If you rebuild, the addresses won't match and the backtrace will be wrong.

If you are running a **pre-built firmware** (downloaded from GitHub Releases or CI), the matching `.elf` file is included inside the downloaded firmware `.zip` archive.
:::

### Erasing a Stored Coredump

From the Zephyr shell (if enabled):

```
coredump erase 0
```

Or the coredump can be erased from the Info app on the watch.

---

## Shell Commands Reference

ZSWatch includes an interactive Zephyr shell for debugging, testing, and automation. The shell is available via UART, RTT, or USB depending on your build configuration.

### Accessing the Shell

Connect to the serial port (UART, RTT, or USB CDC ACM) at **115200 baud** and press **Enter** to see the prompt:

```
uart:~$
```

Type `help` to list all available commands, or `<command> help` to see subcommands.

### Hardware Control

| Command | Description | Example |
|---------|-------------|---------|
| `battery` | Show battery voltage, level, and charging status | `battery` |
| `power status` | Show power state and sleep timer | `power status` |
| `power wake` | Reset idle timeout to keep display awake | `power wake` |
| `vibration run_pattern <pattern>` | Run vibration pattern (`click`, `notification`, `alarm`) | `vibration run_pattern click` |
| `display set_brightness <0-100>` | Set display brightness (percent) | `display set_brightness 50` |
| `display get_brightness` | Get current display brightness | `display get_brightness` |
| `cpu freq` | Show current CPU frequency profile | `cpu freq` |

### App Management

| Command | Description | Example |
|---------|-------------|---------|
| `app list` | List all registered apps and their states | `app list` |
| `app launch <name>` | Launch an app by name (supports names with spaces) | `app launch Calculator` |
| `app close` | Close the currently running app | `app close` |
| `app state` | Show current UI state and running app | `app state` |

### Touch Simulation

Touch simulation commands allow automated testing and remote control via shell.

| Command | Description | Example |
|---------|-------------|---------|
| `touch <x> <y>` | Tap at display coordinates (0-239) | `touch 120 120` |
| `touchdown <x> <y>` | Press and hold at coordinates | `touchdown 50 180` |
| `touchmove <x> <y>` | Move touch to coordinates (while pressed) | `touchmove 100 180` |
| `touchup` | Release touch | `touchup` |

:::tip Testing apps from shell
You can script UI interactions from the companion app or pytest tests:
```bash
app launch Calculator
touch 120 120    # Tap the center button
touchup
app close
```
:::

### Debugging & Diagnostics

| Command | Description | Example |
|---------|-------------|---------|
| `coredump summary` | Show summary of stored coredump | `coredump summary` |
| `coredump log` | Print stored coredump to log output | `coredump log` |
| `coredump erase <index>` | Erase stored coredump by index | `coredump erase 0` |
| `factory_reset` | Erase all settings and reboot device | `factory_reset` |
| `boot start` | Enter serial recovery / bootloader mode | `boot start` |

### Kernel & System Monitoring

ZSWatch enables the **Zephyr kernel shell** for low-level debugging:

| Command | Description | Example |
|---------|-------------|---------|
| `kernel threads` | List all threads, stack usage, CPU time | `kernel threads` |
| `kernel stacks` | Show thread stack usage summary | `kernel stacks` |
| `kernel uptime` | Show system uptime | `kernel uptime` |
| `kernel reboot warm\|cold` | Reboot the device | `kernel reboot cold` |

:::info Thread Monitoring
The `kernel threads` command shows:
- Thread names and priorities
- Stack usage (current / max size)
- CPU utilization percentage
- Thread states (ready, suspended, pending)

This is useful for diagnosing stack overflows and performance issues.
:::

### Event Injection (Testing)

The `event` subcommands allow injecting synthetic sensor events for testing without real sensor input. Run `event help` to see available commands.

### Microphone Commands

The `mic` subcommands control microphone recording. Run `mic help` to see available commands.

---

## Remote Control via Companion App

The ZSWatch companion app can execute shell commands remotely over BLE, enabling:
- Automated testing from a phone/tablet
- Remote diagnostics without serial connection
- Scripted UI interactions

Commands are sent via the GadgetBridge protocol and executed in the shell context. This allows the companion app to launch apps, simulate touches, and query device state.
