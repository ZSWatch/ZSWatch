#include "timer_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>

typedef struct {
    uint32_t timer_id;
    lv_obj_t * ui_timer;
    lv_obj_t * ui_timer_time_left;
    lv_obj_t * ui_start_pause_button_label;
    lv_obj_t * ui_resest_button_label;
    lv_obj_t * ui_delete_button_label;
} timer_item_ui_t;

// Common
static void create_page_indicator(lv_obj_t *container);
static void on_tileview_change(lv_event_t *e);
static lv_obj_t *tv;
static lv_obj_t *ui_page_indicator;
static lv_obj_t *led1;
static lv_obj_t *led2;

// SCREEN: Timer Screen
static void timer_screen_init(lv_obj_t *ui_Screen1);
static void create_time_popup(lv_obj_t* ui_root_container);
static void create_timer_item(lv_obj_t* ui_root_container, timer_app_timer_t timer);
static void ui_event_start_pause_pressed(lv_event_t * e);
static void ui_event_reset_pressed(lv_event_t * e);
static void ui_event_delete_pressed(lv_event_t * e);
static void ui_event_open_popup(lv_event_t * e);
static void ui_event_close_popup(lv_event_t * e);
static void ui_event_create_timer(lv_event_t * e);
static lv_obj_t* ui_timer_page;
lv_obj_t * ui_label_clock;
lv_obj_t * ui_add_timer_button;
lv_obj_t * ui_timer_list_container;
lv_obj_t * ui_time_popup;
lv_obj_t * ui_picker_container;
lv_obj_t * ui_hour_picker;
lv_obj_t * ui_hh_label;
lv_obj_t * ui_hour_roller;
lv_obj_t * ui_min_picker;
lv_obj_t * ui_mm_label;
lv_obj_t * ui_min_roller;
lv_obj_t * ui_sec_picker;
lv_obj_t * ui_ss_label;
lv_obj_t * ui_sec_roller;

// SCREEN: Alarm Screen
static void alarm_screen_init(lv_obj_t *ui_Screen2);
static lv_obj_t* ui_alarm_page;
// TODO

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
    create_timer_item(ui_timer_list_container, timer);
}

