---
slug: progress-devkit-prototype
title: Development Kit Prototype
authors: [jakkra, kampi]
tags: [zswatch]
---

import DockDk from './dk-dock.jpg';
import HrPCB from './ZSWatch-Dock-sch.png';
import DevkitPcb from './dk.jpg';
import DevkitRender from './dk-render.png';
import USBDrawing from './usb.drawio.png';
import USBSchematic from './dock-usb-sch.png';
import HRProgress from './hr_progress.png';

import SubscriptionForm from '@site/src/components/SubscriptionForm';

We’ve made great progress on the hardware front! Our new development kit is the next step toward a more refined ZSWatch, and it allows us to test key features before shrinking everything down to the final form factor.

This Devkit isn’t just for internal testing, it’s also for developers who want to get an early start building software for ZSWatch. It gives full access to all interfaces, and we’ll be sharing more info soon on how to get one.

<img src={DevkitPcb} alt="ZSWatch Development Kit PCB" />

Let’s dive into the details....
<!-- truncate -->

### ZSWatch Development Kit

As a first step in the next ZSWatch design, we created a Development Kit to try out all the new concepts before shrinking everything down to a compact PCB for the final watch.

We’ve completed the prototype design and received the boards. We made a few mistakes, but nothing that couldn’t be manually patched on the PCBs, including:

- Misunderstood USB-C cable routing, so UART on the Dock was connected to SWD on the Devkit. Thankfully, this was easy to patch using the 0-ohm jumpers we had routed.
- The display LED driver didn’t handle unused LED channels well (we only used 2 out of 4), so we’ll switch back to the original driver, which is slightly larger but only has 2 channels—perfect for our use case.
- 2-pin SMD headers were unstable and led to crooked soldering. We’re switching to through-hole headers in the next revision for better alignment.
- Shrink it down a bit for cheaper PCBs.

Some of the Devkit features:
- Each component has its own power header, allowing easy power consumption debugging using a current profiler in ampere mode.
- Breakout of pins going to the Health Tracking PCB.
- I²S header for potential speaker support.
- USB-C connector for exposing ZSWatch signals.

We also tested UV printing from PCBWay and it looks pretty nice! We'll tune the colors a bit in the next version. UV printing adds only a few dollars per PCB, so we think it's worth keeping.

Each feature is laid out in its own area to make the board easy to explore. Headers allow easy access for current measurements.

<img src={DevkitRender} alt="ZSWatch Devkit render with labeled components" />

---

### Devkit and Dock USB-C Interface

One of the biggest updates in this Devkit is the new USB-C interface, which replaces the hard-to-source magnetic connector we used previously.
The final ZSWatch will have a waterproof USB-C connector that connects to a new "Dock". This connector utilizes the extra pins in USB 3 to expose SWD, UART, and RESET, in addition to standard USB functionality.

<img src={DockDk} alt="ZSWatch USB-C Dock prototype" />
:::tip
The dock is fully optional for none development use.
:::

#### Here’s a high-level diagram of how it works

The dock is optional and is meant to make development and debugging easier.

<img src={USBDrawing} alt="USB-C signal breakout schematic" />
<br></br>

<details>
  <summary>USB-C pin usage</summary>
  <img src={USBSchematic} alt="ZSWatch Dock USB-C schematic" />
</details>

---

### Health tracking prototype
[Daniel] have made good progress in fixing up our initial driver for the MAX32664 Biometric Hub and now we get good readings from it. We will continue testing the performance. Read more about it [here in Daniels Linkedin post](https://www.linkedin.com/posts/daniel-kampert_zswatch-zswatch-zephyr-activity-7355485066313015297-EzP6)

We are also working in [upstreaming support for health data](https://github.com/zephyrproject-rtos/zephyr/pull/93484) and [our driver to Zephyr](https://github.com/zephyrproject-rtos/zephyr/pull/93674).

<img src={HRProgress} alt="ZSWatch Devkit render with labeled components" />


### Next up

- Order next PCB revision with fixes
- Begin integration of health-tracking components
- Finalize UV print color tuning
- Start work on new ZSWatch hosuing and design the PCB accordinly

<br />

*Want to stay in the loop? Subscribe below for updates!*

<SubscriptionForm/>

[me]: https://github.com/jakkra
[Daniel]: https://github.com/kampi
