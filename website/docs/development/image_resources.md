---
sidebar_position: 4
---

# Image Resources

Images are stored as `.bin` files in `app/src/images/binaries/S/`. They are packed into one resource image, `lvgl_resources_raw.bin`:

- **On real hardware** the resource image is stored in the **external flash** and must be uploaded separately from the firmware.
- **On the native simulator** the build links the same resource image into the executable, no separate upload needed.

:::tip When do I need to upload images?
Image resources only need to be uploaded to the watch when they **change**: for example, when you add a new icon or update an existing one. If you're just updating firmware without changing images, you can skip this step. Note that uploading takes approximately **3 minutes** over USB/BLE.
:::

## Uploading Images to the Watch

Choose the method that suits you best:

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
  <TabItem value="vscode" label="In VSCode">

  Press <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>P</kbd> -> `Tasks: Run Task` ->
  and run the following tasks (if using older legacy zswatch v4 use the other named Legacy):
  - `Upload Raw FS`

  </TabItem>
  <TabItem value="ble" label="Over BLE / USB (Web)">
  :::note Prerequisites
  - ZSWatch must be flashed with functional firmware.
  - For USB, open **Apps → Update** on the watch and enable **USB**.
  - The web updater enables Bluetooth updates automatically. For a first connection to the computer, enable **Pairable** in **Settings → Bluetooth** on the watch and accept pairing on the computer.
  - On older firmware that does not support automatic Bluetooth setup, enable **BLE** in **Apps → Update** manually.
  - The image resources file (`lvgl_resources_raw.bin`) is bundled in every firmware download from [GitHub Releases](https://github.com/ZSWatch/ZSWatch/releases) and [GitHub Actions](https://github.com/ZSWatch/ZSWatch/actions).
  - Downloading from **GitHub Actions** requires you to be logged in to GitHub.
  :::
  Go to [https://zswatch.dev/update](https://zswatch.dev/update) and follow the instructions. You can select the complete firmware package and use **Update firmware + resources** to upload both firmware and resources, or select `lvgl_resources_raw.bin` in **File System** to upload resources separately.

  </TabItem>
  <TabItem value="terminal" label="From terminal">

  Below for later ZSWatch no need to specify anything else. But for older version such as legacy v4 take a look in [tasks.json](https://github.com/ZSWatch/ZSWatch/blob/main/.vscode/tasks.json) for the exact lines needed for specific ZSWatch version.

  ```sh
  west upload_fs --type raw --ini_file app/boards/zswatch/watchdk/support/qspi_mx25u51245.ini
  ```

  The ZSWatch and the WatchDK use the same `.ini` file (also in `app/boards/zswatch/zswatch/support/`).

  If you want to upload using only `nrfjprog` (no `west upload_fs`), use the generated hex images:

  ```sh
  nrfjprog -f nrf53 \
    --program lvgl_resources_raw.hex \
    --qspisectorerase --verify --reset \
    --qspiini app/boards/zswatch/watchdk/support/qspi_mx25u51245.ini \
    --coprocessor CP_APPLICATION
  ```

  To generate these files locally:

  ```sh
  west upload_fs --type raw --generate_only
  ```

  </TabItem>
</Tabs>

  <details>
    <summary><strong>Troubleshooting</strong></summary>

    :::tip
    If you see <code>No connected debuggers found</code> but <code>nrfjprog --ids</code> and JLink finds the debugger, try this fix:

    ```sh
    export LD_LIBRARY_PATH=/opt/nrf-command-line-tools/lib:$LD_LIBRARY_PATH
    # or add it to your ~/.bashrc
    ```
    :::

    :::tip
    If you get missing pip packages make sure to run `pip install -r app/scripts/requirements.txt`. If you use VSCode Tasks:Run Task it uses the nRF Connect Terminal, so make sure to run the `pip install -r app/scripts/requirements.txt` using the nRF Connect Terminal instead.
    :::
  </details>

## How Images Work Internally

Every file in `app/src/images/binaries/S/` goes into the resource image, which the firmware exposes as the read-only `S:` drive. `ZSW_LV_IMG_USE(my_icon)` is simply the path `"S:my_icon.bin"`.

The resource image is made by `app/scripts/create_custom_resource_image.py` and ends with a hash index of the file names. The firmware keeps no file table in RAM: opening a file hashes its name and reads one index entry from flash. While the external flash is memory mapped (XIP is on whenever the display is on), LVGL draws images straight from flash without copying them to RAM first. With XIP off the same files are read through the flash driver.

A custom watchface background is uploaded as `/user/bg.bin` in LittleFS, which can not be drawn straight from flash. After the background is applied, and at boot if needed, the firmware copies it to the `custom_bg_partition` partition (128 KB, right after the resource image) and draws it from there the same way. Until the copy is done, and on native_sim, the file in LittleFS is drawn.

Firmware and resource image can be updated in any order. Older firmware reads new resource images through the name table at the start of the image, and new firmware reads older images without index, only slower when opening files.

:::note
A few images that must work without the resource image, or that are used in ways the `S:` drive does not cover, are still compiled into the firmware from `.c` files and used with `LV_IMG_DECLARE()`.
:::

## Adding New Image Resources

To add a new icon or image (for example, an app icon):

### 1. Create the Source Image

Find or create a `.png` image. App icons are typically **32×32 pixels** with a transparent background.

### 2. Convert to an LVGL `.bin` File

Use the converter that comes with LVGL. Name the `.png` what the image shall be called in the code:

```bash
python modules/lib/gui/lvgl/scripts/LVGLImage.py --ofmt BIN --cf RGB565A8 -o app/src/images/binaries/S/ my_app_icon.png
```

Use `--cf RGB565A8` if the image has transparency and `--cf RGB565` for opaque images. Do not use compression or the indexed formats, they have to be decoded to RAM before drawing.

If you already have an LVGL v9 `.c` file, for example from the [LVGL Image Converter](https://lvgl.io/tools/imageconverter), convert it instead:

```bash
python app/scripts/lvgl_c_array_to_bin_lvgl9.py <path_to_.c_file> app/src/images/binaries/S/
```

Only the `.bin` file is added to the repository. The native simulator picks it up on the next build.

### 3. Upload to the Watch

Upload the updated image resources to the watch using one of the [upload methods](#uploading-images-to-the-watch) above.

### Using the Image in Code

In your source file, declare and use the image with the ZSWatch macros:

```c
ZSW_LV_IMG_DECLARE(my_app_icon);               // File scope declaration

.icon = ZSW_LV_IMG_USE(my_app_icon),           // In application_t struct or LVGL code
```
