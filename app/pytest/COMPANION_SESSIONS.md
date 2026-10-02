# Companion sessions

Ordinary HIL fixtures recover/flash/reset the watch. `--skip-flash` still resets
it and is unsuitable for phone background/reconnection tests.

Use `--preserve-session` with an explicitly selected physical `--board` and
`--device-config /absolute/path/devices_local.yaml` (or the local override next
to conftest.py). Select one audited companion test module explicitly:

```bash
python -m pytest app/pytest/companion_session_probe.py \
  --board watchdk@1 --device-config /absolute/path/devices_local.yaml \
  --preserve-session -s --basetemp /tmp/zswatch-probe-001
```

Use a fresh basetemp directory; pytest clears it on startup. The probe requires
separate existing UART and USB SMP ports and installed pytest-asyncio/SMP
dependencies. It saves `app state` and `voice_memo status` responses there.
It does not connect over BLE or prove phone background feature acceptance.

```yaml
devices:
  watchdk@1:
    serial_port: /dev/serial/by-id/VERIFIED_UART
    usb_cdc_port: /dev/serial/by-id/VERIFIED_USB_SMP
```

Session mode disables fixture deployment, implicit resets and PPK2 power setup.
It rejects unmarked tests, firmware restore fixtures and PPK2 tests during
collection; companion probes without the session flag are also rejected before
hardware setup. Audited tests use `@pytest.mark.companion_session` and must not
reset, switch power, deploy firmware or take over phone BLE in their body.
This is a fixture safety policy, not a sandbox for arbitrary test code.

Separate deployment from observation. Failed ordinary deployment now produces
an error instead of a skip. Existing unrelated HIL tests retain their normal
deployment/reset behavior.
Unknown selected boards and empty device catalogs fail collection. A known board
without an optional capability (such as PPK2) instead produces an empty parameter
set for that test, which pytest skips or deselects. This preserves full-suite
`--board=native_sim -m linux_only` collection in CI.

Run fixture safety tests outside the hardware conftest tree:

```bash
python -m pytest app/pytest_unit/test_session_safety.py -q
```

These execute this conftest in temporary projects with fake deployment/power
helpers; no actual debugger, UART or phone is accessed. The companion application's
`scripts/companion_hil/README.md` documents preflight, collectors and a tested
Python environment for the USB probe. That environment currently uses
pytest 9.1.1, pytest-asyncio 1.4.0, PyYAML 6.0.3, pyserial 3.5 and
smpclient[ble,serial] 7.3.0.
