---
sidebar_position: 6
---

# Watch Tools

[Watch Tools](https://zswatch.dev/tools) lets you change the watchface background, capture live logs and download crash reports from a browser with Web Bluetooth. For the first connection, enable **Settings → Bluetooth → Pairable** on the watch, then click **Connect watch** and accept pairing.

## Background

Pick a background or open your own image, adjust the crop and click **Apply to watch**. Images are processed locally, and **Restore default background** returns to the firmware's default. Older firmware may need an [update](../firmware/firmware_updates.md) to support custom backgrounds.

## Live Logs

Open **Logs** and click **Start capture** to stream firmware logs, with filtering, search and export. The **Bluetooth traffic** view shows the raw NUS and SMP packets. Logs are kept in memory only, and file transfers stop the capture.

## Crash Reports

Open **Crash reports** and click **Check watch** to download the stored crash dump, or use **Import dump** to open a saved file. Up to 20 reports are kept in browser storage, so download the ones you want to keep. **Erase from watch…** removes the dump from the watch after confirmation.
