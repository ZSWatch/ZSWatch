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

LV_IMG_DECLARE(auracast);

static lv_obj_t *_root = NULL;

static application_t app = {
    .name = "LEA Assistant",
    .icon = &auracast,
    .start_func = lea_assistant_app_start,
    .stop_func = lea_assistant_app_stop
};

static void close_app(void)
{
    zsw_app_manager_app_close_request(&app);
}

static void on_source_selected(lea_assistant_device_t *device)
{
    LOG_DBG("Source %s selected", device->name);

    // Could use `MESSAGE_SUBTYPE_ADD_SOURCE`, but calling it directly so don't need to fill buffer and parse LTV
    add_source(device->sid, device->pa_interval, device->broadcast_id, &device->addr);
    close_app();
}

static void on_sink_selected(lea_assistant_device_t *device)
{
    LOG_DBG("Sink %s selected", device->name);

    // Could use `MESSAGE_SUBTYPE_CONNECT_SINK`, but calling it directly so don't need to fill buffer and parse LTV
    connect_to_sink(&device->addr);

    /// @todo connecting screen

    lea_assistant_ui_show_source(_root, on_source_selected);
}

static void lea_assistant_app_start(lv_obj_t *root, lv_group_t *group)
{
    _root = root;
    LOG_DBG("LEA Assistant app start");

    message_handler(&(struct webusb_message ) {
        .sub_type = MESSAGE_SUBTYPE_START_SINK_SCAN
    }, 0);

    lea_assistant_ui_show(_root, on_sink_selected, close_app);
}

static void lea_assistant_app_stop(void)
{
    lea_assistant_ui_remove();

    message_handler(&(struct webusb_message ) {
        .sub_type = MESSAGE_SUBTYPE_STOP_SCAN
    }, 0);

    /// @todo disconnect PA sync
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

void lea_assistant_app_sink_connected()
{
    message_handler(&(struct webusb_message ) {
        .sub_type = MESSAGE_SUBTYPE_START_SOURCE_SCAN
    }, 0);
}

SYS_INIT(lea_assistant_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
