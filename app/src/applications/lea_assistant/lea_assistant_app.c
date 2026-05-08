/**
 * @file lea_assistant_app.c
 * @author Leonardo Bispo
 *
 * @brief LE Audio Broadcast assistant, a Broadcast Assistant can find Broadcast Audio Streams for a Broadcast Receiver.
 *
 * @see https://github.com/larsgk/web-broadcast-assistant
 * @see https://www.bluetooth.com/learn-about-bluetooth/feature-enhancements/le-audio/le-audio-specifications/
 *
 */
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/zbus/zbus.h>

#include "lea_assistant_ui.h"
#include "lea_assistant_app.h"
#include "managers/zsw_app_manager.h"
#include "ble/ble_comm.h"
#include "events/ble_event.h"
#include "ui/utils/zsw_ui_utils.h"

#include "message_handler.h"

LOG_MODULE_REGISTER(lea_assistant_app, CONFIG_ZSW_LEA_ASSISTANT_APP_LOG_LEVEL);

// Functions needed for all applications
static void lea_assistant_app_start(lv_obj_t *root, lv_group_t *group);
static void lea_assistant_app_stop(void);
static void show_source_stage(void);
static void show_source_stage_async(void *data);
static void show_complete_async(void *data);
static void show_complete_work_handler(struct k_work *work);
static void close_app_work_handler(struct k_work *work);
static void on_source_selected(lea_assistant_device_t *device);

ZSW_LV_IMG_DECLARE(auracast);

static lv_obj_t *_root = NULL;
static bool source_stage_visible;
static bool fake_source_added;
static bool completion_visible;

K_WORK_DELAYABLE_DEFINE(show_complete_work, show_complete_work_handler);
K_WORK_DELAYABLE_DEFINE(close_app_work, close_app_work_handler);

static application_t app = {
    .name = "LEA Assistant",
    .icon = ZSW_LV_IMG_USE(auracast),
    .start_func = lea_assistant_app_start,
    .stop_func = lea_assistant_app_stop
};

static void close_app(void)
{
    zsw_app_manager_app_close_request(&app);
}

static void close_app_async(void *data)
{
    LV_UNUSED(data);
    close_app();
}

static void close_app_work_handler(struct k_work *work)
{
    LV_UNUSED(work);
    lv_async_call(close_app_async, NULL);
}

static void add_fake_source_entry(void)
{
    if (!IS_ENABLED(CONFIG_ZSW_LEA_ASSISTANT_FAKE_SOURCE_DISCOVERY) || fake_source_added) {
        return;
    }

    lea_assistant_device_t fake_source = {
        .sid = 1,
        .pa_interval = 0x00a0,
        .broadcast_id = 0x5a015e,
    };

    strcpy(fake_source.name, "Zephyr Keynote");
    fake_source.addr.type = BT_ADDR_LE_RANDOM;
    fake_source.addr.a.val[0] = 0x5e;
    fake_source.addr.a.val[1] = 0x01;
    fake_source.addr.a.val[2] = 0x5a;
    fake_source.addr.a.val[3] = 0xee;
    fake_source.addr.a.val[4] = 0xff;
    fake_source.addr.a.val[5] = 0xc0;

    LOG_INF("Adding fake LE Audio source for demo UI");
    lea_assistant_ui_add_source_list_entry(&fake_source);
    fake_source_added = true;
}

static void show_source_stage(void)
{
    if (_root == NULL) {
        return;
    }

    if (!source_stage_visible) {
        lea_assistant_ui_show_source(_root, on_source_selected);
        source_stage_visible = true;
    }

    add_fake_source_entry();
}

static void show_source_stage_async(void *data)
{
    LV_UNUSED(data);
    show_source_stage();
}

static void show_complete_async(void *data)
{
    LV_UNUSED(data);

    if (_root == NULL || completion_visible) {
        return;
    }

    completion_visible = true;
    lea_assistant_ui_show_complete(_root, "Audio ready");
    k_work_reschedule(&close_app_work, K_MSEC(1800));
}

static void show_complete_work_handler(struct k_work *work)
{
    LV_UNUSED(work);
    lv_async_call(show_complete_async, NULL);
}

static void on_source_selected(lea_assistant_device_t *device)
{
    int err;

    LOG_DBG("Source %s selected", device->name);

    completion_visible = false;
    lea_assistant_ui_show_syncing(_root, device->name);

    // Could use `MESSAGE_SUBTYPE_ADD_SOURCE`, but calling it directly so don't need to fill buffer and parse LTV
    err = add_source(device->sid, device->pa_interval, device->broadcast_id, &device->addr);
    if (err) {
        LOG_ERR("Failed to add source (%d)", err);
        lea_assistant_ui_show_error(_root, "Source failed", "Receiver not ready");
        return;
    }

    if (IS_ENABLED(CONFIG_ZSW_LEA_ASSISTANT_FAKE_SOURCE_DISCOVERY)) {
        k_work_reschedule(&show_complete_work, K_MSEC(1200));
    }
}

static void on_sink_selected(lea_assistant_device_t *device)
{
    int err;

    LOG_DBG("Sink %s selected", device->name);

    // Could use `MESSAGE_SUBTYPE_CONNECT_SINK`, but calling it directly so don't need to fill buffer and parse LTV
    err = connect_to_sink(&device->addr);
    if (err) {
        LOG_ERR("Failed to connect to sink (%d)", err);
        lea_assistant_ui_show_error(_root, "Sink failed", "Connection failed");
        return;
    }

    source_stage_visible = false;
    fake_source_added = false;
    completion_visible = false;
    lea_assistant_ui_show_connecting(_root, device->name);
}

static void lea_assistant_app_start(lv_obj_t *root, lv_group_t *group)
{
    _root = root;
    source_stage_visible = false;
    fake_source_added = false;
    completion_visible = false;
    k_work_cancel_delayable(&show_complete_work);
    k_work_cancel_delayable(&close_app_work);

    LOG_DBG("LEA Assistant app start");

    message_handler(&(struct webusb_message ) {
        .sub_type = MESSAGE_SUBTYPE_START_SINK_SCAN
    }, 0);

    lea_assistant_ui_show(_root, on_sink_selected, close_app);
}

static void lea_assistant_app_stop(void)
{
    k_work_cancel_delayable(&show_complete_work);
    k_work_cancel_delayable(&close_app_work);

    lea_assistant_ui_remove();

    message_handler(&(struct webusb_message ) {
        .sub_type = MESSAGE_SUBTYPE_STOP_SCAN
    }, 0);

    /// @todo disconnect PA sync

    _root = NULL;
    source_stage_visible = false;
    fake_source_added = false;
    completion_visible = false;
}

static int lea_assistant_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

void lea_assistant_app_add_source_entry(lea_assistant_device_t *device)
{
    lea_assistant_ui_add_source_list_entry(device);
}

void lea_assistant_app_add_sink_entry(lea_assistant_device_t *device)
{
    lea_assistant_ui_add_sink_list_entry(device);
}

void lea_assistant_app_sink_connected(void)
{
    message_handler(&(struct webusb_message ) {
        .sub_type = MESSAGE_SUBTYPE_START_SOURCE_SCAN
    }, 0);

    lv_async_call(show_source_stage_async, NULL);
}

void lea_assistant_app_source_added(void)
{
    k_work_cancel_delayable(&show_complete_work);
    lv_async_call(show_complete_async, NULL);
}

SYS_INIT(lea_assistant_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
