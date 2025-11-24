---
sidebar_position: 3
---

# Running on Linux Without Real Hardware

## 1. Install Dependencies

```bash
sudo apt-get install build-essential
sudo apt-get install libsdl2-dev 
sudo usermod -aG bluetooth $USER
sudo setcap cap_net_admin=eip $(which hciconfig)
```

:::warning
You need to log out and in again (or restart) for the above to take effect.  
Bluetooth won't work if this is not done.
:::

## 2. Build the Project

Compiling is done the same way as for real hardware.  
No extra `.conf` or overlay files are needed.


**Extra CMake Args:**  
Add the following under **Extra CMake Args**:
```
-DSB_CONF_FILE=sysbuild_no_mcuboot_no_xip.conf
```


:::info
If you name the build directory <code>build</code>, debugging will work out-of-the-box with the default VS Code <code>.vscode/launch.json</code> path:
```
"program": "${workspaceFolder}/app/build/app/zephyr/zephyr.exe"
```
:::

---

## 3. Run ZSWatch

- Go to **Debug** in VS Code.
- Choose **Debug Native (with sudo)** and start it.

:::caution
You will be prompted for sudo, as Zephyr Bluetooth requires sudo access.  
(There is currently no workaround for this requirement.)
:::

---

Once running, a window should pop up.  
Debug logs will show in the VS Code Terminal window named `"cppdbg: zephyr.exe"`.

You can navigate using:

- **Mouse**: Click to interact
- **Enter**: Select
- **Arrow Up**: Navigate Next/Up
- **Arrow Down**: Navigate Previous/Down