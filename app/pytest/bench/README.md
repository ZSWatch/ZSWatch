# Bench helpers

Shared building blocks for testing on a real watch, used by pytest and by
one-off investigation scripts. Run from `app/pytest` with the test venv, or
set `PYTHONPATH=app/pytest` for a script elsewhere.

Devices come from `../devices.yaml`. Desk setups are `bench_only` entries,
which pytest skips unless one is selected with `--board`. The tools pick a
device with `--device NAME` or `ZSW_DEVICE`.

| Module | What it does |
| --- | --- |
| `bench.ble` | One BLE connection with SMP (shell) and NUS (companion protocol) |
| `bench.dfu` | BLE firmware upload with hash skipping, reboot and confirmation |
| `bench.ppk` | PPK2 as the battery: a long-running session plus measurements |
| `bench.swd` | Read firmware variables by name from the ELF over J-Link |
| `bench.toolchain` | Run `west`, `nrfjprog` etc. with the environment from `.nrf_env.sh` (or `ZSW_NCS_TOOLCHAIN`) |

```bash
export ZSW_DEVICE=bench-watchdk

python -m bench.ble shell "power status" "hr status"
python -m bench.ble gb '{"t":"voice_memo","action":"list"}'
python -m bench.ble scan
python -m bench.ble pair
python -m bench.ble logs
python -m bench.dfu ../build/dfu_application.zip

python -m bench.ppk serve &          # keep running; supply stays on
python -m bench.ppk measure --seconds 60 --label idle --out results/
python -m bench.ppk cycle --off-seconds 3

python -m bench.swd --elf ../build/app/zephyr/zephyr.elf is_stationary motion_state

python -m bench.toolchain west build -b watchdk@1/nrf5340/cpuapp .. -d ../build
```

In a script:

```python
from bench.ble import WatchLink, watch_target
from bench.devices import get_device

device = get_device()
async with WatchLink(watch_target(device), disable_smp_on_exit=True) as watch:
    print(await watch.shell("battery"))
```

## BLE development on macOS and Linux

The BLE commands need Python 3.11+, `bleak`, `smpclient` and `PyYAML`. Install
them in a virtual environment (`python -m pip install bleak smpclient PyYAML`).
Linux uses BlueZ. On macOS, run the commands in Terminal and allow its Bluetooth
access when prompted; check **System Settings → Privacy & Security → Bluetooth**
if scanning fails. Restart Terminal after changing its permission.

Disconnect the phone first: the watch accepts one BLE connection at a time.
For the first computer connection, open **Settings → Bluetooth → Pairable** on
the watch, run `python -m bench.ble --device NAME pair`, and accept pairing on
the computer. Subsequent commands reuse that bond.

Linux selects the watch by its `mac` entry in `devices.yaml` (or
`devices_local.yaml` when present). macOS uses the advertised name
`ZSWatch-XX`, where `XX` is the last MAC byte. Set `ble_name` to override the
name when the watch advertises a different name. `scan` lists
advertised names and the addresses seen by this computer.

```bash
python -m bench.dfu --device bench-zswatch ../build/dfu_application.zip
python -m bench.ble --device bench-zswatch logs
```

DFU reads the zip's manifest by `image_index`: 0 is the internal app, 2 is
external/XIP, and 1 is the network core (included only with `--net`). It skips
images already running and reuses completed uploads. An interrupted upload
reconnects and resumes, up to five attempts per image. It refuses to proceed
while a voice recording is active, recording status is unavailable, or another
build is pending.

After checking every uploaded hash, it marks the images pending, reboots,
reconnects for up to four minutes, verifies every selected image booted, and
confirms the new images. `--no-confirm` leaves them for a test boot; the next
reboot reverts unconfirmed images. Rerunning the same zip also finishes
confirmation if a previous run stopped after boot. Use a package for the
correct board. This command installs firmware only; upload matching image
resources separately before firmware when the resources have changed.

`logs` prints the firmware's BLE log frames, reconnecting and enabling logs
again after a disconnect or reboot. Ctrl+C stops the command and disables BLE
logging if the watch is still connected. Debug firmware provides more logs
than release firmware. SMP stays enabled after DFU; use `bench.ble smp off`
to turn it off.

Offline checks (no hardware, pairing or flash access):

```bash
python -m unittest discover -s bench/tests -v
```

Things to keep in mind for power measurements:

- SMP keeps a fast connection interval. Disable it (`disable_smp_on_exit` or
  `bench.ble smp off`) and disconnect before a capture.
- Opening the PPK2 toggles the supply and reboots the watch. Keep `serve`
  running between captures instead.
- An SWD read powers the nRF53 debug domain until the next power cycle. Read
  after a capture, then `bench.ppk cycle` before the next one.
