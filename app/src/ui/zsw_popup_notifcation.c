#include <zsw_popup_notifcation.h>

#include <lvgl.h>

typedef struct {
    lv_obj_t *panel;
    lv_obj_t *title;
    lv_obj_t *body;
    lv_obj_t *close_btn;
} notif_box_t;

static void on_notification_closed(lv_event_t *e);
static void on_notification_expand(lv_event_t *e);
static void close_notif_timer(lv_timer_t *timer);
static void remove_notification(uint32_t notif_id);

static on_close_notif_cb_t on_close_cb;
static uint32_t active_notif_id;
static lv_timer_t *auto_close_timer;
static notif_box_t notif_box;

void zsw_notification_popup_show(char *title, char *body, notification_src_t icon, uint32_t id,
                                 on_close_notif_cb_t close_cb,
                                 uint32_t close_after_seconds)
{
    active_notif_id = id;
    on_close_cb = close_cb;

    // create panel
    notif_box.panel = lv_obj_create(lv_scr_act());
    lv_obj_set_width(notif_box.panel, 160);
    lv_obj_set_height(notif_box.panel, LV_SIZE_CONTENT);
    lv_obj_set_align(notif_box.panel, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_x(notif_box.panel, 0);
    lv_obj_set_y(notif_box.panel, -25);
    lv_obj_set_style_pad_bottom(notif_box.panel, 3, 0); // reduce padding
    lv_obj_clear_flag(notif_box.panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(notif_box.panel, 15, 0);
    lv_obj_set_style_bg_color(notif_box.panel, lv_color_hex(0x0C0C1A), 0);
    lv_obj_set_style_border_color(notif_box.panel, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);
    lv_obj_set_style_border_width(notif_box.panel, 1, 0);
    lv_obj_add_event_cb(notif_box.panel, on_notification_expand, LV_EVENT_PRESSED, NULL);

    // create title text
    notif_box.title = lv_label_create(notif_box.panel);
    lv_obj_set_x(notif_box.title, -5);
    lv_obj_set_y(notif_box.title, -10);
    // TODO: replace bell with application symbol
    lv_label_set_text_fmt(notif_box.title, "%s %s", LV_SYMBOL_BELL, title);
    lv_label_set_long_mode(notif_box.title, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(notif_box.title, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_text_font(notif_box.title, &lv_font_montserrat_14, 0);

    // create body text
    if (strlen(body) > 0) {
        notif_box.body = lv_label_create(notif_box.panel);
        lv_obj_set_width(notif_box.body, 155);
        lv_obj_set_height(notif_box.body, LV_SIZE_CONTENT);
        lv_obj_set_style_max_height(notif_box.body, 60, 0);
        lv_obj_set_x(notif_box.body, -8);
        lv_obj_set_y(notif_box.body, 5);
        lv_label_set_long_mode(notif_box.body, LV_LABEL_LONG_DOT);
        lv_label_set_text(notif_box.body, body);
        lv_obj_set_style_text_font(notif_box.body, &lv_font_montserrat_16, 0);
    }

    // create close button
    notif_box.close_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_width(notif_box.close_btn, 30);
    lv_obj_set_height(notif_box.close_btn, 30);
    lv_obj_align_to(notif_box.close_btn, notif_box.panel, LV_ALIGN_OUT_TOP_RIGHT, 15, 15);
    lv_obj_set_style_radius(notif_box.close_btn, 20, 0);
    lv_obj_set_style_bg_color(notif_box.close_btn, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);

    lv_obj_add_event_cb(notif_box.close_btn, on_notification_closed, LV_EVENT_PRESSED, NULL);

    lv_obj_t *ui_x = lv_label_create(notif_box.close_btn);
    lv_obj_set_align(ui_x, LV_ALIGN_CENTER);
    lv_label_set_text(ui_x, LV_SYMBOL_CLOSE);

    // set auto close timer
    auto_close_timer = lv_timer_create(close_notif_timer, close_after_seconds * 1000, NULL);
    lv_timer_set_repeat_count(auto_close_timer, 1);
}

void zsw_notification_popup_remove(void)
{
    remove_notification(active_notif_id);
}

bool zsw_notification_popup_is_shown(void)
{
    return notif_box.panel != NULL;
}

static void on_notification_closed(lv_event_t *e)
{
    zsw_notification_popup_remove();
}

static void on_notification_expand(lv_event_t *e)
{
    lv_label_set_long_mode(notif_box.body, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_max_height(notif_box.body, LV_SIZE_CONTENT, 0);

    // algin button after notification expand
    lv_obj_align_to(notif_box.close_btn, notif_box.panel, LV_ALIGN_OUT_TOP_RIGHT, 15, 15);

    // click on expanded message will close and remove message
    lv_obj_remove_event_cb(notif_box.panel, on_notification_expand);
    lv_obj_add_event_cb(notif_box.panel, on_notification_closed, LV_EVENT_PRESSED, NULL);

    if (auto_close_timer) {
        lv_timer_del(auto_close_timer);
        auto_close_timer = NULL;
    }
}

static void close_notif_timer(lv_timer_t *timer)
{
    remove_notification(0);
}

static void remove_notification(uint32_t notif_id)
{
    if (auto_close_timer) {
        lv_timer_del(auto_close_timer);
        auto_close_timer = NULL;
    }

    if (notif_box.panel) {
        lv_obj_del(notif_box.panel);
        notif_box.panel = NULL;
    }

    if (notif_box.close_btn) {
        lv_obj_del(notif_box.close_btn);
        notif_box.close_btn = NULL;
    }

    on_close_cb(notif_id);
}