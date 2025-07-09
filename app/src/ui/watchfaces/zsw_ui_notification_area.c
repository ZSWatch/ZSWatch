#include "lvgl.h"
#include "zsw_ui_notification_area.h"
#include "ui/utils/zsw_ui_utils.h"

ZSW_LV_IMG_DECLARE(ui_img_chat_png);    // assets/chat.png
ZSW_LV_IMG_DECLARE(ui_img_bluetooth_png);    // assets/bluetooth.png

static void delete_event(lv_event_t *e)
{
    k_free(lv_event_get_user_data(e));
}

zsw_ui_notification_area_t *zsw_ui_notification_area_add(lv_obj_t *parent)
{
    zsw_ui_notification_area_t *not_area = k_malloc(sizeof(zsw_ui_notification_area_t));
    __ASSERT_NO_MSG(not_area);
    memset(not_area, 0, sizeof(zsw_ui_notification_area_t));
    not_area->ui_notifications_container = lv_obj_create(parent);
    // When the lvgl object is deleted, then we also free the memory allocated for the zsw_ui_notification_area_t.
    lv_obj_add_event_cb(not_area->ui_notifications_container, delete_event, LV_EVENT_DELETE, not_area);

    lv_obj_set_width(not_area->ui_notifications_container, LV_SIZE_CONTENT);
    lv_obj_set_height(not_area->ui_notifications_container, LV_SIZE_CONTENT);
    lv_obj_set_align(not_area->ui_notifications_container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(not_area->ui_notifications_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(not_area->ui_notifications_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(not_area->ui_notifications_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(not_area->ui_notifications_container, lv_color_hex(0xFFFFFF),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(not_area->ui_notifications_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(not_area->ui_notifications_container, lv_color_hex(0x000000),
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(not_area->ui_notifications_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(not_area->ui_notifications_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(not_area->ui_notifications_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(not_area->ui_notifications_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(not_area->ui_notifications_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    not_area->ui_notification_icon = lv_img_create(not_area->ui_notifications_container);
    lv_img_set_src(not_area->ui_notification_icon, ZSW_LV_IMG_USE(ui_img_chat_png));
    lv_obj_set_width(not_area->ui_notification_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(not_area->ui_notification_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(not_area->ui_notification_icon, LV_ALIGN_CENTER);
    lv_obj_clear_flag(not_area->ui_notification_icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(not_area->ui_notification_icon, LV_OBJ_FLAG_HIDDEN);

    not_area->ui_notification_count_label = lv_label_create(not_area->ui_notification_icon);
    lv_obj_set_width(not_area->ui_notification_count_label, LV_SIZE_CONTENT);
    lv_obj_set_height(not_area->ui_notification_count_label, LV_SIZE_CONTENT);
    lv_obj_set_x(not_area->ui_notification_count_label, -3);
    lv_obj_set_y(not_area->ui_notification_count_label, -3);
    lv_obj_set_align(not_area->ui_notification_count_label, LV_ALIGN_CENTER);
    lv_label_set_text(not_area->ui_notification_count_label, "");
    lv_obj_set_style_text_font(not_area->ui_notification_count_label, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    not_area->ui_bt_icon = lv_img_create(not_area->ui_notifications_container);
    lv_img_set_src(not_area->ui_bt_icon, ZSW_LV_IMG_USE(ui_img_bluetooth_png));
    lv_obj_set_width(not_area->ui_bt_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(not_area->ui_bt_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(not_area->ui_bt_icon, LV_ALIGN_CENTER);
    lv_obj_clear_flag(not_area->ui_bt_icon, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(not_area->ui_bt_icon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_img_recolor(not_area->ui_bt_icon, lv_color_hex(0x0082FC), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(not_area->ui_bt_icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    return not_area;
}

void zsw_ui_notification_area_num_notifications(zsw_ui_notification_area_t *not_area, int num_notifications)
{
    if (!not_area) {
        return;
    }

    if (num_notifications == not_area->num_notifictions) {
        return;
    }

    not_area->num_notifictions = num_notifications;
    if (num_notifications > 0) {
        lv_label_set_text_fmt(not_area->ui_notification_count_label, "%d", num_notifications);
        lv_obj_clear_flag(not_area->ui_notification_icon, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(not_area->ui_notification_icon, LV_OBJ_FLAG_HIDDEN);
    }
}

void zsw_ui_notification_area_ble_connected(zsw_ui_notification_area_t *not_area, bool connected)
{
    if (!not_area) {
        return;
    }

    if (connected) {
        lv_obj_clear_flag(not_area->ui_bt_icon, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(not_area->ui_bt_icon, LV_OBJ_FLAG_HIDDEN);
    }
}