#include <qr_code/qr_code_ui.h>
#include <lvgl.h>
#include "assert.h"

static lv_obj_t *root_page = NULL;

void qr_code_ui_show(lv_obj_t *root)
{
    assert(root_page == NULL);

    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    // Don't want it to be scollable. Putting anything close the edges
    // then LVGL automatically makes the page scrollable and shows a scroll bar.
    // Does not loog very good on the round display.
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *img = lv_img_create(root_page);
    lv_img_set_src(img, "S:qr_code.bin");
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(img, 240, 240);
}

void qr_code_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}
