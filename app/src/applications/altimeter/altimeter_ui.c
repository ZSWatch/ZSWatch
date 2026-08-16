#include "altimeter_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include <stdio.h>

// Keep your altimeter_ui_map and lv_image_dsc_t altimeter_ui here...

static lv_obj_t *root_page = NULL;
static lv_obj_t *alt_label = NULL;
static lv_obj_t *pressure_label = NULL;
static lv_obj_t *status_label = NULL;

void altimeter_ui_show(lv_obj_t *root)
{
    root_page = lv_obj_create(root);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(root_page, lv_color_hex(0x000000), LV_PART_MAIN);

    // Altitude Label (Big Text)
    alt_label = lv_label_create(root_page);
    lv_obj_set_style_text_font(alt_label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_label_set_text(alt_label, "-- m");
    lv_obj_align(alt_label, LV_ALIGN_CENTER, 0, -10);

    // Pressure Label
    pressure_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(pressure_label, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_label_set_text(pressure_label, "-- hPa");
    lv_obj_align(pressure_label, LV_ALIGN_CENTER, 0, 25);

    // Status Label
    status_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(status_label, lv_color_hex(0xff0000), LV_PART_MAIN); // Red until calibrated
    lv_label_set_text(status_label, "Uncalibrated");
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void altimeter_ui_remove(void)
{
    if (root_page != NULL) {
        lv_obj_del(root_page);
        root_page = NULL;
    }
}

void altimeter_ui_update(float altitude, float raw_pressure, bool is_calibrated)
{
    if (root_page == NULL) return; // Guard to prevent crashing if screen is off

    // Update Text
    lv_label_set_text_fmt(alt_label, "%.0f m", altitude);
    lv_label_set_text_fmt(pressure_label, "%.1f hPa", raw_pressure);

    // Update Status Label styling
    if (is_calibrated) {
        lv_label_set_text(status_label, "Calibrated");
        lv_obj_set_style_text_color(status_label, lv_color_hex(0x00FF00), LV_PART_MAIN); // Green
    } else {
        lv_label_set_text(status_label, "Uncalibrated");
        lv_obj_set_style_text_color(status_label, lv_color_hex(0xFF0000), LV_PART_MAIN); // Red
    }
}