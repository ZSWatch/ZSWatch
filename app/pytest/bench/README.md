# Bench helpers

Run from `app/pytest`. Select an entry from `devices.yaml` or
`devices_local.yaml` with `--device NAME` or `ZSW_DEVICE`.
Pair the watch with the computer beforehand and disconnect the phone.

```bash
export ZSW_DEVICE=bench-watchdk

python -m bench.ble scan
python -m bench.ble pair
python -m bench.ble shell "power status" "hr status"
python -m bench.ble gb '{"t":"voice_memo","action":"list"}'
python -m bench.ble logs                    # Ctrl+C to stop
python -m bench.ble smp off

python -m bench.dfu ../build/dfu_application.zip
python -m bench.dfu --net ../build/dfu_application.zip
python -m bench.dfu --no-confirm ../build/dfu_application.zip

python -m bench.ppk serve &                 # keep running; supply stays on
python -m bench.ppk measure --seconds 60 --label idle --out results/
python -m bench.ppk cycle --off-seconds 3

python -m bench.swd --elf ../build/app/zephyr/zephyr.elf is_stationary motion_state
python -m bench.toolchain west build -b watchdk@1/nrf5340/cpuapp .. -d ../build
```
