<div align="center">
  <h1>ZSWatch</h1>

[![License](https://img.shields.io/badge/License-GPL%203.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![discord](https://img.shields.io/badge/chat-discord-blue?logo=discord&logoColor=white)](https://discord.gg/8XfNBmDfbY)

<p float="left">
<img src=".github/images/in_use.jpg" width="67.5%" object-fit="cover"/>
<img src=".github/images/v3_case.jpg" width="31%" object-fit="cover"/>
</p>
<sub>
  ZSWatch v3 CNC:ed Stainless Steel (left), Clear Resin 3D print (right)
</sub>
</div>
<br/>
<br/>


Smartwatch built from scratch, both hardware and software. Built on the [Zephyr™ Project](https://www.zephyrproject.org/) RTOS, hence the name **ZSWatch** - *Zephyr Smartwatch*.
<br/>

<kbd><img title="Overview" src=".github/images/many_in_row.jpg"/></kbd><br/>

<kbd><img title="Overview" src=".github/images/overview.jpg"/></kbd><br/>

**Synced remote control over BLE**

[https://user-images.githubusercontent.com/64562059/234390129-321d4f35-cb4b-45e8-89d9-20ae292f34fc.mp4](https://github.com/jakkra/ZSWatch/assets/4318648/8d0f40c2-d519-4db1-8634-b43caa502cbe)

<p align="center" >
  <a href="https://www.youtube.com/watch?v=MmCzV0jV9hs"><img width="55%" src=".github/images/presentation.png" ></a>
</p>
<p align="center">Watch my presentation at Zephyr Developer Summit 2023</p>


## Building or getting one
Head over to the hardware repos https://github.com/jakkra/ZSWatch-HW and https://github.com/jakkra/ZSWatch-Dock for information about ordering the PCBs and assembly from PCBWay.

Some things are still in progress:
- Documentation such as building instructions.
- Dock casing.

**I'll also build a few initial kits** (assembled) for those who don't want or can build ZSWatch themselves.<br/>
In addition to assembled ZSWatch and dock you will get the following compared to if you build it yourself:
- **A magnetic dock connector and cable**. I have got custom ordered cables to fit the needs of ZSWatch.
- **Dock with onboard SEGGER J-Link OB debgger**, which means you won't need an external debugger for ZSWatch development and flashing.
- **Possibly CNC:ed casing in Stainless steel** (in progress).

If you are interested in a kit, or want to get notified when the missing parts above are resolved, fill in your **[mail here (Google form)](https://forms.gle/G48Sm5zDe9aCaYtT9)** and I'll send a reminder when it's ready.

**Or** if you want to get notified for releases and when the missing parts are finished simply press the `Watch` button (next to Fork and Star) -> `Custom -> Releases` and you will see in your feed when it's officially released.
<br/>
<br/>

# Table of content ZSWatch
- [Building or getting one](#building-or-getting-one)
- [Hardware features](#hardware-features)
- [Charger/Dock](#chargerdock)
- [Enclosure/Casing](#enclosurecasing)
- [Software Features](#software-features)
   * [Features and progress](#features-and-progress)
- [Watchfaces](#watchfaces)
- [Smartphone communication](#smartphone-communication)
  - [Android phone communication](#android-phone-communication)
  - [iOS device communication](#ios-device-communication)
- [PCB](#pcb)
- [ZSWatch v1 in action (Note old, not updated for latest HW and SW).](#zswatch-v1-in-action-note-old-not-updated-for-latest-hw-and-sw)
- [Environment, Compiling and running the code](#environment-compiling-and-running-the-code)
- [Writing apps for the Application Manager](#writing-apps-for-the-application-manager)
- [Other tools](#other-tools)
- [Licence GPL-3.0](#licence-gpl-30)
- [Thanks](#thanks)


## Hardware features
- nRF5340 BLE chip (u-blox NORA-B10 module).
  - 128 MHz Dual core.
  - 512 KB RAM.
  - 1 MB Flash.
  - 30 MHz SPI for display.
- [240x240 round display](https://www.buydisplay.com/240x240-round-ips-tft-lcd-display-1-28-inch-capactive-touch-circle-screen) with touch screen.
- IMU [Bosch BMI270](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi270-ds000.pdf), with this one it's possible to do many fancy things such as navigation using gestures and the typical smartwatch wakeup by moving the arm so the display is viewable.
- Bosch [BME688](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme688/) Environmental sensor with AI.
- Bosch [BMP581](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp581/) High performance pressure sensor accuracy in units of ~20cm's.
- ST [LIS2MDLTR](https://www.st.com/resource/en/datasheet/lis2mdl.pdf) Magnetometer.
- Renesas [AT25SL128A](https://eu.mouser.com/datasheet/2/698/REN_DS_AT25SL128A_109S_032023_DST_20230329-3076025.pdf) 16 MB external flash.
- Broadcom [APDS-9306-065](https://docs.broadcom.com/docs/AV02-4755EN) Light Sensor for automatic brightness control.
- Option to not mount some sensors to save BOM cost.

## Charger/Dock
Option with and without onboard SEGGER J-Link OB debugger.<br>
As the debugger requires a license, this will only be availible as part of a kit.

See more at https://github.com/jakkra/ZSWatch-Dock
<p float="left">
<img src=".github/images/dock.jpg" width="48%" object-fit="cover"/>
<img src=".github/images/dock_connector.jpg" width="49%" object-fit="cover"/>
</p>

## Enclosure/Casing
3D printed casing with 3D printed buttons, option CNC:able casing in metal.

## Software Features
- Bluetooth LE communications with [GadgetBridge](https://codeberg.org/Freeyourgadget/Gadgetbridge) Android app.
- Also support Bluetooth Direction Finding so the watch can act as a tag and is trackable using any [u-blox AoA antenna board](https://www.u-blox.com/en/product/ant-b10-antenna-board)
- Multiple Watchfaces showing:
   - Standard stuff as time, date, battery
   - Weather
   - Step count
   - Number unread notifications
   - Environmental data
   - ...
- Pop-up notifications
- [Application picker and app concept](#writing-apps-for-the-application-manager)
   - [Setting menu system, with easy extendability](app/src/applications/settings/)
   - [Music control app](app/src/applications/music_control/)
   - [Settings app](app/src/applications/settings/)
   - [Compass app](app/src/applications/compass/)
   - etc.
- Step counting
- Gestures
- And much more
...

### Features and progress
There are almost endless of posiblities for features that could be implemented, see [here for full progress](https://github.com/users/jakkra/projects/1) and in GitHub issues.

## Watchfaces
Alternative watch faces can be chosen by selecting one or many of the appropriate Kconfig. <br>
Swiping left to right will change the watchface.
for something else, and can then be disabled.

`CONFIG_WATCHFACE_ANALOG=n`<br>
`CONFIG_WATCHFACE_DIGITAL=y`<br>
`CONFIG_WATCHFACE_MINIMAL=y`<br>


To select watchface background see below config in prj.conf:

`CONFIG_WATCHFACE_BACKGROUND_SPACE=y`<br>
`CONFIG_WATCHFACE_BACKGROUND_FLOWER=n`<br>
`CONFIG_WATCHFACE_BACKGROUND_PLANET=n`<br>
`CONFIG_WATCHFACE_BACKGROUND_NONE=n`<br>

<p float="center">
<img src=".github/images/watchfaces.png" width="99%"/>
</p>

## Smartphone communication

### Android phone communication
Fortunately there is a great Android app called [GadgetBridge](https://codeberg.org/Freeyourgadget) which handles everything needed on the phone side, such as notifications management, music control and so much more... The ZSWatch right now pretends to be one of the supported Smart Watches in Gadgetbridge, following the same API as it does. In future there may be a point adding native support, we'll see.

#### Pairing
- In the watch go to Settings -> Bluetooth -> Enable pairing
- Now go reconnect to the watch from Gadgetbridge app.
- You should now be paired.

### iOS device communication
Apple exposes [Apple Notification Center Service](https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleNotificationCenterServiceSpecification/Specification/Specification.html) GATT server which handles notifications management on the phone side, music control is done in the same fashion using [Apple Media Service](https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleMediaService_Reference/Specification/Specification.html)... The ZSWatch communicates straight to the iOS with no extra Apps.

#### Pairing
- In the watch go to Settings -> Bluetooth -> Enable pairing
- Now go to your device settings -> Bluetooth and choose "ZSWatch"
- You should be prompted to pair and allow share notifications

## PCB
A 4 layer board which measures 38mm in diameter designed in KiCad.<br>
More info here: https://github.com/jakkra/ZSWatch-HW

<p float="left">
<img src=".github/images/pcb.jpg" width="83%" object-fit="cover"/>
</p>
<p float="left">
<img src=".github/images/front_pcb_render.png" width="43%" object-fit="cover"/>
<img src=".github/images/back_pcb_render.png" width="40%" object-fit="cover"/>
</p>

## ZSWatch v1 in action (Note old, not updated for latest HW and SW).
|*Music control*|*Accelerometer for step count and tap detection*|
|---|---|
|  <img src=".github/images/music.gif" object-fit="cover" /> |  <img src=".github/images/accel.gif" object-fit="cover" /> |
|*Notifications from phone (Gmail here)*|*Settings*|
|  <img src=".github/images/notifications.gif" object-fit="cover" />    |  <img src=".github/images/settings.gif" object-fit="cover"/> |


https://github.com/jakkra/ZSWatch/assets/4318648/8d8ec724-8145-4a30-b241-e69a8c2853bf

## Environment, Compiling and running the code
See [GETTING_STARTED.md](GETTING_STARTED.md)

## Writing apps for the Application Manager
I recommend watching [this part](https://youtu.be/MmCzV0jV9hs?t=1398) of my presentation at Zephyr Developer Summit where I build a sample app.

Check out [the sample application](app/src/applications/template/) for the general app design. The main idea is each app have an `<app_name>_app.c` file which registers the app, chooses icon and drives the logic for the app. Then there should be one or more files named for example `<app_name>_ui.c` containing pure LVGL code with no dependencies to Zephyr or the watch software. The idea is that this UI code should be runnable in a LVGL simulator to speed up development of UI, however right now that's not set up yet. The `<app_name>_app.c` will do all logic and call functions in `<app_name>_ui.c` to update the UI accordingly. 

Each application needs to have a way to close itself, for example a button, and then through callback tell the `application_manager.c` to close the app:

When user clicks an app in the app picker:
- `application_manager.c` deletes it's UI elements and calls the `application_start_fn`.
- `<app_name>_app.c` will do necessary init and then call the `<app_name>_ui.c` to draw the app UI.
- User can now navigate arund and the application and do whatever.

When user for example presses a close button in the application:
- Typically a callback from the UI code in `<app_name>_ui.c` will call `<app_name>_app.c` to tell that user requested to close the app. `<app_name>_app.c` will notify `application_manager.c` that it want to close itself. `application_manager.c` will then call `<app_name>_app.c` `application_stop_fn` and `<app_name>_app.c` will tell UI to close then do necessary de-init and return.
- `application_manager.c` will now draw the app picker again.

The application manager can also at any time close a running application by calling it's `application_stop_fn`.

## Other tools
- Visit https://jakkra.github.io/ZSWatch-Web-Dashboard to connect and view sensor data in a browser that supports Web Bluetooth [(Source code)](https://github.com/jakkra/ZSWatch-Web-Dashboard 
)

## Licence GPL-3.0
Main difference from MIT is now that if anyone want to build something more with this, then they need to also open source their changes back to the project, which I thinks is fair. This is so everyone can benefit from those improvements. If you think this is wrong for some reason feel free to contact me, I'm open to change the LICENCE.

## Thanks
<a href="https://www.segger.com/"><img width="25%" src=".github/images/SEGGER-Logo-the-embedded-experts-RGB.jpg" ></a>

SEGGER for supporting with SEGGER-OB licenses which makes the dock a fully functional programmer and debugger for ZSWatch.
Thanks to this the project will be much more approchable for persons without a J-Link debugger, letting them have full development and debugging capabilites.
