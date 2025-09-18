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

#include <lvgl.h>
#include <inttypes.h>

#include "managers/zsw_notification_manager.h"
#include "ui/utils/zsw_ui_utils.h"

typedef struct {
    zsw_not_mngr_notification_t *notification;
    lv_obj_t *deltaLabel;
    lv_obj_t *panel;
} active_notification_t;

typedef void(*on_notification_remove_cb_t)(uint32_t id);

void notifications_ui_page_init(on_notification_remove_cb_t not_removed_cb);

void notifications_ui_page_create(lv_obj_t *parent, lv_group_t *group);

void notifications_ui_page_close(void);

void notifications_ui_add_notification(zsw_not_mngr_notification_t *not, lv_group_t *group);

void notifications_ui_remove_notification(uint32_t id);
