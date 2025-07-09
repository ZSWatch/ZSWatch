#include "timer_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>
#include "assert.h"

typedef struct {
    uint32_t timer_id;
    lv_obj_t *ui_timer;
    lv_obj_t *ui_timer_time_left;
    lv_obj_t *ui_start_pause_button_label;
    lv_obj_t *ui_resest_button_label;
    lv_obj_t *ui_delete_button_label;
} timer_item_ui_t;

// Common
static void create_page_indicator(lv_obj_t *container);
static void on_tileview_change(lv_event_t *e);
static lv_obj_t *tv;
static lv_obj_t *ui_page_indicator;
static lv_obj_t *led1;
static lv_obj_t *led2;

static lv_obj_t *ui_time_popup;
static lv_obj_t *ui_picker_container;
static lv_obj_t *ui_picker_new_alarm_time;
static uint8_t num_picker_digits;
static uint8_t picker_digits[sizeof("HHMMSS") - 1];

// SCREEN: Timer Screen
static void timer_screen_init(lv_obj_t *ui_Screen1);
static void create_timer_popup(lv_obj_t *ui_root_container);
static void create_timer_item(lv_obj_t *ui_root_container, timer_app_timer_t timer);
static void ui_event_start_pause_pressed(lv_event_t *e);
static void ui_event_reset_pressed(lv_event_t *e);
static void ui_event_delete_pressed(lv_event_t *e);
static void ui_event_open_popup(lv_event_t *e);
static void ui_event_close_popup(lv_event_t *e);
static void ui_event_create_timer(lv_event_t *e);
static void ui_event_picker_button_pressed(lv_event_t *e);
static lv_obj_t *ui_timer_page;
static lv_obj_t *ui_label_clock;
static lv_obj_t *ui_add_timer_button;
static lv_obj_t *ui_timer_list_container;

// SCREEN: Alarm Screen
static void alarm_screen_init(lv_obj_t *ui_Screen2);
static lv_obj_t *ui_alarm_page;
static lv_obj_t *ui_add_alarm_button;
static lv_obj_t *ui_alarm_list_container;

static timer_item_ui_t ui_timers[TIMER_UI_MAX_TIMERS];

static lv_obj_t *root_page = NULL;
static on_timer_created on_timer_created_cb;
static on_timer_event on_timer_event_cb;

void timer_ui_show(lv_obj_t *root, on_timer_created on_create, on_timer_event on_event)
{
    assert(root_page == NULL);
    on_timer_created_cb = on_create;
    on_timer_event_cb = on_event;
    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    // Don't want it to be scollable. Putting anything close the edges
    // then LVGL automatically makes the page scrollable and shows a scroll bar.
    // Does not look very good on the round display.
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);

    tv = lv_tileview_create(root_page);
    lv_obj_set_style_pad_all(tv, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(tv, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(tv, LV_OPA_TRANSP, 0);
    // Remove scroolbar on tv
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    ui_timer_page = lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR);
    timer_screen_init(ui_timer_page);
    ui_alarm_page = lv_tileview_add_tile(tv, 1, 0, LV_DIR_HOR);
    alarm_screen_init(ui_alarm_page);
    create_page_indicator(root_page);

    create_timer_popup(root_page);

    // Add callback to tileview when new page changed, call set_indicator_page
    lv_obj_add_event_cb(tv, on_tileview_change, LV_EVENT_VALUE_CHANGED, NULL);
}

void timer_ui_remove(void)
{
    assert(root_page != NULL);
    lv_obj_del(root_page);
    root_page = NULL;
}

void timer_ui_add_timer(timer_app_timer_t timer)
{
    assert(root_page != NULL);
    if (timer.type == TYPE_TIMER) {
        create_timer_item(ui_timer_list_container, timer);
    } else {
        create_timer_item(ui_alarm_list_container, timer);
    }
}

void timer_ui_update_timer(timer_app_timer_t timer)
{
    if (!root_page) {
        return;
    }
    lv_label_set_text_fmt(ui_timers[timer.timer_id].ui_timer_time_left, "%02d:%02d:%02d", timer.remaining_hour,
                          timer.remaining_min, timer.remaining_sec);
    switch (timer.state) {
        case TIMER_STATE_PLAYING:
            lv_label_set_text(ui_timers[timer.timer_id].ui_start_pause_button_label, LV_SYMBOL_PAUSE);
            break;
        case TIMER_STATE_PAUSED:
            lv_label_set_text(ui_timers[timer.timer_id].ui_start_pause_button_label, LV_SYMBOL_PLAY);
            break;
        case TIMER_STATE_STOPPED:
            lv_label_set_text(ui_timers[timer.timer_id].ui_start_pause_button_label, LV_SYMBOL_PLAY);
            break;
        default:
            break;
    }
}

