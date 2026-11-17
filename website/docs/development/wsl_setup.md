# Windows + WSL: Native Simulator Setup

Use WSL 2 for simulator development and Windows for hardware work.

| Environment | Use it for |
|-------------|------------|
| **Windows** | Hardware builds, flashing, and debugging ([toolchain](./toolchain.md), [compiling](./compiling.md), [debugging](./debugging.md)) |
| **WSL Ubuntu** | Simulator development and tests; optional firmware builds for browser upload |

:::warning Keep hardware flashing on Windows
WSL's remote-device workflow cannot program ZSWatch external QSPI/XIP, and debugging may flash automatically. Use the [browser updater](#upload-wsl-built-firmware) for WSL-built firmware.
:::

## 1. Install WSL and Ubuntu

This guide uses **Ubuntu 24.04 LTS on an x64 Windows PC**. WSL is not separately listed as a tested Nordic host. Use a Windows installation with [WSLg support](https://learn.microsoft.com/en-us/windows/wsl/tutorials/gui-apps) for the simulator window.

In **PowerShell as Administrator**:

```powershell
wsl --install -d Ubuntu-24.04
wsl --update
wsl --list --verbose
```

Restart if prompted, then launch Ubuntu and create your Linux username and password. Confirm `wsl --list --verbose` shows `VERSION 2`; otherwise run `wsl --set-version Ubuntu-24.04 2`.

Run the remaining commands in Ubuntu. This guide covers x64 only (`uname -m` should print `x86_64`).

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

Install the Linux toolchain version recommended in the [toolchain guide](./toolchain.md#2-set-up-vs-code). The NCS source revision is pinned in [`app/west.yml`](https://github.com/ZSWatch/ZSWatch/blob/main/app/west.yml) and fetched by `west update`.

```bash
nrfutil sdk-manager toolchain install --ncs-version v3.3.0
```

If the recommended version is unavailable, consult [Nordic's installation guidance](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/installation/install_ncs.html).

## 3. Set Up the Linux Workspace

Keep the Windows checkout unchanged. Create a separate Linux-side checkout (`~/src/ZSWatch`); [Linux-side storage is faster](https://learn.microsoft.com/en-us/windows/wsl/filesystems). Use Git to transfer changes between workspaces; their uncommitted edits and build files are separate.

Start the Linux toolchain shell before running `west`:

```bash
nrfutil sdk-manager toolchain launch --ncs-version v3.3.0 --shell
```

In the shell, clone the project. For a fork, use its URL and check out your branch before initializing west:

```bash
mkdir -p ~/src
cd ~/src
git clone --recursive https://github.com/ZSWatch/ZSWatch.git
cd ZSWatch
west init -l app
west update
```

The toolchain shell provides Python. Install the project requirements there:

```bash
python -m pip install -r zephyr/scripts/requirements.txt \
  -r nrf/scripts/requirements-build.txt \
  -r app/scripts/requirements.txt
```

## 4. Build and Run

From the workspace root, inside the toolchain shell:

```bash
west build --build-dir app/build app \
  --board native_sim/native/64 -- \
  -DSB_CONF_FILE=sysbuild_no_mcuboot_no_xip.conf \
  -DEXTRA_CONF_FILE=boards/test_mode.conf
./app/build/app/zephyr/zephyr.exe
```

A simulated display opens with logs in the terminal; `test_mode.conf` skips onboarding. See [simulator controls and shell commands](./linux_development.md#3-run-zswatch). Hardware testing is still needed for BLE, sensors, power, and flash behavior.

For later edits, rebuild with the same command:

```bash
west build --build-dir app/build
```

In each new Ubuntu terminal, repeat the toolchain launch command above and enter `~/src/ZSWatch`. If changing build configuration, add `--pristine` to the build command.

### Edit in VS Code

Install [VS Code](https://code.visualstudio.com/) and Microsoft's **WSL** extension on Windows. Run `code .` from the Linux checkout and confirm the remote indicator says **WSL: Ubuntu-24.04**. Use a separate Windows VS Code window for hardware work.

### Optional: Run Native Tests

With the toolchain shell active:

```bash
python -m pip install -r app/pytest/requirements.txt
cd app/pytest
pytest test_native_app.py::TestNativeSim -s --app "Calc"
```

For headless tests, Xvfb and ImageMagick are included in the dependencies above.

### Upload WSL-Built Firmware

For hardware build targets, see [Compiling the Software](./compiling.md#board-targets). The simulator executable cannot be flashed.

Copy `app/<hardware-build-dir>/dfu_application.zip` to Windows (or access it through `\\wsl$\Ubuntu-24.04\home\<LinuxUser>\src\ZSWatch\app\<hardware-build-dir>`), then follow the [web updater instructions](./compiling.md#firmware-images). For recovery, see [MCUboot mode](./compiling.md#enter-mcuboot-mode).
