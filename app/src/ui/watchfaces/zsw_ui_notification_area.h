/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
