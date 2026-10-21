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
| `bench.ppk` | PPK2 as the battery: a long-running session plus measurements |
| `bench.swd` | Read firmware variables by name from the ELF over J-Link |
| `bench.toolchain` | Run `west`, `nrfjprog` etc. with the environment from `.nrf_env.sh` (or `ZSW_NCS_TOOLCHAIN`) |

```bash
export ZSW_DEVICE=bench-watchdk

python -m bench.ble shell "power status" "hr status"
python -m bench.ble gb '{"t":"voice_memo","action":"list"}'

python -m bench.ppk serve &          # keep running; supply stays on
python -m bench.ppk measure --seconds 60 --label idle --out results/
python -m bench.ppk cycle --off-seconds 3

python -m bench.swd --elf ../build/app/zephyr/zephyr.elf is_stationary motion_state

python -m bench.toolchain west build -b watchdk@1/nrf5340/cpuapp .. -d ../build
```

In a script:

```python
from bench.ble import WatchLink
from bench.devices import get_device

device = get_device()
async with WatchLink(device["mac"], disable_smp_on_exit=True) as watch:
    print(await watch.shell("battery"))
```

Things to keep in mind for power measurements:

- SMP keeps a fast connection interval. Disable it (`disable_smp_on_exit` or
  `bench.ble smp off`) and disconnect before a capture.
- Opening the PPK2 toggles the supply and reboots the watch. Keep `serve`
  running between captures instead.
- An SWD read powers the nRF53 debug domain until the next power cycle. Read
  after a capture, then `bench.ppk cycle` before the next one.
