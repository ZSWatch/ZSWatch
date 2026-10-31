---
sidebar_position: 6
description: Set up the ZSWatch companion app for Android and iOS, sync voice notes, configure AI, customize your watchface, and update your watch.
---

# Companion App

The ZSWatch companion app connects your watch to **Android or iPhone** over Bluetooth. Use it to sync voice notes, get transcripts and AI answers, manage notifications, change your watchface background, view health data, and update firmware. The app is open source and built with Flutter.

## What you can do

| Feature | What it does |
| --- | --- |
| Voice Notes | Sync recordings from the watch, play them back, search transcripts, and organize notes. |
| Voice AI and Chat | Turn spoken notes into summaries and actions, or ask questions from the watch and read the answer on your wrist. |
| Notifications and music | Read phone notifications and control playback. Android also supports replies when the phone notification offers a reply action. |
| Health and analytics | View steps, heart rate, activity history, battery trends, and connection history. |
| Watchface backgrounds | Choose a preset or crop and upload your own image. |
| Firmware updates | Install releases, development builds, or local firmware packages and matching image resources. |
| Developer tools | Read logs, run shell commands, inspect sensors, browse watch files, and download crash dumps. |

:::info App and firmware versions
This guide covers the features in the current app source. Store releases can lag behind, and newer features also need compatible watch firmware. If a screen or feature is missing, check your app version in **Settings** and see [Firmware Updates](../firmware/firmware_updates.md) for updating the watch.
:::

## Download

### iOS

