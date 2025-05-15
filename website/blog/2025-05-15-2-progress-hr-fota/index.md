---
slug: progress-hr-fota
title: Health PCB and Firmware Update
authors: [jakkra, kampi]
tags: [zswatch]
---
import FotaScreenshot from './fota.png';
import HrPCB from './hr.jpg';
import HrBLE from './hr_ble.jpg';

import SubscriptionForm from '@site/src/components/SubscriptionForm';

It's now been two months since our last update and a new one is very much due!

We’ve hit two major milestones:
- A prototype PCB for health tracking
- Firmware updates over USB and BLE

<img src={HrPCB} />

Let’s dive into the details....
<!-- truncate -->
### Health Tracking Prototype
We’ve completed the design and assembly of a prototype board to validate the future health-tracking add-on PCB for ZSWatch. This board helps us test components, write drivers, and troubleshoot more easily before moving to a final compact version.

To simplify testing, we integrated an **nRF54L15 BLE MCU**, allowing the board to operate as a standalone unit.

The cool part? It can stream heart rate data live to the ZSWatch over Bluetooth! Below is a quick demo of heart rate data being plotted in real time. It's still very much a hacky prototype—but it works.

<img src={HrBLE} />

Check out the in-progress driver and app here:  
[https://github.com/ZSWatch/Zephyr-MAX32664C](https://github.com/ZSWatch/Zephyr-MAX32664C)

### Firmware Updates via BLE and USB
We’ve added a bootloader, which means you can now update the ZSWatch firmware wirelessly via BLE—or through USB (in case ZSWatch firmware breaks and Bluetooth isn’t available.).

Even better: you can update directly from the [Firmware Update page on zswatch.dev](https://zswatch.dev/update).  
Firmware artifacts are pulled from GitHub Actions automatically, making updates simple.

We also added support for uploading the full **filesystem image**, which contains most UI assets.

<img src={FotaScreenshot} />

Improvements for later:
- Ideally, only real known working releases should show
- Bundle firmware and filesystem into a single update process

You can also update the firmware using [nRF Connect Device Manager](https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-Device-Manager) or other tools listed in the [Zephyr documentation](https://docs.zephyrproject.org/latest/services/device_mgmt/mcumgr.html#tools-libraries).


### Bonus (XIP)
Another smaller, but very important update is that we after some fiddling around got **XIP (Execute-in-Place)** working on ZSWatch, allowing code to be run directly from the external flash. It was essential to fit the bootloader and USB support, and it opens up a lot more headroom for the firmware going forward.

### Next up
- Evaluate the health tracking PCB’s performance
- Design a casing for wearing the health tracking PCB in order 

<br></br>

*Want to stay in the loop? Subscribe below for updates!*
<SubscriptionForm/>

[me]: https://github.com/jakkra
[Daniel]: https://github.com/kampi