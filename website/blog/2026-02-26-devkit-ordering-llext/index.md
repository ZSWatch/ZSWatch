---
slug: devkit-ordering-llext-apps
title: DevKit Ready to Order, Main Watch PCB & Dynamic App Loading Experiments
authors: [jakkra]
tags: [zswatch, zswatch_hardware, zswatch_software]
---

import SubscriptionForm from '@site/src/components/SubscriptionForm';
import WatchDkElecrow from './watchdk_elecrow.png';
import WatchAngledTop from './ZSWatch-Watch-angled_top.png';
import WatchAngledBottom from './ZSWatch-Watch-angled_bottom.png';

The ZSWatch DevKit is finally ready to [order at Elecrow!](https://www.elecrow.com/zswatch-development-kit.html). Documentation has been improved a lot. On the software side, we've been experimenting with something pretty exciting: dynamically loading apps onto the watch, no reflashing required. Let's break it all down.

<!-- truncate -->

### ZSWatch DevKit: Ready to Order!

Finally the ZSWatch DevKit is now available for ordering! Everything ships in one package: PCB, display, and vibration motor, with all components soldered and ready to go.

<a href={WatchDkElecrow} target="_blank" rel="noopener noreferrer">
  <img src={WatchDkElecrow} alt="ZSWatch DevKit at Elecrow" style={{maxWidth: '500px', width: '100%'}} />
</a>

The DevKit runs the full ZSWatch firmware with all features. It's meant for prototyping, driver development, and early experimentation before the final watch form factor is available. 

**Pricing:** **$99** including main PCB, display, and vibration motor. Battery not included, but the kit runs fine from USB power alone.

:::tip Ready to get one?
**[Order the ZSWatch DevKit from Elecrow here!](https://www.elecrow.com/zswatch-development-kit.html)**

This is a limited initial run, mainly to get hardware into developer hands early and for us to test Elecrow as a fulfillment partner for the real ZSWatch later.
:::

There is also a new [Getting Started guide](https://zswatch.dev/docs/getting-started/watchdk-quickstart) that covers everything needed for getting started with the DevKit. **If you do get a DevKit, any feedback on the docs (or the hardware!) is very welcome. This is our first run, so there may be some rough edges we haven't caught yet.**

**Links:**
- [Quickstart Guide](https://zswatch.dev/docs/getting-started/watchdk-quickstart)
- [Firmware (GitHub)](https://github.com/ZSWatch/ZSWatch)
- [Hardware (KiCad)](https://github.com/ZSWatch/Watch-DevKit-HW)
- [Schematics & production files](https://github.com/ZSWatch/Watch-DevKit-HW/tree/main/production/watch-dk-RELEASED)

---

### Watch PCBs: First Prototypes Sent for Manufacturing

We also finished the first prototype design of the main watch PCB! This is essentially the DevKit design shrunk down to fit inside the watch housing. It has been sent off for manufacturing, so we should have boards back in a month or so.

<div style={{display: 'flex', gap: '1rem', flexWrap: 'wrap', justifyContent: 'center'}}>
  <a href={WatchAngledTop} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 200px', maxWidth: '350px'}}>
    <img src={WatchAngledTop} alt="ZSWatch PCB render top" style={{width: '100%'}} />
  </a>
  <a href={WatchAngledBottom} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 200px', maxWidth: '350px'}}>
    <img src={WatchAngledBottom} alt="ZSWatch PCB render bottom" style={{width: '100%'}} />
  </a>
</div>

We'll write more about the watch hardware and the extension PCB design in the next blog post.

---

### Companion App: LLEXT Dynamic App Loading (Experiment)

We've been experimenting with Zephyr's **LLEXT (Linkable Loadable Extensions)** subsystem to enable dynamically loading apps onto the watch at runtime, without reflashing the firmware. This is purely experimental for now and there are still things to figure out before this could be merged, but the results so far are pretty fun.

Here's a demo of uploading and running apps using LLEXT:

<video controls src="/video/install_llext_apps.mp4" style={{width: '100%', maxHeight: '520px'}} />

#### How LLEXT Works

LLEXT apps are compiled as position-independent shared objects (`.llext` ELF binaries) that link against symbols exported by the firmware. At runtime, the LLEXT loader resolves symbols, loads code into memory, and calls the app's `app_entry()` function, which registers the app with the app manager just like a built-in app would.

On a constrained device like the nRF5340 there were a few challenges to solve: streaming app code directly to external QSPI flash (XIP) so it doesn't eat into RAM, copying critical callback functions to internal flash so they survive when XIP is off for power savings, and generating small trampolines to handle the ARM PIC GOT base register (R9) correctly for callbacks.

For the full technical details, check out the [PR description in #526](https://github.com/ZSWatch/ZSWatch/pull/526). For general LLEXT info, see the [Zephyr LLEXT documentation](https://docs.zephyrproject.org/latest/services/llext/index.html).

:::note
This is an experiment, not something to expect merged soon. There are still open questions around app lifecycle, memory management, and how to best handle the XIP power trade-offs. But it's a fun proof of concept!
:::

You can follow the progress in [Pull Request #526](https://github.com/ZSWatch/ZSWatch/pull/526).

---

### Bonus: Web-Based App Editor (Experiment)

Supporting LLEXT in the firmware also enables the use of the **ZSWatch EDK (Extension Development Kit)**, a self-contained package extracted from a firmware build that contains all the headers and compiler flags needed to compile LLEXT apps without having the full ZSWatch and Zephyr source tree. This means you can build apps for ZSWatch externally.

As a fun experiment, I hacked together a browser-based code editor. It uses a Monaco editor (same engine as VS Code), sends source files to a backend API server with just the ARM cross-compiler, and can install the compiled app to the watch over Web Bluetooth. Still very early and not hosted publicly yet, but here's a quick demo:

<video controls src="/video/web_app.mp4" style={{width: '100%', maxHeight: '520px'}} />

And here it is running on the watch:

<video controls src="/video/web-editor-app-demo.mp4" style={{width: '100%', maxHeight: '520px'}} />

More info in Zephyr docs regarding [LLEXT Extension Development Kit (EDK)](https://docs.zephyrproject.org/latest/services/llext/build.html#llext-extension-development-kit-edk).

---

<br />

*Want to stay in the loop? Subscribe below for updates!*

<SubscriptionForm/>

[me]: https://github.com/jakkra
