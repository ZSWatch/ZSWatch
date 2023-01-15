<div align="center">
  <h1>ZSWatch</h1>
  
<img src=".github/in_use.jpg"/>
<sub>
  The ZSWatch v1
</sub>
</div>
<br/>


Smartwatch built from scratch, both hardware and software. Built on the [Zephyr™ Project](https://www.zephyrproject.org/) RTOS, hence the name **ZSWatch** - *Zephyr Smartwatch*.
<br/>


## Hardware Features in ZSWatch v1
- nRF52833 BLE chip [(u-blox ANNA-B402 module)](https://content.u-blox.com/sites/default/files/ANNA-B402_DataSheet_UBX-20032372.pdf).
- 1.28" 240x240 IPS TFT Circular Display with [GC9A01 driver](https://www.buydisplay.com/1-28-inch-tft-lcd-display-240x240-round-circle-screen-for-smart-watch).
- Accelerometer for step counting etc. [(LIS2DS12TR)](https://www.st.com/content/ccc/resource/technical/document/datasheet/ce/32/55/ac/e1/87/46/84/DM00177048.pdf/files/DM00177048.pdf/jcr:content/translations/en.DM00177048.pdf).
- Pulse oximetry and heartrate using [(MAX30101EFD)](https://datasheets.maximintegrated.com/en/ds/MAX30101.pdf)).
- Vibration motor with haptics driver to give better vibration control [(DRV2603RUNT)](https://www.ti.com/lit/ds/symlink/drv2603.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1662847620221).
- External 8MB flash [(MX25R6435FZNIL0)](https://static6.arrow.com/aropdfconversion/a35c4dff799e6b2e44732bd665796d74a8a3f62a/244684669660894mx25r6435f20wide20range2064mb20v1..pdf).
- Battery charger and battery supervisor ([MAX1811ESA+ datasheet](https://datasheets.maximintegrated.com/en/ds/MAX1811.pdf), [TLV840MAPL3](https://www.ti.com/lit/ds/symlink/tlv840-q1.pdf?ts=1662823963602&ref_url=https%253A%252F%252Fwww.ti.com%252Fpower-management%252Fsupervisor-reset-ic%252Fproducts.html)).
- 3 buttons for navigation (prev/next/enter)
- 220 mAh Li-Po battery.
- Sapphire Crystal Glass to protect the display.

## Upcoming Hardware features in ZSWatch v2
- nRF5340 BLE chip (u-blox NORA-B10 module)
- **Touch screen** with [same size and features as v1](https://www.buydisplay.com/240x240-round-ips-tft-lcd-display-1-28-inch-capactive-touch-circle-screen)
- 8MB external flash will *probably* be removed due to larger size of u-blox NORA-B10 vs. ANNA-B402.
- Find another way to dock the clock for charging and programming, maybe can find some connector similar to what smartwatches normally have.

## Charger/Dock
Basic pogo-pin dock that connects the power and SWD pins to the bottom of the watch.

## Enclosure/Casing
3D printed casing with 3D printed buttons. Does it's job, but for revision v2 of the watch I'll probably do something CNC'd for nicer looks.

## Software Features
- Bluetooth LE communications with [GadgetBridge](https://codeberg.org/Freeyourgadget/Gadgetbridge) Android app.
- Also support Bluetooth Direction Finding so the watch can act as a tag and is trackable using any [u-blox AoA antenna board](https://www.u-blox.com/en/product/ant-b10-antenna-board)
- Watchface that shows:
   - Standard stuff as time, date, battery
   - Weather
   - Step count
   - Number unread notifications
   - Heart rate (not implemented yet however)
- Pop-up notifications
- Setting menu system, with easy extendability
- [Application picker and app concept](#writing-apps-for-the-application-manager)
   - [Music control app](app/src/applications/music_control/)
   - [Settings app](app/src/applications/settings/)
   - etc.
- Step counting

### Larger not yet implemented SW Features and TODOs
- Heart rate, right now only samples the raw data, but no heart rate is calculated from it.
- Proper BLE pairing, currently removed due to flash constraints (fixed by nRF5340 upgrade).
- Watchface should also be an application.
- Refactoring of `main.c`, should have way less logic, utlize Zephyr architecture more.

## Android phone communication
Fortunately there is a great Android app called [GadgetBridge](https://codeberg.org/Freeyourgadget) which handles everything needed on the phone side, such as notifications management, music control and so much more... The ZSWatch right now pretends to be one of the supported Smart Watches in Gadgetbridge, following the same API as it does. In future there may be a point adding native support, we'll see.

## PCB
A 4 layer board which measures 36mm in diameter designed in KiCad.

<p float="left">
<img src=".github/pcb.jpg" width="49%" object-fit="cover"/>
<img src=".github/parts.jpg" width="49%" object-fit="cover"/>
</p>
<p float="left">
<img src=".github/inside.jpg" width="49%" object-fit="cover"/>
<img src=".github/back.jpg" width="49%" object-fit="cover"/>
</p>

## ZSWatch in action
|*Music control*|*Accelerometer for step count and tap detection*|
|---|---|
|  <img src=".github/music.gif" object-fit="cover" />  |  <img src=".github/accel.gif" object-fit="cover" /> |
|*Notifications from phone (Gmail here)*|*Settings*|
|  <img src=".github/notifications.gif" object-fit="cover" />    |  <img src=".github/settings.gif" object-fit="cover"/> |

## Writing apps for the Application Manager
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

## Dock
Very basic, will be re-worked for next watch revision v2.
<p float="left">
<img src=".github/dock.jpg" width="420"/>
</p>
