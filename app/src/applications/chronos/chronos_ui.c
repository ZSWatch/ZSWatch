

#include "chronos_ui.h"
#include "ui/utils/zsw_ui_utils.h"

#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>

#include "ble/chronos/ble_chronos.h"

ZSW_LV_IMG_DECLARE(image_chronos_icon);



static lv_obj_t *root_page = NULL;



lv_obj_t *add_page(lv_obj_t *parent)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_set_width(page, lv_pct(100));
    lv_obj_set_height(page, lv_pct(100));
    lv_obj_set_align(page, LV_ALIGN_CENTER);
    lv_obj_add_flag(page, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_add_flag(page, LV_OBJ_FLAG_SNAPPABLE);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_style_bg_color(page, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(page, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(page, LV_DIR_VER);
    lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // lv_obj_set_style_bg_color(page, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(page, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(page, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(page, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(page, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    return page;
}

void add_app_title(lv_obj_t *parent, const char *title, const void *src)
{
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_set_width(panel, lv_pct(100));
    lv_obj_set_height(panel, LV_SIZE_CONTENT); /// 70
    lv_obj_set_align(panel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(panel, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(panel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *app_icon = lv_img_create(panel);
    lv_img_set_src(app_icon, src);
    lv_obj_set_width(app_icon, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(app_icon, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(app_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(app_icon, LV_OBJ_FLAG_ADV_HITTEST);  /// Flags
    lv_obj_clear_flag(app_icon, LV_OBJ_FLAG_SCROLLABLE); /// Flags

    lv_obj_t *app_label = lv_label_create(panel);
    lv_obj_set_width(app_label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(app_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(app_label, LV_ALIGN_CENTER);
    lv_label_set_text(app_label, title);
    lv_obj_set_style_text_font(app_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void on_notification_received_cb(const chronos_notification_t *notification)
{
    add_notification(notification->icon, notification->app, notification->time, notification->message);
}


void chronos_ui_init(lv_obj_t *root)
{
    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_style_bg_color(root_page, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(root_page, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(root_page, lv_pct(100));
    lv_obj_set_height(root_page, lv_pct(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(root_page, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(root_page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_add_flag(root_page, LV_OBJ_FLAG_SCROLL_ONE); /// Flags
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_snap_x(root_page, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scroll_dir(root_page, LV_DIR_HOR);
    lv_obj_set_style_radius(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(root_page, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(root_page, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(root_page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *page = add_page(root_page);
    notifications_init(page);

    for (int i = 0; i < 10; i++) {
        chronos_notification_t *notif = get_notification(i);
        if (notif->available) {
            add_notification(notif->icon, notif->app, notif->time, notif->message);
        }

    }

    page = add_page(root_page);
    weather_init(page);

    for (int i = 0; i < 7; i++) {
        add_weekly("SUN", ZSW_LV_IMG_USE(image_chronos_icon), 16 + 7);
    }

    page = add_page(root_page);
    navigation_init(page);


    navIconState(false);
    navigateInfo("Inactive", "Chronos", "Start navigation on Google maps");

    // for (int i = 0; i < 5; i++)
    // {
    //     lv_obj_t *test = add_page(root_page);

    //     add_app_title(test, "Test", ZSW_LV_IMG_USE(image_chronos_icon));

    //     lv_obj_t *label2 = lv_label_create(test);
    //     lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
    //     lv_obj_set_width(label2, 150);
    //     lv_label_set_text(label2, "It is a circularly scrolling text. ");
    //     lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
    // }

    register_notification_callback(on_notification_received_cb);

    send_info();
}

void chronos_ui_deinit()
{

    register_notification_callback(NULL);

    lv_obj_del(root_page);
    root_page = NULL;
}


