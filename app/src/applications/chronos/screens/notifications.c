

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/logging/log.h>

#include "../chronos_ui.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(chronos_notification, CONFIG_ZSW_BLE_LOG_LEVEL);

ZSW_LV_IMG_DECLARE(chronos_logo_icon);

ZSW_LV_IMG_DECLARE(chronos_sms_icon);      // SMS
ZSW_LV_IMG_DECLARE(chronos_mail_icon);     // Mail
ZSW_LV_IMG_DECLARE(chronos_penguin_icon);  // Penguin
ZSW_LV_IMG_DECLARE(chronos_skype_icon);    // Skype
ZSW_LV_IMG_DECLARE(chronos_whatsapp_icon); // WhatsApp
ZSW_LV_IMG_DECLARE(chronos_line_icon);     // Line
ZSW_LV_IMG_DECLARE(chronos_twitter_x_icon);// Twitter
ZSW_LV_IMG_DECLARE(chronos_facebook_icon); // Facebook
ZSW_LV_IMG_DECLARE(chronos_messenger_icon);// Messenger
ZSW_LV_IMG_DECLARE(chronos_instagram_icon);// Instagram
ZSW_LV_IMG_DECLARE(chronos_weibo_icon);    // Weibo
ZSW_LV_IMG_DECLARE(chronos_kakao_icon);    // Kakao
ZSW_LV_IMG_DECLARE(chronos_viber_icon);    // Viber
ZSW_LV_IMG_DECLARE(chronos_vkontakte_icon);// Vkontakte
ZSW_LV_IMG_DECLARE(chronos_telegram_icon); // Telegram
ZSW_LV_IMG_DECLARE(chronos_chrns_icon);    // Chronos
ZSW_LV_IMG_DECLARE(chronos_wechat_icon);     // Wechat

static const void *notificationIcons[] = {
    ZSW_LV_IMG_USE(chronos_sms_icon),       // SMS
    ZSW_LV_IMG_USE(chronos_mail_icon),      // Mail
    ZSW_LV_IMG_USE(chronos_penguin_icon),   // Penguin
    ZSW_LV_IMG_USE(chronos_skype_icon),     // Skype
    ZSW_LV_IMG_USE(chronos_whatsapp_icon),  // WhatsApp
    ZSW_LV_IMG_USE(chronos_mail_icon),      // Mail2
    ZSW_LV_IMG_USE(chronos_line_icon),      // Line
    ZSW_LV_IMG_USE(chronos_twitter_x_icon), // Twitter
    ZSW_LV_IMG_USE(chronos_facebook_icon),  // Facebook
    ZSW_LV_IMG_USE(chronos_messenger_icon), // Messenger
    ZSW_LV_IMG_USE(chronos_instagram_icon), // Instagram
    ZSW_LV_IMG_USE(chronos_weibo_icon),     // Weibo
    ZSW_LV_IMG_USE(chronos_kakao_icon),     // Kakao
    ZSW_LV_IMG_USE(chronos_viber_icon),     // Viber
    ZSW_LV_IMG_USE(chronos_vkontakte_icon), // Vkontakte
    ZSW_LV_IMG_USE(chronos_telegram_icon),  // Telegram
    ZSW_LV_IMG_USE(chronos_chrns_icon),     // Chronos
    ZSW_LV_IMG_USE(chronos_wechat_icon)     // Wechat
};

static lv_obj_t *notification_panel;

static lv_obj_t *ui_emptyNotification;

