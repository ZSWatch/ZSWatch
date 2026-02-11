---
sidebar_position: 5
---

# Writing Apps

## Overview

ZSWatch uses a **self-registering application framework**. There is no central list of apps to edit. Each app registers itself at boot time using Zephyr's `SYS_INIT` mechanism. When you add a new app directory with the right boilerplate, it is automatically discovered and appears in the app picker on the watch.

All apps live under `app/src/applications/<app_name>/`. The easiest way to create a new app is to **copy an existing one** (look through the existing apps and copy the most similar one) and modify it.

For app and UI work, it is recommended to start with the [native simulator](./linux_development.md) for fast iteration. If your app depends on data that is only available on real hardware (sensors, etc.), you can mock or fake that data in the simulator and then validate on hardware later.

For the full system architecture including managers, events, and BLE communication, see the [Architecture Overview](./architecture.md).

## App Lifecycle

Every app follows a simple state machine:

```
STOPPED → UI_VISIBLE ↔ UI_HIDDEN → STOPPED
```

| State | Description |
|-------|-------------|
| `ZSW_APP_STATE_STOPPED` | App is not running. No UI exists. |
| `ZSW_APP_STATE_UI_VISIBLE` | App UI is on screen. Safe to update LVGL objects. |
| `ZSW_APP_STATE_UI_HIDDEN` | App is still running but the screen is off. **Do not** touch UI objects. |

### Callback Flow

1. **User opens app** → `start_func(root, group)` is called. Create your UI under `root`.
2. **Screen turns off** (idle timeout) → `ui_unavailable_func()` fires. Stop timers, pause UI updates.
3. **Screen turns back on** → `ui_available_func()` fires. Resume UI updates.
4. **User presses back** → `back_func()` is called. Return `true` to consume the event (stay in app), or `false` to exit.
5. **App exits** → `stop_func()` is called. You **must** clean up all LVGL objects, timers, and event subscriptions.

## Creating a New App Step by Step

The recommended approach is to look through the existing apps and copy the most similar one and rename everything. Here is a walkthrough.

### 1. Create the App Directory

```
app/src/applications/my_app/
```

### 2. Create the Main Source File

Create `my_app_app.c` with the registration pattern:

```c
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include <lvgl.h>
#include <stdbool.h>

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(my_app, LOG_LEVEL_INF);

static void my_app_start(lv_obj_t *root, lv_group_t *group);
static void my_app_stop(void);
static bool my_app_back(void);
static void my_app_ui_unavailable(void);
static void my_app_ui_available(void);

ZSW_LV_IMG_DECLARE(my_app_icon);

static application_t app = {
    .name = "My App",
    .icon = ZSW_LV_IMG_USE(my_app_icon),
    .start_func = my_app_start,
    .stop_func = my_app_stop,
    .back_func = my_app_back,                     // Optional
    .ui_unavailable_func = my_app_ui_unavailable, // Optional
    .ui_available_func = my_app_ui_available,     // Optional
    .category = ZSW_APP_CATEGORY_TOOLS,
};

static void my_app_start(lv_obj_t *root, lv_group_t *group)
{
    // Create your LVGL UI under root
    lv_obj_t *label = lv_label_create(root);
    lv_label_set_text(label, "Hello from My App!");
    lv_obj_center(label);
}

static void my_app_stop(void)
{
    // Clean up all LVGL objects, timers, and subscriptions
}

static bool my_app_back(void)
{
    // Optional: handle the back button.
    // Return true to consume the event (stay in app).
    // Return false to exit the app.
    return false;
}

static void my_app_ui_unavailable(void)
{
    // Optional: called when the screen turns off.
    // Stop timers and pause UI updates here.
}

static void my_app_ui_available(void)
{
    // Optional: called when the screen turns back on.
    // Resume UI updates here.
}

static int my_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(my_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
```

**Key points:**
- `SYS_INIT` at the bottom registers your app at boot. No other file needs to know about it.
- `start_func` receives a `root` LVGL object. Create all your widgets as children of `root`.
- `stop_func` must clean up everything (the app manager deletes `root` for you, but you must delete your own timers and unsubscribe from events).
- `back_func`, `ui_unavailable_func`, and `ui_available_func` are optional but recommended for good UX and correct behavior.

