# ZSWatch Development Instructions

## Project Overview

ZSWatch is an open-source smartwatch built on the **Zephyr RTOS** using the **nRF Connect SDK (NCS) v3.1.0**. The hardware is based on the **nRF5340** SoC (dual-core ARM Cortex-M33, 128 MHz, 512 KB RAM, 1 MB Flash) with a **240×240 round display**, various sensors (IMU, magnetometer, pressure, environment, light), BLE connectivity, and a 64 MB external QSPI flash.

- **License**: GPL-3.0
- **Language**: C (Zephyr/NCS ecosystem)
- **UI framework**: LVGL v9 (via Zephyr integration)
- **Build system**: CMake + west (Zephyr meta-tool)
- **Communication**: BLE with GadgetBridge (Android) and Apple ANCS/AMS (iOS)
- **Website**: [zswatch.dev](https://zswatch.dev) — Docusaurus site in `website/`

## Repository Structure

```
app/                        # Main application (west manifest repo)
├── CMakeLists.txt          # Top-level CMake
├── Kconfig                 # Top-level Kconfig
├── prj.conf                # Main project configuration
├── west.yml                # West manifest (NCS v3.1.0)
├── boards/                 # Board definitions and overlays
│   └── zswatch/            # ZSWatch board definitions
│       ├── watchdk/        # Watch DevKit (current board)
│       └── zswatch_legacy/ # Legacy hardware (v4/v5)
├── src/
│   ├── main.c              # Application entry point
│   ├── applications/       # All watch apps (self-registering)
│   ├── managers/           # System managers (app, power, notification, etc.)
│   ├── events/             # Zbus event definitions and channels
│   ├── ble/                # BLE services (GadgetBridge, AMS, ANCS, CTS, etc.)
│   ├── sensors/            # Sensor drivers and abstractions
│   ├── sensor_fusion/      # Sensor fusion algorithms
│   ├── ui/                 # UI components (app picker, popups, watchfaces)
│   ├── drivers/            # Custom Zephyr drivers
│   ├── images/             # Image assets
│   └── fuel_gauge/         # Battery fuel gauge
├── lvgl_editor/            # LVGL Editor integration (XML-based UI design)
├── drivers/                # Out-of-tree Zephyr drivers
└── dts/                    # Device tree overlays and bindings
website/                        # zswatch.dev documentation website (Docusaurus 3.7)
├── docusaurus.config.js    # Site config (URL, navbar, footer, plugins)
├── sidebars.js             # Sidebar auto-generated from docs/ structure
├── package.json            # Node.js dependencies
├── tailwind.config.js      # Tailwind CSS configuration
├── docs/                   # Documentation pages (Markdown/MDX)
│   ├── intro.md
│   ├── getting-started/    # Quickstart, hardware, software setup
│   ├── development/        # Compiling, toolchain, image resources, Linux dev
│   ├── hardware-design/    # PCB design documentation
│   └── hw-testing/         # Hardware testing procedures
├── blog/                   # Blog posts
├── src/
│   ├── components/         # Custom React components
│   │   ├── FirmwareUpdate/ # Web-based firmware update component
│   │   ├── GithubNavbar/   # GitHub stars navbar widget
│   │   ├── HomepageFeatures/
│   │   └── SubscriptionForm/
│   ├── pages/              # Custom pages (index, update, etc.)
│   ├── theme/              # Docusaurus theme overrides
│   └── css/                # Custom CSS (Tailwind)
└── static/                 # Static assets (images, favicon, etc.)
```

## Coding Conventions

### Naming
- **Public API prefix**: `zsw_` for all shared/public functions and types (e.g., `zsw_app_manager_show()`, `zsw_clock_get_time()`)
- **App-specific prefix**: Use the app name (e.g., `compass_ui_show()`, `music_control_app_start()`)
- **Types**: `snake_case` with `_t` suffix (e.g., `application_t`, `zsw_not_mngr_notification_t`)
- **Enums**: `UPPER_SNAKE_CASE` (e.g., `ZSW_APP_CATEGORY_ROOT`, `MUSIC_CONTROL_UI_PLAY`)
- **Macros/defines**: `UPPER_SNAKE_CASE` (e.g., `ZSW_LV_IMG_DECLARE`, `MAX_APPS`)
- **Kconfig symbols**: `CONFIG_APPLICATIONS_USE_<APP_NAME>`, `CONFIG_APPLICATIONS_CONFIGURATION_<APP>_<SETTING>`

### Style
- **Indentation**: 4 spaces (no tabs)
- **Header guards**: `#pragma once` (not `#ifndef`)
- **Braces**: Opening brace on same line for control structures, next line for function definitions follows standard Zephyr style
- **Static by default**: All file-local functions and variables must be `static`
- **Logging**: Use Zephyr logging: `LOG_MODULE_REGISTER(module_name, LOG_LEVEL_INF);` then `LOG_DBG()`, `LOG_INF()`, `LOG_WRN()`, `LOG_ERR()`
- **Assertions**: `__ASSERT()` / `__ASSERT_NO_MSG()` for runtime checks
- **Unused parameters**: `LV_UNUSED(param)` in LVGL callbacks, `ARG_UNUSED(param)` elsewhere

### Copyright Header

Every C source and header file must include:
```c
/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
```

CMakeLists.txt and Kconfig files use:
```cmake
# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
```

## Application Architecture

### App Registration (SYS_INIT pattern)

Every app self-registers with the app manager at boot using Zephyr's `SYS_INIT`. No central registry exists — apps are discovered automatically.

```c
#include <zephyr/init.h>
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

ZSW_LV_IMG_DECLARE(my_app_icon);

static void my_app_start(lv_obj_t *root, lv_group_t *group);
static void my_app_stop(void);

static application_t app = {
    .name = "My App",
    .icon = ZSW_LV_IMG_USE(my_app_icon),
    .start_func = my_app_start,
    .stop_func = my_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,  // Determines folder in app picker
};

static void my_app_start(lv_obj_t *root, lv_group_t *group)
{
    // Create LVGL UI under root
}

static void my_app_stop(void)
{
    // Clean up all UI objects and timers
}

static int my_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(my_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
```

### application_t struct fields

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `start_func` | `void(*)(lv_obj_t*, lv_group_t*)` | Yes | Called when the app is launched; create UI under `root` |
| `stop_func` | `void(*)(void)` | Yes | Called when exiting; must clean up all LVGL objects/timers |
| `back_func` | `bool(*)(void)` | No | Handle back button; return `true` to consume (stay in app) |
| `ui_unavailable_func` | `void(*)(void)` | No | Called when screen turns off (stop UI updates) |
| `ui_available_func` | `void(*)(void)` | No | Called when screen turns back on (resume UI updates) |
| `name` | `char*` | Yes | Display name in app picker |
| `icon` | `const void*` | Yes | App icon, use `ZSW_LV_IMG_USE(icon_name)` |
| `hidden` | `bool` | No | If `true`, app won't appear in the app picker |
| `category` | `zsw_app_category_t` | Yes | Folder placement in app picker |

### App Categories
```
ZSW_APP_CATEGORY_ROOT      // Top-level (no folder)
ZSW_APP_CATEGORY_TOOLS     // Tools folder
ZSW_APP_CATEGORY_FITNESS   // Fitness folder
ZSW_APP_CATEGORY_SYSTEM    // System folder
ZSW_APP_CATEGORY_GAMES     // Games folder
ZSW_APP_CATEGORY_SENSORS   // Sensors folder
ZSW_APP_CATEGORY_RANDOM    // Random folder
```

### App State Lifecycle
```
STOPPED → UI_VISIBLE ↔ UI_HIDDEN → STOPPED
```
- Check `app.current_state == ZSW_APP_STATE_UI_VISIBLE` before updating UI
- Use `ui_unavailable_func` / `ui_available_func` for screen on/off transitions

### Adding a New App Checklist
1. Create directory: `app/src/applications/<app_name>/`
2. Create `<app_name>_app.c` with the SYS_INIT registration pattern
3. Create `CMakeLists.txt` (optionally gated by Kconfig)
4. Provide an icon image (the build system handles icon storage)

### App CMakeLists.txt (with Kconfig guard)
```cmake
# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_MY_APP)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()
```

### App Kconfig Entry
```kconfig
config APPLICATIONS_USE_MY_APP
    bool
    prompt "Activate the application 'My App'"
    default y
```

## Event System (Zbus)

ZSWatch uses Zephyr's **zbus** for inter-module pub/sub communication.

### Defining an Event

**Header** (`my_event.h`):
```c
#pragma once

struct my_event {
    int some_data;
};
```

**Source** (`my_event.c`):
```c
#include <zephyr/zbus/zbus.h>
#include "my_event.h"

ZBUS_CHAN_DEFINE(my_data_chan,
                struct my_event,
                NULL,                    // validator
                NULL,                    // user_data
                ZBUS_OBSERVERS(listener1, listener2),
                ZBUS_MSG_INIT()
);
```

### Subscribing to Events
```c
ZBUS_CHAN_DECLARE(my_data_chan);
ZBUS_LISTENER_DEFINE(my_listener, my_callback);

static void my_callback(const struct zbus_channel *chan)
{
    const struct my_event *event = zbus_chan_const_msg(chan);
    // IMPORTANT: context-switch to work queue for non-trivial work
    k_work_submit(&my_work);
}
```

### Publishing Events
```c
struct my_event evt = { .some_data = 42 };
zbus_chan_pub(&my_data_chan, &evt, K_MSEC(50));
```

### Periodic Events
Shared periodic channels exist for common timing needs:
```c
// Subscribe to 100ms periodic events
zsw_periodic_chan_add_obs(&periodic_event_100ms_chan, &my_listener);
// Unsubscribe
zsw_periodic_chan_rm_obs(&periodic_event_100ms_chan, &my_listener);
```
Available: `periodic_event_100ms_chan`, `periodic_event_1s_chan`, `periodic_event_10s_chan`

### Existing Event Channels
- `accel_data_chan` — Accelerometer/IMU data
- `activity_state_data_chan` — Activity state changes (active/inactive/not-worn)
- `battery_sample_data_chan` — Battery status
- `ble_comm_data_chan` — BLE communication data
- `environment_data_chan` — Environmental sensor data
- `light_data_chan` — Light sensor
- `magnetometer_data_chan` — Magnetometer
- `music_control_data_chan` — Music control commands
- `pressure_data_chan` — Pressure sensor
- `zsw_notification_data_chan` — Notifications

## LVGL UI Development

### Display Constraints
- **Resolution**: 240×240 pixels, round display
- **Color depth**: 16-bit (RGB565)
- **LVGL version**: v9 (via NCS/Zephyr integration)
- **Theme**: Dark theme by default

### Image Handling
Images may be stored in internal flash or external flash. Always use these macros:
```c
ZSW_LV_IMG_DECLARE(image_name);              // Declare (at file scope)
.icon = ZSW_LV_IMG_USE(image_name),          // Use (in structs/code)
```
These macros handle the internal vs. external flash storage transparently.

### Color Helpers (from zsw_ui.h)
```c
zsw_color_red()         // Error, alerts
zsw_color_blue()        // Information
zsw_color_gray()        // Secondary text
zsw_color_dark_gray()   // Background elements
zsw_color_bg()          // Main background
```

### LVGL Editor Integration
Newer apps use the **LVGL Editor** for XML-based UI design with reactive **subjects**:

1. Define subjects in `app/lvgl_editor/globals.xml`
2. Create component XML files in `app/lvgl_editor/components/<name>/`
3. Generate code from LVGL Editor → produces `*_gen.c` / `*_gen.h`
4. In app code, call `<component>_create(root)` and update UI via subjects:
```c
#include "lvgl_editor_gen.h"   // Global subjects
#include "my_component_gen.h"  // Generated component

// Update UI reactively
lv_subject_copy_string(&my_text_subject, "Hello");
lv_subject_set_int(&my_value_subject, 42);
```

**Do NOT manually edit `*_gen.c` / `*_gen.h`** files — they are auto-generated.

Reference: `app/src/applications/music_control/music_control_app.c` demonstrates the LVGL Editor integration pattern.

### Traditional UI Pattern
Apps not using LVGL Editor separate UI into `*_ui.c` / `*_ui.h`:
```c
void my_app_ui_show(lv_obj_t *root);
void my_app_ui_remove(void);
void my_app_ui_update(int value);
```

## Managers

System-level services in `app/src/managers/`:

| Manager | API Prefix | Purpose |
|---------|-----------|---------|
| App Manager | `zsw_app_manager_` | App registration, lifecycle, app picker |
| Power Manager | `zsw_power_manager_` | Power states, idle timeout, screen on/off |
| Notification Manager | `zsw_notification_manager_` | Notification storage (max 10), add/remove/query |
| Microphone Manager | `zsw_microphone_manager_` | Audio recording |
| USB Manager | `zsw_usb_manager_` | USB connectivity |
| XIP Manager | `zsw_xip_manager_` | Execute-in-place from external flash |

### Power States
```c
ZSW_ACTIVITY_STATE_ACTIVE                // User is actively interacting
ZSW_ACTIVITY_STATE_INACTIVE              // Idle, screen off
ZSW_ACTIVITY_STATE_NOT_WORN_STATIONARY   // Watch not being worn
```

## Build System

### IMPORTANT: How to Run West Commands

**ALL `west` commands (build, flash, upload_fs, etc.) MUST be executed using the nRF Connect SDK environment.** The `west` tool and its dependencies are ONLY available when the proper environment is loaded. **NEVER use VS Code tasks, tasks.json, or `run_task` for west commands.**

**Required workflow for west commands:**

1. **First, generate the environment file** (only needed once per session or if the environment changes):
```python
run_task(id="nrf-connect-shell: Get nRF Connect environment setup", 
         workspaceFolder="<workspace_root_path>")
```
This creates `.nrf_env.sh` in the workspace root with all necessary environment variables (PATH, ZEPHYR_BASE, PYTHONPATH, etc.).

2. **Then, run west commands by sourcing the environment**:
```python
run_in_terminal(
    command="source .nrf_env.sh && west build --build-dir app/build_dbg_dk app --pristine --board watchdk@1/nrf5340/cpuapp -- -DEXTRA_CONF_FILE=\"boards/debug.conf;boards/log_on_uart.conf\" -DEXTRA_DTC_OVERLAY_FILE=\"boards/log_on_uart.overlay\"",
    explanation="Building ZSWatch firmware for Watch DevKit with UART debug logging",
    goal="Build firmware",
    isBackground=True,
    timeout=0
)
```

**Always prefix west commands with `source .nrf_env.sh &&` to load the environment.** Running `west` without sourcing the environment will fail with exit code 127 (command not found) or encounter missing dependencies.

### West Workspace
This is a **Zephyr T2 workspace application** — the `app/` directory is the manifest repository that imports NCS as a dependency.

```bash
# Initialize workspace
west init -l app
west update
```

### Default Build Command

**When the user asks to build or verify a build, default to Watch DevKit (watchdk@1) with UART debug logging unless they request a different variant.**

```bash
west build --build-dir app/build_dbg_dk app --pristine --board watchdk@1/nrf5340/cpuapp -- -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf" -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay"
```

**Remember to source the environment first:** `source .nrf_env.sh && <west command>`

To flash after building:
```bash
source .nrf_env.sh && west flash --build-dir app/build_dbg_dk
```

### Board Targets

Two hardware variants exist under `app/boards/zswatch/`:

| Board | Target | Description |
|-------|--------|-------------|
| **Watch DevKit** | `watchdk@1/nrf5340/cpuapp` | Current hardware (default). Board definition in `app/boards/zswatch/watchdk/` |
| **ZSWatch Legacy v5** | `zswatch_legacy@5/nrf5340/cpuapp` | Legacy ZSWatch v5 hardware. Board definition in `app/boards/zswatch/zswatch_legacy/` |
| **ZSWatch Legacy v4** | `zswatch_legacy@4/nrf5340/cpuapp` | Legacy ZSWatch v4 hardware |
| **Native simulator** | `native_sim/native/64` | Native POSIX simulator (Linux development, CI, and local testing) |

### Native Simulator (Linux) Build

When developing code that is **not platform-specific** (application logic, UI, event handling, etc.), prefer using the native simulator build first. It compiles to a Linux executable, runs on the host machine, opens a simulated display window, and prints Zephyr logs directly to stdout — making the development cycle much faster than flashing hardware.

**Build:**
```bash
source .nrf_env.sh && west build --build-dir app/build app --pristine --board native_sim/native/64 -DSB_CONF_FILE="sysbuild_no_mcuboot_no_xip.conf"
```

**Run (without debugger):**
```bash
# Run as background process — logs go to stdout
# Use --bt-dev=hci0 if BLE is needed (requires sudo and host BT disabled)
app/build/app/zephyr/zephyr.exe
```
The executable opens a display window simulating the 240×240 round screen. Logs print to the terminal in real time.

**Run with debugger (recommended):**
Use the VS Code launch configuration `"Debug Native (sudo)"` defined in `.vscode/launch.json`. It launches `app/build/app/zephyr/zephyr.exe` under GDB with `--bt-dev=hci0`. The `preLaunchTask` disables the host Bluetooth adapter (`hci0`) so Zephyr can claim it.

**Inspecting logs from the agent:**
The agent can run `zephyr.exe` as a background process via `run_in_terminal` and read its stdout to inspect Zephyr logs. If UI interaction is needed to trigger specific code paths, ask the user to perform an action in the simulator window and then check the terminal output for resulting log messages.

**When to use native_sim vs. hardware builds:**
- Use `native_sim` for: application logic, UI layout, event handling, zbus communication, LVGL development, and any code that does not depend on real hardware peripherals.
- Use hardware builds for: BLE pairing/communication, real sensor data, power management, flash storage, and peripheral drivers.

### Debug Logging Configuration

All debug logging requires `boards/debug.conf` as a base. Then choose **one** log transport:

| Transport | Conf file | Overlay file | Notes |
|-----------|-----------|--------------|-------|
| **UART** | `boards/log_on_uart.conf` | `boards/log_on_uart.overlay` | Logs via serial (default choice). Read logs from `/dev/ttyUSB0` |
| **RTT** | `boards/log_on_rtt.conf` | *(none needed)* | Logs via SEGGER RTT. Requires J-Link connection |
| **USB** | `boards/log_on_usb.conf` | `boards/log_on_usb.overlay` | Logs via USB CDC ACM |

- **UART** needs both a `.conf` and `.overlay` file (the overlay routes the console to `uart0`)
- **RTT** only needs the `.conf` file (no overlay)
- **USB** needs both a `.conf` and `.overlay` file

For **release builds** (no logging, optimized for size/speed), use `boards/release.conf` instead of `debug.conf`.

### Build Command Examples

**All commands below must be prefixed with `source .nrf_env.sh &&` when running via `run_in_terminal`.**

```bash
# Watch DevKit with UART debug (DEFAULT)
west build --build-dir app/build_dbg_dk app --pristine --board watchdk@1/nrf5340/cpuapp -- -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf" -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay"

# Watch DevKit with RTT debug (no overlay needed)
west build --build-dir app/build_dbg_dk app --pristine --board watchdk@1/nrf5340/cpuapp -- -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_rtt.conf"

# Legacy ZSWatch v5 with RTT debug
west build --build-dir app/build_dbg_leg_v5 app --pristine --board zswatch_legacy@5/nrf5340/cpuapp -- -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_rtt.conf"

# Legacy ZSWatch v4 with UART debug
west build --build-dir app/build_dbg_leg_v4 app --pristine --board zswatch_legacy@4/nrf5340/cpuapp -- -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf" -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay"

# Native simulator (Linux development, CI)
west build --build-dir app/build app --pristine --board native_sim/native/64 -DSB_CONF_FILE="sysbuild_no_mcuboot_no_xip.conf"
```

Note: `--pristine` forces a clean build. Omit it for incremental builds after the first build.

### Configuration Layering
1. `app/prj.conf` — Base configuration (always applied)
2. `app/boards/<board>.conf` — Board-specific overrides (e.g., `watchdk_nrf5340_cpuapp.conf`)
3. `app/boards/<board>.overlay` — Device tree overlays
4. `EXTRA_CONF_FILE` — Additional Kconfig fragments (`debug.conf`, `release.conf`, `log_on_*.conf`)
5. `EXTRA_DTC_OVERLAY_FILE` — Additional device tree overlays (`log_on_uart.overlay`, `log_on_usb.overlay`)

### Sysbuild
Multi-image builds (MCUboot + IPC radio) are handled via sysbuild:
- `app/sysbuild.conf` / `app/sysbuild.cmake`
- `app/Kconfig.sysbuild`

### Custom West Commands
Defined in `app/scripts/west-commands.yml`:
- `west upload_fs` — Upload filesystem images to external flash

### Filesystem Tasks
Filesystem operations can be run via VS Code tasks (`run_task` tool):
- `Generate Raw FS` / `Generate LittleFS` — Generate filesystem images without uploading
- `Upload Raw FS` / `Upload LittleFS` — Upload filesystem to external flash (watchdk)
- `Upload Raw FS Legacy v4` / `Upload LittleFS Legacy v4` — Upload for legacy v4 hardware
- `Erase external flash` / `Erase external flash Legacy v4` — Erase external flash

### Verifying Builds
When making code changes, always verify the build compiles successfully before finalizing changes. Run the default build command in the terminal and check for errors.

**Always source the nRF environment before running west commands:**
```python
run_in_terminal(
    command="source .nrf_env.sh && west build --build-dir app/build_dbg_dk app --pristine --board watchdk@1/nrf5340/cpuapp -- -DEXTRA_CONF_FILE=\"boards/debug.conf;boards/log_on_uart.conf\" -DEXTRA_DTC_OVERLAY_FILE=\"boards/log_on_uart.overlay\"",
    explanation="Building ZSWatch firmware to verify changes",
    goal="Verify build",
    isBackground=True,
    timeout=0
)
```

### Debugging and Logging

**Retrieving Logs (UART):**
After flashing with UART logging enabled, logs can be retrieved from the debug serial port, typically `/dev/ttyUSB0` on Linux.

```bash
# Read logs in real-time
cat /dev/ttyUSB0

# Or use a serial monitor tool
minicom -b 115200 -D /dev/ttyUSB0

# Screen utility
screen /dev/ttyUSB0 115200
```

**Retrieving Logs (RTT):**
Use SEGGER J-Link RTT Viewer or `JLinkRTTLogger` to read RTT logs while the J-Link debugger is connected.

**Resetting the Watch:**
The watch can be reset remotely using J-Link tools:

```bash
# Reset the nRF5340 via J-Link
nrfjprog --reset
```

## Kconfig Guidelines

### App-Level Kconfig Pattern
```kconfig
menu "My Application"
    config APPLICATIONS_USE_MY_APP
        bool
        prompt "Activate the application 'My App'"
        default y

    menu "Configuration"
        depends on APPLICATIONS_USE_MY_APP

        config APPLICATIONS_CONFIGURATION_MY_APP_REFRESH_INTERVAL_MS
            int
            prompt "Refresh interval in milliseconds"
            default 100
    endmenu
endmenu
```

### Key Config Symbols
- `CONFIG_ZSW_XIP` — Enable execute-in-place from external flash
- `CONFIG_ZSW_FW_UPDATE` — Enable firmware update (MCUboot + MCUmgr)
- `CONFIG_STORE_IMAGES_EXTERNAL_FLASH` — Store images on external flash
- `CONFIG_WATCHFACE_DIGITAL` / `CONFIG_WATCHFACE_MINIMAL` — Watchface selection
- `CONFIG_POWER_MANAGEMENT_IDLE_TIMEOUT_SECONDS` — Screen idle timeout

## BLE Communication

### Architecture
- **GadgetBridge protocol** (`ble_comm.c`) — Android companion app communication
- **Apple ANCS** (`ble_ancs.c`) — iOS notification service
- **Apple AMS** (`ble_ams.c`) — iOS media service
- **Apple CTS** (`ble_cts.c`) — Current time service
- **HID** (`ble_hid.c`) — HID over BLE (e.g., PPT remote)
- **HTTP proxy** (`ble_http.c`) — HTTP requests via phone
- **AoA** (`ble_aoa.c`) — Angle of Arrival (direction finding)

### BLE Data Flow
BLE data arrives via `ble_comm_data_chan` zbus channel. Apps subscribe and filter by `event->data.type`.

## Sensor Abstraction

Sensors are abstracted in `app/src/sensors/`:
- `zsw_imu.h` — IMU (BMI270): accelerometer, gyroscope, gestures, step counting
- `zsw_magnetometer.h` — Magnetometer (LIS2MDL)
- `zsw_pressure_sensor.h` — Pressure/altitude (BMP581)
- `zsw_environment_sensor.h` — Temperature, humidity
- `zsw_light_sensor.h` — Ambient light (APDS-9306)

Sensor data is published via zbus events (e.g., `accel_data_chan`, `magnetometer_data_chan`).

## Testing

### Pytest (Hardware-in-the-Loop)
Tests in `app/pytest/`:
```bash
pytest test_basic.py             # Boot verification
pytest test_ble.py               # BLE connectivity
pytest -m "linux_only"           # Native simulator tests
pytest -m "not linux_only"       # Hardware-only tests
pytest -m "ppk2"                 # Power consumption tests
```

## Common Patterns & Pitfalls

1. **Always context-switch from zbus callbacks** — Zbus listeners run in the publisher's context (often BLE thread with limited stack). Use `k_work_submit()` for any non-trivial work.
2. **Check app state before UI updates** — Guard with `if (app.current_state == ZSW_APP_STATE_UI_VISIBLE)` to avoid updating UI when the screen is off.
3. **Clean up in stop_func** — Delete all LVGL objects, timers, and unsubscribe from periodic events.
4. **Use XIP relocation for large resources** — Large code/data can be relocated to external flash via `zephyr_code_relocate()` with `EXTFLASH_TEXT`/`EXTFLASH_RODATA`.
5. **Round display** — The display is circular 240×240; design UIs accordingly (avoid content in corners).
6. **Memory constrained** — RAM is limited (512 KB shared with BLE stack); be mindful of stack sizes and heap allocations.
7. **Never edit `*_gen.c` / `*_gen.h`** — These are generated by LVGL Editor; edit the XML source instead.
