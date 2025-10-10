---
slug: progress-hw-xip-usb-power
title: Power consumption, HW update & Production test
authors: [jakkra]
tags: [zswatch, zswatch_hardware, zswatch_software]
---

import SubscriptionForm from '@site/src/components/SubscriptionForm';
import WebsiteShell from './website_usb_shell.png';
import WebsiteFull from './website.png';
import DKRender from './dk-1-2-render.png';
import HRv2 from './hr_v2_dk.png';
import PCBWayLogo from './pcbway_logo.svg';

As usual, a lot has landed since our last update, but much of it hasn’t been very visual, so the updates here have been quiet. On hardware, we focused on revision two of the Devkit and the heart‑rate test PCB. On software, we optimized power (especially reducing XIP power draw), improved firmware updates over BLE and USB (WebSerial), and added a couple of apps. We also built a dedicated production test application to verify hardware functions during manufacturing. Below is a quick tour, plus a short technical deep‑dive on how XIP works on ZSWatch and why it matters for battery life.

Let’s dive into the details...
<!-- truncate -->

### DevKit v2 ordered (likely final version)

Last week we ordered what we believe is the final iteration of the ZSWatch Devkit (#2). We expect to receive it in about three weeks. As a trial for fulfillment, we plan to sell a limited quantity (at least initially) through https://www.elecrow.com. We’ll set up a shop there shortly. Make sure to sign up on the email list to not miss it!

<a href={DKRender} target="_blank" rel="noopener noreferrer"><img src={DKRender} alt="ZSWatch Devkit v1.2 render" /></a>

### Heart Rate board v2

We also received version 2 of the heart‑rate test board, and it’s working well. Compared to the v1 prototype from the earlier post ([Health PCB and Firmware Update](/blog/progress-hr-fota)), v2 includes bug fixes and a simplified optical front‑end and an extra photodiode:

- [Single LED module: OSRAM SFH 7016 (Red, Green, IR)](https://look.ams-osram.com/m/1a73f8d425cfcc40/original/SFH-7016.pdf)
- [Two photodiodes: Vishay VEMD8082](https://www.vishay.com/docs/80381/vemd8082.pdf)

<a href={HRv2} target="_blank" rel="noopener noreferrer"><img src={HRv2} alt="Heart‑Rate test board v2 on Devkit" /></a>


<div style={{ display: 'flex', alignItems: 'center', gap: 16, margin: '0.25rem 0' }}>
  <a href="https://www.pcbway.com/" target="_blank" rel="noopener noreferrer" style={{ display: 'inline-block', lineHeight: 0 }}>
    <PCBWayLogo style={{ width: 220, display: 'block' }} />
  </a>
  <div>
    A big thanks to PCBWay for sponsoring manufacturing and assembly of our Heart‑Rate test PCB. That board uses via‑in‑pad, which makes fabrication more expensive. As always boards turned out great!
  </div>
</div>

### XIP management and App UI state

We introduced an XIP manager and refined app state handling to safely disable XIP when the screen is off. This reduces baseline current consumption while ensuring apps and UI code don’t crash when XIP is unavailable. Changes included:
- Keep app UI code in external flash, but keep app logic in internal flash so it can run while XIP is off.
- Add an app “visible/hidden” state so UI updates don’t run when XIP is disabled.
- Careful walkthrough of what can live in external flash and what can’t:
  - Since we disable XIP when the screen is off, background code can’t live in external flash anymore.
  - `mcumgr` and USB live in XIP as they’re only needed during updates. We explicitly enable them when performing updates in the [update app](https://github.com/ZSWatch/ZSWatch/blob/main/app/src/applications/update/update_app.c).

For apps where the UI code is placed in QSPI flash with XIP, apps now need to guard like [this](https://github.com/ZSWatch/ZSWatch/blob/e3052acf96fc31c813b4050af3b9a3d32d527b26/app/src/applications/stopwatch/stopwatch_app.c#L158):
```c
static void update_elapsed_time(void)
{
  if (stopwatch_data.state == STOPWATCH_STATE_RUNNING) {
      uint64_t current_time = k_uptime_get();
      stopwatch_data.elapsed_ms = (uint32_t)(current_time - stopwatch_data.start_time_ms);
      
      // Only update UI if XIP is enabled and UI is visible
      if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
          stopwatch_ui_update_time(stopwatch_data.elapsed_ms);
      }
  }
}
```

### New Applications

- FFT Spectrum visualizer with an emulated mic driver for the Linux/native build (handy for quick dev and testing).
  <video controls src="https://github.com/user-attachments/assets/73f3b879-9343-4f80-ae69-938d120bc3fd" style={{width: '100%', maxHeight: '420px'}} />

- Calculator app based on [Zephyr’s SMF sample](https://github.com/zephyrproject-rtos/zephyr/tree/main/samples/subsys/smf/smf_calculator).
  <a href="https://github.com/user-attachments/assets/841fbd2e-b997-4f68-b037-094cb0d60fca" target="_blank" rel="noopener noreferrer">
    <img src="https://github.com/user-attachments/assets/841fbd2e-b997-4f68-b037-094cb0d60fca" alt="Calculator app on ZSWatch" style={{maxWidth: '320px', width: '100%'}} />
  </a>

### Firmware updates over USB and visual overhaul

We now support firmware updates over USB (in addition to BLE) using the browser’s WebSerial API. This means you can also recover a watch that’s stuck or has broken firmware by entering MCUBoot and performing a serial recovery directly from the website. The firmware update page also got a nice visual overhaul.

<a href={WebsiteFull} target="_blank" rel="noopener noreferrer">
  <img src={WebsiteFull} alt="ZSWatch website showing USB firmware update" />
</a>

### Testability and power consumption
To keep power consumption low while still allowing automated tests on the same firmware, USB stays enabled for ~20s after boot so CI can connect and run firmware‑update tests reliably.

We also enabled shell command support over BLE and USB, so the watch can be managed via the website Shell Console. Added shell commands to some random functions in ZSWatch, not very useful, but fun to play around with.

<a href={WebsiteShell} target="_blank" rel="noopener noreferrer"><img src={WebsiteShell} alt="Shell Console showing large shell output" /></a>

### Production test build

We now have a dedicated [production test firmware](https://github.com/ZSWatch/ZSWatch/tree/main/production_test) that reuses much of the main firmware. This app is required for verifying hardware functions during production and will be used by Elecrow when assembling DKs (and likely future watches) to confirm that all components work correctly.

<video controls src="https://github.com/user-attachments/assets/92a09a1a-dd70-44a3-abb9-1259be6760ec" style={{width: '100%', maxHeight: '420px'}} />
### Watch housing update

Simon, a mechanical engineer, has joined the project to help redesign the watch housing. This will give a much more streamlined and robust case, improving both aesthetics and durability. Nice!

---

### How XIP works on ZSWatch

Execute‑in‑Place (XIP) lets the MCU run code directly from external flash over the QSPI interface. This gives us a lot more flash space for features, but it comes with two important considerations:

- Power: Keeping QSPI and its cache active adds to idle current around 3–5 mA, which is a lot for a watch.
- Safety: Parts of the code (e.g., some of LVGL, mcumgr/USB, etc.) reside in XIP. This code must not be called while XIP is disabled or it will crash.

Some of the things we do in ZSWatch to manage this:
- Some LVGL components are placed in external flash. We keep the most used parts and drawing code in internal flash since XIP is slower.
- App UI code is placed in the XIP area to save internal flash. App logic stays in internal flash so apps can continue running when the screen is off and XIP is disabled.
- XIP manager: A small manager arbitrates XIP enable/disable and provides a simple API so code that needs XIP can request it when needed. The API is ref‑counted and when the count hits 0, XIP is disabled, and vice versa.
- App visibility state: Apps are notified when their UI is visible/hidden so they don’t call into LVGL (or other XIP code paths) when XIP is off.
- On‑demand features: `mcumgr` and USB live in the XIP area and are only enabled for updates.

Result: Idle current drops from ~5 mA to ~500 µA with the display off. When you open an app or start an update, XIP is enabled as needed and then turned off again when we return to an idle screen‑off state or when a firmware update finishes. Going forward we’ll have to be more careful with what lives in XIP and only things that aren’t needed while the screen is off.

---

### What’s next

- Continue power profiling, there’s more to do.
- Validate the Dev Kit when received.
- Work has started on a completely reworked case for the upcoming ZSWatch.
- Start work on real watch PCB and HR PCB, we got design validated to it's "just" to route them.

<br />

Want to stay in the loop? Subscribe below for updates!

<SubscriptionForm/>

---
