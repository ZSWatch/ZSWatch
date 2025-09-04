#include "mic_test_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include "assert.h"

static lv_obj_t *root_page = NULL;
static on_mic_test_ui_event_cb_t toggle_callback;
static lv_obj_t *toggle_button;
static lv_obj_t *button_label;
static lv_obj_t *status_label;
static bool is_recording = false;

static void toggle_button_event_cb(lv_event_t *e)
{
    if (toggle_callback) {
        toggle_callback();
    }
}

void mic_test_ui_show(lv_obj_t *root, on_mic_test_ui_event_cb_t toggle_cb)
{
    assert(root);
    toggle_callback = toggle_cb;

    // Create main container
    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));

    // Title label
    lv_obj_t *title = lv_label_create(root_page);
    lv_label_set_text(title, "Microphone Test");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Toggle button
    toggle_button = lv_btn_create(root_page);
    lv_obj_set_size(toggle_button, 120, 40);
    lv_obj_align(toggle_button, LV_ALIGN_CENTER, 0, -20);
    lv_obj_add_event_cb(toggle_button, toggle_button_event_cb, LV_EVENT_CLICKED, NULL);

    button_label = lv_label_create(toggle_button);
    lv_label_set_text(button_label, "Start");
    lv_obj_center(button_label);

    // Status label
    status_label = lv_label_create(root_page);
    lv_label_set_text(status_label, "Ready");
    lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(status_label, LV_ALIGN_CENTER, 0, 40);

    // Info text
    lv_obj_t *info_label = lv_label_create(root_page);
    lv_label_set_text(info_label, "Start/Stop recording test.\nRTT channel 2 for audio data.");
    lv_obj_set_style_text_align(info_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(info_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_align(info_label, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void mic_test_ui_remove(void)
{
    if (root_page) {
        lv_obj_del(root_page);
        root_page = NULL;
    }
}

void mic_test_ui_set_status(const char *status)
{
    if (status_label) {
        lv_label_set_text(status_label, status);
    }
}

void mic_test_ui_toggle_button_state(void)
{
    is_recording = !is_recording;
    if (button_label) {
        lv_label_set_text(button_label, is_recording ? "Stop" : "Start");
    }
}
