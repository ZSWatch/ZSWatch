---
sidebar_position: 4
---

# Getting image resources into the watch

Many images and icons are placed in external flash and not part of the ZSWatch firmware. There are three ways to do it upload them. Choose the one suiting you best.

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