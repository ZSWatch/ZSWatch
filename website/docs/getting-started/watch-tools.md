---
sidebar_position: 6
---

# Watch Tools

[Watch Tools](https://zswatch.dev/tools) lets you change the watchface background, capture live logs and download crash reports from a browser with Web Bluetooth. For the first connection, enable **Settings → Bluetooth → Pairable** on the watch, then click **Connect watch** and accept pairing.

The **Name filter** field filters the browser's device list by name prefix. For example, `ZSWatch` shows `ZSWatch AA:BB:CC` and other watches with identifiers after the name. Matching is case-sensitive. Clear the field to show all devices. **Connect watch** always opens the filtered device picker so you can choose which watch to connect.

The connection is shared with the firmware updater. Use the site links to switch between **FW Update** and **Watch Tools** without reconnecting. Reloading or closing the browser tab ends the session.

## Background

Pick a background or open your own image, adjust the crop and click **Apply to watch**. Images are processed locally, and **Restore default background** returns to the firmware's default. If the watch does not send a confirmation, Watch Tools checks the active background file before reporting success. Older firmware may need an [update](../firmware/firmware_updates.md) to support custom backgrounds.

## Live Logs

Open **Logs** and click **Start capture** to stream firmware logs, with filtering, search and export. The **Bluetooth traffic** view shows the raw NUS and SMP packets. Logs are kept in memory only, and file transfers stop the capture.

## Crash Reports

Open **Crash reports** and click **Check watch** to download the stored crash dump. Up to 20 reports are kept in browser storage, so download the ones you want to keep. **Erase from watch…** removes the dump from the watch after confirmation.
