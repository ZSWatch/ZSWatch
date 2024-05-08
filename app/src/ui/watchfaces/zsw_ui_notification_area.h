#pragma once

#include "lvgl.h"

typedef struct {
    lv_obj_t *ui_notifications_container; // Must be kept first argument!
    lv_obj_t *ui_notification_icon;
    lv_obj_t *ui_notification_count_label;
    lv_obj_t *ui_bt_icon;
    int num_notifictions;
} zsw_ui_notification_area_t;

zsw_ui_notification_area_t *zsw_ui_notification_area_add(lv_obj_t *parent);

void zsw_ui_notification_area_num_notifications(zsw_ui_notification_area_t *not_area, int num_notifications);

void zsw_ui_notification_area_ble_connected(zsw_ui_notification_area_t *not_area, bool connected);
