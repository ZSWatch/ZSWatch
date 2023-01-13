#ifndef __LV_NOTIFCATION_H
#define __LV_NOTIFCATION_H
#include <lvgl.h>
#include <notification_manager.h>

typedef void(*on_close_not_cb_t)(uint32_t id);


void lv_notification_show(char *title, char *body, notification_src_t icon, uint32_t id, on_close_not_cb_t close_cb,
                          uint32_t close_after_seconds);
void lv_notification_remove(void);
#endif