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
- In the extension:  
  <kbd>Install/Manage toolchain</kbd> → <kbd>Install SDK</kbd> → <kbd>Download 3.0.2</kbd>

---

## 3. Initialize the Project

Open an **nRF Connect Terminal** in VS Code and run:

```bash
pip install -r zephyr/scripts/requirements.txt
pip install -r app/scripts/requirements.txt
west init -l app
west