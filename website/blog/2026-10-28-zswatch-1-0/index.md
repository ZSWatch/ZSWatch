---
slug: zswatch-1-0
title: The New ZSWatch 1.0 is Finished
authors: [jakkra]
tags: [zswatch, zswatch_hardware, zswatch_software, zswatch_hr, nlnet]
---

import SubscriptionForm from '@site/src/components/SubscriptionForm';
import WatchStand from './zswatch_stand.jpg';
import WatchAngled from './zswatch_angled.jpg';
import WatchFront from './zswatch_front.jpg';
import ChatAnswer from './chat_answer.png';
import ChatReplyAnswer from './chat_reply_answer.png';
import PrototypeCase from '../2026-05-31-first-real-prototype/assembled-first-protototype-watch.jpg';

It's been a while since the last update. I took a break over the summer, so not much happened for a few months.

The new ZSWatch 1.0 is here! The hardware is finalized. I expect to get the final build in a week or so, and since it only has a small change, I expect it to be the final one. The case is released and there is a full assembly guide. On the software side, a lot has landed since the last post: heart rate, power consumption improvements, animated watchfaces, voice replies to notifications, a Chat app, and a security audit and an accessibility review.

<a href={WatchStand} target="_blank" rel="noopener noreferrer">
  <img src={WatchStand} alt="The assembled ZSWatch 1.0 on a watch stand" style={{width: '100%', maxHeight: '720px', objectFit: 'contain'}} />
</a>

Let's break it down.

<!-- truncate -->

### The Case and Assembly Guide

