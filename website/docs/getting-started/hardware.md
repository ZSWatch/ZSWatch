---
sidebar_position: 2
---

# Hardware Overview

ZSWatch is built around the **Nordic nRF5340** SoC (dual-core ARM Cortex-M33, 128 MHz, 512 KB RAM, 1 MB Flash), packaged in a **u-blox NORA-B10** module.

## Key Components

| Component | Part | Function |
|-----------|------|----------|
| **SoC** | nRF5340 (NORA-B10) | BLE, application processing |
| **Display** | 1.28" 240×240 round IPS | Capacitive touch, GC9A01 driver |
| **IMU** | Bosch BMI270 | Accelerometer, gyroscope, gestures, step counting |
| **Pressure** | Bosch BMP581 | Barometric pressure (~20 cm altitude accuracy) |
| **Magnetometer** | ST LIS2MDL | Compass heading |
| **Light sensor** | Broadcom APDS-9306 | Ambient light for auto-brightness |
| **External flash** | Macronix MX25U51245G | 64 MB QSPI flash (images, XIP code) |
| **RTC** | Micro Crystal RV-8263 | Timekeeping and alarms |
| **Microphone** | Knowles SPK0641HT4H | I2S digital microphone |
| **PMIC** | Nordic nPM1300 | Battery charging, power management |

## Hardware Variants

| Variant | Description |
|---------|-------------|
| **WatchDK** | Development kit. Larger PCB with breakout headers, debug header, USB-C. Designed for development. |
| **ZSWatch (watch form factor)** | Compact 38mm round PCB for wearable use. (new revision in development based on WatchDK) |

For PCB design files and schematics, see:
- [ZSWatch Hardware repo](https://github.com/ZSWatch/Watch-HW)
- [Watch DevKit Hardware repo](https://github.com/ZSWatch/Watch-DevKit-HW)
- [Dock Hardware repo](https://github.com/ZSWatch/Dock-HW)
- [Extension Hardware repo](https://github.com/ZSWatch/Extension-HW)
