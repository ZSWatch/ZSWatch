/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include "app_version.h"
#include <version.h>
#include <ncs_version.h>
#include "update_ui.h"
#include "managers/zsw_app_manager.h"
#include "managers/zsw_xip_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "filesystem/zsw_filesystem.h"
#include <zephyr/logging/log.h>
#include <zcbor_decode.h>
#include <zephyr/usb/usb_device.h>
#ifndef CONFIG_ARCH_POSIX
#include <zephyr/mgmt/mcumgr/mgmt/callbacks.h>
#include <zephyr/mgmt/mcumgr/grp/img_mgmt/img_mgmt.h>
#include <zephyr/mgmt/mcumgr/transport/smp_bt.h>
#endif

LOG_MODULE_REGISTER(update_app, LOG_LEVEL_INF);

static void update_app_start(lv_obj_t *root, lv_group_t *group);
static void update_app_stop(void);

ZSW_LV_IMG_DECLARE(templates);

static application_t app = {
    .name = "Update",
    .icon = ZSW_LV_IMG_USE(templates),
    .start_func = update_app_start,
    .stop_func = update_app_stop,
    .category = ZSW_APP_CATEGORY_SYSTEM
};

#ifndef CONFIG_ARCH_POSIX
static bool dfu_in_progress = false;
static bool ble_fota_enabled = false;
static bool usb_fota_enabled = false;

static zcbor_state_t decode_img_data(const struct zcbor_string *img_data)
{
    zcbor_state_t state;
    zcbor_new_state(&state, 1, img_data->value, img_data->len, 0, NULL, 0);

    struct zcbor_string decoded_string;
    if (zcbor_tstr_decode(&state, &decoded_string)) {
        LOG_INF("Decoded string: %.*s", decoded_string.len, decoded_string.value);
    } else {
        LOG_ERR("Failed to decode CBOR string");
    }

    return state;
}

static enum mgmt_cb_return upload_confirm_handler(uint32_t event, enum mgmt_cb_return prev_status,
                                                  int32_t *rc, uint16_t *group, bool *abort_more,
                                                  void *data, size_t data_size)
{
    const struct img_mgmt_upload_check *img_data = (const struct img_mgmt_upload_check *)data;

    if (!dfu_in_progress) {
        dfu_in_progress = true;
        zcbor_state_t state = decode_img_data(&img_data->req->img_data);
        LOG_INF("DFU started: Image %u, Offset %u, Size %llu", img_data->req->image, img_data->req->off,
                img_data->action->size);
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            char status_msg[64];
            snprintf(status_msg, sizeof(status_msg), "%s", state.payload);
            update_ui_set_status(status_msg);
        }
    } else {
        LOG_INF("DFU in progress: Image %u, Offset %u, Size %llu", img_data->req->image, img_data->req->off,
                img_data->action->size);
    }

    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        update_ui_set_progress((img_data->req->off * 100) / img_data->action->size);
    }

    return MGMT_CB_OK;
}

static enum mgmt_cb_return dfu_stopped_handler(uint32_t event, enum mgmt_cb_return prev_status,
                                               int32_t *rc, uint16_t *group, bool *abort_more,
                                               void *data, size_t data_size)
{
    if (dfu_in_progress) {
        dfu_in_progress = false;
        LOG_INF("DFU stopped");
    }

    return MGMT_CB_OK;
}

static enum mgmt_cb_return dfu_status_callback(uint32_t event, enum mgmt_cb_return prev_status,
                                               int32_t *rc, uint16_t *group, bool *abort_more,
                                               void *data, size_t data_size)
{
    LOG_INF("DFU started event received");
    return MGMT_CB_OK;
}

static struct mgmt_callback upload_callback = {
    .callback = upload_confirm_handler,
    .event_id = MGMT_EVT_OP_IMG_MGMT_DFU_CHUNK,
};

static struct mgmt_callback stopped_callback = {
    .callback = dfu_stopped_handler,
    .event_id = MGMT_EVT_OP_IMG_MGMT_DFU_STOPPED,
};

static struct mgmt_callback started_callback = {
    .callback = dfu_status_callback,
    .event_id = MGMT_EVT_OP_IMG_MGMT_DFU_STARTED,
};

