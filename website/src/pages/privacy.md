---
title: Privacy Policy
description: Privacy Policy for ZSWatch Companion App
---

# Privacy Policy

**Last updated: October 23, 2026**

This Privacy Policy describes how ZSWatch Companion App ("we", "us", or "our") collects, uses, and shares information when you use our mobile application.

## Summary

- We do not run servers that collect your data. There are no accounts, analytics, ads or tracking.
- Your data stays on your phone and your watch. It only leaves them when a feature you use needs it, and only to the services listed below.

## Information the App Handles

- **Watch pairing**: The app stores your watch's Bluetooth identifier on your phone so it can reconnect.
- **Health and activity data**: Steps, heart rate, battery and similar data synced from your watch are stored on your phone only.
- **Notifications**: On Android, if you grant notification access, the app forwards your phone's notifications to your watch. On iOS, the watch reads notifications directly from the phone through Apple's notification service. Notifications are not stored or sent anywhere else.
- **Location**: If you allow it, the app shares your location with your watch. The watch uses it to request weather and altitude data from Open-Meteo, and the phone relays those requests, so your approximate coordinates are sent to Open-Meteo without any other identifier. Background location keeps weather and GPS working when the app is closed. The app does not keep a location history.
- **Voice memos**: Recordings made on your watch are copied to your phone and stored there. By default they are transcribed and summarized on your phone with on-device models. If you add your own OpenRouter API key and agree to the prompt in the app, the audio, the transcript and the text needed for summaries and Chat answers are sent to [OpenRouter](https://openrouter.ai/privacy), which passes them to the AI model provider you selected. You can delete recordings and transcripts in the app at any time.
- **Calendar and reminders**: If you allow it, the app adds the events, reminders, timers and alarms found in your voice memos. It does not upload your calendar.
- **Photos**: If you pick a photo as a watchface background, it is converted and sent to your watch. The app only gets the photo you pick.
- **Internet requests from the watch**: Some watch apps, such as weather and trivia, fetch data through the phone. The phone only relays these HTTPS requests and adds no personal data.

## Services the App Connects To

| Service | Why | What is sent |
|---------|-----|--------------|
| GitHub | Firmware updates | Nothing personal |
| Hugging Face | Downloading on-device AI models | Nothing personal |
| Open-Meteo | Weather and altitude on the watch | Approximate location |
| OpenRouter, and the AI model provider you select there | Only if you add your own key and agree | Voice memo audio and text, Chat questions |
| Open Trivia Database | Trivia app on the watch | Nothing personal |

Apart from the services above, we do not sell, trade or otherwise transfer your information to anyone.

## Permissions Explained

| Permission | Purpose |
|------------|---------|
| Bluetooth | Communicate with your ZSWatch |
| Location (including background) | Weather and GPS for your watch, also when the app is closed |
| Notifications | Timers, alarms and the connection status |
| Notification access (Android) | Forward phone notifications to your watch |
| Calendar and reminders | Add items found in voice memos |
| Photos | Pick a watchface background |
| Internet | Firmware and model downloads, weather, and optional OpenRouter |

## Data Storage and Deletion

All app data is stored on your phone. You can delete it with the delete options in the app, or by uninstalling the app.

This does not delete data already sent to the services above, such as voice memos sent to OpenRouter and your selected AI model provider. Those services keep and delete data according to their own privacy policies.

## Children's Privacy

The app is not directed at children under 13. We do not knowingly collect information from children.

## Changes to This Policy

We may update this Privacy Policy from time to time. Changes will be posted on this page with an updated revision date.

## Open Source

ZSWatch Companion App is open source. You can review the source code at:
- [https://github.com/ZSWatch/ZSWatch-App](https://github.com/ZSWatch/ZSWatch-App)

## Contact

If you have questions about this Privacy Policy, please open an issue on our GitHub repository or contact us at mail@zswatch.dev.

---

*This app is part of the ZSWatch open-source smartwatch project.*
