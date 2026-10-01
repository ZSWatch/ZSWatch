# Watch Tools

The `/tools` page includes watchface backgrounds, live logs, Bluetooth traffic,
and locally stored crash reports. The watchface flow provides:
33 built-in backgrounds, custom image crop/zoom/position, clock preview, local
LVGL download, BLE upload, cancellation and restore-default commands.

Custom images are converted locally to the app's 240×240 LVGL v9 RGB565 format:
a 12-byte little-endian header, 480-byte stride and black pixels outside the
120-pixel radius. Transparent pixels are composited onto black.

Background assets and categories are copied from
`ZSWatchApp/zswatch_app/assets/backgrounds` and
`lib/providers/watchface_background_provider.dart`.

The shared updater transport enables SMP and rediscovers GATT services when
needed. Filesystem upload targets `/user/bg_new.bin` rather than `/S/full_fs`.
Only after all bytes are acknowledged does it send
`GB({"t":"watchface_bg","action":"apply"})` and await `apply_result` with
`ok: true`. Restoring uses `action: "reset"` and awaits `reset_result`.
No firmware images are uploaded and no reboot command is sent.

SMP is released while editing and after transfer. Cleanup checks the connection
generation so a stale operation cannot disable a new connection's SMP service.
Before each transfer, reconnect after enabling SMP: unregistering and recreating
the service invalidates the watch's notification subscription even when the
client still exposes a cached characteristic.

Run `npm run test:watch-tools` and `npm run build` from `website/`.
The tests validate every imported binary, image encoding/crop geometry and
mocked GATT protocol behavior, including fragmented responses, cancellation,
watch rejection, timeouts and disconnect cleanup.

BLE requires a compatible browser and first-time pairing. In browsers without
Web Bluetooth, background previews, cropping and downloads remain usable.
Browser UI tests do not substitute for testing an actual upload on a watch.

On 2026-10-01, the production manager was tested against a real WatchDK running
0.8.1 through a Web Bluetooth API adapter backed by Bleak/BlueZ. Hardware and
battery detection, automatic SMP enable, acknowledged upload of all 115,212
bytes, `apply_result`, byte-for-byte file readback, and `reset_result` passed.
The default background was restored, its custom file was confirmed absent, and
SMP was disabled before disconnecting. UART logs were captured throughout the
upload and restore. This validates the manager and BLE protocol; rendered browser
interaction and screenshot review remain separate checks.

## Local diagnostics

NUS firmware logging uses `GB({"t":"log","status":true/false})`. There is no
acknowledgement on main, so capture stays pending until a complete
`<BLELOG>…</BLELOG>` frame arrives. The stream reader separates framed logs from
JSON messages and handles fragmented tags and UTF-8. Firmware logs cannot resolve
watchface command promises. Log buffers retain 2,000 lines, traffic retains 1,000
packets, and the UI renders the latest 500 filtered matches. Transfers stop live
capture before reconnecting to SMP.

Crash download probes `/user/coredump.txt` and `/lvgl_lfs/coredump.txt` via MCUmgr
filesystem stat/read, checking offsets, lengths and BEGIN/END markers. SMP v1
ENOENT and v2 filesystem FILE_NOT_FOUND mean an absent dump; unsupported commands,
timeouts, invalid replies and disconnects remain errors. File READ responses must
not enter the shared transport's legacy upload loop.

Legacy crash files have a 72-byte binary summary followed by Zephyr dump text.
Plain text dumps also import. Missing filenames and line 0 remain unavailable.
Browser history uses localStorage, retains up to 20 reports within 3 MB of
serialized storage, and deduplicates normalized text with SHA-256. Raw exports
preserve every byte. Firmware metadata describes the device at download time,
which may differ from the firmware at crash time.

Erase requires UI confirmation, re-downloads and compares the current dump with
the selected download, then invokes the supported `coredump erase 0` SMP shell
command and verifies file absence. Do not invoke firmware's coredump verification
command: main has a fallthrough from verify to erase. The website does not use it.

There is no analysis backend, ELF download, remote upload or backtrace decoding.
The companion app's server flow was an unsupported experiment. Saved filesystem
logs are excluded because tracked main firmware does not enable that backend.

On 2026-10-01, the production diagnostics manager captured a framed firmware log
from the attached WatchDK on 0.8.1, automatically stopped logging for transfer,
and downloaded the existing 1,036-byte legacy crash. Every byte matched the
earlier read-only capture; binary/text import and export matched too. SMP RX/TX
traffic was captured. Logging and SMP were disabled before disconnecting. No dump
was erased, no crash triggered, and no firmware flashed. Erase is covered by
protocol tests, including refusal when the dump has changed; real erase was not
tested because the stored crash was preserved.
