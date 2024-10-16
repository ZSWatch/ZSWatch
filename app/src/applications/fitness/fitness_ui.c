#include "fitness_ui.h"
#include "ui/zsw_ui.h"

static lv_obj_t *root_page = NULL;

void fitness_ui_show(lv_obj_t *root)
{
    assert(root_page == NULL);

    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));

    lv_obj_set_width(root_page, lv_pct(100));
    lv_obj_set_height(root_page, lv_pct(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
}

void fitness_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}