The case is done and released in the new [Watch-3D](https://github.com/ZSWatch/Watch-3D) repository, with STEP and print-ready STL files. Here are all the parts:

<a href="/img/assembly/slicer.jpg" target="_blank" rel="noopener noreferrer">
  <img src="/img/assembly/slicer.jpg" alt="All case parts laid out in the slicer" style={{width: '100%', maxHeight: '520px', objectFit: 'contain'}} />
</a>

There is a new step-by-step **[Assembly guide](/docs/getting-started/assembly)** with photos of each step. The [Quick Start](/docs/getting-started/quickstart) now covers both the ZSWatch and the WatchDK.

<div style={{display: 'flex', gap: '0.75rem'}}>
  <a href={WatchFront} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 0', minWidth: 0}}>
    <img src={WatchFront} alt="ZSWatch from the front" style={{width: '100%', display: 'block'}} />
  </a>
  <a href={WatchAngled} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 0', minWidth: 0}}>
    <img src={WatchAngled} alt="ZSWatch on a desk, seen from an angle" style={{width: '100%', display: 'block'}} />
  </a>
  <a href={PrototypeCase} target="_blank" rel="noopener noreferrer" style={{flex: '0.4895 1 0', minWidth: 0}}>
    <img src={PrototypeCase} alt="The earlier prototype case" style={{width: '100%', display: 'block'}} />
  </a>
</div>

This is my first version of the case, and I'm working on a few more variants. The orange bezel and buttons in the photos are just an experiment with colors. You don't have to print it in orange, print it in any color you like.

---

### Power Optimizations and Heart Rate

While working on heart rate power consumption I found and fixed some power leaks in other parts of the firmware, like a level shifter leaking current and the IMU running faster than needed when the watch is still. The watch now uses less than half the current when stationary.

Heart rate now has three modes:

- **Off:** no automatic measurements, only manual ones from the app.
- **Background:** measures every five minutes while you move, and stops after a good reading or 45 seconds.
- **Continuous:** measures all the time while you move, and stops when you are stationary.

Some numbers measured with a PPK2:

| State | Before | After |
|---|---:|---:|
| Screen off, inactive (no HR) | ~236 µA | ~161 µA |
| Stationary, display supply off (no HR) | ~200 µA | ~68 µA |
| Active HR measurement | 9.01 mA | 1.52 mA |
| Background HR (one 45 s attempt every 5 min, average) | - | ~541 µA |

---

### Animated Watchfaces and Voice Replies

The watchfaces now animate when the screen turns on. A new watchface also animates the current weather: rain, snow, fog, clouds, stars and lightning. And you can now **reply to notifications by voice**: the phone transcribes it, and you check the text on the watch before sending.

<div style={{display: 'flex', gap: '1rem', flexWrap: 'wrap', justifyContent: 'center', textAlign: 'center'}}>
  <div style={{flex: '1 1 200px', maxWidth: '260px'}}>
    <video autoPlay loop muted playsInline src="/video/zswatch_1_0_wake.mp4" style={{width: '100%'}} />
    <em>Wake animation</em>
  </div>
  <div style={{flex: '1 1 200px', maxWidth: '260px'}}>
    <video autoPlay loop muted playsInline src="/video/zswatch_1_0_living_sky.mp4" style={{width: '100%'}} />
    <em>Weather animation</em>
  </div>
  <div style={{flex: '1 1 200px', maxWidth: '260px'}}>
    <video autoPlay loop muted playsInline src="/video/zswatch_1_0_reply.mp4" style={{width: '100%'}} />
    <em>Voice reply</em>
  </div>
</div>

---

### Chat App

The microphone is getting more and more useful. The new **Chat** app lets you ask a question by voice. Long-press the top-left button from anywhere, even with the display off, and ask your question. The phone transcribes it, gets an answer from an LLM through OpenRouter, and shows the answer on the watch. Press Reply to continue the conversation. Chat needs the [companion app](/docs/getting-started/companion_app).

<div style={{display: 'flex', gap: '1rem', flexWrap: 'wrap', justifyContent: 'center', textAlign: 'center'}}>
  <div style={{flex: '1 1 200px', maxWidth: '280px'}}>
    <img src={ChatAnswer} alt="Chat answer on the watch suggesting pasta aglio e olio" style={{width: '100%', borderRadius: '50%'}} />
    <em>"Quick dinner idea"</em>
  </div>
  <div style={{flex: '1 1 200px', maxWidth: '280px'}}>
    <img src={ChatReplyAnswer} alt="Chat answer after a reply, suggesting egg fried rice" style={{width: '100%', borderRadius: '50%'}} />
    <em>Reply: "Something without pasta"</em>
  </div>
</div>

---

### Security Evaluation and Accessibility Review

Through [NLnet](https://nlnet.nl/project/ZSWatch/) we got a security audit by [Radically Open Security](https://www.radicallyopensecurity.com/) and an accessibility review by the HAN Accessibility Lab. Both had some findings.

**Security:** the audit found eight issues, some more serious than others. All of them are fixed in [#588](https://github.com/ZSWatch/ZSWatch/pull/588).

**Accessibility:** the review had a list of points, and I implemented the ones that made sense for the watch, so the UI should now be a bit clearer in some places. See more in [#587](https://github.com/ZSWatch/ZSWatch/pull/587).

---

### New Apps and Community Contributions

- **Altimeter** app by [@elkanamol](https://github.com/elkanamol): altitude from the pressure sensor, history graph, floors climbed and descended, with calibration from location and weather.
- **Hebrew and right-to-left text** in notifications, also by [@elkanamol](https://github.com/elkanamol), plus a fix so long notifications are never cut in the middle of a UTF-8 character.
- **Queued popups** by [@elkanamol](https://github.com/elkanamol), so a popup is no longer lost when another one is showing.
- **Icons for the app category folders** by [@david-buxton](https://github.com/david-buxton).
- **Find device and looping vibration patterns** by [@HongNguyen635](https://github.com/HongNguyen635).

---

### Memory Tracking with MemBrowse

[@michael-membrowse](https://github.com/michael-membrowse) added [MemBrowse](https://membrowse.com) to our CI. Every PR now gets a comment showing how flash and RAM change on each board, like [this one in #621](https://github.com/ZSWatch/ZSWatch/pull/621#issuecomment-6374333007). Both RAM and flash on the nRF5340 are close to full, so seeing exactly what a PR adds is really useful.

While setting it up he also found a mistake in our CMake files: the code, fonts and images from the LVGL Editor were supposed to go to external flash, but never did. Fixing that in [#590](https://github.com/ZSWatch/ZSWatch/pull/590) freed up about 74 KB of internal flash on every board. Thanks!

You can also browse the full memory history for the [ZSWatch build](https://membrowse.com/public/zswatch/zswatch/target/zswatch-v1). The person behind MemBrowse also wrote [a LinkedIn post](https://www.linkedin.com/posts/papernov_embedded-firmware-zephyr-share-7519345401850822657-tYbq/) about it.

---

### Companion App Update

The [companion app](/docs/getting-started/companion_app) got a big rewrite. It is built with Flutter, but the OS kills the Flutter VM when the app is in the background, so it can't keep talking to the watch forever. All the background communication with the watch now lives in native code on both iOS and Android, while the rest of the app is still Flutter. This makes the connection a lot more reliable. New in the app:

- Chat and dictated notification replies (see above).
- Voice memos processed with OpenRouter by default, with on-device processing still available. The app asks before sending any voice data.
- Manage voice notes, and choose which apps forward notifications.
- New first-run setup with clear permission cards.
- Download firmware from CI builds.

---

### What's Next

Now that the watch is done, the most important next step is finding a good place, for example Elecrow, where people can order the watch in an easy way. The other part is making the required parts easy to source, by giving many options for each part.

I'm also working on more case variants, and the companion apps keep getting updated often. Get them for [iOS](https://apps.apple.com/us/app/zswatch/id6759058602) and [Android](/docs/getting-started/companion_app#android) (Android needs an invite for now, so reach out).

<br />

*Want to stay in the loop? Subscribe below for updates!*

<SubscriptionForm/>

[me]: https://github.com/jakkra
