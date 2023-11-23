#pragma once

#include <inttypes.h>
#include <lvgl.h>

#include "managers/zsw_notification_manager.h"

typedef void(*on_notification_remove_cb_t)(uint32_t id);

void notifications_ui_page_init(on_notification_remove_cb_t not_removed_cb);

void notifications_ui_page_create(zsw_not_mngr_notification_t *notifications, uint8_t num_notifications,
                               lv_group_t *input_group);

void notifications_ui_page_close(void);

void notifications_ui_add_notification(zsw_not_mngr_notification_t *not, lv_group_t *group);