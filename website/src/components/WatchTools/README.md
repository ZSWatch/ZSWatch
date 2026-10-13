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

The standalone manager releases SMP while editing and after transfer. Cleanup
checks the connection generation so a stale operation cannot disable a new
connection's SMP service. The website's shared session keeps SMP enabled after
connecting for firmware operations. An idle visible updater or
tools page refreshes that enabled session every 60 seconds, ahead of firmware's
180-second timeout. Hidden pages stop refreshing it, and returning after a long
gap renews GATT subscriptions. Intentional SMP release after tools operations and
live log capture are respected; the heartbeat does not re-enable those sessions.
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
JSON messages and handles fragmented tags and UTF-8. A separate line reader
joins lines spanning multiple 256-byte firmware output frames, retaining severity
and removing ANSI colors after reassembly. Stopping flushes the last partial line;
clearing resets pending text. Unfinished lines are bounded to 16 KB. Capture startup
waits past main firmware’s three-second connection grace period. Firmware logs cannot resolve
watchface command promises. Log buffers retain 2,000 lines, traffic retains 1,000
packets, and the UI renders the latest 500 filtered matches. Transfers stop live
capture before reconnecting to SMP.

Crash download probes `/user/coredump.txt` and `/lvgl_lfs/coredump.txt` via MCUmgr
filesystem stat/read, checking offsets, lengths and BEGIN/END markers. SMP v1
ENOENT and v2 filesystem FILE_NOT_FOUND mean an absent dump; unsupported commands,
timeouts, invalid replies and disconnects remain errors. File READ responses must
not enter the shared transport's legacy upload loop.

Legacy crash files have a 72-byte binary summary followed by Zephyr dump text.
Plain text dumps are also supported when downloaded from the watch. Missing
filenames and line 0 remain unavailable. Crash reports are fetched from the
watch; there is no local file import. Previously downloaded reports can be viewed
in browser history without a connected watch.
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

The updated line reader was checked on the attached WatchDK on 2026-10-01:
starting capture immediately after connect received and assembled two firmware
log lines with parsed severity, and stopping capture returned to the stopped
state. This check used the production manager through the BlueZ adapter; it
did not flash firmware or alter the stored crash dump.


The watch session is owned by `src/theme/Root.js`, shared with Firmware Update.
Leaving Tools keeps the connection and diagnostics buffers alive. Firmware
operations stop log capture before enabling SMP; navigation is blocked during
Tools transfers/commands and firmware updates. The UI uses the shared site
palette and translucent dark surfaces from the firmware page.

Start logs / Stop logs toggle the watch’s BLE logging backend; Pause view only
freezes the display. Visiting Firmware Update no longer prepares SMP or stops
logging. File operations still stop logging to avoid concurrent protocol output.
NUS subscriptions are explicitly renewed on reconnect and before background
commands. Renewal clears incomplete frames; a new BLELOG opening tag also
resynchronizes a damaged log stream without trusting JSON within the damaged log.
Performance mode on the watch does not enable log levels compiled out of firmware.

Latest verification (2026-10-01): 67 firmware/tools/diagnostics tests and the
production build pass. The revised production manager received four real
firmware lines through BlueZ. A background upload received `apply_result` with
`ok: true`; its subsequent readback hit a GATT Database Out Of Sync rejection,
and cleanup restored the backed-up original background with an acknowledgement.
The explicit ATT rejection now has a bounded retry and regression coverage.
A repeat hardware run timed out before connecting. After reset, UART reported
`Unable to find bootable image` and serial recovery mode. Internal app flash
matches the earlier 1 MB backup byte for byte, including a valid image SHA-256.
No firmware was flashed. The final complete hardware rerun remains blocked by
that boot failure; do not describe it as passed.

Verification on 2026-10-02: 82 firmware/tools/diagnostics tests pass. Discovery
coverage includes existing grants, chooser fallback, optional advertisements,
revocation/obsolete responses and blocked storage. SMP tests cover idle refresh,
expired services, intentional disable, log capture, transfers and queued NUS
commands across disconnects. A temporary localhost browser harness exercised the
production connection hook against simulated hardware: page-load prefetch,
reuse of the saved device, the real 60-second timer, resume after expiry, and
suppression during logging passed. The harness was removed afterward. These
checks do not resolve the hardware boot failure or replace a real-watch retest.
