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

- **ZSWatch not appearing in scan:** Make sure **Pairable** is enabled on the watch (**Settings → Bluetooth**). Also ensure **Discover unsupported devices** is enabled and **Scanning intensity** is set to maximum in GadgetBridge settings.
- **Notifications not working:** Verify you selected **Bangle.js** as the device type when adding the watch. Also check that GadgetBridge has notification access permission on your phone (**Android Settings → Apps → GadgetBridge → Notifications**).
- **Weather not syncing:** If you are using the non-Bangle.js version of GadgetBridge, weather requires manual configuration. See the [GadgetBridge Weather Wiki](https://gadgetbridge.org/basics/features/weather/). The Bangle.js version handles weather automatically.
- **Connection drops frequently:** Disable battery optimization for GadgetBridge in Android settings so the OS doesn't kill it in the background.
- **Music control not working:** Ensure GadgetBridge has the notification listener permission, which is also required for media control on Android.

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

- **ZSWatch not appearing in nRF Connect scan:** Make sure **Pairable** is enabled on the watch (**Settings → Bluetooth**). Try moving the phone closer to the watch and ensure Bluetooth is enabled on the iPhone.
- **Pairing fails:** If a previous pairing exists, remove ZSWatch from **iPhone Settings → Bluetooth → My Devices** and try again. Also restart Bluetooth on the iPhone.
- **Notifications not forwarding:** After pairing, iOS should automatically authorize ANCS. If notifications don't appear, unpair and re-pair. Also make sure the apps you want notifications from have notifications enabled in **iPhone Settings → Notifications**.
- **Media control not working:** AMS (Apple Media Service) should work automatically after pairing. If it doesn't, try playing music first, then test the controls on the watch.
