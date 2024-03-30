#include <time.h>

#include "../notification_ui.h"
#include "ui/zsw_ui.h"

LV_FONT_DECLARE(lv_font_montserrat_14_full)

static on_notification_remove_cb_t notification_removed_callback;
static lv_obj_t *main_page;
static lv_obj_t *empty_label;
static lv_timer_t *timer;
static active_notification_t active_notifications[ZSW_NOTIFICATION_MGR_MAX_STORED];

/** @brief          Convert a time in seconds to a age string.
 *  @param delta    Time in seconds
 *  @param buf      Pointer to output buffer
*/
static void notification_delta2char(uint32_t delta, char *buf)
{
    uint32_t minutes = (delta % 3600) / 60;
    uint32_t hours = (delta % 86400) / 3600;
    uint32_t days = (delta % (86400 * 30)) / 86400;

    if (days > 0) {
        sprintf(buf, "%u d", days);
    } else if (hours > 0) {
        sprintf(buf, "%u h", hours);
    } else if (minutes > 0) {
        sprintf(buf, "%u min", minutes);
    } else {
        sprintf(buf, "Now");
    }
}

static bool any_notifiction(void)
{
    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (active_notifications[i].notification != NULL) {
            return true;
        }
    }
    return false;
}

/** @brief
 *  @param timer
*/
static void label_on_timer_callback(lv_timer_t *timer)
{
    char buf[16];
    uint32_t delta;

    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        // Make sure that the notification exists and prevent exceptions because of a fragmented array.
        if ((active_notifications[i].panel != NULL) && (active_notifications[i].deltaLabel != NULL)) {
            delta = time(NULL) - active_notifications[i].notification->timestamp;
            notification_delta2char(delta, buf);

            lv_label_set_text(active_notifications[i].deltaLabel, buf);
        }
    }
}

/** @brief
 *  @param event
*/
static void notification_on_clicked_callback(lv_event_t *event)
{
    uint32_t id;

    id = (uint32_t)lv_event_get_user_data(event);

    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if ((active_notifications[i].notification != NULL) && (active_notifications[i].notification->id == id)) {
            notification_removed_callback(id);
            break;
        }
    }

    if (!any_notifiction()) {
        lv_obj_clear_flag(empty_label, LV_OBJ_FLAG_HIDDEN);
    }
}

