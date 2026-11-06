# WSL Development Setup

This guide sets up a Linux development environment for ZSWatch on Windows using WSL 2 and Ubuntu 24.04. It covers building the firmware and running the native simulator without a watch. Connecting a physical watch is optional and has additional USB setup at the end.

The recommended setup is Ubuntu 24.04 LTS on an x64 Windows PC. Nordic's current requirements list Ubuntu 24.04 x64 as supported; WSL itself is not listed as a separately tested host. Keep the project inside Ubuntu's Linux filesystem, such as `~/src/ZSWatch`, rather than under `/mnt/c`.

## 1. Install WSL 2 and Ubuntu

Open PowerShell as Administrator and install Ubuntu 24.04:

```powershell
wsl --install -d Ubuntu-24.04
```

Restart Windows if prompted. Launch Ubuntu from the Start menu and create the Linux username and password requested on first launch. Then, in PowerShell, update WSL and confirm the distribution uses version 2:

```powershell
wsl --update
wsl --set-default-version 2
wsl --list --verbose
```

The Ubuntu row should show `VERSION 2`. If it shows `1`, convert it with `wsl --set-version Ubuntu-24.04 2`.

In Ubuntu, confirm the architecture is x64:

```bash
uname -m
```

The output should be `x86_64`. Nordic does not list Linux ARM64 as supported for this SDK toolchain.

## 2. Install Linux Build Dependencies

In the Ubuntu terminal, install the host tools used by Zephyr and the native simulator:

```bash
sudo apt update
sudo apt upgrade -y
sudo apt install --no-install-recommends \
  git cmake ninja-build gperf ccache dfu-util device-tree-compiler wget \
  python3-dev python3-venv python3-tk xz-utils file make gcc gcc-multilib \
  g++-multilib libsdl2-dev libmagic1 xvfb imagemagick usbutils
```

These packages include the SDL2 libraries for the simulator. `xvfb` and `imagemagick` are used by the headless pytest runner; `usbutils` provides `lsusb` for the optional hardware setup.

## 3. Install the Nordic Linux Toolchain

