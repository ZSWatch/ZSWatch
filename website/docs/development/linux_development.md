---
sidebar_position: 3
---

# Native Simulator (Linux)

ZSWatch can be compiled as a native Linux executable that opens a simulated 240×240 display window. This is useful for developing and testing application logic, UI layouts, and event handling **without any hardware**.

:::info Linux only
The native simulator only works on **Linux**. macOS and Windows users should use the hardware build targets or develop inside a Linux VM / WSL2.
:::

## 1. Install Dependencies

These packages are needed for the SDL2-based display simulator:

```bash
sudo apt-get install build-essential libsdl2-dev
```

For other distributions:
- **Fedora**: `sudo dnf install SDL2-devel gcc make`
- **Arch**: `sudo pacman -S sdl2 base-devel`

For more information, see the [Zephyr native_sim documentation](https://docs.zephyrproject.org/latest/boards/native/native_sim/doc/index.html).

### Optional: BLE Support

If you want Bluetooth to work in the simulator, you also need:

```bash
sudo usermod -aG bluetooth $USER
sudo setcap cap_net_admin=eip $(which hciconfig)
```

:::warning
You need to log out and in again (or restart) for the group change to take effect.
:::

---

## 2. Build

### From Command Line

Open an **nRF Connect Terminal** (`Ctrl+Shift+P` → `nRF Connect: Create Shell Terminal`) and run:

```bash
west build --build-dir app/build app \
  --board native_sim/native/64 \
  -DSB_CONF_FILE="sysbuild_no_mcuboot_no_xip.conf"
```

### From VS Code (nRF Connect Extension)

1. In the **nRF Connect** sidebar, click **Add build configuration**.
2. **Target:** `native_sim/native/64`
3. **Build Directory:** `build` (this name makes the default debug launch config work)
4. **Extra CMake arguments:** `-DSB_CONF_FILE=sysbuild_no_mcuboot_no_xip.conf`
5. Click **Generate and Build**.

:::info
If you name the build directory `build`, debugging will work out-of-the-box with the default `.vscode/launch.json` path:
```
"program": "${workspaceFolder}/app/build/app/zephyr/zephyr.exe"
```
:::

---

## 3. Run ZSWatch

### Option A: Run Directly (No Debugger, No BLE)

```bash
./app/build/app/zephyr/zephyr.exe
```

The simulated display window will open and Zephyr logs print to the terminal. BLE will not be available, but the UI, apps, and all non-BLE functionality works fine.

### Option B: Run with Debugger (VS Code)

1. Go to **Run and Debug** (`Ctrl+Shift+D`) in VS Code.
2. Select **Debug Native (sudo)** and press **F5**.

:::note
The default launch configuration passes `--bt-dev=hci0` and uses sudo to claim the host Bluetooth adapter. If you don't need BLE, you can run the executable directly as shown above without needed for sudo.
:::

---

Once running, a simulated display window should appear.

**Navigation:**

| Input | Action |
|-------|--------|
| **Mouse click** | Touch interaction |
| **Enter** | Select / confirm |
| **Backspace** | Back button |
| **Arrow Up** | Navigate next / up |
| **Arrow Down** | Navigate previous / down |