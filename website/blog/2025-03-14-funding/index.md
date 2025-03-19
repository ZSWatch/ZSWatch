---
slug: funding
title: We received funding!
authors: [jakkra]
tags: [zswatch, nlnet]
---
import ZSWatchLogo from './ZSWatch_logo_with_text.png';
import NlnetLogo from './NGI0Core_tag.png';
import NGI0CoreLogo from './nlnet_banner.png';
import SubscriptionForm from '@site/src/components/SubscriptionForm';

<p float="left">
  <img src={ZSWatchLogo} width="33%" />
  <img src={NlnetLogo} width="33%" /> 
  <img src={NGI0CoreLogo} width="33%" />
</p>


For the last few months there have not been much progress in ZSWatch. **But** that's about to change, because we have received 50 000 € from the [NLnet Foundation](https://nlnet.nl/project/ZSWatch/) to take ZSWatch to the next level!

The NLnet Foundation is funded by the European Commission's *Next Generation Internet*:

> We support organisations and people who contribute to an open internet for all. We fund projects that help fix the internet through open hardware, open software, open standards, open science and open data.

Which ZSWatch falls pretty well under.

Below we go into more details about what this means.

<!-- truncate -->
### How the funding will be used

The funding will allow [me] to work 50% and [Daniel] to work 20% on ZSWatch. The rest of the money will go to hardware and hardware prototyping costs. The funded project will run for 1 year.

The goal is to both rework and improve ZSWatch. Mainly to make it more accessible to a broader audience and grow the community. This will be done by making it cheaper, easier to assemble and use more accessible parts. Right now getting all parts and assembling a ZSWatch is not super easy, this we aim to change!

**Our goal is within one year have a robust watch that is easy to assembly that you can easily get your hands on!**

### High level plan
There are so much more details defined, but including that would make this post very long. So here is the high level tasks we aim to finish.
:::tip Goals
    - New daughter board for health tracking.
        - This will be a optional PCB that connects to the main PCB.
        - Since HR algorithms are no easy task, we are planning to use [MAX32664(C) Biometric Snesor Hub with embedded algorithms](https://www.analog.com/en/products/max32664.html).
    - New main board.
        - Cheaper.
        - Optimize physical layout for improved mechanical assembly.
        - Changes to accomodate the daughter board.
    - Change of components on ZSWatch.
    - New simpler dock.
    - Case redesign.
    - Production test software.
    - Documentation using [Docusaurus](https://docusaurus.io/).
        - You are reading on it now!
        - We will host the blog and all documentation.
    - Improve software usability.
    - Lightwight app store.
        - We want to investigate [Zephyr LLEXT](https://docs.zephyrproject.org/latest/services/llext/index.html) to see if it's feasable to dynamically load applications to ZSWatch.
    - Testing.
        - We need more automated tests to easy catch regressions.
    - Phone integration improvements.
:::

We will post updates of our progress so don't forget to sign up for our email list!

<SubscriptionForm/>

[me]: https://github.com/jakkra
[Daniel]: https://github.com/kampi