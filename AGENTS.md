# ZSWatch firmware: agent instructions

Single source of instructions for coding agents (Claude Code, Codex, Copilot).
Each `CLAUDE.md` only imports the `AGENTS.md` next to it.

Subdirectories with their own instructions. Read the file before working there;
not every agent loads nested files by itself:

| Directory | Instructions |
| --- | --- |
| `website/` | `website/AGENTS.md` (zswatch.dev, Docusaurus) |

ZSWatch is an open-source smartwatch on Zephyr / nRF Connect SDK (NCS
`v3.3.0-preview1`, pinned in `app/west.yml`). nRF5340 (dual Cortex-M33: app +
net core), 240×240 round display, LVGL 9, 64 MB external QSPI flash. C, GPL-3.0.

## Repository

| Path | What it is |
| --- | --- |
| `app/` | The firmware and west manifest repo (T2 workspace: `west init -l app && west update`). |
| `app/src/applications/` | Watch apps, one folder each, self-registering. |
| `app/src/managers/` | System services: app, power, notification, HR, microphone, USB, XIP... |
| `app/src/events/` | zbus channel definitions. |
| `app/src/ble/` | Gadgetbridge (`gadgetbridge/ble_gadgetbridge.c`), ANCS/AMS/CTS, HID, HTTP proxy, AoA. |
| `app/src/sensors/`, `sensor_fusion/`, `drivers/`, `ext_drivers/` | Sensor abstractions and drivers. |
| `app/src/ui/` | UI controller, app picker, popups, watchfaces. |
| `app/src/zsw_shell.c` | Most shell commands (used heavily for testing). |
| `app/lvgl_editor/` | LVGL Editor XML project; generates `*_gen.c/h`. |
| `app/boards/` | Board definitions (`zswatch/<board>/`) and conf/overlay fragments. |
| `app/pytest/` | Hardware-in-the-loop and native_sim tests, plus the bench tools (`bench/`). |
| `app/scripts/` | West extension commands and image/resource tools. |
| `production_test/` | Separate factory test firmware. |
| `website/` | zswatch.dev (Docusaurus). |
| `hardware/` | KiCad submodules. Do not touch unless asked. |
| `zephyr/`, `nrf/`, `nrfxlib/`, `modules/`, `bootloader/` | West-managed SDK. Never edit. |

`.agent_tools/` (git-excluded, may not exist) is a scratch area for agent helper
scripts and logs. Put throwaway scripts there, not in `/tmp` or the tree.

## Boards

| Target | Hardware |
| --- | --- |
| `zswatch@1/nrf5340/cpuapp` | ZSWatch (current watch) |
| `watchdk@1/nrf5340/cpuapp` | Watch DevKit |
| `zswatch_legacy@5/nrf5340/cpuapp`, `@4` | Legacy ZSWatch v5 / v4 |
| `native_sim/native/64` | Linux simulator (SDL display window, logs on stdout) |

Use the board the task is about. If none is given, use `watchdk@1`.
Heart rate (MAX32664C) is an opt-in variant: add
`boards/zswatch/<board>/<board>_nrf5340_cpuapp_hr.conf` and `.overlay` to the
extra conf/overlay lists.

## Build

West needs the NCS toolchain environment. If the repo root has `.nrf_env.sh`
(gitignored, machine-specific), prefix commands with `source .nrf_env.sh &&`.
Without it, run `eval "$(python app/pytest/bench/toolchain.py --print)"` once,
or prefix single commands with `python -m bench.toolchain` from `app/pytest`.
It uses `ZSW_NCS_TOOLCHAIN` or the newest bundle in `~/ncs/toolchains`.
Plain `west` failing with 127 means the environment is missing.

```sh
# Hardware, debug, logs on UART (the usual dev build)
west build --build-dir app/build_dbg_dk app --board watchdk@1/nrf5340/cpuapp -- \
  -DEXTRA_CONF_FILE="boards/debug.conf;boards/log_on_uart.conf" \
  -DEXTRA_DTC_OVERLAY_FILE="boards/log_on_uart.overlay"

west flash --erase --build-dir app/build_dbg_dk

# native_sim with test mode (skips onboarding so `app launch` works at once)
west build --build-dir app/build app --board native_sim/native/64 -- \
  -DSB_CONF_FILE=sysbuild_no_mcuboot_no_xip.conf -DEXTRA_CONF_FILE="boards/test_mode.conf"
```