static bool toggle_usb_fota(void)
{
    usb_fota_enabled = !usb_fota_enabled;

    if (usb_fota_enabled) {
        if (IS_ENABLED(CONFIG_USB_DEVICE_STACK)) {
            zsw_xip_enable();
            int ret = usb_enable(NULL);
            if (ret && ret != -EALREADY) {
                LOG_ERR("Failed to enable USB: %d", ret);
                update_ui_set_status("Status: USB enable failed");
                usb_fota_enabled = false; // Revert state
                zsw_xip_disable();
                update_ui_update_usb_button_state(false);
                return false;
            }
            if (ret == -EALREADY) {
                LOG_INF("USB stack already enabled");
            } else {
                LOG_INF("USB stack enabled");
            }
            update_ui_set_status("Status: USB FOTA enabled - Ready for updates");
            update_ui_update_usb_button_state(true);
            return true;
        }
    } else {
        if (IS_ENABLED(CONFIG_USB_DEVICE_STACK)) {
            int ret = usb_disable();
            if (ret) {
                LOG_ERR("Failed to disable USB");
                update_ui_set_status("Status: USB disable failed");
                usb_fota_enabled = true; // Revert state
                update_ui_update_usb_button_state(true);
                return false;
            }
            zsw_xip_disable();
            update_ui_set_status("Status: USB FOTA disabled");
            LOG_INF("USB stack disabled");
            update_ui_update_usb_button_state(false);
            return true;
        }
    }
    update_ui_update_usb_button_state(false);
    return false;
}

static bool toggle_ble_fota(void)
{
    if (!ble_fota_enabled) {
        // Enable XIP before enabling MCUmgr as MCUmgr code is in XIP
        zsw_xip_enable();
        int rc = smp_bt_register();
        if (rc != 0) {
            LOG_ERR("Failed to register BLE SMP: %d", rc);
            update_ui_set_status("Status: BLE FOTA enable failed");
            update_ui_update_ble_button_state(false);
            zsw_xip_disable();
            return false;
        }
        ble_fota_enabled = true;
        update_ui_set_status("Status: BLE FOTA enabled - Ready for updates");
        LOG_INF("BLE FOTA enabled");
        update_ui_update_ble_button_state(true);
        return true;
    } else {
        int rc = smp_bt_unregister();
        if (rc != 0) {
            LOG_ERR("Failed to unregister BLE SMP: %d", rc);
            update_ui_set_status("Status: BLE FOTA disable failed");
            update_ui_update_ble_button_state(true);
            return false;
        }
        ble_fota_enabled = false;
        // Disable XIP after disabling MCUmgr
        zsw_xip_disable();
        update_ui_set_status("Status: BLE FOTA disabled");
        LOG_INF("BLE FOTA disabled");
        update_ui_update_ble_button_state(false);
        return true;
    }
}

#else
static bool toggle_ble_fota(void)
{
    update_ui_set_status("Status: BLE FOTA not available on POSIX");
    update_ui_update_ble_button_state(false);
    return false;
}

static bool toggle_usb_fota(void)
{
    update_ui_set_status("Status: USB FOTA not available on POSIX");
    update_ui_update_usb_button_state(false);
    return false;
}
#endif

static void update_app_start(lv_obj_t *root, lv_group_t *group)
{
    update_ui_show(root, toggle_ble_fota, toggle_usb_fota);
}

static void update_app_stop(void)
{
    update_ui_remove();
}

static int update_app_add(void)
{
    zsw_app_manager_add_application(&app);
#ifndef CONFIG_ARCH_POSIX
    mgmt_callback_register(&started_callback);
    mgmt_callback_register(&upload_callback);
    mgmt_callback_register(&stopped_callback);

    ble_fota_enabled = false;
    usb_fota_enabled = false;

    int rc = smp_bt_unregister();
    if (rc != 0) {
        LOG_WRN("BLE SMP already unregistered or failed to unregister. Check init priority. Error: %d", rc);
    }
#endif
    return 0;
}

// CONFIG_APPLICATION_INIT_PRIORITY + 1 as we can't call smp_bt before it has been initialized
// which is done at CONFIG_APPLICATION_INIT_PRIORITY
SYS_INIT(update_app_add, APPLICATION, 91);
