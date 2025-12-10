---
slug: dk-and-hr-integration
title: DevKit Orders, Speaker, Heart Rate Integration & Companion App
authors: [jakkra]
tags: [zswatch, zswatch_hardware, zswatch_software, zswatch_hr]
---

import SubscriptionForm from '@site/src/components/SubscriptionForm';
import DkConnection from './dk_and_hr_dk_connection.jpg';
import DkWithHrPlot from './dk_and_hr_dk_connection.jpg';
import WatchDk from './watchdk.jpg';
import HrPcbLayout from './hr_pcb_layout.png';
import HrPcbRenderBottom from './hr_pcb_render_bottom.png';
import HrPcbRenderTop from './hr_pcb_render_top.png';

The ZSWatch DevKit is almost ready for ordering, we've completed the heart rate integration into the main firmware, created a new app picker UI, finished the health PCB add-on with a new audio codec (meaning we will have a speaker), and we built a dedicated companion app. Let's break it all down!

<!-- truncate -->

### DevKit Ready to Order from Elecrow

After a while of iterations and testing, the ZSWatch DevKit is ready to be manufactured and shipped! They will be available from [Elecrow](https://www.elecrow.com) for fulfillment, where everything will be shipped together in one package.

<a href={WatchDk} target="_blank" rel="noopener noreferrer">
  <img src={WatchDk} alt="ZSWatch DevKit" />
</a>

**Pricing:** The DevKit will cost approximately **$100**, but may change slightly.

**Battery:** The battery is optional for the DevKit. We'll provide links to suitable batteries that you can purchase separately. Most 3.7V LiPo batteries should work.

**UV Print Not Available:**
The DevKit from Elecrow won't have the nice UV print shown on the PCB images as Elecrow doesn't support UV printing AFAIK. The PCB will have regular silkscreen instead in one color.


:::info ZSWatch Devkit Interest
We are doing a small run of ZSWatch DevKit, mostly to let you get some hardware to play around with early and for us to try out Elecrow for selling the real ZSWatch later. The DevKit will include everything seen in the image above except battery.
:::

<iframe src="https://docs.google.com/forms/d/e/1FAIpQLSddz945DTzGNd_O4Zb7u_1hbY4pClZSJ0cdJ5DcihJ5CNXivw/viewform?embedded=true" style={{width: '100%', height: '726px', border: 0}} marginheight="0" marginwidth="0">Läser in …</iframe>

---

### Heart Rate Integration with ZSWatch Firmware

We've completed the heart rate sensor integration into the main ZSWatch firmware! The health tracking PCB now connects to the headers and heart rate data can be displayed and plotted live.

<a href={DkWithHrPlot} target="_blank" rel="noopener noreferrer">
  <img src={DkWithHrPlot} alt="ZSWatch DevKit showing heart rate plot" />
</a>

Here's a video walkthrough of the heart rate interface in action:

<iframe 
  src="https://player.vimeo.com/video/1143873344?badge=0&amp;autopause=0&amp;player_id=0&amp;app_id=58479" 
  frameBorder="0" 
  allow="autoplay; fullscreen; picture-in-picture; clipboard-write; encrypted-media; web-share" 
  referrerPolicy="strict-origin-when-cross-origin" 
  title="ZSWatch Heart Rate Demo"
  style={{width: '100%', aspectRatio: '16/9'}}
/>

---

### New App Picker UI

The app picker got a rework, I think it looks much better now! It's now much smoother to navigate through your apps. Still some small tuning is needed, but you can do apps and folders now.

App picker in action:

<iframe 
  src="https://player.vimeo.com/video/1145177199?badge=0&amp;autopause=0&amp;player_id=0&amp;app_id=58479"
  width="350"
  height="620"
  frameBorder="0"
  allow="autoplay; fullscreen; picture-in-picture; clipboard-write; encrypted-media; web-share"
  referrerPolicy="strict-origin-when-cross-origin"
  title="ZSWatch App Walkthrough"
/>


You can see all the details in the [Pull Request #503](https://github.com/ZSWatch/ZSWatch/pull/503).

---

### Health PCB Add-on Complete with bonus Audio Codec!

The health tracking PCB add-on for the watch is now finished! This compact board packs a lot of functionality into just **16.8 mm diameter** (8.4 mm radius). It's a super small PCB which houses A LOT!

<div style={{display: 'flex', gap: '1rem', flexWrap: 'wrap', justifyContent: 'center'}}>
  <a href={HrPcbLayout} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 200px', maxWidth: '300px'}}>
    <img src={HrPcbLayout} alt="Health PCB Layout" style={{width: '100%'}} />
  </a>
  <a href={HrPcbRenderTop} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 200px', maxWidth: '300px'}}>
    <img src={HrPcbRenderTop} alt="Health PCB Render Top" style={{width: '100%'}} />
  </a>
  <a href={HrPcbRenderBottom} target="_blank" rel="noopener noreferrer" style={{flex: '1 1 200px', maxWidth: '300px'}}>
    <img src={HrPcbRenderBottom} alt="Health PCB Render Bottom" style={{width: '100%'}} />
  </a>
</div>

It was quite a challenge to fit everything, especially because of the special routing needed for the HR sensors, we had to take much care. All credits go to [Daniel](https://github.com/kampi)!

One interesting addition: we've included a [Renesas DA7212 audio codec](https://www.renesas.com/en/document/dst/da7212-datasheet?r=1563326) on the health PCB. The DA7212 comes in a tiny 34-ball WL-CSP (4.54 mm x 1.66 mm) package, and the health PCB already requires expensive PCB features like via-in-pad and filled vias for the heart rate sensors. Since we're already paying for these advanced fabrication features, we went for it! Meaning we will have a speaker on ZSWatch!

---

### Dedicated ZSWatch Companion App

Here's something that wasn't planned at all: we are building a dedicated ZSWatch companion app! With how good LLMs have become at helping with app development, it was surprisingly easy to get started, so why not?

The app is written in Flutter, and and works both Android and iOS. This means iOS users will finally be able to get the full ZSWatch experience!

:::note
[Gadgetbridge](https://gadgetbridge.org/) still works great on Android for basic functionality, but this dedicated app adds many more ZSWatch-specific features.
:::

The app has many new nice features, but watch the video for details!

<iframe 
  src="https://player.vimeo.com/video/1143868178?badge=0&amp;autopause=0&amp;player_id=0&amp;app_id=58479" 
  width="350" 
  height="620" 
  frameBorder="0" 
  allow="autoplay; fullscreen; picture-in-picture; clipboard-write; encrypted-media; web-share" 
  referrerPolicy="strict-origin-when-cross-origin" 
  title="ZSWatch App Walkthrough"
/>

---

### What's Next
We are right now trying to finish the CAD for the new ZSWatch housing. Once we do that, we will route the new main PCB board, that is just the ZSWatch Devkit but in a smaller form factor that fits in the watch. The design is already verified so should be a simple job! 

#### Other stuff
- Tell Elecrow to start building DevKits orders
- Continue iOS app development
- More health tracking features and driver improvements
- Integration of audio functionality

<br />

*Want to stay in the loop and be notified when DevKit orders open? Subscribe below!*

<SubscriptionForm/>

[me]: https://github.com/jakkra