Config layers: `app/prj.conf` → `app/boards/<board>.conf/.overlay` →
`EXTRA_CONF_FILE` / `EXTRA_DTC_OVERLAY_FILE` fragments. Fragments in
`app/boards/`: `debug.conf` or `release.conf`, then one log transport:
`log_on_uart` (.conf + .overlay), `log_on_rtt` (.conf only), `log_on_usb`
(.conf + .overlay). Others: `test_mode.conf`, `no_touch`, `clockwise_rotation`.
Add `-DCONFIG_ZSW_TEST_AUTO_LAUNCH_APP=\"Calculator\"` to open an app at boot.

Add `--pristine` after changing Kconfig/overlay sets. Changing a Kconfig default
can break boards that set it, so check all boards when the change is shared.

West extension commands (`app/scripts/west-commands.yml`): `west upload_fs`
(resource image to external flash), `west format` (astyle), `west coredump`.

CI (`.github/workflows/build.yml`) builds debug and release for every board in
the table above, plus the HR variants, the native_sim test build, production
test images and MemBrowse memory reports. Flash and RAM are tight: report
size changes from the build's memory summary when a change adds code or buffers.

## Testing

Prefer native_sim for UI, app logic, zbus and settings. Use hardware for BLE,
sensors, power, flash/XIP and drivers. Don't stop at "it builds": run it.

### pytest (`app/pytest/`)

```sh
source .venv/bin/activate && cd app/pytest   # deps: requirements.txt
pytest test_native_app.py::TestNativeSim -s --app "Calculator"   # boot, launch, screenshot, close
pytest test_hw_app.py::test_shell_cmd -s --board watchdk@1 --skip-flash --shell-cmd "battery"
pytest -m linux_only        # native_sim only
pytest -m ppk2              # power tests, need a PPK2
```

`native_sim_runner.py` handles Xvfb, the shell PTY (printed as `uart connected
to pseudotty`), log capture and screenshots (`/tmp` by default). Read the PNG to
check the UI. Boot marker in logs: `Enter active`. Crash markers: `ASSERTION
FAIL`, `FATAL`. BLE on native_sim needs `app/tools/capwrap` (see its
`setup.sh`) instead of sudo.

Devices (serial ports, J-Link serial, BLE MAC, PPK2) are in `devices.yaml`;
`devices_local.yaml` replaces it when present. `bench_only` entries are desk
setups, skipped by pytest unless selected with `--board`.

### Bench tools (`app/pytest/bench/`)

For scripted work on a real watch, use these instead of writing new
serial/BLE plumbing. See `bench/README.md`. Select a device with
`--device NAME` or `ZSW_DEVICE`.

```sh
python -m bench.ble shell "power status" "hr status"      # shell over BLE SMP
python -m bench.ble gb '{"t":"voice_memo","action":"list"}' # companion protocol over NUS
python -m bench.ppk serve &                                 # PPK2 as the battery; keep it running
python -m bench.ppk measure --seconds 60 --label idle --out results/
python -m bench.swd --elf <build>/app/zephyr/zephyr.elf <variable>...  # read RAM by symbol
```

### Shell

Available on hardware over USB CDC (SMP) or BLE SMP, and on native_sim over the
PTY. Run `help` or `<cmd> -h` for the current list. Useful ones: `app
list|launch <name>|close|state`, `touch <x> <y>` (and `touchdown`/`touchmove`/
`touchup`), `input` (Zephyr input subsystem), `event battery|pressure ...`,
`power status|wake|sleep`, `hr status|mode|measure|motion`, `voice_memo
start|stop|list|status`, `mic gain_get|gain_set`, `battery`, `stack`,
`coredump summary`. Add a shell command or a `LOG_INF` marker when a feature is
otherwise hard to observe.

### Hardware gotchas

- Watch DevKit has two ports: FTDI UART (logs only, 115200, TX only) and USB
  CDC ACM (SMP shell). USB turns off about 20 s after boot unless the host holds
  the CDC port open (DTR), so open it promptly and keep it open.
