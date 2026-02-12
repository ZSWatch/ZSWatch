---
sidebar_position: 5
---

# Phone Setup

ZSWatch supports both Android and iOS. The setup differs depending on your platform.

## Android

### Option 1: GadgetBridge (Recommended)

[GadgetBridge](https://gadgetbridge.org/) is a mature, open-source Android app that works with ZSWatch and many other smartwatches.

#### Install GadgetBridge

Download GadgetBridge (Bangle.js version) from:
- [Google Play Store](https://play.google.com/store/apps/details?id=com.espruino.gadgetbridge.banglejs)

Not recommended: Original Gadgetbridge also works, but then internet access from the watch is not supported.
- [Official project page](https://gadgetbridge.org/)
- [F-Droid](https://f-droid.org/packages/nodomain.freeyourgadget.gadgetbridge/)

#### Add the Watch

1. On ZSWatch, go to **Settings → Bluetooth** and enable **Pairable**.
2. Open **GadgetBridge** on your phone.
3. Go to **Settings → Discover and pair options**.
4. Enable **Discover unsupported devices** and set **Scanning intensity** to maximum.
5. Go back to the main screen and press the **+** (plus) button.
6. GadgetBridge will scan. You should see a device called **ZSWatch**.
7. **Long press** on it.
8. In the dropdown, select **Bangle.js** as the device type and press **OK**.

#### Pair

1. Tap the newly added device in GadgetBridge to start pairing.
2. A Popup should be seen on ZSWatch that it was paired.
3. The watch icon in GadgetBridge should now indicate ZSWatch is connected.

##### Weather
If you are not using the Bangle.js version of GadgetBridge, you need to configure weather manually; follow the GadgetBridge Weather Wiki for setup.
[GadgetBridge Weather Wiki](https://gadgetbridge.org/basics/features/weather/)

#### Troubleshooting
TBD

---

### Option 2: ZSWatch Companion App (Experimental)

:::warning Experimental
The ZSWatch Companion App is under active development and may be unstable. GadgetBridge (Option 1) is recommended for most users. Use the companion app only for testing.
:::

The [ZSWatch Companion App](./companion_app.md) is an open-source Flutter app built specifically for ZSWatch. It provides notifications, music control, firmware updates, health tracking, and developer tools, all in one app.

Download the latest APK from [GitHub Releases](https://github.com/ZSWatch/ZSWatch-App/releases) or see the [Companion App](./companion_app.md) page for full details and setup instructions.

---

## iOS

### ZSWatch Companion App (Experimental)

:::warning Experimental
The iOS companion app is under active development. On iOS, the watch can directly use ANCS/AMS for notifications and media control without any app, which is the recommended approach for most users.
:::

The [ZSWatch Companion App](./companion_app.md) also runs on iOS, providing firmware updates, health tracking, developer tools, and more. On iOS, notification forwarding and media control are handled automatically by the watch using Apple ANCS/AMS, no app needed for those features.

Currently there is no prebuilt iOS app available. You'll need to build from source, see the [Companion App](./companion_app.md) page.

---

### Pairing (without the companion app)

On iOS, ZSWatch uses Apple's [ANCS](https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleNotificationCenterServiceSpecification/Specification/Specification.html) (notifications) and [AMS](https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleMediaService_Reference/Specification/Specification.html) (media control) services directly. No companion app is required.

#### Pair

1. On ZSWatch, go to **Settings → Bluetooth** and enable **Pairable**.
2. On your iPhone, open the **nRF Connect** app (free, from the App Store).
3. Scan for devices and connect to **ZSWatch**.
4. Press Bond in the **nRF Connect** app.
5. Accept the pairing request on both devices.

Once paired, notifications and media control work automatically through the system BLE services.

:::note
We are working on making ZSWatch appear directly in the iOS Bluetooth settings so that nRF Connect will not be needed for pairing in the future.
:::

#### Troubleshooting
TBD
