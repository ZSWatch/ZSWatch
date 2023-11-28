#include "speedometer_ui.h"

#define MIN_SPEED 0
#define MAX_SPEED 35

static lv_obj_t *meter = NULL;
static lv_meter_indicator_t *indic;

void speedometer_set_value(uint32_t speed)
{
    lv_meter_set_indicator_value(meter, indic, speed);
}

void speedometer_ui_show(lv_obj_t *root)
{
    assert(meter == NULL);

    // Create the root container
    meter = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(meter, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(meter, LV_PCT(100), LV_PCT(100));
    // Don't want it to be scollable. Putting anything close the edges
    // then LVGL automatically makes the page scrollable and shows a scroll bar.
    // Does not loog very good on the round display.
    lv_obj_set_scrollbar_mode(meter, LV_SCROLLBAR_MODE_OFF);

    // create meter object
    meter = lv_meter_create(lv_scr_act());
    lv_obj_center(meter);
    lv_obj_set_size(meter, 200, 200);

    /*Create a label below the meter */
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "km/h");
    lv_obj_align_to(label, meter, LV_ALIGN_BOTTOM_MID, 0, -15);    /*Align top of the slider*/

    /*Add a scale first*/
    lv_meter_scale_t *scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 31, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter, scale, 6, 4, 15, lv_color_black(), 10);

    /*Add a blue arc to the start*/
    indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter, indic, MIN_SPEED);
    lv_meter_set_indicator_end_value(meter, indic, MIN_SPEED + 10);

    /*Make the tick lines blue at the start of the scale*/
    indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(meter, indic, MIN_SPEED);
    lv_meter_set_indicator_end_value(meter, indic, MIN_SPEED + 10);

    /*Add a red arc to the end*/
    indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter, indic, MAX_SPEED - 10);
    lv_meter_set_indicator_end_value(meter, indic, MAX_SPEED);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                     0);
    lv_meter_set_indicator_start_value(meter, indic, MAX_SPEED - 10);
    lv_meter_set_indicator_end_value(meter, indic, MAX_SPEED);

    /*Add a needle line indicator*/
    indic = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    /*Set the meter scale range*/
    lv_meter_set_scale_range(meter, scale, MIN_SPEED, MAX_SPEED, 270, 135);
}

void speedometer_ui_remove(void)
{
    lv_obj_del(meter);
    meter = NULL;
}
