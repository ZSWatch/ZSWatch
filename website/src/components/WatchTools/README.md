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

Run `npm run test:watch-tools` and `npm run build` from `website/`.
The tests validate every imported binary, image encoding/crop geometry and
mocked GATT protocol behavior, including fragmented responses, cancellation,
watch rejection, timeouts and disconnect cleanup.

BLE requires a compatible browser and first-time pairing. In browsers without
Web Bluetooth, background previews, cropping and downloads remain usable.
Browser UI tests do not substitute for testing an actual upload on a watch.
