---
sidebar_position: 3
---

# Software Overview

ZSWatch runs on the [Zephyr RTOS](https://www.zephyrproject.org/) using the [nRF Connect SDK (NCS) v3.1.0](https://docs.nordicsemi.com/bundle/ncs-3.1.0/page/nrf/index.html). The UI is built with [LVGL v9](https://lvgl.io/).

## Key Software Features

- **Bluetooth LE** communication with [GadgetBridge](https://gadgetbridge.org/) (Android) and Apple ANCS/AMS (iOS)
- **Multiple watchfaces** with real-time data (time, weather, steps, battery, notifications)
- **App framework** with self-registering applications and an app picker with folder support
- **Sensor fusion** for compass heading and activity detection
- **Step counting** and gesture recognition (wrist tilt wake, etc.)
- **Firmware updates** over BLE and USB (via MCUboot + MCUmgr)
- **Pop-up notifications** from your phone
- **Music control** over BLE
- **Native Linux simulator** for development without hardware

## Built-in Apps

Compass, Music Control, Stopwatch, Timer, Calculator, Weather, Notifications, Settings, Fitness tracking, Flashlight, Sensors Summary, and more.

## Learn More

- [Architecture Overview](../development/architecture.md) - system design, event bus, data flows
- [Writing Apps](../development/writing_apps.md) - create your own watch apps
- [Compiling the Software](../development/compiling.md) - build from source