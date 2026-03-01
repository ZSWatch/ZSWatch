---
sidebar_position: 1
---

# Setting up the Toolchain

## 1. Install Required Tools

- [nRF Util](https://www.nordicsemi.com/Products/Development-tools/nRF-Util/)  
  <sub>→ Add to your <code>PATH</code></sub>
- [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nRF-Command-Line-Tools/Download)  
  <sub>→ Add to your <code>PATH</code></sub>
- [SEGGER J-Link](https://www.segger.com/downloads/jlink/)

---


## 2. Set Up VS Code

- Install the **nRF Connect VSCode Extension Pack**

- In the extension:  
  <kbd>Manage toolchain</kbd> → <kbd>Install Toolchain</kbd> → <kbd>Download v3.3.0</kbd>

:::caution Toolchain version
The Toolchain version **should** match the version in `app/west.yml`. Check the `revision` field under `sdk-nrf`, at the time of writing it is **v3.3.0-preview1**, so install the closest available SDK version (v3.3.0). If the versions don't match, `west update` or the build may fail.
:::

---

## 3. Clone the ZSWatch project

```bash
git clone https://github.com/ZSWatch/ZSWatch.git --recursive
```

Now open the cloned project in VSCode.

---

## 4. Initialize the Project

Open an **nRF Connect Terminal** (not a regular) in VS Code:
`(ctrl + shift + p) -> nRF Connect: Create Shell Terminal)`

Then run:

```bash
west init -l app
west update
```

Next, install the required Python packages. Both lines are needed. The first installs Zephyr's dependencies, the second installs ZSWatch-specific scripts (image upload tools, etc.).

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
  <TabItem value="linux" label="Linux / macOS" default>

  ```bash
  pip install -r zephyr/scripts/requirements.txt
  pip install -r app/scripts/requirements.txt
  ```

  :::tip
  These `pip install` commands must be run in the **nRF Connect Terminal** (not a regular terminal), because the nRF Connect extension manages a Python virtual environment with the correct paths.
  :::

  </TabItem>
  <TabItem value="windows" label="Windows">

  ```bash
  pip install -r zephyr/scripts/requirements.txt
  pip install --no-build-isolation -r app/scripts/requirements.txt
  ```

  :::note
  The `--no-build-isolation` flag on Windows works around a pip build environment issue when compiling `pynrfjprog` from source.
  :::

  </TabItem>
</Tabs>