---
sidebar_position: 7
---

# GadgetBridge Protocol

ZSWatch communicates with Android phones using the [GadgetBridge](https://gadgetbridge.org/) app, pretending to be a **Bangle.js** device. This page gives a brief overview of the protocol for developers.

## Protocol Overview

GadgetBridge sends and receives **JSON messages** wrapped in a `GB(...)` envelope with a newline terminator:

```
GB({"t":"notify","id":123,"title":"Hello","body":"World"})\n
```

The BLE communication is implemented in `app/src/ble/gadgetbridge/ble_gadgetbridge.c`. Incoming messages are parsed and published to the `ble_comm_data_chan` zbus channel, where any app or manager can subscribe to them.

## Supported Message Types

### Phone → Watch (Inbound)

| Type | Description |
|------|-------------|
| **Notifications** | Title, body, sender, source app (id, actions) |
| **Time sync** | Unix timestamp and timezone offset |
| **Weather** | Temperature, humidity, weather code, wind, description |
| **Music info** | Artist, album, track name, duration, position |
| **Music state** | Play/pause, shuffle, repeat, playback position |
| **GPS data** | Latitude, longitude, altitude, speed |
| **HTTP responses** | Responses to HTTP requests made by the watch |

### Watch → Phone (Outbound)

| Type | Description |
|------|-------------|
| **Notification actions** | Dismiss, open, mute, reply |
| **Music control** | Play/pause, next, previous |
| **Activity data** | Steps, heart rate, activity type |
| **HTTP requests** | Requests forwarded through the phone's internet |

## GadgetBridge Version

ZSWatch is recommended to be used with the **Bangle.js version** of GadgetBridge, which supports internet access forwarding (HTTP proxy) from the watch through the phone. The original GadgetBridge also works, but without HTTP proxy support.

- [GadgetBridge Bangle.js on Google Play](https://play.google.com/store/apps/details?id=com.espruino.gadgetbridge.banglejs)

## Key Implementation Details

- **Max packet size:** 2000 bytes
- **Character encoding:** Handles GadgetBridge's non-standard UTF-16 encoding (converted to UTF-8 internally)
- **Data types** are defined in `app/src/ble/ble_comm.h` via the `ble_comm_data_type_t` enum

For phone setup instructions, see the [Phone Setup](../getting-started/phone_setup.md) guide. For the overall BLE architecture, see the [Architecture Overview](./architecture.md#ble-communication).
