#include "update_ui.h"
#include "ui/zsw_ui.h"
#include "filesystem/zsw_filesystem.h"
#include <assert.h>

static lv_obj_t *root_page = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *progress_bar = NULL;
static void (*fota_usb_callback)(bool enabled) = NULL;

void update_ui_set_fota_usb_callback(void (*fota_usb_cb)(bool enabled))
{
    fota_usb_callback = fota_usb_cb;
}

// Callback for the firmware update button
static void btn_fw_update_cb(lv_event_t *e)
{
    void (*start_fw_update_cb)(void) = lv_event_get_user_data(e);
    if (start_fw_update_cb) {
        start_fw_update_cb();
    }
}

static void btn_flash_erase_cb(lv_event_t *e)
{
    // Perform the flash erase operation
    update_ui_set_status("Status: Erasing flash... Don't do anything until it's done.");
    int rc = zsw_filesytem_erase();
    if (rc == 0) {
        update_ui_set_status("Status: Flash erase successful");
    } else {
        update_ui_set_status("Status: Flash erase failed");
    }
}

static void btn_fota_usb_cb(lv_event_t *e)
{
    static bool fota_usb_enabled = false;

    fota_usb_enabled = !fota_usb_enabled;

    if (fota_usb_enabled) {
        update_ui_set_status("Status: FOTA over USB Enabled");
    } else {
        update_ui_set_status("Status: FOTA over USB Disabled");
    }

    if (fota_usb_callback) {
        fota_usb_callback(fota_usb_enabled);
    }
}

void update_ui_show(lv_obj_t *root, void (*start_fw_update_cb)(void))
{
    assert(root_page == NULL);

    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_width(root_page, lv_pct(100));
    lv_obj_set_height(root_page, lv_pct(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Create a button for enabling firmware update
    lv_obj_t *btn_fw_update = lv_btn_create(root_page);
    lv_obj_set_size(btn_fw_update, 150, 50);
    lv_obj_align(btn_fw_update, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_add_event_cb(btn_fw_update, btn_fw_update_cb, LV_EVENT_CLICKED, start_fw_update_cb);
    lv_obj_t *btn_label = lv_label_create(btn_fw_update);
    lv_label_set_text(btn_label, "Start Update");
    lv_obj_center(btn_label);

    // Create a label for status text
    status_label = lv_label_create(root_page);
    lv_label_set_text(status_label, "Status: Idle");
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 80);

    // Create a progress bar for firmware update progress
    progress_bar = lv_bar_create(root_page);
    lv_obj_set_size(progress_bar, 200, 20);
    lv_obj_align(progress_bar, LV_ALIGN_TOP_MID, 0, 120);
    lv_bar_set_range(progress_bar, 0, 100);
    lv_bar_set_value(progress_bar, 0, LV_ANIM_OFF);

    // Create a button for erasing flash
    lv_obj_t *btn_flash_erase = lv_btn_create(root_page);
    lv_obj_set_size(btn_flash_erase, 150, 50);
    lv_obj_align(btn_flash_erase, LV_ALIGN_TOP_MID, 0, 180);
    lv_obj_add_event_cb(btn_flash_erase, btn_flash_erase_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *btn_erase_label = lv_label_create(btn_flash_erase);
    lv_label_set_text(btn_erase_label, "Erase Flash");
    lv_obj_center(btn_erase_label);

    // Create a button for enabling/disabling FOTA over USB
    lv_obj_t *btn_fota_usb = lv_btn_create(root_page);
    lv_obj_set_size(btn_fota_usb, 150, 50);
    lv_obj_align(btn_fota_usb, LV_ALIGN_TOP_MID, 0, 240);
    lv_obj_add_event_cb(btn_fota_usb, btn_fota_usb_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *btn_fota_usb_label = lv_label_create(btn_fota_usb);
    lv_label_set_text(btn_fota_usb_label, "Toggle FOTA USB");
    lv_obj_center(btn_fota_usb_label);
}

void update_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void update_ui_set_status(const char *status)
{
    if (status_label) {
        lv_label_set_text(status_label, status);
    }
}

void update_ui_set_progress(int progress)
{
    if (progress_bar) {
        lv_bar_set_value(progress_bar, progress, LV_ANIM_OFF);
    }
}