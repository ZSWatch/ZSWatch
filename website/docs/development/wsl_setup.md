# Windows + WSL: Native Simulator Setup

Use WSL 2 alongside your Windows development environment to run ZSWatch's Linux simulator. This gives you a quick **edit, build, run** loop for UI and application logic without flashing a watch.

| Environment | Use it for |
|-------------|------------|
| **Windows** | Hardware builds, flashing, and on-target debugging using the existing [toolchain](./toolchain.md), [compiling](./compiling.md), and [debugging](./debugging.md) guides |
| **WSL Ubuntu** | Native simulator development and tests; optionally build hardware firmware for upload through the Windows browser |

:::warning Keep hardware flashing on Windows
Do not flash ZSWatch through WSL's remote-device workflow. External QSPI/XIP programming is not supported by that workflow for ZSWatch, and incomplete programming may leave the watch unable to boot. A successful J-Link connection does not verify flashing support. Debug actions can also flash automatically.

Firmware built in WSL can instead be uploaded using the [browser updater](#optional-upload-wsl-built-firmware). This does **not** provide on-target debugging.
:::

## 1. Install WSL and Ubuntu

This guide uses **Ubuntu 24.04 LTS on an x64 Windows PC**. Nordic lists Ubuntu 24.04 x64 as a supported host, but does not list WSL as a separately tested environment. Use a Windows installation with [WSLg support](https://learn.microsoft.com/en-us/windows/wsl/tutorials/gui-apps) to display the simulator window.

In **PowerShell as Administrator**:

```powershell
wsl --install -d Ubuntu-24.04
wsl --update
wsl --list --verbose
```

Restart if prompted, then launch Ubuntu and create your Linux username and password. The distribution should show `VERSION 2`; otherwise run `wsl --set-version Ubuntu-24.04 2` in PowerShell.

**Run all remaining commands in Ubuntu**, unless stated otherwise. Check that `uname -m` prints `x86_64`; this guide does not cover ARM64.

## 2. Install Linux Dependencies and Toolchain

Install the build tools, SDL2 display libraries, and headless test tools:

```bash
sudo apt update
sudo apt install --no-install-recommends \
  git cmake ninja-build gperf ccache dfu-util device-tree-compiler wget \
  python3-dev python3-venv python3-tk xz-utils file make gcc gcc-multilib \
  g++-multilib libsdl2-dev libmagic1 xvfb imagemagick
```

Download **Linux x64 nRF Util** from [Nordic](https://www.nordicsemi.com/Products/Development-tools/nRF-Util/Download) to Windows Downloads. Replace the placeholders below with your Windows account and the downloaded filename:

```bash
install -Dm755 "/mnt/c/Users/<WindowsUser>/Downloads/<downloaded-file>" \
  "$HOME/.local/bin/nrfutil"
echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.bashrc"
source "$HOME/.bashrc"
nrfutil --version
nrfutil install sdk-manager
nrfutil sdk-manager search
```

The [toolchain guide](./toolchain.md#2-set-up-vs-code) currently recommends packaged toolchain `v3.3.0` for the NCS source revision `v3.3.0-preview1` pinned in `app/west.yml`. If `v3.3.0` is listed, install it:

```bash
nrfutil sdk-manager install v3.3.0
```

If it is unavailable, check [Nordic's installation guidance](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/installation/install_ncs.html) before choosing another version. The next step fetches ZSWatch's pinned SDK sources; do not use SDK Manager's separate source checkout instead.

:::info Separate environments
Keep your existing Windows toolchain. WSL needs its own **Linux** toolchain and Python environment; do not use Windows executables or a Windows nRF Connect terminal for these builds.
:::

## 3. Create the Linux Workspace

### Already have ZSWatch on Windows?

Leave that workspace unchanged. Create a separate checkout under `~/src/ZSWatch` for WSL builds. Windows paths such as `C:\src\ZSWatch` are accessible as `/mnt/c/src/ZSWatch`, but [Linux-side storage is faster for Linux builds](https://learn.microsoft.com/en-us/windows/wsl/filesystems).

Use the same fork and branch in both checkouts. Transfer committed changes through Git; uncommitted edits are not shared automatically. Do not copy Windows build directories, virtual environments, or generated environment files into WSL.

### Initialize the checkout

Start a Linux toolchain shell **before running any `west` command**:

```bash
nrfutil sdk-manager toolchain launch --ncs-version v3.3.0 --shell
```

In the shell it opens, clone the project. If contributing through a fork, substitute your fork URL and check out your development branch before initializing west:

```bash
mkdir -p ~/src
cd ~/src
git clone --recursive https://github.com/ZSWatch/ZSWatch.git
cd ZSWatch
west init -l app
west update
```

`west update` downloads the pinned SDK and modules; allow time and disk space for a separate Linux workspace.

The Nordic toolchain shell provides the Python environment; no separate virtual environment is needed. Install the project requirements inside that shell:

```bash
python -m pip install -r zephyr/scripts/requirements.txt \
  -r nrf/scripts/requirements-build.txt \
  -r app/scripts/requirements.txt
```

## 4. Build and Run the Simulator

From `~/src/ZSWatch`, inside the toolchain shell:

```bash
west build --build-dir app/build app \
  --board native_sim/native/64 -- \
  -DSB_CONF_FILE=sysbuild_no_mcuboot_no_xip.conf \
  -DEXTRA_CONF_FILE=boards/test_mode.conf
./app/build/app/zephyr/zephyr.exe
```

A simulated **240 x 240 display** should open, with logs in the terminal. `test_mode.conf` skips onboarding so you can begin testing immediately. Use the mouse for touch input; see the [simulator controls and shell commands](./linux_development.md#3-run-zswatch).

No watch, debugger, USB forwarding, Bluetooth adapter, or `sudo` is needed for this basic workflow. The simulator does not replace hardware testing for BLE, sensors, power, or flash behavior.

## 5. Daily Development Loop

For each new Ubuntu terminal, launch the toolchain shell, then enter the workspace **inside that shell**:

```bash
nrfutil sdk-manager toolchain launch --ncs-version v3.3.0 --shell
cd ~/src/ZSWatch
```

After editing, stop the running simulator, rebuild, and run again:

```bash
west build --build-dir app/build
./app/build/app/zephyr/zephyr.exe
```

If changing board or configuration arguments, repeat the full build command from step 4 with `--pristine` added after `west build`.

### Edit in VS Code

Install [VS Code](https://code.visualstudio.com/) on Windows and Microsoft's **WSL** extension. Run `code .` from the Linux checkout, allow the WSL server installation, and confirm the remote indicator says **WSL: Ubuntu-24.04**.

Keep the Windows checkout in a separate, non-WSL VS Code window for hardware work. The nRF Connect extension is not required for this WSL command-line workflow. In any new terminal, start the Linux toolchain shell as above; do not source an environment file generated on Windows.

### Optional: Run Native Tests

Using the simulator build from step 4 and the active Linux environment:

```bash
python -m pip install -r app/pytest/requirements.txt
cd app/pytest
pytest test_native_app.py::TestNativeSim -s --app "Calculator"
```

The runner uses Xvfb for a headless display and ImageMagick for screenshots.

### Optional: Upload WSL-Built Firmware

You can build firmware in WSL for your [hardware board target](./compiling.md#board-targets), then upload it with [zswatch.dev/update](https://zswatch.dev/update) in a **Windows browser** with Web Serial support, such as Chrome or Edge. The native simulator executable cannot be flashed to a watch.

1. Copy `app/<hardware-build-dir>/dfu_application.zip` to a Windows folder, or access it through `\\wsl$\Ubuntu-24.04\home\<LinuxUser>\src\ZSWatch\app\<hardware-build-dir>`.
2. Connect the watch to Windows by USB and select **USB Serial** in the updater. If needed, use [MCUboot recovery mode](./compiling.md#enter-mcuboot-mode).
3. Select the ZIP under **Firmware package** and choose **Update firmware**. Prefer the complete ZIP; individual `.bin` files are available under **Advanced** and require the correct image index.

The browser uploads firmware only: **you cannot use it for breakpoints, stepping, or on-target debugging**. Use the Windows nRF Connect/J-Link workflow for that.

## Further Reading

- [Native Simulator](./linux_development.md): controls, shell commands, and Linux development options.
- [Nordic remote-device guide](https://docs.nordicsemi.com/r/bundle/nrf-connect-vscode/page/guides/bd_work_with_boards.html/how-to-add-remote-devices): background only, **not a supported ZSWatch flashing/debugging procedure**. Keep hardware operations on Windows as described above.
