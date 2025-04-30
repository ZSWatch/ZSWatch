#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include "app_version.h"
#include <version.h>
#include <ncs_version.h>
#include "update_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "filesystem/zsw_filesystem.h"
#include <zephyr/mgmt/mcumgr/mgmt/callbacks.h>
#include <zephyr/logging/log.h>
#include <zephyr/mgmt/mcumgr/grp/img_mgmt/img_mgmt.h>
#include <zephyr/mgmt/mcumgr/transport/smp_bt.h>
#include <zephyr/usb/usb_device.h>
#include <zcbor_decode.h>

LOG_MODULE_REGISTER(update_app, LOG_LEVEL_INF);

static bool dfu_in_progress = false;

static void update_app_start(lv_obj_t *root, lv_group_t *group);
static void start_fw_update(void);
static void update_app_stop(void);

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
        char status_msg[64];
        snprintf(status_msg, sizeof(status_msg), "%s", state.payload);
        update_ui_set_status(status_msg);

    } else {
        LOG_INF("DFU in progress: Image %u, Offset %u, Size %llu", img_data->req->image, img_data->req->off,
                img_data->action->size);
    }

    update_ui_set_progress((img_data->req->off * 100) / img_data->action->size);

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

ZSW_LV_IMG_DECLARE(templates);

static application_t app = {
    .name = "Update",
    .icon = ZSW_LV_IMG_USE(templates),
    .start_func = update_app_start,
    .stop_func = update_app_stop
};

static void update_app_start(lv_obj_t *root, lv_group_t *group)
{
    update_ui_show(root, start_fw_update);
}

static void update_app_stop(void)
{
    update_ui_remove();
}

static void start_fw_update(void)
{
    // Trigger the DFU process
    update_ui_set_status("Status: Starting DFU...");
    int rc = smp_bt_register();
    if (rc != 0) {
        update_ui_set_status("Status: DFU Start Failed");
    } else {
        update_ui_set_status("Status: DFU Ready");
    }
}

static void fota_usb_state_changed(bool enabled)
{
    if (enabled) {
        LOG_INF("FOTA over USB has been enabled");
        if (IS_ENABLED(CONFIG_USB_DEVICE_STACK)) {
            int ret = usb_enable(NULL);
            if (ret) {
                LOG_ERR("Failed to enable USB");
                return;
            }

            // Suspend USB device
            ret = usb_disable();
            if (ret) {
                LOG_ERR("Failed to disable USB");
                return;
            }
            if (ret) {
                return;
            }
            LOG_INF("USB stack enabled");
        }
    } else {
        LOG_WRN("FOTA over USB disable not implemented");
        // Add logic to disable FOTA over USB
    }
}

static int update_app_add(void)
{
    zsw_app_manager_add_application(&app);

    mgmt_callback_register(&started_callback);
    mgmt_callback_register(&upload_callback);
    mgmt_callback_register(&stopped_callback);

    update_ui_set_fota_usb_callback(fota_usb_state_changed);

    int rc = smp_bt_unregister();
    if (rc != 0) {
        update_ui_set_status("Status: DFU failed to unregister");
    }

    return 0;
}

SYS_INIT(update_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);