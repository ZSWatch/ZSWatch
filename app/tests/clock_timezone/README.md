# UTC and fractional timezone regression

The BLE time event now carries an explicit `tz_offset_valid` field inside the
firmware, so `E.setTimeZone(0)` resets a previously nonzero offset and time-only
Gadgetbridge/Chronos/CTS events preserve the selected timezone. This field is
internal; existing BLE messages and SMP libraries/protocol are unchanged.

The formatter converts the phone's east-of-UTC offset to a POSIX `TZ` string
with hours and minutes, including half-hour and quarter-hour zones. It rejects
non-finite/out-of-range offsets and insufficient buffers. The main handler uses
a real `time_t` value rather than aliasing the 32-bit BLE seconds field. Existing
RTC instant-preserving adjustment remains in place.

Run the host-only regression without hardware/serial/firmware fixtures:

```bash
python3 -m unittest discover -s app/tests/clock_timezone -v
```

It compiles the actual formatter with strict warnings and checks libc local-time
results for UTC reset, positive/negative, half-hour, quarter-hour and date-boundary
offsets, plus invalid values/buffers. It does not validate the physical RTC.

On 2026-10-03 the existing native_sim and WatchDK configurations compiled:

```bash
source .nrf_env.sh
west build --build-dir app/build_pairing_audit
west build --build-dir app/build_pairing_watchdk
```

These are already configured local build directories. Fresh builds should use
the standard native_sim/WatchDK instructions and appropriate board/configuration.
No watch firmware was flashed, reset or recording deleted for this change.
Physical acceptance remains: nonzero→UTC, half/quarter-hour offsets, RTC-backed
time, reconnect, manual timezone change and DST on the phone/watch pair. The
Android native prototype also checks effective offset changes during its
maintenance tick, since DST can change offset without changing timezone ID.
