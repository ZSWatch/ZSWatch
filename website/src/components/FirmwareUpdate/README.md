# Firmware Update

The `/update` page supports USB Serial and Bluetooth updates, GitHub releases,
CI builds, and local firmware packages. The browser needs Web Serial or Web
Bluetooth for device operations. Firmware selection and package inspection also
work while disconnected.

## Select and update

1. In **Choose firmware**, choose **Releases** or **CI Builds** and filter by hardware and Debug/Release build type. Connecting
   over Bluetooth automatically selects the reported hardware, as in the app.
   **All hardware** overrides this filter.
2. Choose a version or branch, then select a firmware image. If GitHub blocks the browser download, use the displayed
   download link and select the resulting ZIP in **Firmware package**. CI ZIPs
   are downloaded in a separate tab and require GitHub sign-in.
3. Select the complete ZIP without extracting it. The parser finds
   `dfu_application.zip`, `dfu_application_rotated.zip`, and
   `lvgl_resources_raw.bin`, including files inside a directory. Select
   **Use rotated display firmware** to reload the matching variant.
4. Connect to the watch. For the first Bluetooth connection, enable Pairable in
   Settings → Bluetooth on the watch and accept pairing on the computer.
   Bluetooth sends the GadgetBridge SMP-enable command,
   waits for service registration, and discovers the update service. If Chrome
   retains an old service list, it reconnects and retries discovery. Older
   firmware can still use manually enabled SMP. USB must be enabled on the watch.
   **Update firmware** uploads each firmware image, confirms each image by hash,
   and restarts the watch while keeping the current resources. The separate
   **Update firmware + resources** action uploads and acknowledges resources
   first, then updates firmware. Include resources when the new version changes
   icons or graphics. Resource updates require application mode; recovery mode
   supports firmware only.

Firmware and filesystem can also be uploaded separately. Firmware-only updates
automatically confirm and restart; filesystem-only uploads restart after the
acknowledged transfer. Bluetooth reconnects after reboot, re-enables SMP, and
checks that the selected image hashes are active before reporting success.
If reconnecting fails, the page reports that verification is still needed.
USB requires manual reconnection after reboot; it does not report a verified
update until the watch has been inspected.

The connection card displays reported hardware, firmware and battery level,
with a warning below 20% battery. Hardware comes from GadgetBridge `ver.hw`,
with a DIS fallback only when it includes an actual board identity. USB uses
the optional SMP OS-info command when supported. Older USB builds and MCUBoot
may not report hardware; manual hardware selection stays available.

Standalone BIN selection and its image index are hidden behind **Advanced**.
The default picker and drag/drop accept ZIP packages only. Disabling Advanced
clears a selected standalone BIN. BINs use the known filename mapping, or the
selected image index for custom names. DFU `manifest.json` image indices take priority over filename mappings.
Recovery mode maps image indices to MCUBoot slots and waits 30 seconds after
uploading the network core.

Progress shows the current step, bytes, speed, estimated remaining time, and
transfer progress. The update log can be expanded. While an operation is running, device
controls and package changes are disabled; navigation is blocked and a screen
wake lock is requested when supported. Uploads can be cancelled before the
confirmation/restart phase. On errors, the selection stays available to retry.

## Implementation

- `FirmwareUpdateApp.js`: page layout and connection/device controls.
- `components/PrebuiltFirmwares.js`: release/CI selection and filters.
- `components/UpdateStatus.js`: compact ZIP selection, update action, progress and logs.
- `hooks/useFirmwareUpdate.js`: shared firmware/filesystem operation state.
- `hooks/useFirmwareManagement.js`: GitHub loading, refresh, and download state.
- `hooks/useMCUManager.js`: transport selection, connection and image state.
- `utils/firmwarePackage.js`: nested ZIP parsing, variants and validation.
- `utils/updateWorkflow.js`: update order and acknowledged confirmation/reset.
- `utils/UpdateMCUManager.js`: acknowledged transfers, bounded retries,
  cancellation, Bluetooth connection/reconnection and SMP error handling.
- `utils/watchBluetooth.js`: NUS commands, streaming GadgetBridge messages,
  version/hardware detection, battery reads/notifications and SMP enabling.
- `utils/githubApi.js`: GitHub releases, latest firmware build per branch, and
  download helpers. Discovery ignores expired artifacts and workflows without
  firmware artifacts before deduplicating branches.

The `mcumgr-web` submodule is used without changes to its transport source.

## Verification

From `website/`, run:

```sh
npm run test:firmware-update
npm run build
npm run serve -- --port 3000 --no-open
```

Tests use Node's test runner and the Babel dependencies bundled with Docusaurus.
They cover package parsing, slots, protocol acknowledgements, cancellation,
errors, update sequencing, recovery mode, confirmation/reset, GitHub filtering,
mocked GATT connections, exact NUS commands, fragmented replies, service
rediscovery, battery updates, reboot/reconnection and image verification.

Hardware check on 2026-10-01: the production `UpdateMCUManager` and
`WatchBluetooth` JavaScript ran against a real WatchDK through a small
Web Bluetooth API adapter backed by Bleak/BlueZ, with UART captured at 115200.
It detected `watchdk@1/nrf5340/cpuapp`, firmware `0.8.1` and 88% battery,
enabled SMP with `GB({"t":"smp","status":true})`, rediscovered services and
read active images 0 and 2. An SMP reset then reconnected, enabled SMP again
and verified the installed images. SMP was disabled after testing.
The first encrypted connection required the watch's Pairable window.

This verifies actual hardware protocol behavior, including the production
JavaScript's reconnection flow. Chrome's native Web Bluetooth flow and a full
firmware/filesystem upload still require end-to-end testing; no firmware was
flashed during this check.

## Sample GitHub catalog

In development, open `/update?githubDemo=1` to test releases, CI branches,
hardware/build filters and image selection without GitHub API calls. Sample
entries are labelled and cannot download or load firmware. The query parameter
has no effect in production. Normal `/update` uses GitHub data.


Firmware Update and Watch Tools share one manager and connection state in the
Docusaurus Root provider. Client-side navigation keeps the selected transport,
watch identity and active connection. Diagnostics buffers and capture also live
in that provider. Returning to Firmware Update keeps live logging active;
firmware operations stop logging and prepare SMP when needed. Explicit
disconnect or a device disconnect clears both views. Tools require Bluetooth;
a USB connection remains labelled as connected but Tools operations are disabled.

The package card shows the filename once, image count and resource size. Advanced
exposes standalone BIN slots, package contents and manual
resource uploads. NUS commands request the fast interval via the supported SMP
enable command before optional metadata reads, with no unconditional two-second
wait. An existing OS Bluetooth link still requires a browser GATT session.

The Bluetooth Connect action always opens the browser's device picker. The name
field applies a case-sensitive prefix filter, including watch names with a
unique identifier after `ZSWatch`. There is no saved-watch shortcut or separate
choose-another action. Navigation between the pages retains the active shared
connection; selecting a new watch after disconnecting uses the picker again.

While an enabled SMP session is idle on a visible updater/tools page, a
60-second heartbeat sends the existing SMP-enable command to refresh firmware's
180-second timeout. Requests and transfers refresh activity themselves. After a
gap of two minutes or more, the next heartbeat prepares fresh GATT subscriptions
in case the service expired. Hidden pages and other routes do not keep SMP alive;
returning prepares it again if necessary. Intentionally disabled SMP, live logs,
active requests/transfers and USB connections are excluded. Complete NUS commands
are queued so heartbeat and UI fragments cannot interleave.
