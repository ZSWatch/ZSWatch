# UTC and fractional timezone regression

The BLE time event now carries an explicit `tz_offset_valid` field inside the
firmware, so `E.setTimeZone(0)` resets a previously nonzero offset and time-only
Gadgetbridge/Chronos time-only events preserve the selected timezone. This field is
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

The 2026-10-14 iPhone check found that a successful hardware RTC startup skipped
restoring retained TZ. A new owned recording after a normal dashboard reboot was
about two hours ahead of UTC; after explicit companion sync, another recording
was within one second of its measured start. RTC startup now restores TZ before
choosing hardware or software time. Existing recording headers remain unchanged.

CTS Current Time fields are local time per the [Bluetooth specification](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/CTS_v1.0/out/en/index-en.html).
The client now reads fresh Local Time Information for each time update and sends
UTC plus the effective standard/DST offset in the same internal event. Unknown,
reserved or unavailable offsets leave the current clock intact, allowing the
companion's explicit UTC sync. Host tests cover signed quarter-hour standard
offsets, DST and rejected unknown/reserved values. Discovery retries now retain
the actual connection and cancel on disconnect. WatchDK@1 sysbuild passed on
NCS v3.3.0-preview1; physical acceptance of this correction is in progress.