void chronos_ui_notifications_init(lv_obj_t *page)
{

    chronos_ui_add_app_title(page, "Notifications", ZSW_LV_IMG_USE(chronos_logo_icon));

    notification_panel = lv_obj_create(page);
    lv_obj_set_width(notification_panel, lv_pct(100));
    lv_obj_set_height(notification_panel, LV_SIZE_CONTENT); /// 50
    lv_obj_set_align(notification_panel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(notification_panel, LV_FLEX_FLOW_COLUMN_REVERSE);
    lv_obj_set_flex_align(notification_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(notification_panel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_radius(notification_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(notification_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(notification_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(notification_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(notification_panel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(notification_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    chronos_ui_clear_notifications(true);

}

int get_notification_icon_index(int id)
{
    switch (id) {
        case 0x03:
            return 0;
        case 0x04:
            return 1;
        case 0x07:
            return 2;
        case 0x08:
            return 3;
        case 0x0A:
            return 4;
        case 0x0B:
            return 5;
        case 0x0E:
            return 6;
        case 0x0F:
            return 7;
        case 0x10:
            return 8;
        case 0x11:
            return 9;
        case 0x12:
            return 10;
        case 0x13:
            return 11;
        case 0x14:
            return 12;
        case 0x16:
            return 13;
        case 0x17:
            return 14;
        case 0x18:
            return 15;
        case 0xC0:
            return 16;
        case 0x09:
            return 17;
        default:
            return 0;
    }
}

void chronos_ui_hide_notification_empty()
{
    if (notification_panel == NULL) {
        return;
    }
    if (ui_emptyNotification == NULL) {
        return;
    }
    lv_obj_add_flag(ui_emptyNotification, LV_OBJ_FLAG_HIDDEN);

}

void chronos_ui_clear_notifications(bool info)
{
    if (notification_panel == NULL) {
        return;
    }
    lv_obj_clean(notification_panel);

    if (!info) {
        return;
    }

    ui_emptyNotification = lv_label_create(notification_panel);
    lv_obj_set_width(ui_emptyNotification, lv_pct(80));
    lv_obj_set_height(ui_emptyNotification, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_emptyNotification, LV_ALIGN_CENTER);
    lv_label_set_text(ui_emptyNotification, "No notifications. Check back later");
    lv_obj_set_style_text_color(ui_emptyNotification, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_emptyNotification, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_emptyNotification, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_emptyNotification, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_emptyNotification, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_emptyNotification, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_emptyNotification, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_emptyNotification, 0, LV_PART_MAIN | LV_STATE_DEFAULT);


}

void chronos_ui_add_notification(chronos_notification_t notification)
{
    if (notification_panel == NULL) {
        return;
    }
    lv_obj_t *item = lv_obj_create(notification_panel);
    lv_obj_set_width(item, lv_pct(100));
    lv_obj_set_height(item, LV_SIZE_CONTENT); /// 50
    lv_obj_set_align(item, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(item, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(item, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_radius(item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(item, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(item, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(item, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(item, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(item, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *title_panel = lv_obj_create(item);
    lv_obj_set_width(title_panel, lv_pct(100));
    lv_obj_set_height(title_panel, LV_SIZE_CONTENT); /// 50
    lv_obj_set_align(title_panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(title_panel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_radius(title_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(title_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(title_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(title_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(title_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(title_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(title_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(title_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *app_icon = lv_img_create(title_panel);
    lv_img_set_src(app_icon, notificationIcons[get_notification_icon_index(notification.icon)]);
    lv_obj_set_width(app_icon, LV_SIZE_CONTENT);         /// 100
    lv_obj_set_height(app_icon, LV_SIZE_CONTENT);        /// 1
    lv_obj_add_flag(app_icon, LV_OBJ_FLAG_ADV_HITTEST);  /// Flags
    lv_obj_clear_flag(app_icon, LV_OBJ_FLAG_SCROLLABLE); /// Flags


    lv_obj_t *app_name = lv_label_create(title_panel);
    lv_obj_set_width(app_name, 170);  /// 1
    lv_obj_set_height(app_name, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(app_name, 40);
    lv_obj_set_y(app_name, 0);
    lv_label_set_long_mode(app_name, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(app_name, notification.title);
    lv_obj_set_style_text_font(app_name, CHRONOS_FONT_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    LV_FONT_DECLARE(lv_font_montserrat_14);
    lv_obj_t *time_label = lv_label_create(title_panel);
    lv_obj_set_width(time_label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(time_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(time_label, LV_ALIGN_BOTTOM_RIGHT);
    lv_label_set_text_fmt(time_label, "%02d:%02d", notification.time.hour, notification.time.minute);
    lv_obj_set_style_text_font(time_label, CHRONOS_FONT_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *message_label = lv_label_create(item);
    lv_obj_set_width(message_label, lv_pct(100));
    lv_obj_set_height(message_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(message_label, LV_ALIGN_CENTER);
    lv_label_set_text(message_label, notification.message);
    lv_obj_set_style_text_font(message_label, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);
}