void timer_ui_set_time(int hour, int min, int second)
{
    lv_label_set_text_fmt(ui_label_clock, "%02d:%02d", hour, min);
}

void timer_ui_remove_timer(timer_app_timer_t timer)
{
    assert(root_page != NULL);
    lv_obj_del(ui_timers[timer.timer_id].ui_timer);
}

static void create_page_indicator(lv_obj_t *container)
{
    ui_page_indicator = lv_obj_create(container);
    lv_obj_set_width(ui_page_indicator, 100);
    lv_obj_set_height(ui_page_indicator, 10);

    lv_obj_align(ui_page_indicator, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_clear_flag(ui_page_indicator, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_page_indicator, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_page_indicator, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_page_indicator, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_page_indicator, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    led1  = lv_led_create(ui_page_indicator);
    lv_obj_align(led1, LV_ALIGN_CENTER, -10, 0);
    lv_obj_set_size(led1, 7, 7);
    lv_led_off(led1);

    led2  = lv_led_create(ui_page_indicator);
    lv_obj_align(led2, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(led2, 7, 7);
    lv_led_off(led2);
}

static void set_indicator_page(int page)
{
    lv_color_t on_color = lv_color_hex(0xE6898B);
    lv_color_t off_color = lv_color_hex(0xFFFFFF);

    switch (page) {
        case 0:
            lv_led_set_color(led1, on_color);
            lv_led_set_color(led2, off_color);
            break;
        case 1:
            lv_led_set_color(led1, off_color);
            lv_led_set_color(led2, on_color);
            break;
        default:
            break;
    }
}

static void timer_screen_init(lv_obj_t *ui_root_container)
{
    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_width(ui_root_container, 0, LV_PART_MAIN);

    lv_obj_set_style_border_width(ui_root_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_root_container, lv_pct(100));
    lv_obj_set_height(ui_root_container, lv_pct(100));
    lv_obj_set_align(ui_root_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_root_container, lv_color_hex(0x333131), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_root_container, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_label_clock = lv_label_create(ui_root_container);
    lv_obj_set_width(ui_label_clock, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_label_clock, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_label_clock, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_label_clock, "-:-");
    lv_obj_set_style_text_color(ui_label_clock, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_label_clock, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_label_clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_label_clock, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_label_clock, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_label_clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Label12 = lv_label_create(ui_root_container);
    lv_obj_set_width(ui_Label12, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label12, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label12, -3);
    lv_obj_set_y(ui_Label12, -85);
    lv_obj_set_align(ui_Label12, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label12, "Timers");
    lv_obj_set_style_text_color(ui_Label12, lv_color_hex(0xAAADFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label12, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Label12, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_add_timer_button = lv_btn_create(ui_root_container);
    lv_obj_set_width(ui_add_timer_button, 40);
    lv_obj_set_height(ui_add_timer_button, 40);
    lv_obj_set_x(ui_add_timer_button, 0);
    lv_obj_set_y(ui_add_timer_button, -25);
    lv_obj_set_align(ui_add_timer_button, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_radius(ui_add_timer_button, 90, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Label6 = lv_label_create(ui_add_timer_button);
    lv_obj_set_width(ui_Label6, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label6, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label6, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label6, LV_SYMBOL_PLUS);

    ui_timer_list_container = lv_obj_create(ui_root_container);
    lv_obj_remove_style_all(ui_timer_list_container);
    lv_obj_set_width(ui_timer_list_container, 240);
    lv_obj_set_height(ui_timer_list_container, 123);
    lv_obj_set_x(ui_timer_list_container, 0);
    lv_obj_set_y(ui_timer_list_container, -8);
    lv_obj_set_align(ui_timer_list_container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_timer_list_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_timer_list_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(ui_timer_list_container, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_timer_list_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_add_timer_button, ui_event_open_popup, LV_EVENT_CLICKED, NULL);
}

static void create_timer_item(lv_obj_t *ui_root_container, timer_app_timer_t timer)
{
    assert(root_page != NULL);
    ui_timers[timer.timer_id].ui_timer = lv_obj_create(ui_root_container);
    lv_obj_t *ui_timer = ui_timers[timer.timer_id].ui_timer;

    lv_obj_set_width(ui_timer, 200);
    lv_obj_set_height(ui_timer, 50);
    lv_obj_set_x(ui_timer, -3);
    lv_obj_set_y(ui_timer, -70);
    lv_obj_set_align(ui_timer, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_timer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_timer, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_timer, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_scrollbar_mode(ui_timer, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(ui_timer, lv_color_hex(0x444444), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_timer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_timer, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_timer, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_timer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_timer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_timer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_timer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_timer, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_timers[timer.timer_id].ui_timer_time_left = lv_label_create(ui_timer);
    lv_obj_t *ui_timer_time_left = ui_timers[timer.timer_id].ui_timer_time_left;
    lv_obj_set_width(ui_timer_time_left, LV_PCT(39));   /// 1
    lv_obj_set_height(ui_timer_time_left, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_timer_time_left, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_timer_time_left, LV_LABEL_LONG_CLIP);
    lv_label_set_text_fmt(ui_timer_time_left, "%02d:%02d:%02d", timer.hour, timer.min, timer.sec);
    lv_obj_set_style_text_color(ui_timer_time_left, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_timer_time_left, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_timer_time_left, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_timer_time_left, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_timer_time_left, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_timer_time_left, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_timer_time_left, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_timer_time_left, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_start_pause_button = lv_btn_create(ui_timer);
    lv_obj_set_width(ui_start_pause_button, 33);
    lv_obj_set_height(ui_start_pause_button, 33);
    lv_obj_set_align(ui_start_pause_button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_start_pause_button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_start_pause_button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_start_pause_button, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_start_pause_button, lv_color_hex(0xFF8427), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_start_pause_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_start_pause_button, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_start_pause_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_start_pause_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_start_pause_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_timers[timer.timer_id].ui_start_pause_button_label = lv_label_create(ui_start_pause_button);
    lv_obj_t *ui_start_pause_button_label = ui_timers[timer.timer_id].ui_start_pause_button_label;
    lv_obj_set_width(ui_start_pause_button_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_start_pause_button_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_start_pause_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_start_pause_button_label, LV_SYMBOL_PLAY);

    if (timer.type == TYPE_TIMER) {
        lv_obj_t *ui_reset_button = lv_btn_create(ui_timer);
        lv_obj_set_width(ui_reset_button, 33);
        lv_obj_set_height(ui_reset_button, 33);
        lv_obj_set_align(ui_reset_button, LV_ALIGN_CENTER);
        lv_obj_add_flag(ui_reset_button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
        lv_obj_clear_flag(ui_reset_button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
        lv_obj_set_style_radius(ui_reset_button, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_reset_button, lv_color_hex(0xFF8427), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui_reset_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(ui_reset_button, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(ui_reset_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(ui_reset_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_spread(ui_reset_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

        ui_timers[timer.timer_id].ui_resest_button_label = lv_label_create(ui_reset_button);
        lv_obj_t *ui_resest_button_label = ui_timers[timer.timer_id].ui_resest_button_label;
        lv_obj_set_width(ui_resest_button_label, LV_SIZE_CONTENT);   /// 1
        lv_obj_set_height(ui_resest_button_label, LV_SIZE_CONTENT);    /// 1
        lv_obj_set_align(ui_resest_button_label, LV_ALIGN_CENTER);
        lv_label_set_text(ui_resest_button_label, LV_SYMBOL_REFRESH);

        lv_obj_add_event_cb(ui_reset_button, ui_event_reset_pressed, LV_EVENT_CLICKED, (void *)timer.timer_id);
    }

    lv_obj_t *ui_delete_button = lv_btn_create(ui_timer);
    lv_obj_set_width(ui_delete_button, 33);
    lv_obj_set_height(ui_delete_button, 33);
    lv_obj_set_align(ui_delete_button, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_delete_button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_delete_button, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_delete_button, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_delete_button, lv_color_hex(0xFF8427), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_delete_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_delete_button, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_delete_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_delete_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_delete_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_timers[timer.timer_id].ui_delete_button_label = lv_label_create(ui_delete_button);
    lv_obj_t *ui_delete_button_label = ui_timers[timer.timer_id].ui_delete_button_label;
    lv_obj_set_width(ui_delete_button_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_delete_button_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_delete_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_delete_button_label, LV_SYMBOL_TRASH);

    lv_obj_add_event_cb(ui_start_pause_button, ui_event_start_pause_pressed, LV_EVENT_CLICKED, (void *)timer.timer_id);
    lv_obj_add_event_cb(ui_delete_button, ui_event_delete_pressed, LV_EVENT_CLICKED, (void *)timer.timer_id);
}

static void create_timer_popup(lv_obj_t *ui_root_container)
{
    assert(root_page != NULL);
    ui_time_popup = lv_obj_create(ui_root_container);
    lv_obj_remove_style_all(ui_time_popup);
    lv_obj_set_width(ui_time_popup, lv_pct(100));
    lv_obj_set_height(ui_time_popup, lv_pct(100));
    lv_obj_set_align(ui_time_popup, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_time_popup, LV_OBJ_FLAG_EVENT_BUBBLE);     /// Flags
    lv_obj_add_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN);     /// Flags
    lv_obj_clear_flag(ui_time_popup, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t *ui_time_popup_container = lv_obj_create(ui_time_popup);
    lv_obj_remove_style_all(ui_time_popup_container);
    lv_obj_set_width(ui_time_popup_container, lv_pct(75));
    lv_obj_set_height(ui_time_popup_container, lv_pct(70));
    lv_obj_set_align(ui_time_popup_container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_time_popup_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_time_popup_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_time_popup_container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_time_popup_container, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_time_popup_container, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_time_popup_container, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_picker_container = lv_obj_create(ui_time_popup_container);
    lv_obj_remove_style_all(ui_picker_container);
    lv_obj_set_width(ui_picker_container, lv_pct(100));
    lv_obj_set_height(ui_picker_container, lv_pct(100));
    lv_obj_set_align(ui_picker_container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_picker_container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(ui_picker_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_picker_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_pad_row(ui_picker_container, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_picker_container, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_picker_new_alarm_time = lv_label_create(ui_picker_container);
    lv_obj_set_width(ui_picker_new_alarm_time, lv_pct(100));
    lv_obj_set_height(ui_picker_new_alarm_time, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_picker_new_alarm_time, LV_ALIGN_CENTER);
    lv_label_set_text(ui_picker_new_alarm_time, "00:00:00");
    lv_obj_set_style_text_color(ui_picker_new_alarm_time, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_picker_new_alarm_time, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_picker_new_alarm_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_picker_new_alarm_time, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_picker_new_alarm_time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_picker_new_alarm_time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_picker_new_alarm_time, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_picker_new_alarm_time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_keypad1 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad1, lv_pct(30));
    lv_obj_set_height(ui_keypad1, lv_pct(17));
    lv_obj_set_align(ui_keypad1, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad1, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)1);

    lv_obj_t *ui_Label5 = lv_label_create(ui_keypad1);
    lv_obj_set_width(ui_Label5, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label5, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label5, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label5, "1");

    lv_obj_t *ui_keypad2 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad2, lv_pct(30));
    lv_obj_set_height(ui_keypad2, lv_pct(17));
    lv_obj_set_align(ui_keypad2, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad2, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)2);

    lv_obj_t *ui_Label7 = lv_label_create(ui_keypad2);
    lv_obj_set_width(ui_Label7, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label7, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label7, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label7, "2");

    lv_obj_t *ui_keypad3 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad3, lv_pct(30));
    lv_obj_set_height(ui_keypad3, lv_pct(17));
    lv_obj_set_align(ui_keypad3, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad3, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)3);

    lv_obj_t *ui_Label8 = lv_label_create(ui_keypad3);
    lv_obj_set_width(ui_Label8, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label8, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label8, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label8, "3");

    lv_obj_t *ui_keypad4 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad4, lv_pct(30));
    lv_obj_set_height(ui_keypad4, lv_pct(17));
    lv_obj_set_align(ui_keypad4, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad4, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)4);

    lv_obj_t *ui_Label9 = lv_label_create(ui_keypad4);
    lv_obj_set_width(ui_Label9, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label9, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label9, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label9, "4");

    lv_obj_t *ui_keypad5 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad5, lv_pct(30));
    lv_obj_set_height(ui_keypad5, lv_pct(17));
    lv_obj_set_align(ui_keypad5, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad5, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)5);

    lv_obj_t *ui_Label10 = lv_label_create(ui_keypad5);
    lv_obj_set_width(ui_Label10, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label10, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label10, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label10, "5");

    lv_obj_t *ui_keypad6 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad6, lv_pct(30));
    lv_obj_set_height(ui_keypad6, lv_pct(17));
    lv_obj_set_align(ui_keypad6, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad6, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)6);

    lv_obj_t *ui_Label11 = lv_label_create(ui_keypad6);
    lv_obj_set_width(ui_Label11, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label11, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label11, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label11, "6");

    lv_obj_t *ui_keypad7 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad7, lv_pct(30));
    lv_obj_set_height(ui_keypad7, lv_pct(17));
    lv_obj_set_align(ui_keypad7, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad7, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)7);

    lv_obj_t *ui_Label13 = lv_label_create(ui_keypad7);
    lv_obj_set_width(ui_Label13, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label13, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label13, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label13, "7");

    lv_obj_t *ui_keypad8 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad8, lv_pct(30));
    lv_obj_set_height(ui_keypad8, lv_pct(17));
    lv_obj_set_align(ui_keypad8, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad8, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)8);

    lv_obj_t *ui_Label14 = lv_label_create(ui_keypad8);
    lv_obj_set_width(ui_Label14, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label14, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label14, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label14, "8");

    lv_obj_t *ui_keypad9 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad9, lv_pct(30));
    lv_obj_set_height(ui_keypad9, lv_pct(17));
    lv_obj_set_align(ui_keypad9, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad9, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)9);

    lv_obj_t *ui_Label15 = lv_label_create(ui_keypad9);
    lv_obj_set_width(ui_Label15, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label15, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label15, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label15, "9");

    lv_obj_t *ui_keypad0 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad0, lv_pct(30));
    lv_obj_set_height(ui_keypad0, lv_pct(17));
    lv_obj_set_align(ui_keypad0, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad0, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)0);

    lv_obj_t *ui_Label16 = lv_label_create(ui_keypad0);
    lv_obj_set_width(ui_Label16, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label16, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label16, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label16, "0");

    lv_obj_t *ui_keypad00 = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_keypad00, lv_pct(30));
    lv_obj_set_height(ui_keypad00, lv_pct(17));
    lv_obj_set_align(ui_keypad00, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(ui_keypad00, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)10);

    lv_obj_t *ui_Label18 = lv_label_create(ui_keypad00);
    lv_obj_set_width(ui_Label18, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label18, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label18, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label18, LV_SYMBOL_BACKSPACE);

    lv_obj_t *ui_time_set_button = lv_btn_create(ui_picker_container);
    lv_obj_set_width(ui_time_set_button, lv_pct(30));
    lv_obj_set_height(ui_time_set_button, lv_pct(17));
    lv_obj_set_align(ui_time_set_button, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_time_set_button, lv_color_hex(0xFF8520), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_time_set_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Label4 = lv_label_create(ui_time_set_button);
    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label4, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label4, "ADD");

    lv_obj_add_event_cb(ui_time_set_button, ui_event_create_timer, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_time_popup, ui_event_close_popup, LV_EVENT_ALL, NULL);
}

static void alarm_screen_init(lv_obj_t *ui_root_container)
{
    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_width(ui_root_container, 0, LV_PART_MAIN);

    lv_obj_set_style_border_width(ui_root_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_root_container, lv_pct(100));
    lv_obj_set_height(ui_root_container, lv_pct(100));
    lv_obj_set_align(ui_root_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_root_container, lv_color_hex(0x333131), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_root_container, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_label_clock = lv_label_create(ui_root_container);
    lv_obj_set_width(ui_label_clock, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_label_clock, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_label_clock, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_label_clock, "-:-");
    lv_obj_set_style_text_color(ui_label_clock, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_label_clock, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_label_clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_label_clock, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_label_clock, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_label_clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Label12 = lv_label_create(ui_root_container);
    lv_obj_set_width(ui_Label12, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label12, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label12, -3);
    lv_obj_set_y(ui_Label12, -85);
    lv_obj_set_align(ui_Label12, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label12, "Alarms");
    lv_obj_set_style_text_color(ui_Label12, lv_color_hex(0xAAADFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label12, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Label12, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_add_alarm_button = lv_btn_create(ui_root_container);
    lv_obj_set_width(ui_add_alarm_button, 40);
    lv_obj_set_height(ui_add_alarm_button, 40);
    lv_obj_set_x(ui_add_alarm_button, 0);
    lv_obj_set_y(ui_add_alarm_button, -25);
    lv_obj_set_align(ui_add_alarm_button, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_radius(ui_add_alarm_button, 90, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Label6 = lv_label_create(ui_add_alarm_button);
    lv_obj_set_width(ui_Label6, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label6, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label6, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label6, LV_SYMBOL_PLUS);

    ui_alarm_list_container = lv_obj_create(ui_root_container);
    lv_obj_remove_style_all(ui_alarm_list_container);
    lv_obj_set_width(ui_alarm_list_container, 240);
    lv_obj_set_height(ui_alarm_list_container, 123);
    lv_obj_set_x(ui_alarm_list_container, 0);
    lv_obj_set_y(ui_alarm_list_container, -8);
    lv_obj_set_align(ui_alarm_list_container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_alarm_list_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_alarm_list_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(ui_alarm_list_container, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_alarm_list_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_add_alarm_button, ui_event_open_popup, LV_EVENT_CLICKED, NULL);
}

static void ui_event_open_popup(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        num_picker_digits = 0;
        memset(picker_digits, 0, sizeof(picker_digits));
        lv_label_set_text(ui_picker_new_alarm_time, "00:00:00");
        lv_obj_clear_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN);
    }
}

static void ui_event_close_popup(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        lv_obj_add_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN);
    }
}

static ui_timer_type_t get_current_page_type(void)
{
    lv_obj_t *current = lv_tileview_get_tile_act(tv);
    if (current == ui_alarm_page) {
        return TYPE_ALARM;
    } else {
        return TYPE_TIMER;
    }
}

static void ui_event_create_timer(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        lv_obj_add_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN);
        uint8_t hour = picker_digits[5] * 10 + picker_digits[4];
        uint8_t min = picker_digits[3] * 10 + picker_digits[2];
        uint8_t sec = picker_digits[1] * 10 + picker_digits[0];

        on_timer_created_cb(hour, min, sec, get_current_page_type());
    }
}

static void shift_out_number(uint8_t *array, int arr_len)
{
    memmove(array, array + 1, arr_len - 1);
    array[arr_len - 1] = 0;
}

static void shift_in_number(uint8_t *array, int arr_len, uint8_t number)
{
    memmove(array + 1, array, arr_len - 1);
    array[0] = number;
}

static void ui_event_picker_button_pressed(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        uint32_t value = (uint32_t)lv_event_get_user_data(e);
        if (value == 10) {
            // 00 (delete)
            if (num_picker_digits > 0) {
                shift_out_number(picker_digits, sizeof(picker_digits));
                num_picker_digits--;
            }
        } else {
            // 0-9
            if ((num_picker_digits < sizeof(picker_digits)) && !(value == 0 && num_picker_digits == 0)) {
                shift_in_number(picker_digits, sizeof(picker_digits), value);
                num_picker_digits++;
            }
        }
        lv_label_set_text_fmt(ui_picker_new_alarm_time, "%02d:%02d:%02d", picker_digits[5] * 10 + picker_digits[4],
                              picker_digits[3] * 10 + picker_digits[2], picker_digits[1] * 10 + picker_digits[0]);
    }
}

static void ui_event_start_pause_pressed(lv_event_t *e)
{
    on_timer_event_cb(TIMER_EVT_START_PAUSE_RESUME, (uint32_t)lv_event_get_user_data(e));
}

static void ui_event_reset_pressed(lv_event_t *e)
{
    on_timer_event_cb(TIMER_EVT_RESET, (uint32_t)lv_event_get_user_data(e));
}

static void ui_event_delete_pressed(lv_event_t *e)
{
    on_timer_event_cb(TIMER_EVT_DELETE, (uint32_t)lv_event_get_user_data(e));
}

static void on_tileview_change(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    printk("Tileview changed: %d\n", event_code);
    if (event_code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *current = lv_tileview_get_tile_act(tv);
        if (current == ui_timer_page) {
            set_indicator_page(0);
        } else if (current == ui_alarm_page) {
            set_indicator_page(1);
        } else {
            LV_LOG_ERROR("Failed finding parent!\n");
        }
    }
}