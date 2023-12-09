#include "calendar_ui.h"
#include "app_version.h"
#include <zsw_clock.h>

static lv_obj_t *root_page = NULL;

void calendar_ui_show(lv_obj_t *root)
{
    assert(root_page == NULL);

    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    
    lv_obj_t * calendar = lv_calendar_create(root_page);
    lv_obj_set_size(calendar, 180, 180);
    struct tm *time = zsw_clock_get_time();
    //lv_calendar_set_showed_date(calendar, 2024, 12);
    //lv_calendar_set_today_date(calendar, 2024, 12,20);
    int year =  time->tm_year + 1900;
    int month = time->tm_mon + 1;
    lv_calendar_set_showed_date(calendar, year, month);
    lv_calendar_set_today_date(calendar, year, month, time->tm_mday);
    lv_calendar_header_arrow_create(calendar);
    lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 0);
}

void calendar_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}
