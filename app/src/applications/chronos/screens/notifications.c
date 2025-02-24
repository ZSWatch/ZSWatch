



#include "../chronos_ui.h"
#include "ui/utils/zsw_ui_utils.h"


ZSW_LV_IMG_DECLARE(image_chronos_icon);

ZSW_LV_IMG_DECLARE(image_sms);      // SMS
ZSW_LV_IMG_DECLARE(image_mail);     // Mail
ZSW_LV_IMG_DECLARE(image_penguin);  // Penguin
ZSW_LV_IMG_DECLARE(image_skype);    // Skype
ZSW_LV_IMG_DECLARE(image_whatsapp); // WhatsApp
ZSW_LV_IMG_DECLARE(image_line);     // Line
ZSW_LV_IMG_DECLARE(image_twitter_x);// Twitter
ZSW_LV_IMG_DECLARE(image_facebook); // Facebook
ZSW_LV_IMG_DECLARE(image_messenger);// Messenger
ZSW_LV_IMG_DECLARE(image_instagram);// Instagram
ZSW_LV_IMG_DECLARE(image_weibo);    // Weibo
ZSW_LV_IMG_DECLARE(image_kakao);    // Kakao
ZSW_LV_IMG_DECLARE(image_viber);    // Viber
ZSW_LV_IMG_DECLARE(image_vkontakte);// Vkontakte
ZSW_LV_IMG_DECLARE(image_telegram); // Telegram
ZSW_LV_IMG_DECLARE(image_chrns);    // Chronos
ZSW_LV_IMG_DECLARE(image_wechat);     // Wechat

static const void *notificationIcons[] = {
    &image_sms,       // SMS
    &image_mail,      // Mail
    &image_penguin,   // Penguin
    &image_skype,     // Skype
    &image_whatsapp,  // WhatsApp
    &image_mail,      // Mail2
    &image_line,      // Line
    &image_twitter_x, // Twitter
    &image_facebook,  // Facebook
    &image_messenger, // Messenger
    &image_instagram, // Instagram
    &image_weibo,     // Weibo
    &image_kakao,     // Kakao
    &image_viber,     // Viber
    &image_vkontakte, // Vkontakte
    &image_telegram,  // Telegram
    &image_chrns,     // Chronos
    &image_wechat     // Wechat
};

static lv_obj_t *notification_panel;

void notifications_init(lv_obj_t *page)
{

    add_app_title(page, "Notifications", ZSW_LV_IMG_USE(image_chronos_icon));

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
}

int getNotificationIconIndex(int id)
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

void add_notification(int id, const char *title, const char *time, const char *message)
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
    lv_img_set_src(app_icon, notificationIcons[getNotificationIconIndex(id)]);
    lv_obj_set_width(app_icon, LV_SIZE_CONTENT);         /// 100
    lv_obj_set_height(app_icon, LV_SIZE_CONTENT);        /// 1
    lv_obj_add_flag(app_icon, LV_OBJ_FLAG_ADV_HITTEST);  /// Flags
    lv_obj_clear_flag(app_icon, LV_OBJ_FLAG_SCROLLABLE); /// Flags

    lv_obj_t *app_name = lv_label_create(title_panel);
    lv_obj_set_width(app_name, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(app_name, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(app_name, 45);
    lv_obj_set_y(app_name, 0);
    lv_label_set_text(app_name, title);
    lv_obj_set_style_text_font(app_name, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    LV_FONT_DECLARE(lv_font_montserrat_14);
    lv_obj_t *time_label = lv_label_create(title_panel);
    lv_obj_set_width(time_label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(time_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(time_label, LV_ALIGN_BOTTOM_RIGHT);
    lv_label_set_text(time_label, time);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *message_label = lv_label_create(item);
    lv_obj_set_width(message_label, lv_pct(100));
    lv_obj_set_height(message_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(message_label, LV_ALIGN_CENTER);
    lv_label_set_text(message_label, message);
    lv_obj_set_style_text_font(message_label, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
}