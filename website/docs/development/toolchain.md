---
sidebar_position: 1
---

# Setting up the Toolchain

## 1. Install Required Tools

- [SEGGER J-Link](https://www.segger.com/downloads/jlink/)
- [nRF Util](https://www.nordicsemi.com/Products/Development-tools/nRF-Util/)  
  <sub>→ Add to your <code>PATH</code></sub>
- [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nRF-Command-Line-Tools/Download)  
  <sub>→ Add to your <code>PATH</code></sub>

---


## 2. Set Up VS Code

- Install the **nRF Connect VSCode Extension Pack**

### Recommended Nordic VS Code Plugin Versions

> **Note:** Newer updates of the Nordic plugins can cause issues. The latest versions have really weird behaviours in a specific version, so it is recommended to use the following versions:

- **nRF Connect for VS Code**: `2025.5.152`
- **nRF DeviceTree**: `2025.4.22`
- **nRF Kconfig**: `2025.4.26`
- **nRF Terminal**: `2024.9.14`

If you experience problems, try downgrading to these versions.

- In the extension:  
  <kbd>Install/Manage toolchain</kbd> → <kbd>Install SDK</kbd> → <kbd>Download 3.0.2</kbd>

---

## 3. Initialize the Project

Open an **nRF Connect Terminal** (not a regular) in VS Code and run:

```bash
pip install -r zephyr/scripts/requirements.txt
pip install -r app/scripts/requirements.txt
west init -l app
west