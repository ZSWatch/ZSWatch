#pragma once

#include <lvgl.h>
#include <inttypes.h>

#include "managers/zsw_notification_manager.h"

typedef struct {
    zsw_not_mngr_notification_t *notification;
    lv_obj_t *deltaLabel;
    lv_obj_t *panel;
} active_notification_t;

LV_IMG_DECLARE(ui_img_whatsapp_png);
LV_IMG_DECLARE(ui_img_gadget_png);
LV_IMG_DECLARE(ui_img_mail_png);
LV_IMG_DECLARE(ui_img_call_png);
LV_IMG_DECLARE(ui_img_trash_png);
LV_IMG_DECLARE(ui_img_youtube_png);
LV_IMG_DECLARE(ui_img_homeassistant_png);
LV_IMG_DECLARE(ui_img_discord_png);
LV_IMG_DECLARE(ui_img_linkedin_png);

typedef void(*on_notification_remove_cb_t)(uint32_t id);

void notifications_ui_page_init(on_notification_remove_cb_t not_removed_cb);

void notifications_ui_page_create(lv_obj_t *parent, lv_group_t *group);

void notifications_ui_page_close(void);

void notifications_ui_add_notification(zsw_not_mngr_notification_t *not, lv_group_t *group);

void notifications_ui_remove_notification(uint32_t id);