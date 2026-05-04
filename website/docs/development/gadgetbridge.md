---
sidebar_position: 7
---

# GadgetBridge Protocol

ZSWatch communicates with Android phones using the **GadgetBridge** companion app via BLE. The protocol is JSON-based, allowing the watch to receive notifications, weather updates, music controls, and other data from the phone, as well as send commands back.

:::info
For iOS devices, ZSWatch uses standard Apple services (ANCS, AMS, CTS) instead of GadgetBridge. See the [Architecture Overview](./architecture.md#ble-communication) for details.
:::

## Message Format

GadgetBridge messages are JSON objects sent over BLE with a **type field** (`"t"`) that identifies the message category:

````json
{
  "t": "message_type",
  "field1": "value1",
  "field2": 123
}
````

The watch parses the `"t"` field and dispatches to the appropriate handler. Common message types include:

- `notify` - Notification data
- `weather` - Weather forecast
- `musicInfo` / `musicState` - Music control data
- `find` - Find my phone
- `watchface_bg` - Custom watchface background commands *(new)*
- `smp` - MCUmgr/SMP protocol messages

## Custom Watchface Backgrounds

:::tip New Feature
Added in PR #568, you can now upload custom watchface backgrounds from the GadgetBridge app.
:::

The watch supports uploading a custom background image to replace the default watchface background. The companion app uses a two-step process:

### 1. Upload the Image File

The companion app first uploads the custom background image to the watch's LittleFS filesystem using **MCUmgr file upload**:

- **Protocol**: MCUmgr FS management (SMP over BLE)
- **Target path**: `/lfs1/watchface_bg_staged.bin` (staged location)
- **Format**: LVGL v9 binary image format (`.bin`)

The image must be in **LVGL v9 binary format**. The companion app should convert the user's image (PNG/JPEG) to the correct format and size before uploading.

### 2. Apply or Reset the Background

After uploading, the companion app sends a `watchface_bg` command to apply or reset the background:

**Apply (use the uploaded image):**
````json
{
  "t": "watchface_bg",
  "action": "apply"
}
````

**Reset (revert to default):**
````json
{
  "t": "watchface_bg",
  "action": "reset"
}
````

### Response Format

The watch responds with a result message:

**Success:**
````json
{
  "t": "watchface_bg",
  "action": "apply_result",
  "ok": true
}
````

**Failure:**
````json
{
  "t": "watchface_bg",
  "action": "apply_result",
  "ok": false,
  "rc": -2,
  "error": "staged_missing"
}
````

**Error codes:**
- `"staged_missing"` - The staged image file was not found; upload it first
- `"invalid_background"` - The uploaded file is not a valid LVGL image
- `"operation_failed"` - Other error occurred

### Implementation Details

When the `apply` command is received:
1. The watch validates the staged image at `/lfs1/watchface_bg_staged.bin`
2. If valid, it moves the file to `/lfs1/watchface_bg.bin` (active location)
3. The watchface app is notified to reload the background
4. The new background is displayed on all compatible watchfaces

When the `reset` command is received:
1. The watch deletes `/lfs1/watchface_bg.bin` (if present)
2. The watchface app reverts to the built-in default background

:::note
The `reset` operation may block briefly until LVGL finishes any pending rendering work to ensure the file is not in use.
:::

## File Upload Protocol

The MCUmgr file upload protocol used for watchface backgrounds (and voice memo downloads) includes improvements for BLE reliability:

### Offset Mismatch Recovery

BLE file uploads use **write-without-response** for speed, which can occasionally result in packet loss. If the companion app's upload offset doesn't match the watch's current file offset, the watch will:

1. **Not abort the upload** (as older implementations did)
2. **Respond with the current server offset** in the MCUmgr response
3. **Reset the idle timeout** to keep the file handle open during recovery
4. Allow the mcumgr client library to resend from the correct position

This graceful recovery significantly improves upload reliability over BLE.

### Render Blocking

During file write operations, the display rendering is automatically blocked to prevent flash access conflicts. This ensures data integrity when writing to external flash while the display controller may also be accessing it.

## Future Enhancements

Potential future additions to the GadgetBridge protocol:
- Additional customization options (colors, complications)
- Widget configuration from the companion app
- Sync of app settings and preferences

---

For the overall BLE architecture and data flow, see [Architecture Overview](./architecture.md#ble-communication).