Download the Linux x64 `nrfutil` executable from [Nordic's nRF Util downloads](https://www.nordicsemi.com/Products/Development-tools/nRF-Util/Download). In Ubuntu, put it on your Linux `PATH`:

```bash
mkdir -p "$HOME/.local/bin"
```

Choose either option to put the downloaded file in `$HOME/.local/bin` with the name `nrfutil`.

**Option A: Copy it from Windows Downloads in the Ubuntu terminal.** Replace `<WindowsUser>` and `<downloaded-file>` with the Windows account name and actual downloaded filename:

```bash
install -Dm755 "/mnt/c/Users/<WindowsUser>/Downloads/<downloaded-file>" "$HOME/.local/bin/nrfutil"
```

**Option B: Move it manually in File Explorer.** From Ubuntu, open the destination folder in Windows File Explorer:

```bash
explorer.exe "$HOME/.local/bin"
```

Drag the downloaded Linux executable from Windows Downloads into that folder and rename it `nrfutil`. Then, in Ubuntu, add the folder to `PATH` and verify the executable:

```bash
chmod +x "$HOME/.local/bin/nrfutil"
echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.bashrc"
source "$HOME/.bashrc"
nrfutil --version
```

Install Nordic's SDK Manager command and inspect the available SDK/toolchain versions:

```bash
nrfutil install sdk-manager
nrfutil sdk-manager search
```

ZSWatch pins its source manifest to NCS `v3.3.0-preview1` in `app/west.yml`. The existing toolchain guide recommends the closest available packaged toolchain, `v3.3.0`. If that version is listed, install it:

```bash
nrfutil sdk-manager install v3.3.0
```

This provides the Linux toolchain environment. The ZSWatch workspace's own west manifest will fetch the pinned preview source in the next step; do not substitute the SDK Manager's separate source checkout for the ZSWatch checkout. If `v3.3.0` is no longer available, stop before choosing a newer toolchain and check Nordic's [command-line installation guidance](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/installation/install_ncs.html) for the supported toolchain for this pinned revision.

If the nRF Connect extension is already installed in Windows VS Code, you can leave it installed for Windows builds. Its managed terminal and Windows toolchain are separate from WSL; for this guide, install and use the Linux `nrfutil` and toolchain inside Ubuntu. Do not run the WSL build using the extension's Windows-managed terminal or toolchain.

Nordic's guide includes troubleshooting for SDK Manager installation locks and other command options.

## 4. Clone and Initialize ZSWatch

Before running any `west` command, including `west init`, start the NCS toolchain shell from Ubuntu:

```bash
nrfutil sdk-manager toolchain launch --ncs-version v3.3.0 --shell
```

Run the following commands in the shell that this starts. This is what makes the SDK's `west` command available; running `west` in the original Ubuntu shell will result in `Command 'west' not found`. Repeat the launch command whenever you open a new Ubuntu terminal for development.

Clone into Ubuntu's Linux home directory for better filesystem performance:

```bash
mkdir -p ~/src
cd ~/src
```

If you are contributing through a GitHub fork, clone your fork so `origin` points to your account, then add the main project as `upstream`:

```bash
git clone --recursive https://github.com/<YOUR-USERNAME>/ZSWatch.git
cd ZSWatch
git remote add upstream https://github.com/ZSWatch/ZSWatch.git
git remote -v
```

If you are not using a fork, clone the main project directly:

```bash
git clone --recursive https://github.com/ZSWatch/ZSWatch.git
cd ZSWatch
```

Then initialize the workspace in either checkout:

```bash
west init -l app
west update
```

For a fork, `west init -l app` uses the `app/west.yml` in your fork. Keep that manifest in sync with the main project unless you intentionally need different SDK or module revisions.

`west update` downloads the SDK repositories selected by `app/west.yml`, including the pinned NCS revision. It can take a while and uses substantial disk space.

## 5. Install ZSWatch Python Requirements

From the ZSWatch repository root (`~/src/ZSWatch`), create the virtual environment at `.venv`. This is the workspace's expected interpreter path (`${workspaceFolder}/.venv/bin/python`), and activating it makes the build-script dependencies available:

```bash
python3 -m venv .venv
source .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install -r nrf/scripts/requirements-build.txt \
  -r app/scripts/requirements.txt
```

When opening another terminal, launch the NCS toolchain shell, enter the checkout, and activate the same environment:

```bash
nrfutil sdk-manager toolchain launch --ncs-version v3.3.0 --shell
cd ~/src/ZSWatch
source .venv/bin/activate
```

Do not use the Windows nRF Connect terminal or a Windows-installed toolchain for builds in this WSL checkout. The repository's `.nrf_env.sh` is generated by a Windows VS Code task and is not needed here.

## 6. Open the Project in VS Code

Install [Visual Studio Code](https://code.visualstudio.com/) on Windows and the Microsoft **WSL** extension. In the Ubuntu terminal, from the project root, run:

```bash
code .
```

If prompted, allow VS Code to install its WSL server. Check the lower-left remote indicator says `WSL: Ubuntu-24.04`. Install any desired editor extensions in the WSL context. The nRF Connect VS Code extension is not required for the command-line workflow in this guide.

## 7. Build the Firmware

Building does not require a watch or debugger. This example builds a WatchDK debug firmware image:

```bash
west build --build-dir app/build_dbg_dk app \
  --board watchdk@1/nrf5340/cpuapp -- \
  -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf" \
  -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay"
```

Keep the logging config and overlay paired: `log_on_uart.conf` uses `log_on_uart.overlay`. RTT logging uses `boards/log_on_rtt.conf` and does not have an overlay, so omit `-DEXTRA_DTC_OVERLAY_FILE` when selecting RTT. If correcting options from a failed or differently configured build, add `--pristine` after `west build` to clear the cached configuration.

The build output is under `app/build_dbg_dk`. Other board targets and build configurations are listed in [Compiling the Software](./compiling.md).

## 8. Build and Run the Native Simulator

WSLg provides Linux GUI windows on supported Windows installations. Build the Linux simulator with test mode enabled so its Zephyr shell is available:

```bash
west build --build-dir app/build app \
  --board native_sim/native/64 -- \
  -DSB_CONF_FILE=sysbuild_no_mcuboot_no_xip.conf \
  -DEXTRA_CONF_FILE=boards/test_mode.conf
```

Run it from the project root:

```bash
./app/build/app/zephyr/zephyr.exe
```

The SDL display window should open and logs will appear in the terminal. No sudo access, watch, debugger, or BLE adapter is required for the basic simulator workflow. Use the mouse for touch input and the keyboard controls in the [Native Simulator guide](./linux_development.md).

## 9. Optional: Run Native Tests

Install the pytest dependencies in the same Python environment:

```bash
python -m pip install -r app/pytest/requirements.txt
```

From the project root, run a native simulator app test:

```bash
cd app/pytest
pytest test_native_app.py::TestNativeSim -s --app "Calculator"
```

The runner uses Xvfb for a headless display and ImageMagick for screenshots. The test build should be in `app/build`, as in the simulator command above.

## 10. Optional: Connect a Physical Watch

For flashing and debugging through the nRF Connect extension, use [Nordic's Add Remote Device workflow](https://docs.nordicsemi.com/r/bundle/nrf-connect-vscode/page/guides/bd_work_with_boards.html/how-to-add-remote-devices) with [SEGGER J-Link Remote Server](https://kb.segger.com/WSL). The USB J-Link stays connected to Windows; the extension in WSL reaches it over TCP/IP. The probe does not need its own Ethernet interface.

Do not forward the J-Link to WSL with `usbipd` for this workflow. J-Link Remote Server does not forward the watch's USB serial or UART ports; those require a separate connection.

### Windows: Start J-Link Remote Server

1. Install the Windows **J-Link Software and Documentation Pack** from [SEGGER J-Link downloads](https://www.segger.com/downloads/jlink/).
2. Connect the J-Link to Windows by USB and connect its SWD interface to the powered watch or WatchDK.
3. Open **J-Link Remote Server**, not **J-Link GDB Server**.
4. Select **USB**, **Use LAN**, and listening port **19020**. Leave **Serial No.** unchecked unless you need to select among multiple probes.
5. Click **OK**, select the probe if prompted, and leave the server running and waiting for connections.

Allow inbound TCP port `19020` from the WSL network if Windows Firewall blocks the connection. Restrict access to trusted hosts; do not disable the firewall or expose the server to the Internet.

### WSL: Install SEGGER J-Link

The Windows J-Link installation is not available to Ubuntu. Download the Linux **64-bit DEB** package from [SEGGER J-Link downloads](https://www.segger.com/downloads/jlink/), then place the `.deb` file in Ubuntu's `~/Downloads` folder.

**Option A: Copy it from Windows Downloads in Ubuntu.** Replace `<WindowsUser>` and `<JLink-package>.deb` with the Windows account name and actual downloaded filename:

```bash
mkdir -p "$HOME/Downloads"
cp "/mnt/c/Users/<WindowsUser>/Downloads/<JLink-package>.deb" "$HOME/Downloads/"
```

**Option B: Move it manually in File Explorer.** Open Ubuntu's Downloads folder:

```bash
explorer.exe "$HOME/Downloads"
```

Drag the downloaded Linux 64-bit DEB package from Windows Downloads into that folder. Then install it from Ubuntu, replacing the placeholder with the actual filename:

```bash
sudo apt install "$HOME/Downloads/<JLink-package>.deb"
```

### WSL: Find and Test the Windows Address

In Ubuntu, find the Windows host address for WSL's default NAT networking:

```bash
export JLINK_HOST=$(ip route show default | awk '{print $3; exit}')
echo "$JLINK_HOST"
```

For example, if it prints `172.20.144.1`, use that address, not the WSL guest's own IP. This address can change after restarting WSL. The variable is local to the terminal; repeat the command in a new terminal.

If you explicitly enabled WSL mirrored networking, use `export JLINK_HOST=127.0.0.1` instead. See [Microsoft's WSL networking guidance](https://learn.microsoft.com/en-us/windows/wsl/networking).

Test the application-core connection from Ubuntu:

```bash
JLinkExe -device nrf5340_xxaa_app \
  -ip "$JLINK_HOST" -if SWD -speed 4000 -autoconnect 1
```

Successful output includes `Connecting to J-Link ...O.K.` and `Cortex-M33 identified.` Type `exit` at the `J-Link>` prompt to release the probe before using the extension. If it cannot connect, check Windows Remote Server, the host address, and the firewall first.

### WSL VS Code: Add the Remote Device

1. Open the Linux checkout with `code .` and confirm the remote indicator says **WSL: Ubuntu-24.04**.
2. Install **nRF Connect for VS Code** in the WSL context. The Windows installation alone is not sufficient. Configure the Linux SDK/toolchain installed earlier, not a Windows toolchain.
3. Add the existing `app` application and its `app/build_dbg_dk` build configuration to the extension if they are not already listed.
4. In the extension's **Connected Devices** view, use **Add Remote Device**, following [Nordic's instructions](https://docs.nordicsemi.com/r/bundle/nrf-connect-vscode/page/guides/bd_work_with_boards.html/how-to-add-remote-devices).
5. Enter the Windows host address printed above and port **19020**. If the dialog takes a combined address, enter `<Windows-host-IP>:19020`, for example `172.20.144.1:19020`.
6. Select the added remote device when flashing or debugging.

Port `19020` is the remote J-Link connection, not GDB port `2331`. The extension may still use `nrfutil` internally; adding the remote device provides the network connection instead of relying on local USB discovery.

### Flash and Debug in the Extension

Stop any standalone `JLinkExe`, `west attach`, or `west debugserver` session first so it releases the probe. Keep only the Windows J-Link Remote Server running; the extension manages the debugging session.

- **Build:** build the `app/build_dbg_dk` configuration in the nRF Connect view.
- **Flash:** use the build configuration's **Flash** action and select the remote device. Flash the complete sysbuild configuration, not just the `app` domain, when updating the complete firmware.
- **Debug:** use the build configuration's **Debug** action with the remote device. Check whether flashing before debugging is enabled before starting.
- **Attach:** use the extension's attach action for the application core when you want to debug firmware already running without loading a new image. The local build must match the installed firmware.

Use the extension-generated debug configuration for this workflow, not **Debug Native (sudo)**, which is for the Linux simulator. The manual `west debugserver` and custom GDB-server attach configuration are not needed here. Once attached, use VS Code breakpoints, stepping, variables, and call stacks. See [Debugging](./debugging.md) for additional project guidance.

:::warning Debug optimizations and flash space
The project's `boards/debug.conf` leaves `CONFIG_DEBUG_OPTIMIZATIONS` disabled because enabling it can add over 100 kB to the firmware. The extension may warn that debug options are not enabled. Optimized builds can still be inspected, but source stepping and variable visibility may be limited. For a build optimized for debugging, enable `CONFIG_DEBUG_OPTIMIZATIONS=y` in a local configuration fragment, rebuild, and flash the matching images. Other features may need to be disabled to fit in flash.
:::

Do not enable erase or recovery just to resolve a connection failure: those operations can delete watch data. If flashing reports a domain or external QSPI/XIP programming error, inspect the error before changing runners or flashing only one domain.

## References

- [Nordic nRF Connect SDK installation](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/installation/install_ncs.html)
- [Nordic SDK requirements](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/installation/recommended_versions.html)
- [Zephyr Linux host setup](https://docs.zephyrproject.org/latest/develop/getting_started/installation_linux.html)
- [Microsoft WSL USB setup](https://learn.microsoft.com/en-us/windows/wsl/connect-usb)
