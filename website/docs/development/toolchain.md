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
  <kbd>Install/Manage toolchain</kbd> → <kbd>Install SDK</kbd> → <kbd>Download 3.1.0</kbd>

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