/** @brief
 *  @param parent
 *  @param not
 *  @param group
*/
static void build_notification_entry(lv_obj_t *parent, zsw_not_mngr_notification_t *not, lv_group_t *group)
{
    lv_obj_t *ui_Panel;
    lv_obj_t *ui_LabelSource;
    lv_obj_t *ui_LabelTimeDelta;
    lv_obj_t *ui_ImageIcon;
    lv_obj_t *ui_LabelHeader;
    lv_obj_t *ui_LabelBody;

    uint32_t index;

    const void *image_source;
    const char *source;
    char buf[16];

    // Look for a free location for the notification.
    // TODO: Make me better
    index = 0xFFFFFFFF;
    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if (active_notifications[i].notification == NULL) {
            index = i;

            break;
        }
    }

    if (index == 0xFFFFFFFF) {
        return;
    }

    image_source = zsw_ui_utils_icon_from_notification(not->src);
    source = zsw_ui_utils_source_from_notification(not->src);

    ui_Panel = lv_obj_create(parent);
    lv_obj_set_width(ui_Panel, 200);
    lv_obj_set_height(ui_Panel, 100);
    lv_obj_set_align(ui_Panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Panel, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_style_bg_color(ui_Panel, zsw_color_gray(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_Panel, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_Panel, notification_on_clicked_callback, LV_EVENT_LONG_PRESSED, (void *)not->id);

    ui_LabelSource = lv_label_create(ui_Panel);
    lv_obj_set_width(ui_LabelSource, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelSource, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LabelSource, 15);
    lv_obj_set_y(ui_LabelSource, -30);
    lv_obj_set_align(ui_LabelSource, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_LabelSource, source);
    lv_obj_clear_flag(ui_LabelSource, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                      LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_color(ui_LabelSource, lv_color_hex(0x8C8C8C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LabelSource, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LabelTimeDelta = lv_label_create(ui_Panel);
    lv_obj_set_width(ui_LabelTimeDelta, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelTimeDelta, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LabelTimeDelta, 70);
    lv_obj_set_y(ui_LabelTimeDelta, -30);
    lv_obj_set_align(ui_LabelTimeDelta, LV_ALIGN_CENTER);

    notification_delta2char(time(NULL) - not->timestamp, buf);
    lv_label_set_text(ui_LabelTimeDelta, buf);

    lv_obj_clear_flag(ui_LabelTimeDelta,
                      LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_color(ui_LabelTimeDelta, lv_color_hex(0x8C8C8C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LabelTimeDelta, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    active_notifications[index].panel = ui_Panel;
    active_notifications[index].deltaLabel = ui_LabelTimeDelta;
    active_notifications[index].notification = not;

    ui_ImageIcon = lv_img_create(ui_Panel);
    lv_obj_set_width(ui_ImageIcon, 16);
    lv_obj_set_height(ui_ImageIcon, 16);
    lv_obj_set_x(ui_ImageIcon, -85);
    lv_obj_set_y(ui_ImageIcon, -30);
    lv_obj_set_align(ui_ImageIcon, LV_ALIGN_CENTER);
    lv_img_set_src(ui_ImageIcon, image_source);
    lv_obj_clear_flag(ui_ImageIcon, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                      LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);

    ui_LabelHeader = lv_label_create(ui_Panel);
    lv_obj_set_width(ui_LabelHeader, 180);
    lv_obj_set_height(ui_LabelHeader, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LabelHeader, 0);
    lv_obj_set_y(ui_LabelHeader, 0);
    lv_obj_set_align(ui_LabelHeader, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LabelHeader, not->sender);
    lv_label_set_long_mode(ui_LabelHeader, LV_LABEL_LONG_DOT);
    lv_obj_clear_flag(ui_LabelHeader, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                      LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_color(ui_LabelHeader, lv_color_hex(0x587BF8), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LabelHeader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LabelHeader, &lv_font_montserrat_14_full, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LabelBody = lv_label_create(ui_Panel);
    lv_obj_set_width(ui_LabelBody, 180);
    lv_obj_set_height(ui_LabelBody, 25);
    lv_obj_set_x(ui_LabelBody, 0);
    lv_obj_set_y(ui_LabelBody, 30);
    lv_obj_set_align(ui_LabelBody, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LabelBody, not->body);
    lv_label_set_long_mode(ui_LabelBody, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_clear_flag(ui_LabelBody, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                      LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_text_color(ui_LabelBody, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LabelBody, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LabelBody, &lv_font_montserrat_14_full, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LabelBody, zsw_color_gray(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LabelBody, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_LabelBody, zsw_color_gray(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_LabelBody, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_LabelBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_LabelBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_LabelBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_LabelBody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Remove the cursor and the highlighting (visible for the first entry).
    lv_obj_clear_state(ui_LabelBody, LV_STATE_CHECKED | LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY);
}

void notifications_ui_page_init(on_notification_remove_cb_t not_removed_cb)
{
    notification_removed_callback = not_removed_cb;
    memset(active_notifications, 0, sizeof(active_notifications));
}

void notifications_ui_page_create(lv_obj_t *parent, lv_group_t *group)
{
    main_page = lv_obj_create(parent);
    lv_obj_set_scrollbar_mode(main_page, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_size(main_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_border_side(main_page, LV_BORDER_SIDE_NONE, 0);
    lv_obj_center(main_page);

    lv_obj_set_flex_flow(main_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(main_page, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(main_page, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(main_page, LV_SCROLLBAR_MODE_OFF);

    empty_label = lv_label_create(parent);
    lv_label_set_text(empty_label, "No notifications");
    lv_obj_set_style_text_font(empty_label, &lv_font_montserrat_14_full, 0);
    lv_obj_set_style_text_color(empty_label, lv_color_hex(0x8C8C8C), 0);
    lv_obj_center(empty_label);

    /*
        ui_ImgButtonClearAll = lv_imgbtn_create(lv_obj_create(parent));
        lv_imgbtn_set_src(ui_ImgButtonClearAll, LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_trash_png, NULL);
        lv_imgbtn_set_src(ui_ImgButtonClearAll, LV_IMGBTN_STATE_PRESSED, NULL, &ui_img_trash_png, NULL);
        lv_obj_set_width(ui_ImgButtonClearAll, 32);
        lv_obj_set_height(ui_ImgButtonClearAll, 32);
        lv_obj_align(ui_ImgButtonClearAll, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_align(ui_ImgButtonClearAll, LV_ALIGN_CENTER);
        lv_obj_clear_flag(ui_ImgButtonClearAll, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_O
        BJ_FLAG_SNAPPABLE);
        lv_obj_add_event_cb(ui_ImgButtonClearAll, on_ImgButtonClearAll_clicked, LV_EVENT_PRESSED, NULL);
    */

    // Update the notifications position manually firt time.
    lv_event_send(main_page, LV_EVENT_SCROLL, NULL);

    timer = lv_timer_create(label_on_timer_callback, 5000UL, NULL);
}

void notifications_ui_page_close(void)
{
    if (timer != NULL) {
        lv_timer_del(timer);
    }

    lv_obj_del(main_page);
    lv_obj_del(empty_label);
    main_page = NULL;
    timer = NULL;
}

void notifications_ui_add_notification(zsw_not_mngr_notification_t *not, lv_group_t *group)
{
    if (main_page == NULL) {
        return;
    }

    if (!lv_obj_has_flag(empty_label, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_add_flag(empty_label, LV_OBJ_FLAG_HIDDEN);
    }

    build_notification_entry(main_page, not, group);
    lv_obj_scroll_to_view(lv_obj_get_child(main_page, -1), LV_ANIM_ON);
    lv_obj_update_layout(main_page);
}

void notifications_ui_remove_notification(uint32_t id)
{
    if (main_page == NULL) {
        return;
    }

    for (uint32_t i = 0; i < ZSW_NOTIFICATION_MGR_MAX_STORED; i++) {
        if ((active_notifications[i].notification != NULL) && (active_notifications[i].notification->id == id)) {
            lv_obj_add_flag(active_notifications[i].panel, LV_OBJ_FLAG_HIDDEN);
            lv_obj_del(active_notifications[i].panel);
            active_notifications[i].panel = NULL;
            active_notifications[i].deltaLabel = NULL;
            active_notifications[i].notification = NULL;
            if (lv_obj_get_child(main_page, -1)) {
                lv_obj_scroll_to_view(lv_obj_get_child(main_page, -1), LV_ANIM_ON);
            }
            lv_obj_update_layout(main_page);

            break;
        }
    }

    if (!any_notifiction()) {
        lv_obj_clear_flag(empty_label, LV_OBJ_FLAG_HIDDEN);
    }
}