### 3. Add a Kconfig Entry (optional)
If the app shall not always be included in build and you want the user to choose then create a `Kconfig` file in your app directory:

```kconfig
# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

menu "My App"
    config APPLICATIONS_USE_MY_APP
        bool
        prompt "Activate the application 'My App'"
        default y

    menu "Configuration"
        depends on APPLICATIONS_USE_MY_APP

        config APPLICATIONS_CONFIGURATION_MY_APP_REFRESH_INTERVAL_MS
            int
            prompt "Refresh interval in milliseconds"
            default 100
    endmenu
endmenu
```

Then include it from the parent `app/src/applications/Kconfig` by adding:

```kconfig
rsource "my_app/Kconfig"
```

### 4. Create CMakeLists.txt

```cmake
# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_MY_APP)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()
```

The `CONFIG_APPLICATIONS_USE_MY_APP` guard lets users enable/disable your app via Kconfig. If the app requires a specific HW not available on all ZSWatch versions, this can also be included in the `if`.

### 5. Provide an Icon

Place an icon image for your app. The build system handles icon storage (internal or external flash). In your code, declare and use the icon with:

For details on adding or updating icons, see [Image Resources](./image_resources.md).

```c
ZSW_LV_IMG_DECLARE(my_app_icon);       // File scope declaration

.icon = ZSW_LV_IMG_USE(my_app_icon),   // In the application_t struct
```

These macros abstract away whether the image is stored in internal or external flash.

## The `application_t` Struct

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `name` | `char *` | Yes | Display name shown in the app picker. |
| `icon` | `const void *` | Yes | App icon. Use `ZSW_LV_IMG_USE(icon_name)`. |
| `start_func` | `void (*)(lv_obj_t *, lv_group_t *)` | Yes | Called when the app launches. Create all UI under `root`. |
| `stop_func` | `void (*)(void)` | Yes | Called when the app exits. Must clean up all LVGL objects, timers, and subscriptions. |
| `back_func` | `bool (*)(void)` | No | Called on back button press. Return `true` to consume (stay in app), `false` to exit. |
| `ui_unavailable_func` | `void (*)(void)` | No | Called when the screen turns off. Pause UI updates. |
| `ui_available_func` | `void (*)(void)` | No | Called when the screen turns back on. Resume UI updates. |
| `category` | `zsw_app_category_t` | Yes | Determines which folder the app appears in within the app picker. |
| `hidden` | `bool` | No | If `true`, the app does not appear in the app picker (useful for background services). |

## App Categories

Categories control where your app appears in the app picker's folder structure:

| Category | Constant | Description |
|----------|----------|-------------|
| Root | `ZSW_APP_CATEGORY_ROOT` | Top-level, no folder. Appears directly in the main app list. |
| Tools | `ZSW_APP_CATEGORY_TOOLS` | Tools folder. |
| Fitness | `ZSW_APP_CATEGORY_FITNESS` | Fitness folder. |
| System | `ZSW_APP_CATEGORY_SYSTEM` | System folder. |
| Games | `ZSW_APP_CATEGORY_GAMES` | Games folder. |
| Sensors | `ZSW_APP_CATEGORY_SENSORS` | Sensors folder. |
| Random | `ZSW_APP_CATEGORY_RANDOM` | Random/misc folder. |

## Subscribing to Events (Zbus)

ZSWatch uses Zephyr's **zbus** for inter-module event communication. Apps can subscribe to sensor data, BLE events, notifications, and more.

### Example: Listening for BLE Data

```c
#include <zephyr/zbus/zbus.h>
#include "events/ble_event.h"

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void handle_ble_data(struct k_work *item);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(my_app_ble_lis, zbus_ble_comm_data_callback);

static K_WORK_DEFINE(ble_data_work, handle_ble_data);

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    // WARNING: This runs in the publisher's context (e.g., BLE thread).
    // Do NOT do heavy work or touch LVGL here.
    // Store the data/state etc.
    // Context-switch to a work queue instead:
    k_work_submit(&ble_data_work);
}

static void handle_ble_data(struct k_work *item)
{
    // Safe to do real work here (runs on the system work queue)
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        // Update UI
    }
}
```

:::warning
Zbus listener callbacks run in the **publisher's thread context**, which often has a small stack (e.g., the BLE thread). Always use `k_work_submit()` to move non-trivial processing to the system work queue.
:::

