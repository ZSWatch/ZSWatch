# Watch Tools

The `/tools` page handles watchface backgrounds, live logs, Bluetooth traffic and crash reports, all processed locally in the browser. Background assets are copied from `ZSWatchApp/zswatch_app/assets/backgrounds`. Custom images are converted to the app's 240×240 LVGL v9 RGB565 format and uploaded over SMP to `/user/bg_new.bin`, then applied with `GB({"t":"watchface_bg","action":"apply"})`.

SMP is enabled only for transfers. After enabling it, the manager reconnects before subscribing, because re-registering the service invalidates the watch's notification subscription even when the browser still has a cached characteristic.

Logs use `GB({"t":"log","status":true})` and arrive as `<BLELOG>…</BLELOG>` frames on NUS. Crash dumps are read from `/user/coredump.txt` or `/lvgl_lfs/coredump.txt` and erased with the `coredump erase 0` shell command. Never send the coredump verify command, because on main it falls through to erase.

Run `npm run test:watch-tools` and `npm run build` from `website/`. The tests use mocked GATT, so also test uploads on a real watch.