void timer_ui_update_timer(timer_app_timer_t timer) {
    lv_label_set_text_fmt(ui_timers[timer.timer_id].ui_timer_time_left, "%02d:%02d:%02d", timer.remaining_hour, timer.remaining_min, timer.remaining_sec);
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

void timer_screen_init(lv_obj_t *ui_root_container)
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

    lv_obj_t* ui_Time = lv_label_create(ui_root_container);
    lv_obj_set_width(ui_Time, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Time, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Time, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Time, "23:49");
    lv_obj_set_style_text_color(ui_Time, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Time, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Time, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Time, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Time, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_Label12 = lv_label_create(ui_root_container);
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

    lv_obj_t* ui_Label6 = lv_label_create(ui_add_timer_button);
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

    create_time_popup(ui_root_container);

    lv_obj_add_event_cb(ui_add_timer_button, ui_event_open_popup, LV_EVENT_CLICKED, NULL);
}

static void create_timer_item(lv_obj_t* ui_root_container, timer_app_timer_t timer)
{
    ui_timers[timer.timer_id].ui_timer = lv_obj_create(ui_root_container);
    lv_obj_t* ui_timer = ui_timers[timer.timer_id].ui_timer;

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
    lv_obj_t* ui_timer_time_left = ui_timers[timer.timer_id].ui_timer_time_left;
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

    lv_obj_t* ui_start_pause_button = lv_btn_create(ui_timer);
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
    lv_obj_t* ui_start_pause_button_label = ui_timers[timer.timer_id].ui_start_pause_button_label;
    lv_obj_set_width(ui_start_pause_button_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_start_pause_button_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_start_pause_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_start_pause_button_label, LV_SYMBOL_PLAY);

    lv_obj_t* ui_reset_button = lv_btn_create(ui_timer);
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
    lv_obj_t* ui_resest_button_label = ui_timers[timer.timer_id].ui_resest_button_label;
    lv_obj_set_width(ui_resest_button_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_resest_button_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_resest_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_resest_button_label, LV_SYMBOL_REFRESH);

    lv_obj_t* ui_delete_button = lv_btn_create(ui_timer);
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
    lv_obj_t* ui_delete_button_label = ui_timers[timer.timer_id].ui_delete_button_label;
    lv_obj_set_width(ui_delete_button_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_delete_button_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_delete_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_delete_button_label, LV_SYMBOL_TRASH);

    lv_obj_add_event_cb(ui_start_pause_button, ui_event_start_pause_pressed, LV_EVENT_CLICKED, (void*)timer.timer_id);
    lv_obj_add_event_cb(ui_reset_button, ui_event_reset_pressed, LV_EVENT_CLICKED, (void*)timer.timer_id);
    lv_obj_add_event_cb(ui_delete_button, ui_event_delete_pressed, LV_EVENT_CLICKED, (void*)timer.timer_id);
}

static void create_time_popup(lv_obj_t* ui_root_container)
{
    ui_time_popup = lv_obj_create(ui_root_container);
    lv_obj_remove_style_all(ui_time_popup);
    lv_obj_set_width(ui_time_popup, lv_pct(100));
    lv_obj_set_height(ui_time_popup, lv_pct(100));
    lv_obj_set_align(ui_time_popup, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_EVENT_BUBBLE);     /// Flags
    lv_obj_clear_flag(ui_time_popup, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t* ui_time_popup_container = lv_obj_create(ui_time_popup);
    lv_obj_remove_style_all(ui_time_popup_container);
    lv_obj_set_height(ui_time_popup_container, lv_pct(70));
    lv_obj_set_width(ui_time_popup_container, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_align(ui_time_popup_container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_time_popup_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_time_popup_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_time_popup_container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_time_popup_container, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_time_popup_container, lv_color_hex(0x272727), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_time_popup_container, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_time_popup_container, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_time_popup_container, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_time_popup_container, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_time_popup_container, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_time_popup_container, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_time_popup_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_picker_container = lv_obj_create(ui_time_popup_container);
    lv_obj_remove_style_all(ui_picker_container);
    lv_obj_set_width(ui_picker_container, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_picker_container, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_picker_container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_picker_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_picker_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_picker_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_pad_row(ui_picker_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_picker_container, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_hour_picker = lv_obj_create(ui_picker_container);
    lv_obj_remove_style_all(ui_hour_picker);
    lv_obj_set_width(ui_hour_picker, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_hour_picker, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_hour_picker, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_hour_picker, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_hour_picker, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_hour_picker, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_pad_left(ui_hour_picker, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_hour_picker, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_hour_picker, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_hour_picker, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_hh_label = lv_label_create(ui_hour_picker);
    lv_obj_set_width(ui_hh_label, LV_SIZE_CONTENT);   /// 100
    lv_obj_set_height(ui_hh_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_hh_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_hh_label, "Hour");
    lv_obj_set_style_text_color(ui_hh_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_hh_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_hh_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_hour_roller = lv_roller_create(ui_hour_picker);
    lv_roller_set_options(ui_hour_roller, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n24\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_height(ui_hour_roller, 100);
    lv_obj_set_width(ui_hour_roller, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(ui_hour_roller, -26);
    lv_obj_set_y(ui_hour_roller, 0);
    lv_obj_set_align(ui_hour_roller, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_hour_roller, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_hour_roller, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_min_picker = lv_obj_create(ui_picker_container);
    lv_obj_remove_style_all(ui_min_picker);
    lv_obj_set_width(ui_min_picker, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_min_picker, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_min_picker, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_min_picker, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_min_picker, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_min_picker, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t* ui_mm_label = lv_label_create(ui_min_picker);
    lv_obj_set_width(ui_mm_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_mm_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_mm_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_mm_label, "Min");
    lv_obj_set_style_text_color(ui_mm_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_mm_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_mm_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_min_roller = lv_roller_create(ui_min_picker);
    lv_roller_set_options(ui_min_roller, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n24", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_height(ui_min_roller, 100);
    lv_obj_set_width(ui_min_roller, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(ui_min_roller, -28);
    lv_obj_set_y(ui_min_roller, -34);
    lv_obj_set_align(ui_min_roller, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_min_roller, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_min_roller, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_min_roller, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_min_roller, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_sec_picker = lv_obj_create(ui_picker_container);
    lv_obj_remove_style_all(ui_sec_picker);
    lv_obj_set_width(ui_sec_picker, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_sec_picker, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_sec_picker, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_sec_picker, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_sec_picker, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_sec_picker, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t* ui_ss_label = lv_label_create(ui_sec_picker);
    lv_obj_set_width(ui_ss_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_ss_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_ss_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_ss_label, "Sec");
    lv_obj_set_style_text_color(ui_ss_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ss_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_ss_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_sec_roller = lv_roller_create(ui_sec_picker);
    lv_roller_set_options(ui_sec_roller, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n24", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_height(ui_sec_roller, 100);
    lv_obj_set_width(ui_sec_roller, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(ui_sec_roller, -28);
    lv_obj_set_y(ui_sec_roller, -34);
    lv_obj_set_align(ui_sec_roller, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_sec_roller, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_sec_roller, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_sec_roller, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_sec_roller, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_time_set_button = lv_btn_create(ui_time_popup_container);
    lv_obj_set_height(ui_time_set_button, 30);
    lv_obj_set_width(ui_time_set_button, LV_SIZE_CONTENT);   /// 40
    lv_obj_set_align(ui_time_set_button, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_time_set_button, lv_color_hex(0xFF8520), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_time_set_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_Label4 = lv_label_create(ui_time_set_button);
    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label4, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label4, "ADD");

    lv_obj_add_event_cb(ui_time_set_button, ui_event_create_timer, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_time_popup, ui_event_close_popup, LV_EVENT_ALL, NULL);
}

void alarm_screen_init(lv_obj_t *ui_root_container)
{
    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_width(ui_root_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_root_container = lv_obj_create(ui_root_container);
    lv_obj_set_style_border_width(ui_root_container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_root_container, lv_pct(100));
    lv_obj_set_height(ui_root_container, lv_pct(100));
    lv_obj_set_align(ui_root_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_root_container, lv_color_hex(0x333131), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_root_container, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* ui_Label4 = lv_label_create(ui_root_container);
    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label4, 20);
    lv_obj_set_y(ui_Label4, 2);
    lv_obj_set_align(ui_Label4, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Label4, "Alarms page goes here");
    lv_obj_set_style_text_color(ui_Label4, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void ui_event_open_popup(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED) {
        lv_obj_clear_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN);
    }
}

static void ui_event_close_popup(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED) {
        lv_obj_add_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN);
    }
}

static void ui_event_create_timer(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED) {
        lv_obj_add_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN);
        uint32_t hour = lv_roller_get_selected(ui_hour_roller);
        uint32_t min = lv_roller_get_selected(ui_min_roller);
        uint32_t sec = lv_roller_get_selected(ui_sec_roller);
        on_timer_created_cb(hour, min, sec);
    }
}

static void ui_event_start_pause_pressed(lv_event_t * e) {
    on_timer_event_cb(TIMER_EVT_START_PAUSE_RESUME, (uint32_t)lv_event_get_user_data(e));
}

static void ui_event_reset_pressed(lv_event_t * e) {
    on_timer_event_cb(TIMER_EVT_RESET, (uint32_t)lv_event_get_user_data(e));
}

static void ui_event_delete_pressed(lv_event_t * e) {
    on_timer_event_cb(TIMER_EVT_DELETE, (uint32_t)lv_event_get_user_data(e));
}

static void on_tileview_change(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *curent = lv_tileview_get_tile_act(tv);
        if (curent == ui_timer_page) {
            set_indicator_page(0);
        } else if (curent == ui_alarm_page) {
            set_indicator_page(1);
        } else {
            LV_LOG_ERROR("Failed finding parent!\n");
        }
    }
}