- Power measurements: disconnect BLE and turn SMP off first (it keeps a fast
  connection interval). `bluetoothctl disconnect` if a stale bleak link stays.
  Opening the PPK2 power-cycles the watch, so keep `bench.ppk serve` running.
- Any SWD access (`nrfjprog --memrd`, `bench.swd`) keeps the debug power domain
  on (+~1.5 mA) until a power cycle (`bench.ppk cycle`).
- If flashing fails because the QSPI flash is in deep power-down, or the watch
  is stuck in a crash loop: `nrfjprog --recover --coprocessor CP_NETWORK`,
  `nrfjprog --recover`, then flash with `--erase`. This wipes settings and
  bonds, so use it only when needed.
- Ask before anything that changes the user's phone pairing or deletes data
  on the watch (recordings, settings, factory reset).

## Code conventions

- 4 spaces, `#pragma once`, Zephyr brace style. Run `west format` on changed files.
- `static` for everything file-local. Public API prefixed `zsw_`; app-local
  functions prefixed with the app name. Types `snake_case_t`, enums/macros
  `UPPER_SNAKE_CASE`.
- Logging: `LOG_MODULE_REGISTER(name, LOG_LEVEL_INF)` + `LOG_DBG/INF/WRN/ERR`.
- `ARG_UNUSED()` / `LV_UNUSED()` for unused params; `__ASSERT()` for invariants.
- New C files get the GPL-3.0 header used in existing files; CMake/Kconfig files
  get `# SPDX-License-Identifier: Apache-2.0`. Use the current year.
- Kconfig: `CONFIG_APPLICATIONS_USE_<APP>` enables an app,
  `CONFIG_APPLICATIONS_CONFIGURATION_<APP>_<SETTING>` configures it.

## Architecture notes

- **Apps** register themselves with `SYS_INIT(..., APPLICATION, ...)` calling
  `zsw_app_manager_add_application(&app)` with an `application_t` (name, icon via
  `ZSW_LV_IMG_USE`, category, `start_func(root, group)`, `stop_func`, optional
  `back_func`, `ui_unavailable_func` / `ui_available_func`). `stop_func` must
  delete every LVGL object/timer and unsubscribe. Only touch the UI while
  `app.current_state == ZSW_APP_STATE_UI_VISIBLE`. Copy an existing app in
  `app/src/applications/` as the template.
- **zbus**: channels in `app/src/events/`. Listeners run in the publisher's
  thread (often the BLE thread with a small stack), so move real work to a work
  queue. Periodic ticks: `zsw_periodic_chan_add_obs(&periodic_event_{100ms,1s,10s}_chan, ...)`.
- **LVGL**: images via `ZSW_LV_IMG_DECLARE/USE` (internal or external flash).
  Convert a new external image with
  `python app/scripts/lvgl_c_array_to_bin_lvgl9.py <file.c> app/src/images/binaries/S/`.
  LVGL Editor apps: edit the XML in `app/lvgl_editor/`, never `*_gen.c/h`;
  `music_control` is the reference. Round 240×240 display: keep content out of
  the corners.
- **Memory**: RAM is shared with the BLE stack and nearly full. Size stacks and
  buffers deliberately (`stack` shell command shows high-water marks). Large
  code/rodata can go to external flash with XIP (`zephyr_code_relocate`).
- **Power**: screen-off, inactive and not-worn/stationary states come from the
  power manager. Background work must not keep the CPU, sensors or a short BLE
  interval busy when idle.
- **Companion protocol**: Gadgetbridge/BangleJS JSON over Nordic UART
  (`GB({...})` from the phone). The ZSWatch app (separate repo) and Gadgetbridge
  both depend on it, so keep changes backward compatible.

## Git

- `main` is the default branch; PRs target it and CodeRabbit reviews them.
- When a change alters behavior the docs describe (build commands, boards,
  Kconfig, shell, architecture), update `website/docs/` in the same PR.
- Small, focused commits with plain messages; no AI co-author or attribution
  trailers.
- Don't commit build output, logs, local reports, `.agent_tools/` or
  `.nrf_env.sh`. `hardware/` submodule pointers are often dirty locally; leave
  them out of commits unless the change is about hardware.
- Push only when asked.
