---
sidebar_position: 4
---

# Image Resources

ZSWatch uses two forms of image storage depending on the target:

- **On real hardware:** Images are stored as `.bin` files in the **external flash** and must be uploaded separately from the firmware.
- **On the native simulator:** Images are compiled from `.c` source files directly into the binary, no separate upload needed.

Both forms originate from the same source image. The workflow below explains how they fit together.

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
  - On the watch, go to **Apps → Update** and enable **USB** and/or **BLE** before starting.
  - The image resources file (`lvgl_resources_raw.bin`) is bundled in every firmware download from [GitHub Releases](https://github.com/ZSWatch/ZSWatch/releases) and [GitHub Actions](https://github.com/ZSWatch/ZSWatch/actions).
  - Downloading from **GitHub Actions** requires you to be logged in to GitHub.
  :::
  Go to [https://zswatch.dev/update](https://zswatch.dev/update) and follow the instructions.

  </TabItem>
  <TabItem value="terminal" label="From terminal">

  Below for later ZSWatch no need to specify anything else. But for older version such as legacy v4 take a look in [tasks.json](https://github.com/ZSWatch/ZSWatch/blob/main/.vscode/tasks.json) for the exact lines needed for specific ZSWatch version.

  ```sh
  west upload_fs --type raw --ini_file app/boards/zswatch/watchdk/support/qspi_mx25u51245.ini
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

Images in ZSWatch exist in two places:

| File type | Location | Used by | Purpose |
|-----------|----------|---------|---------|
| `.c` files | `app/src/images/` (and subdirectories like `icons/`, `weather_icons/`, etc.) | **Native simulator** and as source for `.bin` conversion | Compiled into the binary for native_sim; also used to generate `.bin` files |
| `.bin` files | `app/src/images/binaries/S/` | **Real hardware** (WatchDK) | Stored in external flash, loaded at runtime via the `S:` storage prefix |

When you build for the **native simulator**, the `.c` files are compiled directly into the binary, so images just work. When you build for **hardware**, the firmware references images from external flash. The `.bin` files in `binaries/S/` are packaged into the `lvgl_resources_raw.bin` blob that you upload to the watch.

## Adding New Image Resources

To add a new icon or image (for example, an app icon):

### 1. Create the Source Image

Find or create a `.png` image. App icons are typically **32×32 pixels** with a transparent background.

### 2. Convert to LVGL C File

Use the **LVGL Image Converter v9** at [https://lvgl.io/tools/imageconverter](https://lvgl.io/tools/imageconverter):

- Select your image file.
- **Color format:** `RGB565` for opaque images, or `RGB565A8` if the image has transparency.
- Make sure you are using the **LVGL v9** converter (not v8).
- Download the generated `.c` file.

### 3. Place the `.c` File

Put the `.c` file in the appropriate directory:

| Image type | Directory |
|-----------|-----------|
| App icons | `app/src/images/icons/` |
| General images | `app/src/images/` (root) |
| App specific | `app/src/applications/your_app` |


If the `.c` file is placed in `images` folder it's automatically picked up by the build system. This makes the image available for the **native simulator**.

### 4. Generate the `.bin` File for Hardware

Convert the `.c` file to a `.bin` binary for external flash storage:

```bash
python app/scripts/lvgl_c_array_to_bin_lvgl9.py <path_to_.c_file> app/src/images/binaries/S/
```

For example:
```bash
python app/scripts/lvgl_c_array_to_bin_lvgl9.py app/src/images/icons/my_app_icon.c app/src/images/binaries/S/
```

This generates a `.bin` file in `app/src/images/binaries/S/` which will be included in the next `lvgl_resources_raw.bin` build.

### 5. Upload to the Watch

After adding the `.bin` file, upload the updated image resources to the watch using one of the [upload methods](#uploading-images-to-the-watch) above.

### Summary

When adding a new image, you need files in **two locations**:

1. **`.c` file** in `app/src/images/<appropriate_subdir>/` or in the app folder (for native_sim builds and as the source for conversion)
2. **`.bin` file** in `app/src/images/binaries/S/` (for hardware builds, generated from step 4)

### Using the Image in Code

In your source file, declare and use the image with the ZSWatch macros:

```c
ZSW_LV_IMG_DECLARE(my_app_icon);               // File scope declaration

.icon = ZSW_LV_IMG_USE(my_app_icon),           // In application_t struct or LVGL code
```

These macros automatically resolve to the correct storage location (internal flash on native_sim, external flash `S:` prefix on hardware).