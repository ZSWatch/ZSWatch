---
sidebar_position: 10
---

# Tips and Troubleshooting

This page collects common pitfalls new users encounter and suggests activities to help you get started with ZSWatch development.

## Common Pitfalls

### 1. Missing Icons and Images After Flashing

**Symptom:** The watch UI loads but all icons are missing or show blank squares.

**Cause:** Image resources (icons, graphics) are stored in external flash and are **not** included in the firmware binary. They must be uploaded separately.

**Fix:** Upload the image resources using one of the methods described in [Image Resources](../development/image_resources.md). The quickest way is via the [web updater](https://zswatch.dev/update) — make sure to enable USB or BLE updates on the watch first (**Apps → Update**).

---

### 2. Using a Regular Terminal Instead of the nRF Connect Terminal

**Symptom:** `west` commands fail with "command not found" or missing dependencies.

**Cause:** The `west` tool and its required environment variables (`ZEPHYR_BASE`, toolchain paths, etc.) are only available inside the nRF Connect Terminal provided by the VS Code extension.

**Fix:** Always open an **nRF Connect Terminal** in VS Code: press `Ctrl+Shift+P` and select **nRF Connect: Create Shell Terminal**. Run all `west` commands there.

---

### 3. Forgetting `--recursive` When Cloning

**Symptom:** Build errors about missing modules or submodule references.

**Cause:** The repository uses Git submodules. If you clone without `--recursive`, they won't be fetched.

**Fix:** Clone with:
```bash
git clone https://github.com/ZSWatch/ZSWatch.git --recursive
```
If you already cloned without `--recursive`, run:
```bash
git submodule update --init --recursive
```

---

### 4. Skipping `west init` and `west update`

**Symptom:** Build fails with missing packages, Zephyr modules, or CMake errors.

**Cause:** The Zephyr workspace needs to be initialized. `west init -l app` sets up the workspace manifest and `west update` fetches all external dependencies (Zephyr, NCS modules, HALs, etc.).

**Fix:** In the nRF Connect Terminal, run:
```bash
west init -l app
west update
```
Also install the required Python packages (both lines are needed):
```bash
pip install -r zephyr/scripts/requirements.txt
pip install -r app/scripts/requirements.txt
```

:::note Windows
On Windows, the second pip command needs the `--no-build-isolation` flag:
```bash
pip install --no-build-isolation -r app/scripts/requirements.txt
```
:::

---

### 5. Forgetting to Enable Updates on the Watch

**Symptom:** The web updater at [zswatch.dev/update](https://zswatch.dev/update) cannot connect to the watch, or USB/BLE update fails silently.

**Cause:** Firmware and resource updates over USB or BLE must be explicitly enabled on the watch before each update session.

**Fix:** On the watch, navigate to **Apps → Update** and toggle **USB** and/or **BLE** to **ON** before starting the update.

---

### 6. RTC Jumper Misconfiguration (No Battery)

**Symptom:** The watch loses time every time USB is disconnected, or the RTC doesn't work at all.

**Cause:** By default the RTC power jumper may be set to VBAT. Without a battery connected, the RTC has no power source.

**Fix:** Set the RTC jumper to **VSYS** so it is powered from USB. See the [WatchDK Quick Start](./watchdk-quickstart.md#optional-battery--rtc-jumper) for the jumper diagram.

---

### 7. Confusing Debug Logging Configurations

**Symptom:** No log output, or build errors from mismatched conf/overlay files.

**Cause:** Debug logging requires a base `debug.conf` **plus** a log transport config. Some transports also need a device tree overlay, others don't:

| Transport | Config files | Overlay needed? |
|-----------|-------------|-----------------|
| **UART** | `debug.conf` + `log_on_uart.conf` | Yes — `log_on_uart.overlay` |
| **RTT** | `debug.conf` + `log_on_rtt.conf` | No |
| **USB** | `debug.conf` + `log_on_usb.conf` | Yes — `log_on_usb.overlay` |

**Fix:** Always include `debug.conf` as a base, then add the transport-specific config. Only add the overlay file if the transport requires one. See [Compiling - Debug Logging](../development/compiling.md#debug-logging) for examples.

---

### 8. Not Logging Out After Linux Permission Changes

**Symptom:** Bluetooth doesn't work in the native simulator, or permission errors when running `zephyr.exe`.

**Cause:** The `usermod` and `setcap` commands used during [native simulator setup](../development/linux_development.md) require a logout/login (or restart) to take effect.

**Fix:** Log out and back in (or restart your machine) after running the permission commands from the [native simulator setup](../development/linux_development.md#1-install-dependencies). Group membership and capabilities only take effect after a new login session.

---

### 9. Updating LVGL UI Without Checking App State

**Symptom:** Random crashes or watchdog resets, especially when the screen turns off.

**Cause:** When the screen turns off, the app enters the `UI_HIDDEN` state. Updating LVGL objects in this state can crash the system.

**Fix:** Always guard UI updates:
```c
if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
    lv_label_set_text(my_label, "Updated!");
}
```

---

### 10. Forgetting to Clean Up in `stop_func`

**Symptom:** Crash or erratic behavior after closing an app and opening another one.

**Cause:** If an app doesn't unsubscribe from periodic events or delete its timers in `stop_func`, callbacks will continue firing after the app is closed.

**Fix:** Always unsubscribe and clean up:
```c
static void my_app_stop(void)
{
    zsw_periodic_chan_rm_obs(&periodic_event_1s_chan, &my_listener);
    if (my_timer) {
        lv_timer_del(my_timer);
        my_timer = NULL;
    }
}
```

---

### 11. GadgetBridge Device Type Selection

**Symptom:** GadgetBridge connects but notifications, weather, or music control don't work.

**Cause:** GadgetBridge supports many different smartwatch protocols. ZSWatch uses the Bangle.js JSON messaging protocol, so the device type must be set correctly for GadgetBridge to use the right protocol handler.

**Fix:** When adding ZSWatch in GadgetBridge, long-press the discovered device and select **Bangle.js** from the dropdown. See [Phone Setup](./phone_setup.md#add-the-watch) for full steps.

---

## Getting Started — Suggested Activities

Here are ten activities to help you get familiar with the ZSWatch project. They are roughly ordered from easiest to more involved.

### 1. Set Up the WatchDK and Explore the UI

Follow the [WatchDK Quick Start](./watchdk-quickstart.md) to power on, update firmware, and upload image resources. Then spend time navigating the watch UI — try all the buttons, open different apps, and explore the settings. Understanding the user experience helps when you start developing.

### 2. Pair with Your Phone

Connect ZSWatch to your phone using [GadgetBridge](./phone_setup.md#option-1-gadgetbridge-recommended) (Android) or the native [iOS pairing flow](./phone_setup.md#pairing-without-the-companion-app). Test notifications, music control, and weather sync to see how the BLE communication works in practice.

### 3. Set Up the Development Toolchain

Install VS Code, the nRF Connect SDK, and all required tools following the [Toolchain Setup](../development/toolchain.md) guide. Clone the repo, run `west init` and `west update`. Getting a working build environment is the foundation for everything else.

### 4. Build and Flash Firmware from Source

Following the [Compiling](../development/compiling.md) guide, create a build configuration and flash it to the watch. Start with the default WatchDK UART debug configuration. Successfully building from source proves your toolchain is correctly set up.

### 5. Try the Native Simulator (Linux)

If you are on Linux, set up the [native simulator](../development/linux_development.md). Build for `native_sim/native/64` and run it. This gives you a simulated display window on your desktop, which is the fastest way to iterate on UI and app logic without reflashing hardware.

### 6. Read the Architecture Overview

Study the [Architecture Overview](../development/architecture.md) to understand how the system fits together: zbus events, sensor data flow, BLE communication, power states, and the app lifecycle. This conceptual understanding will save you time when writing or debugging code.

### 7. Study a Simple Existing App

Open the **flashlight** app (`app/src/applications/flashlight/`) — it is one of the simplest apps with roughly 100 lines of code. Read through `flashlight_app.c` and `flashlight_ui.c` to see how an app registers itself, creates LVGL widgets, handles user input, and cleans up. The **stopwatch** and **timer** apps are also good references for slightly more complex patterns.

### 8. Create Your Own "Hello World" App

Follow the [Writing Apps](../development/writing_apps.md) guide to create a minimal app. Copy the flashlight app as a starting point, rename everything, and make it display your own text or simple UI. Build, flash, and confirm it appears in the app picker on the watch.

### 9. Subscribe to a Zbus Event

Extend your app to subscribe to a zbus channel — for example, listen to `battery_sample_data_chan` and display the current battery voltage, or subscribe to `periodic_event_1s_chan` and update a counter every second. This teaches you the core event-driven pattern used throughout ZSWatch. See the [Subscribing to Events](../development/writing_apps.md#subscribing-to-events-zbus) section.

### 10. Explore Sensor Data and BLE Communication

Subscribe to sensor channels (accelerometer, magnetometer, pressure) and display their data in your app. Or listen to `ble_comm_data_chan` to react to incoming notifications or weather updates. This is where the real power of the platform comes together — sensor data, BLE, and UI all interacting through the event system.

---

:::tip Join the Community
If you get stuck, the [ZSWatch Discord](https://discord.gg/8XfNBmDfbY) is a great place to ask questions. The community is friendly and happy to help newcomers!
:::
