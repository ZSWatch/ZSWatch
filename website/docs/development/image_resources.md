---
sidebar_position: 4
---

# Image Resources

Many icons and images in the ZSWatch UI are stored in **external flash** and are not part of the firmware binary. You need to upload them separately after flashing firmware.

## Uploading Images to the Watch

Choose the method that suits you best:

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
  <TabItem value="vscode" label="In VSCode">

  Press <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>P</kbd> 
  and run the following tasks (if using older legacy zswatch v4 use the other named Legacy):
  - `Upload Raw FS`

  </TabItem>
  <TabItem value="ble" label="Over BLE / USB (Web)">
  :::note Prerequisites
  - ZSWatch must be flashed with functional firmware.
  - On the watch, go to **Apps → Update** and enable **USB** and/or **BLE** before starting.
  - The image resources file (`lvgl_resources_raw.bin`) is bundled in every firmware download from [GitHub Releases](https://github.com/ZSWatch/ZSWatch/releases) and [GitHub Actions](https://github.com/ZSWatch/ZSWatch/actions).
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
    If you get missing pip packages make sure to run `pip install app/scripts/requirements.txt`. If you use VSCode Tasks:Run Task it uses the nRF Connect Terminal, so make sure to run the `pip install app/scripts/requirements.txt` using the nRF Connect Terminal instead.
    :::
  </details>

## Adding New Image Resources

To add your own icons or images to the watch:

1. Find or create a `.png` / `.jpg` / `.svg` image.
2. Convert it to a `.c` file using the [LVGL Image Converter](https://lvgl.io/tools/imageconverter). Use color format **RGB565**, or **RGB565A8** if the image has a transparent background.
3. Place the `.c` file in an appropriate folder, for example `app/src/images/icons/`.
4. Convert the `.c` file to a binary:
   ```bash
   python app/scripts/lvgl_c_array_to_bin_lvgl9.py <path_to_.c_file> app/src/images/binaries/S/
   ```
5. Upload the updated image resources to the watch using one of the methods above.

:::info
`app/src/images/binaries/S/` is the folder where binary image files are stored. They are automatically included when generating the image resource blob.
:::