Install the app from the [App Store](https://apps.apple.com/us/app/zswatch/id6759058602).

### Android

Install the app from [Google Play](https://play.google.com/store/apps/details?id=com.zswatch.app). If Google Play says you do not have access to the testing release, contact us on [Discord](https://discord.gg/8XfNBmDfbY) or email [mail@zswatch.dev](mailto:mail@zswatch.dev) with your Google account email address.

You can also check [GitHub Releases](https://github.com/ZSWatch/ZSWatch-App/releases) for downloadable APKs.

:::tip Explore without a watch
Enable **Demo Mode** in **Settings** to explore the app with sample data.
:::

## Connect your watch

1. Turn on Bluetooth on your phone and keep the watch nearby.
2. On ZSWatch, open **Settings → Bluetooth → Pairable → Enable**.
3. Open the app. The welcome screen explains location, notifications, and voice setup. Use **Allow** for the features you want, then tap **Continue to watch setup**. You can revisit permissions in **Settings** later.
4. Open **Add Watch**, tap **Find watch**, and choose your ZSWatch. Depending on your phone, selection happens in a system accessory dialog or a list in the app.
5. Complete any pairing prompts and wait for the dashboard to show the connected watch.

Use **Settings → Manage watches** to reconnect to a saved watch, rename it, or add another. Disconnect the current watch before adding a different one.

### Permissions

| Permission | Used for |
| --- | --- |
| Bluetooth / nearby devices | Finding and connecting to the watch. |
| App notifications | The Android background connection indicator; timers, alarms, and app alerts on iOS. |
| Notification access (Android) | Reading phone notifications and forwarding them to the watch, including supported reply actions and media controls. |
| Location | Sharing the phone's location with the watch for weather and GPS features. For background use, follow the app's guidance to select **Allow all the time** on Android or **Always** on iOS. |
| Calendar / Reminders | Creating actions extracted from voice notes. Configure these in **Voice Memo AI** when you want to use them. |

On iPhone, also open **Settings → Bluetooth → your ZSWatch** and enable **Share System Notifications** after pairing. This is separate from allowing the companion app to show its own alerts.

## Voice notes, transcription, and AI

Record a memo on the watch and the app syncs it to your phone when connected. Open **Voice** on the dashboard, or **Settings → Voice notes**, to see your recordings.

In **Voice Notes** you can:

- Play recordings and view their processing status, transcript, summary, and extracted actions.
- Search titles, transcripts, and actions; searching the **Inbox** also finds finished notes.
- Rename notes, edit or copy transcripts, and export audio or text.
- Transcribe a recording without analysis, or retry transcription and analysis separately.
- Move notes to **Done**, select several notes for bulk actions, and restore notes from **Deleted**.

Deleted recordings stay on the phone for 30 days unless you permanently delete them sooner. Deleting a note from the phone leaves the watch's original recording and any calendar entries or scheduled alerts in place.

### Set up cloud processing

The default AI option uses **OpenRouter** for transcription and analysis on both Android and iOS.

1. Open **Settings → Voice memo models** to reach **Voice Memo AI**. You can also open this during first-run setup.
2. Create an [OpenRouter API key](https://openrouter.ai/keys), add credit to that account, and paste the key into the app.
3. Tap **Verify and save**, review the data-sharing prompt, and choose **Allow** to finish setup.
4. Choose the transcription and analysis models you want to use.
5. Check **Automatically process new recordings** and **Analyze after transcription**. Saving a verified key enables automatic processing and analysis; turn analysis off if you only want transcripts.

Automatic-processing preferences apply across your watches. Existing recordings are processed manually; use a recording's menu or **Process all unprocessed** to start them.

:::note Cloud processing and privacy
Cloud processing sends recordings, transcripts, dictated replies, and Chat questions to OpenRouter and the selected model provider. It needs an internet connection and uses your OpenRouter credit. The app requests zero-data-retention routing. Remove the key in the app to stop using it for cloud processing. See the [Privacy Policy](/privacy) for details.
:::

### Turn speech into actions

With analysis enabled, a memo can produce a summary and extract reminders, calendar events, timers, and alarms. For example, try “Set a timer for ten minutes” or “Remind me to call Alex tomorrow at nine.”

Results return to compatible watch firmware. Review the result and use **Undo** during the offered window if it was misunderstood. The app can automatically create timed actions after that window, and individual actions also offer review and creation controls in Voice Notes. Grant the relevant permissions under **Calendar Integration** first.

On Android, timers and alarms use the system Clock app, and calendar actions use the selected calendar. On iOS, events and reminders use Calendar and Reminders; timers and alarms are scheduled as companion app notifications, so allow app notifications for those alerts.

### Ask questions with Chat

Open **Chat** on the watch, record a question, and the companion app processes it through OpenRouter and sends the answer back to the watch. Follow-up questions use a short conversation history for that watch. This needs firmware with the Chat app, an internet connection, and a configured OpenRouter key.

### Experimental on-device processing

Enable **Experimental: on-device processing** in **Voice Memo AI** to use downloaded models instead of OpenRouter. Audio stays on the phone in this mode.

- **Android:** Whisper transcription and local model analysis are available. Download the selected speech and analysis models before enabling automatic processing.
- **iOS:** Local Whisper transcription is available. Local analysis, including extracting reminders and timers, is not supported.

Local processing can be slower and needs storage and memory for the models. Watch Chat uses OpenRouter.

## Notifications and music

### Android notifications and replies

Grant **Notification access**, then open **Notifications** from the dashboard or Settings. **Forward notifications** sends notifications from all apps by default; uncheck an app to block it.

For notifications that provide an Android reply action, you can reply from the watch. Dictated replies are transcribed and returned to the watch for review before you send them. Reply support depends on the phone app and the action it exposes; it is not available for every notification. Dictation needs a configured transcription option, but does not require memo analysis.

Music controls use the phone's active media session. Notification access also lets the app discover those sessions.

### iPhone notifications and music

The watch uses Apple's system services directly: **ANCS** for notifications and **AMS** for media controls. Enable **Share System Notifications** in the watch's iPhone Bluetooth settings. Android-style notification replies are not available through ANCS.

## Health, analytics, and customization

Open **Health** for daily steps, activity breakdowns, and heart-rate data, with day, week, and month views. Available readings depend on your watch hardware and enabled sensors.

**Analytics** shows battery history, drain estimates, connection uptime, and connection events. Leave the watch connected over time to build useful history.

Open **Watchface** to choose a preset background or select your own image, adjust the crop, and upload it. **Reset to Default** restores the firmware background. Background support depends on the installed watchface and firmware. You can also change backgrounds from the browser using [Watch Tools](./watch-tools.md).

The dashboard includes **Find watch** to help locate it nearby and **Restart Watch** when you need to reboot it.

## Weather, location, and internet access

Two settings control how the watch uses your phone:

- **Watch location** shares phone location for watch features such as weather and GPS. Follow the location-permission guidance in the app for background use.
- **Watch internet requests** lets the watch fetch online information through the phone, including requests used by weather and other watch apps.

These features need a connected watch. Online requests also need an internet connection on the phone.

## Firmware updates

Tap **Update Firmware** on the dashboard. Choose an official release, a CI development build, or a local package. The app filters builds for your watch hardware; check the selected hardware and display variant before starting.

The app supports both firmware and **LVGL resources** (icons and graphics). Use matching resources when a firmware update changes them. Keep the watch nearby and wait for the update and post-restart verification to finish.

See [Firmware Updates](../firmware/firmware_updates.md) for package contents, resource requirements, and recovery options. Development builds may contain unfinished changes.

## Developer tools

Open **Developer** on the dashboard for:

- **Log Viewer** and **Comm Log:** firmware logs and Bluetooth protocol traffic.
- **Shell:** commands over the watch's management connection.
- **Sensors:** live sensor data and a 3D orientation view from sensor fusion.
- **File System:** browse and download watch files.
- **Crash Reports:** inspect crash history, download coredumps, and export them for debugging.

Sensor and logging availability depends on the firmware build. For tools that run without installing the phone app, see [Watch Tools](./watch-tools.md).

## Background connection and troubleshooting

The connection can continue in the background on Android and iOS while you use other apps. The app attempts to reconnect when the watch returns to range. **Disconnect** deliberately stops that connection; select the saved watch and connect again to resume.

### Watch not found or not reconnecting

Check Bluetooth on the phone, keep the watch nearby, and enable **Pairable** on the watch when adding it. Review the Bluetooth permission in app Settings. If you deliberately disconnected, reconnect through **Manage watches**.

On Android, open **Settings → App Permissions → Battery optimization** and allow background operation in your phone's battery settings. Phone-specific power saving can still interrupt background work. Android's ongoing app notification shows the connection state.

### Notifications missing

On Android, check **Notification access**, **Forward notifications**, and whether the source app is unchecked. On iPhone, check **Share System Notifications** in Bluetooth settings.

### Voice note waiting for processing

Check the status in **Voice Notes**. For cloud jobs, check internet access, your OpenRouter key, and credit. For local jobs, check that the selected models are downloaded. A note already synced before automatic processing was enabled must be processed manually.

If a problem persists, report it in the [app issue tracker](https://github.com/ZSWatch/ZSWatch-App/issues) with your phone model, OS version, app version, watch firmware, and steps to reproduce it. [Gadgetbridge](./phone_setup.md#option-1-gadgetbridge-recommended) is also available as an alternative Android companion.

## Building from source

The [app repository](https://github.com/ZSWatch/ZSWatch-App) contains the Flutter app, native Android/iOS companions, and build instructions. Use a Flutter SDK that satisfies `zswatch_app/pubspec.yaml`; iOS builds also require macOS and Xcode.

```bash
git clone --recurse-submodules https://github.com/ZSWatch/ZSWatch-App.git
cd ZSWatch-App/zswatch_app
flutter pub get
dart run build_runner build --delete-conflicting-outputs
flutter run
```

See the repository for platform setup and release builds. Bug reports and contributions are welcome on [GitHub](https://github.com/ZSWatch/ZSWatch-App) and [Discord](https://discord.gg/8XfNBmDfbY).
