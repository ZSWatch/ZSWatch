---
slug: intro
title: What is ZSWatch
authors: [jakkra]
tags: [zswatch, intro]
---
If you have not heard of ZSWatch before, here is an intro!
<!-- truncate -->
import SubscriptionForm from '@site/src/components/SubscriptionForm';
import ReactPlayer from 'react-player'
import HardwareOverview from '@site/static/img/Hardware_Overview_transparent.png'

import StainlessWatchStand from '@site/static/img/real_photos/stainless_stand.JPG'
import StainlessWatchFrontWide from '@site/static/img/real_photos/stainless_front_wide.JPG'
import ClearWatchFrontWide from '@site/static/img/real_photos/clear_front_wide.JPG'


Built on the [Zephyr™ Project](https://www.zephyrproject.org/) RTOS, hence the name **ZSWatch** - *Zephyr Smartwatch*.

ZSWatch is a free and open source smartwatch you can build almost from scratch - including software, hardware, and mechanics. Everything from the lowest level BLE radio driver code to PCB and casing is available and can be customised to suit your needs. 

<center>How ZSWatch looks like today.</center>
<img src={StainlessWatchFrontWide} />
<img src={ClearWatchFrontWide} />
<img src={StainlessWatchStand} /> 

<p></p>

<center>Video walking through some of the UI and features.</center>
<ReactPlayer width="100%" playing muted controls url='https://github.com/ZSWatch/ZSWatch/assets/4318648/ec1a94fd-a682-4559-9e68-f3e5bfcbe682' />

### Software
Short feature overview:
- Bluetooth LE communications with [GadgetBridge](https://codeberg.org/Freeyourgadget/Gadgetbridge) Android app.
- Multiple Watchfaces showing:
  - Standard stuff such as time, date, battery
  - Weather
  - Step count
  - Number of unread notifications
  - Environmental data
  - ...
- Pop-up notifications
- Application picker and app concept
  - Setting menu system, with easy extendability
  - Music control app
  - Settings app
  - Compass app
  - etc.
- Step counting
- Gestures
- iOS support (limited)
- And much more
...

### Hardware
ZSWatch is equipped with a range of powerful hardware components that make it a versatile and capable smartwatch.

<img src={HardwareOverview} />

<details>
    <summary>Components description</summary>
    - nRF5340 BLE chip ([u-blox NORA-B10 module](https://www.u-blox.com/en/product/nora-b1-series-open-cpu)): The watch is powered by a 128 MHz dual-core nRF5340 BLE chip, providing high-performance processing capabilities.
    
    - Nordic [nPM1300](https://docs.nordicsemi.com/category/npm1300-category) PMIC: The power management integrated circuit ensures efficient power delivery and battery life estimations.

    - 240x240 round display: ZSWatch features a vibrant and responsive 240x240 round display with a capacitive touch screen, ensuring a smooth and intuitive user experience.

    - IMU [Bosch BMI270](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi270-ds000.pdf): The integrated IMU allows for advanced features such as gesture-based navigation and smartwatch wake-up by arm movement.

    - Bosch [BME688](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme688/) Environmental sensor: The BME688 sensor provides AI-enhanced environmental data, enabling the watch to monitor air quality and other environmental parameters.

    - Bosch [BMP581](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp581/) High-performance pressure sensor: The BMP581 sensor offers accurate pressure measurements with a precision of approximately 20cm.

    - ST [LIS2MDLTR](https://www.st.com/resource/en/datasheet/lis2mdl.pdf) Magnetometer: The magnetometer enables the watch to detect magnetic fields, opening up possibilities for compass and navigation applications.

    - Macronix [MX25U51245GZ4I00](https://www.mouser.de/datasheet/2/819/MX25U51245G_2c_1_8V_2c_512Mb_2c_v1_4-3371129.pdf) 64 MB external flash: The external flash provides large storage space for data and UI resources.

    - Broadcom [APDS-9306-065](https://docs.broadcom.com/docs/AV02-4755EN) Light Sensor: The light sensor enables automatic brightness control.

    - Micro Crystal [RV-8263-C8](https://www.microcrystal.com/en/products/real-time-clock-rtc-modules/rv-8263-c8) RTC: The real-time clock module ensures accurate timekeeping and supports alarm functions.

    - Knowles [SPK0641HT4H-1](https://www.knowles.com/docs/default-source/model-downloads/spk0641ht4h-1-rev-a.pdf) I2S microphone: The built-in microphone allows for audio recording and voice control capabilities.


    Option to not mount sensors: ZSWatch offers the flexibility to exclude certain sensors, allowing for cost optimization based on specific requirements.

    This powerful combination of hardware components ensures that ZSWatch delivers a rich and immersive smartwatch experience.
</details>

<SubscriptionForm/>
