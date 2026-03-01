---
sidebar_position: 6
---

# Companion App

:::info
The **iOS** companion app is available on the [App Store](https://apps.apple.com/us/app/zswatch/id6759058602) and is the recommended way to use ZSWatch with an iPhone. The **Android** version is experimental, to get access, reach out on [Discord](https://discord.gg/8XfNBmDfbY) or email [mail@zswatch.dev](mailto:mail@zswatch.dev). For Android, [GadgetBridge](./phone_setup.md#option-1-gadgetbridge-recommended) remains a solid alternative.
:::

ZSWatch has an open-source companion app built with Flutter that works on both **Android** and **iOS**.

## Overview

The companion app connects to ZSWatch over BLE and replaces GadgetBridge on Android while adding features that aren't available through standard iOS ANCS/AMS services.

<div style={{textAlign: 'center', marginBottom: '2rem'}}>
  <iframe
    src="https://player.vimeo.com/video/1143868178?title=0&byline=0&portrait=0"
    width="640"
    height="360"
    frameBorder="0"
    allow="autoplay; fullscreen; picture-in-picture"
    allowFullScreen
    style={{maxWidth: '100%', borderRadius: '8px'}}
  />
</div>

## Features

| Feature | Android | iOS |
|---------|:-------:|:---:|
| BLE connection & auto-reconnect | ✅ | ✅ |
| Notification forwarding | ✅ | — (uses ANCS) |
| Music control | ✅ | — (uses AMS) |
| Firmware update (DFU) | ✅ | ✅ |
| LVGL resource upload | ✅ | ✅ |
| Health data (steps, heart rate) | ✅ | ✅ |
| Battery & connection analytics | ✅ | ✅ |
| GPS location relay | ✅ | ✅ |
| Weather sync | ✅ | ✅ |
| HTTP proxy for watch | ✅ | ✅ |
| Developer tools (logs, sensors) | ✅ | ✅ |
| Background BLE connection | ✅ | ✅ |

:::note iOS
On iOS, notification forwarding and media control are handled natively by the watch using Apple [ANCS](https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleNotificationCenterServiceSpecification/Specification/Specification.html) and [AMS](https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleMediaService_Reference/Specification/Specification.html) services — no app involvement needed for those features.
:::

## Download

### iOS (Recommended)

Download the official ZSWatch companion app from the [App Store](https://apps.apple.com/us/app/zswatch/id6759058602).

After installing:
1. Open the app and grant the requested permissions (Bluetooth, Location).
2. On ZSWatch, go to **Settings → Bluetooth** and enable **Pairable**.
3. Tap **Scan** in the app to find your watch.
4. Tap the watch to connect and pair.

### Android (Experimental)

:::warning Access Required
The Android companion app is experimental and currently requires an invite. To request access, reach out on [Discord](https://discord.gg/8XfNBmDfbY) or email [mail@zswatch.dev](mailto:mail@zswatch.dev). For a stable experience, [GadgetBridge](./phone_setup.md#option-1-gadgetbridge-recommended) is recommended.
:::

After receiving access:
1. Open the app and grant the requested permissions (Bluetooth, Location, Notifications).
2. On ZSWatch, go to **Settings → Bluetooth** and enable **Pairable**.
3. Tap **Scan** in the app to find your watch.
4. Tap the watch to connect and pair.

## Building from Source

The app is a standard Flutter project. You need:

- [Flutter SDK](https://docs.flutter.dev/get-started/install) 3.10+ (stable channel)
- [Android Studio](https://developer.android.com/studio) for Android builds
- [Xcode](https://developer.apple.com/xcode/) 15+ for iOS builds (macOS only)

```bash
# Clone with submodules (includes MCUmgr fork)
git clone --recurse-submodules https://github.com/ZSWatch/ZSWatch-App.git

# Or if already cloned without submodules:
# git submodule update --init

cd ZSWatch-App/zswatch_app

# Install dependencies
flutter pub get

# Generate code (database, state management)
dart run build_runner build --delete-conflicting-outputs

# Run in debug mode on connected device
flutter run

# Build release APK (Android)
flutter build apk --release
```

For more details, see the [full README](https://github.com/ZSWatch/ZSWatch-App).

## Source Code

- **App repository**: [github.com/ZSWatch/ZSWatch-App](https://github.com/ZSWatch/ZSWatch-App)
- **MCUmgr fork** (DFU plugin): [github.com/ZSWatch/Flutter-nRF-Connect-Device-Manager](https://github.com/ZSWatch/Flutter-nRF-Connect-Device-Manager)
