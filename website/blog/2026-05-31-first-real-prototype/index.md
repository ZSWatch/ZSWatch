---
slug: first-real-prototype-working
title: First Real "New" ZSWatch Prototype is Working
authors: [jakkra]
tags: [zswatch, zswatch_hardware, zswatch_software, zswatch_hr]
---

import SubscriptionForm from '@site/src/components/SubscriptionForm';
import AssembledPrototype from './assembled-first-protototype-watch.jpg';
import BarePcbAndHr from './bare-pcb-watch-and-hr-extention.jpg';
import HrMountedBottom from './bare-pcb-with-hr-mounted-bottom-view.jpg';
import UsbDebugBreakout from './watch-usb-c-connected-debugger-breakout.jpg';
import AppBackgrounds from './app_pre_made_watch_bgs.jpg';
import AppCropBackground from './app_pick_local_bg_crop.jpg';
import PrototypeCaseZip from './watch-prototype-case.zip';

The first real "reworked" ZSWatch prototype is assembled, running, and confirmed working! This is the small watch PCB we have been working toward after validating the larger DevKit. We also have an initial case, the heart-rate extension PCB is working, the speaker is alive, and the companion app has some new microphone and watchface features.

<a href={AssembledPrototype} target="_blank" rel="noopener noreferrer">
  <img src={AssembledPrototype} alt="First assembled ZSWatch prototype in a wearable case" style={{width: '100%', maxHeight: '720px', objectFit: 'contain'}} />
</a>

Let's break it down.

<!-- truncate -->

### First Watch PCB Prototype: Working!

The main watch PCB prototypes are finished, and the important part is done and they work. This is the compact PCB that takes what we validated on the ZSWatch DevKit and fits it into the real watch size.

So far the hardware bring-up has gone very well and all tested functions are verified.

<div style={{display: 'flex', gap: '1rem', flexWrap: 'wrap', justifyContent: 'center'}}>
  <a href={BarePcbAndHr} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 320px', maxWidth: '520px'}}>
    <img src={BarePcbAndHr} alt="ZSWatch main PCB and heart-rate extension PCB" style={{width: '100%'}} />
  </a>
  <a href={HrMountedBottom} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 320px', maxWidth: '520px'}}>
    <img src={HrMountedBottom} alt="ZSWatch PCB with heart-rate extension mounted on the bottom" style={{width: '100%'}} />
  </a>
</div>

The watch PCB is much smaller, but the design was already verified on hardware before this form factor existed. In the pictures above you can see the main PCB and the optional extension PCB. Ignore the marked red area, those parts are snipped off.

---

### Prototype Case

We also now have an initial prototype case. It is not the final housing, and it will change quite a bit, but it lets us wear the watch.

The final case is still in progress. The goal there is better visuals, easier assembly, and a design that is much nicer to build.

Below you can also see a minimal and super cheap dock breakout that exposes SWD debug and UART for anyone interested in those.  
<a href={UsbDebugBreakout} target="_blank" rel="noopener noreferrer">
  <img src={UsbDebugBreakout} alt="ZSWatch prototype connected through the USB-C debugger breakout" style={{width: '100%', maxHeight: '720px', objectFit: 'contain'}} />
</a>


The current WIP case files are available here for anyone who wants to look at or experiment with them:

<p>
  <a href={PrototypeCaseZip} download>
    Download the current prototype case ZIP
  </a>
</p>

:::note
This case is work in progress. Expect the final case to change, especially around assembly, button design, and the overall visual finish.
:::

---

### Buttonless Mode

One thing I added in firmware is a setting that allows using the watch without physical buttons (swipe up is back). That is useful while experimenting with different case variants, and it opens the door to a cleaner-looking enclosure.

But there are drawbacks. Bootloader recovery still requires one physical button, so if firmware ever ends up in a bad state, you would need to open the watch to press the recovery button. Waking from full power-off has similar limitations. So a completely buttonless configuration can work, but it will be more limited from a recovery and power-control perspective.

A possible middle ground is a case variant with only the two tiny right-side buttons. Those buttons are the important ones: one goes to the nPM1300 PMIC, and the other is for bootloader recovery if firmware breaks. Both are needed for the most robust setup.

---

### Heart Rate Extension and Speaker

The heart-rate extension PCB is also working. Heart-rate readings are working, and the speaker path is working too.

This is especially nice because the extension board is very compact and includes some tricky hardware. Daniel has been pushing a lot of this work forward, and getting both HR and speaker verified on the real watch stack is a great step.

Some work to tune the bottom of the case is still needed to make it work well with the HR LEDs and sensors.

A proper demo video will come when I have time to record one.

---

### Companion App: Quick Voice Capture and Local AI

I have also added several nice features to the mobile apps that make use of the onboard microphone.

Hold down the top-left button and the watch starts a short recording. The recording is sent automatically to the app, transcribed locally (Whisper), then a local LLM classifies it into useful things like reminders, task lists, calendar events, or just an idea dump.

The recording, transcription, and classification are designed around running locally on the phone rather than sending everything to a cloud service.

This makes the microphone feel much more useful than just storing raw audio.

---

### Watchface Background Upload from the App

Another companion app feature is watchface background upload. You can pick from pre-made backgrounds, or select and crop an image locally before sending it to the watch.

<div style={{display: 'flex', gap: '1rem', flexWrap: 'wrap', justifyContent: 'center'}}>
  <a href={AppBackgrounds} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 260px', maxWidth: '360px'}}>
    <img src={AppBackgrounds} alt="ZSWatch companion app showing pre-made watchface backgrounds" style={{width: '100%'}} />
  </a>
  <a href={AppCropBackground} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 260px', maxWidth: '360px'}}>
    <img src={AppCropBackground} alt="ZSWatch companion app cropping a watchface background" style={{width: '100%'}} />
  </a>
</div>

---

### What's Next

Now that the watch PCB, HR extension, and speaker are verified, the next step is to make sure it all works reliably and that it is easy to build and put together a complete watch. Some small mechanical things may change in the PCB, but as far as we can see, everything works as intended.
<br />

*Want to stay in the loop? Subscribe below for updates!*

<SubscriptionForm/>

[me]: https://github.com/jakkra
