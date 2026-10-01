# Watch Tools

The `/tools` page implements the companion app's watchface background flow:
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
