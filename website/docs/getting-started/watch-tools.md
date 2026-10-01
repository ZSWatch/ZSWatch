---
sidebar_position: 6
---

# Watch Tools

Use [Watch Tools](https://zswatch.dev/tools) to change your ZSWatch watchface
background, capture live logs, and download crash reports from your computer. You can choose one of the companion app's
backgrounds or crop your own photo. The background changes without restarting
the watch.

## Connect Your Watch

1. Open Watch Tools in a browser with Web Bluetooth support.
2. For your first connection to the computer, open **Settings → Bluetooth** on
   the watch and enable **Pairable**.
3. Click **Connect watch**, select your watch, and accept pairing on the computer.

Bluetooth transfer mode is enabled automatically when needed. You do not need
to enable BLE in the watch's Update app.

## Choose a Background

- Select a background from the collection, or click **Choose image** to open a
  PNG, JPEG, WebP, GIF or BMP file up to 20 MB.
- For a custom image, drag the preview to position it and adjust the zoom.
  The position sliders provide another way to move the image.
- Turn **Show time** on or off to check how the background looks behind a clock.
  This only changes the preview.
- Click **Apply to watch** and keep the page open until the watch confirms the
  change. You can cancel during upload; the previous background stays active.

Images are processed locally and fitted to the watch's 240×240 round display.
You can also use **Download background file** without connecting a watch.

## Restore the Default

Click **Restore default background** to return to the background included with
your firmware.

If the watch does not confirm a change, check its connection and update to
firmware that supports custom watchface backgrounds. See
[Firmware Updates](../firmware/firmware_updates.md).

## Live Logs

Open **Logs** and click **Start capture**. The page waits for a framed log from
the watch before showing that it is receiving logs. A quiet watch may take time
to produce a message.

- Filter firmware logs by severity or search text. Filters cannot enable levels
  that were compiled out of the firmware.
- **Pause view** freezes the display while capture continues. **Auto-scroll**
  follows new messages. **Clear view** clears this page's buffer.
- **Copy filtered** and **Download text** export all matching buffered messages.
- **Bluetooth traffic** shows NUS and SMP packets with RX/TX filters and text or
  hex display, independently of firmware log capture.

The page keeps the most recent 2,000 firmware log lines and 1,000 Bluetooth
packets, and displays the latest 500 matches. File transfers stop firmware log
capture; click **Start capture** again afterward. Disconnecting stops capture.
Logs are kept in memory until you close or reload the page.

## Crash Reports

Open **Crash reports** and click **Check watch** to download its stored dump.
The tools support both `/lvgl_lfs/coredump.txt` on current main firmware and
`/user/coredump.txt` on newer firmware. Downloading leaves the watch's copy intact.

The report shows the saved timestamp and crash location when available. Hardware
and firmware details describe the watch at download time; the firmware may have
changed since the crash. Some crashes have no filename or line number.

Use **Download original** to preserve the exact downloaded file, or **Download
text** for its readable dump contents. **Import dump** opens either format locally
without connecting a watch. A report's raw dump is available under **Dump contents**.

Browser history keeps up to 20 unique reports within a storage limit. Repeated
downloads of the same dump count once. Location counts summarize these unique
saved reports, rather than the number of times a dump was downloaded. Browser
storage can be cleared or unavailable, so download reports you want to keep.
**Remove local copy** only removes a report from browser history.

After downloading, **Erase from watch…** asks for confirmation, checks that the
watch still has the same dump, and verifies that it was removed. The browser copy
is kept. Current firmware stores one crash dump on the watch.

All diagnostics are processed locally. There is no analysis server or automatic
backtrace decoding. Saved filesystem logs are not offered because current main
firmware does not enable that logging backend.
