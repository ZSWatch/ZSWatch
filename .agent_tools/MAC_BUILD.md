# WatchDK build on macOS

Validated 2026-10-13 with NCS v3.3.0-preview1, matching app/west.yml,
installed under /opt/nordic/ncs/v3.3.0-preview1 by nrfutil sdk-manager.
The firmware checkout can live outside the SDK west workspace. LVGL XIP
relocation resolves ZEPHYR_LVGL_MODULE_DIR, rather than an assumed sibling
modules directory; missing relocation had overflowed internal FLASH by 80668
bytes. No partitions or product features were changed to make it fit.

From the app repository root (the Python tools environment contains nrfutil):

```sh
.venv-watch/bin/nrfutil sdk-manager toolchain launch \
  --ncs-version v3.3.0-preview1 \
  --chdir /opt/nordic/ncs/v3.3.0-preview1 -- \
  west build --build-dir /tmp/zswatch-fw-3.3-preview1 \
  /Users/jakkra/Documents/ZSWatch-App/ZSWatch/app \
  --board watchdk@1/nrf5340/cpuapp --sysbuild -- \
  '-DEXTRA_CONF_FILE=boards/debug.conf;boards/log_on_uart.conf' \
  '-DEXTRA_DTC_OVERLAY_FILE=boards/log_on_uart.overlay' \
  -DDEBUG_THREAD_INFO=Off -Dapp_DEBUG_THREAD_INFO=Off \
  -Dmcuboot_DEBUG_THREAD_INFO=Off -Dipc_radio_DEBUG_THREAD_INFO=Off \
  -Db0n_DEBUG_THREAD_INFO=Off
```

Use paths for the actual checkout. The disabled debug thread-info options avoid
layout changes from this diagnostic setting across sysbuild domains.

The complete sysbuild passes. App internal FLASH: 838052 / 916992 bytes
(91.39%); external executable FLASH: 481216 / 1048064 bytes (45.91%).
dfu_application.zip contains paired internal image 0 (838604 bytes), external
image 2 (481768 bytes) and radio image 1 (176012 bytes), version 0.8.1+0.
Use the matched set; a relinked internal image may reference changed external
code addresses. This build has not been installed or accepted over BLE.

The existing 2048 dependency needed LVGL 9 private structure headers and
lv_result_t/LV_RESULT_OK names, plus a malformed #elif fixed to #else.
The BMP5 driver initializes FIFO length for its error path. These narrow
compatibility fixes are local commits on codex/ios-companion in their respective
submodules and are pinned in the firmware gitlinks. No upstream push is claimed.
