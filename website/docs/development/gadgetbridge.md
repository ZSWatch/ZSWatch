---
sidebar_position: 3
---

# GadgetBridge Protocol

## Overview

ZSWatch communicates with Android devices through the [GadgetBridge](https://gadgetbridge.org/) companion app using a **JSON-based protocol over BLE**. The protocol enables bidirectional communication for notifications, music control, weather updates, time synchronization, and custom watchface backgrounds.

Implementation: `app/src/ble/gadgetbridge/ble_gadgetbridge.c`

## Protocol Format

All messages are **JSON strings** exchanged via BLE characteristics. Each message contains a `"t"` (type) field that identifies the message category.

### Common Message Types

| Type | Direction | Purpose |
|------|-----------|---------|
| `"notify"` | Phone → Watch | Incoming notification |
| `"musicinfo"` | Phone → Watch | Now playing music metadata |
| `"musicstate"` | Phone → Watch | Playback state (play/pause/stop) |
| `"weather"` | Phone → Watch | Weather information |
| `"time"` | Phone → Watch | Current time sync |
| `"call"` | Phone → Watch | Incoming call notification |
| `"nav"` | Phone → Watch | Navigation instructions |
| `"musiccontrol"` | Watch → Phone | Music control command (play/pause/next/prev) |
| `"voice_memo"` | Both | Audio memo commands/notifications |
| `"watchface_bg"` | Both | Custom watchface background management |

## Custom Watchface Backgrounds

:::info Added in PR #568
Custom watchface background upload allows users to personalize their watch by uploading custom images from the companion app.
:::

### Overview

The custom watchface background feature is a **two-step process**:

1. **File Upload** - The companion app uploads an LVGL binary image file to `/lfs1/watchface_bg.bin` using MCUmgr/SMP file upload
2. **Apply/Reset Command** - The companion app sends a GadgetBridge `watchface_bg` JSON command to apply or reset the background

### Step 1: File Upload (MCUmgr/SMP)

The companion app uses the standard **MCUmgr file upload protocol** (via BLE SMP) to upload the background image:

- **Target path**: `/lfs1/watchface_bg.bin`
- **File format**: LVGL v9 binary image (`.bin` format)
- **Recommended size**: 240×240 pixels (to match watch display)
- **Color format**: RGB565 (16-bit)
- **Compression**: LVGL RLE compression recommended for size reduction

:::tip Converting Images
Use the LVGL Image Converter tool or the project's conversion script:
```bash
python app/scripts/lvgl_c_array_to_bin_lvgl9.py <input.c> <output_dir>/
```
:::

#### MCUmgr Upload Protocol

The MCUmgr protocol handles file uploads in chunks with automatic offset tracking. **PR #568 improvements**:

- **Graceful offset mismatch recovery** - If the watch and app get out of sync (e.g., due to BLE packet loss), the watch now sends the current offset in the error response, allowing the app to resume from the correct position
- **Render blocking during upload** - The UI render loop is temporarily blocked during file and flash operations to prevent corruption from concurrent access

### Step 2: Apply or Reset Background

After the file is uploaded, the companion app sends a GadgetBridge JSON command to **apply** or **reset** the background.

#### Apply Command

**Request** (Phone → Watch):
````json
{
  "t": "watchface_bg",
  "action": "apply"
}
````

**Response** (Watch → Phone):
````json
{
  "t": "watchface_bg",
  "action": "apply_result",
  "ok": true
}
````

**Error Response** (if file missing or invalid):
````json
{
  "t": "watchface_bg",
  "action": "apply_result",
  "ok": false,
  "rc": -2,
  "error": "staged_missing"
}
````

#### Reset Command

Removes the custom background and reverts to the built-in default.

**Request** (Phone → Watch):
````json
{
  "t": "watchface_bg",
  "action": "reset"
}
````

**Response** (Watch → Phone):
````json
{
  "t": "watchface_bg",
  "action": "reset_result",
  "ok": true
}
````

### Error Codes

| Error String | Description |
|-------------|-------------|
| `"staged_missing"` | The uploaded file `/lfs1/watchface_bg.bin` is not found |
| `"invalid_background"` | The uploaded file is not a valid LVGL image |
| `"operation_failed"` | Generic failure (check watch logs for details) |

### Implementation Details

When the watch receives an `apply` command:

1. Calls `watchface_app_reload_bg()` to load the new image from `/lfs1/watchface_bg.bin`
2. Validates the image header (LVGL v9 format)
3. Updates the active watchface with the new background
4. Sends a JSON response back to the companion app

When the watch receives a `reset` command:

1. Calls `watchface_app_reset_bg()` to delete `/lfs1/watchface_bg.bin`
2. Reverts to the built-in default background
3. Blocks until LVGL finishes any active rendering to prevent corruption
4. Sends a JSON response back to the companion app

:::warning Filesystem Access
The apply/reset operations may block for up to a few hundred milliseconds while ensuring safe filesystem and rendering state. This prevents race conditions between the BLE thread and LVGL render thread.
:::

## Music Control

### Music Metadata (Phone → Watch)

````json
{
  "t": "musicinfo",
  "artist": "Artist Name",
  "album": "Album Name",
  "track": "Track Name",
  "dur": 240,
  "c": 120,
  "of": 2,
  "tot": 12
}
````

**Fields:**
- `artist`, `album`, `track` - Now playing metadata
- `dur` - Track duration in seconds
- `c` - Current playback position in seconds
- `of` - Track number in album
- `tot` - Total tracks in album

:::info Added in PR #568
Watchfaces can now display music information via the new `set_music(track, artist)` API.
:::

### Music State (Phone → Watch)

````json
{
  "t": "musicstate",
  "state": "play"
}
````

**Possible states:** `"play"`, `"pause"`, `"stop"`

### Music Control Commands (Watch → Phone)

````json
{
  "t": "musiccontrol",
  "action": "play"
}
````

**Possible actions:** `"play"`, `"pause"`, `"next"`, `"previous"`, `"volume_up"`, `"volume_down"`

## Weather Updates

````json
{
  "t": "weather",
  "temp": 22,
  "hum": 65,
  "code": 1
}
````

**Fields:**
- `temp` - Temperature in °C
- `hum` - Humidity percentage (added in PR #568)
- `code` - Weather condition code (mapping TBD)

## Notifications

````json
{
  "t": "notify",
  "id": 12345,
  "src": "Gmail",
  "title": "New Email",
  "body": "You have 3 new messages",
  "sender": "sender@example.com",
  "tel": "+1234567890"
}
````

## Time Synchronization

````json
{
  "t": "time",
  "timestamp": 1715123456
}
````

The watch synchronizes its RTC with the phone's current time (Unix timestamp).

## Voice Memos

:::info App Renamed
The "Voice Memo" app was renamed to **"Memos"** in PR #568, with updated icons (mic and drop).
:::

### Memo Recorded Notification (Watch → Phone)

````json
{
  "t": "voice_memo",
  "action": "recorded",
  "file": "/lfs1/voice_memos/memo_001.opus"
}
````

The companion app can then download the file via MCUmgr and perform transcription/classification.

## See Also

- [Architecture Overview](./architecture.md#ble-communication) - Overall BLE communication architecture
- [Writing Apps](./writing_apps.md) - How to subscribe to BLE data events in your app
- MCUmgr Documentation - For file upload/download protocol details