## Periodic Events

For apps that need regular updates (e.g., refreshing a sensor reading), ZSWatch provides shared periodic event channels. Subscribe in `start_func` and unsubscribe in `stop_func`:

```c
#include "events/zsw_periodic_event.h"

ZBUS_CHAN_DECLARE(periodic_event_100ms_chan);
ZBUS_LISTENER_DEFINE(my_app_100ms_listener, my_periodic_callback);

static void my_app_start(lv_obj_t *root, lv_group_t *group)
{
    // ... create UI ...
    zsw_periodic_chan_add_obs(&periodic_event_100ms_chan, &my_app_100ms_listener);
}

static void my_app_stop(void)
{
    zsw_periodic_chan_rm_obs(&periodic_event_100ms_chan, &my_app_100ms_listener);
    // ... clean up UI ...
}

static void my_periodic_callback(const struct zbus_channel *chan)
{
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        // Update UI at 100ms intervals
    }
}
```

Available periodic channels:

| Channel | Interval |
|---------|----------|
| `periodic_event_100ms_chan` | 100 ms |
| `periodic_event_1s_chan` | 1 second |
| `periodic_event_10s_chan` | 10 seconds |

## UI Pattern

### Recommended: Separate Logic and UI

Split your app into a logic file and a UI file:

```
app/src/applications/my_app/
├── my_app_app.c       # App registration, lifecycle, data handling
├── my_app_ui.c        # LVGL widget creation and updates
└── my_app_ui.h        # UI function prototypes
```

The UI header exposes a simple interface:

```c
#pragma once

#include <lvgl.h>

void my_app_ui_show(lv_obj_t *root);
void my_app_ui_remove(void);
void my_app_ui_update_value(int value);
```

This keeps the app logic clean and the LVGL code contained. The compass app is a good example of this pattern. See `compass_app.c` and `compass_ui.c` / `compass_ui.h`.

### Advanced: LVGL Editor (XML-Based UI)

For more complex UIs, ZSWatch supports the **LVGL Editor**, an XML-based visual design tool that generates C code automatically. See the LVGL Editor guide in the repo: https://github.com/zswatch/ZSWatch/blob/main/app/lvgl_editor/README.md

With this approach:

1. Define reactive **subjects** in `app/lvgl_editor/globals.xml`.
2. Create component XML files in `app/lvgl_editor/components/<name>/`.
3. Generate code from the editor → produces `*_gen.c` / `*_gen.h` files.
4. In your app code, create the component and update UI via subjects:

```c
#include "lvgl_editor_gen.h"
#include "my_component_gen.h"

lv_subject_copy_string(&my_text_subject, "Hello");
lv_subject_set_int(&my_value_subject, 42);
```

The music control app (`music_control_app.c`) is a good reference for this pattern.

:::caution
Never manually edit `*_gen.c` or `*_gen.h` files. They are auto-generated by the LVGL Editor and will be overwritten.
:::

## Common Pitfalls

### 1. Always Check App State Before Updating UI

The screen can turn off while your app is running. Guard all LVGL calls where needed:

```c
if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
    lv_label_set_text(my_label, "Updated!");
}
```

### 2. Clean Up Everything in `stop_func`

Delete all timers, unsubscribe from periodic events, and remove UI elements:

```c
static void my_app_stop(void)
{
    zsw_periodic_chan_rm_obs(&periodic_event_100ms_chan, &my_app_100ms_listener);
    if (refresh_timer) {
        lv_timer_del(refresh_timer);
        refresh_timer = NULL;
    }
    my_app_ui_remove();
}
```

Forgetting to unsubscribe from periodic events will cause callbacks to fire after the app is closed, likely crashing the watch.

### 3. Context-Switch From Zbus Callbacks

Never do heavy work or call LVGL functions directly in a zbus listener callback. Use `k_work_submit()`:

```c
static void zbus_callback(const struct zbus_channel *chan)
{
    // Copy data if needed, then:
    k_work_submit(&my_work);
}
```

### 4. Round Display

The display is 240×240 pixels and **circular**. Content placed in the corners will be clipped. Design your layouts accordingly.

### 5. Memory Constraints

RAM is limited (512 KB shared with the BLE stack). Be mindful of stack sizes, heap allocations, and the number of LVGL objects you create.


