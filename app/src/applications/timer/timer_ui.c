#include "timer_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>
#include "assert.h"
#include <zephyr/sys/util.h>
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"

#define TIMER_COLOR             lv_color_hex(0x5E5CE6)
#define ALARM_COLOR             ZSW_COLOR_ORANGE

#define LIST_WIDTH              208
#define LIST_TOP                46
#define LIST_HEIGHT             132
#define ITEM_HEIGHT             52
#define ITEM_BUTTON_SIZE        38
// Touch area outside each row button
#define ITEM_BUTTON_EXT_CLICK   3
#define ADD_BUTTON_SIZE         40

#define KEY_WIDTH               52
#define KEY_HEIGHT              32
#define KEY_GAP                 6
#define KEYPAD_TOP              62
#define KEY_BACKSPACE           10
#define KEY_ADD                 11

#define NO_TIMER                UINT32_MAX

typedef struct {
    lv_obj_t *ui_timer;
    lv_obj_t *ui_timer_time_left;
    lv_obj_t *ui_start_pause_button;
} timer_item_ui_t;

typedef struct {
    lv_obj_t *tile;
    lv_obj_t *ring;
    lv_obj_t *list;
    lv_obj_t *empty_label;
} timer_page_t;

static void page_init(timer_page_t *page, lv_obj_t *tile, const char *title, const char *empty_text, lv_color_t color);
static void create_timer_popup(lv_obj_t *parent);
static void create_timer_item(timer_page_t *page, timer_app_timer_t timer);
static void update_empty_label(timer_page_t *page);
static void ui_event_start_pause_pressed(lv_event_t *e);
static void ui_event_reset_pressed(lv_event_t *e);
static void ui_event_delete_pressed(lv_event_t *e);
static void ui_event_open_popup(lv_event_t *e);
static void ui_event_picker_button_pressed(lv_event_t *e);
static void on_tileview_change(lv_event_t *e);

static lv_obj_t *root_page = NULL;
static lv_obj_t *tv;
static lv_obj_t *ui_page_indicator;
static lv_obj_t *ui_label_clock;
static timer_page_t timer_page;
static timer_page_t alarm_page;
static timer_item_ui_t ui_timers[TIMER_UI_MAX_TIMERS];
// The timer that the ring of the timer page follows
static uint32_t ring_timer_id;

static lv_obj_t *ui_time_popup;
static lv_obj_t *ui_picker_new_alarm_time;
static uint8_t num_picker_digits;
static uint8_t picker_digits[sizeof("HHMMSS") - 1];

static on_timer_created on_timer_created_cb;
static on_timer_event on_timer_event_cb;

void timer_ui_show(lv_obj_t *root, on_timer_created on_create, on_timer_event on_event)
{
    assert(root_page == NULL);
    on_timer_created_cb = on_create;
    on_timer_event_cb = on_event;
    ring_timer_id = NO_TIMER;
    memset(ui_timers, 0, sizeof(ui_timers));

    // One glow for all pages, it stays in place while the pages are swiped
    root_page = zsw_ui_screen_create(root, TIMER_COLOR);

    tv = lv_tileview_create(root_page);
    lv_obj_set_size(tv, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(tv, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    page_init(&timer_page, lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR), "TIMERS", "No timers", TIMER_COLOR);
    page_init(&alarm_page, lv_tileview_add_tile(tv, 1, 0, LV_DIR_HOR), "ALARMS", "No alarms", ALARM_COLOR);

    // The alarm page shows the time of day above its title, to set an alarm against
    ui_label_clock = zsw_ui_label_create(alarm_page.tile, "", ZSW_FONT_CAPTION, ZSW_COLOR_TEXT);
    lv_obj_align(ui_label_clock, LV_ALIGN_TOP_MID, 0, 9);

    ui_page_indicator = zsw_ui_page_dots_create(root_page, 2);
    create_timer_popup(root_page);

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
    create_timer_item(timer.type == TYPE_TIMER ? &timer_page : &alarm_page, timer);
}

static void set_time_text(lv_obj_t *label, uint32_t hour, uint32_t min, uint32_t sec)
{
    // Most timers are shorter than an hour: leave the hours out then
    if (hour > 0) {
        lv_label_set_text_fmt(label, "%u:%02u:%02u", hour, min, sec);
    } else {
        lv_label_set_text_fmt(label, "%02u:%02u", min, sec);
    }
}

// The ring of the timer page shows how much is left of the first running timer.
static void update_ring(timer_app_timer_t *timer)
{
    uint32_t total = timer->hour * 3600 + timer->min * 60 + timer->sec;
    uint32_t remaining = timer->remaining_hour * 3600 + timer->remaining_min * 60 + timer->remaining_sec;

    if (timer->type != TYPE_TIMER) {
        return;
    }

    if (timer->state == TIMER_STATE_PLAYING && total > 0 && timer->timer_id <= ring_timer_id) {
        ring_timer_id = timer->timer_id;
        zsw_ui_ring_set_value(timer_page.ring, (remaining * ZSW_UI_RING_MAX) / total, false);
    } else if (timer->timer_id == ring_timer_id && timer->state != TIMER_STATE_PLAYING) {
        ring_timer_id = NO_TIMER;
        zsw_ui_ring_set_value(timer_page.ring, 0, true);
    }
}

void timer_ui_update_timer(timer_app_timer_t timer)
{
    if (!root_page) {
        return;
    }

    timer_item_ui_t *item = &ui_timers[timer.timer_id];
    bool running = timer.state == TIMER_STATE_PLAYING;

    set_time_text(item->ui_timer_time_left, timer.remaining_hour, timer.remaining_min, timer.remaining_sec);
    lv_obj_set_style_text_color(item->ui_timer_time_left, running ? ZSW_COLOR_TEXT : ZSW_COLOR_TEXT_DIM, LV_PART_MAIN);
    zsw_ui_round_button_set_symbol(item->ui_start_pause_button, running ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
    update_ring(&timer);
}

void timer_ui_set_time(int hour, int min, int second)
{
    LV_UNUSED(second);
    lv_label_set_text_fmt(ui_label_clock, "%02d:%02d", hour, min);
}

void timer_ui_remove_timer(timer_app_timer_t timer)
{
    assert(root_page != NULL);
    lv_obj_del(ui_timers[timer.timer_id].ui_timer);
    ui_timers[timer.timer_id].ui_timer = NULL;

    if (timer.timer_id == ring_timer_id) {
        ring_timer_id = NO_TIMER;
        zsw_ui_ring_set_value(timer_page.ring, 0, true);
    }
    update_empty_label(timer.type == TYPE_TIMER ? &timer_page : &alarm_page);
}

static void update_empty_label(timer_page_t *page)
{
    if (lv_obj_get_child_count(page->list) == 0) {
        lv_obj_remove_flag(page->empty_label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(page->empty_label, LV_OBJ_FLAG_HIDDEN);
    }
}

static void page_init(timer_page_t *page, lv_obj_t *tile, const char *title, const char *empty_text, lv_color_t color)
{
    lv_obj_remove_flag(tile, LV_OBJ_FLAG_SCROLLABLE);

    page->tile = zsw_ui_page_create(tile);
    page->ring = zsw_ui_ring_create(page->tile, color);
    zsw_ui_title_create(page->tile, title, ZSW_COLOR_TEXT_DIM);

    page->empty_label = lv_label_create(page->tile);
    lv_label_set_text_static(page->empty_label, empty_text);
    lv_obj_set_style_text_font(page->empty_label, ZSW_FONT_BODY, LV_PART_MAIN);
    lv_obj_set_style_text_color(page->empty_label, ZSW_COLOR_TEXT_DISABLED, LV_PART_MAIN);
    lv_obj_align(page->empty_label, LV_ALIGN_CENTER, 0, -14);

    page->list = lv_obj_create(page->tile);
    lv_obj_remove_style_all(page->list);
    lv_obj_set_size(page->list, LIST_WIDTH, LIST_HEIGHT);
    lv_obj_align(page->list, LV_ALIGN_TOP_MID, 0, LIST_TOP);
    lv_obj_set_flex_flow(page->list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(page->list, 6, LV_PART_MAIN);
    lv_obj_set_scroll_dir(page->list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(page->list, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *add_button = zsw_ui_round_button_create(page->tile, LV_SYMBOL_PLUS, ADD_BUTTON_SIZE, color);
    lv_obj_align(add_button, LV_ALIGN_BOTTOM_MID, 0, -24);
    lv_obj_add_event_cb(add_button, ui_event_open_popup, LV_EVENT_CLICKED, NULL);
    zsw_ui_slide_in(add_button, 0);
}

static lv_obj_t *create_item_button(lv_obj_t *parent, const char *symbol, lv_color_t color, lv_event_cb_t cb,
                                    uint32_t timer_id)
{
    lv_obj_t *button = zsw_ui_round_button_create(parent, symbol, ITEM_BUTTON_SIZE, color);
    lv_obj_set_style_text_font(lv_obj_get_child(button, 0), ZSW_FONT_BODY, LV_PART_MAIN);
    lv_obj_set_ext_click_area(button, ITEM_BUTTON_EXT_CLICK);
    lv_obj_add_flag(button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_event_cb(button, cb, LV_EVENT_CLICKED, (void *)(uintptr_t)timer_id);

    return button;
}

static void create_timer_item(timer_page_t *page, timer_app_timer_t timer)
{
    assert(root_page != NULL);
    timer_item_ui_t *item = &ui_timers[timer.timer_id];
    lv_color_t color = timer.type == TYPE_TIMER ? TIMER_COLOR : ALARM_COLOR;

    item->ui_timer = lv_obj_create(page->list);
    lv_obj_remove_style_all(item->ui_timer);
    lv_obj_set_size(item->ui_timer, LV_PCT(100), ITEM_HEIGHT);
    lv_obj_set_style_radius(item->ui_timer, ITEM_HEIGHT / 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(item->ui_timer, ZSW_COLOR_SURFACE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(item->ui_timer, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_pad_left(item->ui_timer, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_right(item->ui_timer, 7, LV_PART_MAIN);
    lv_obj_set_style_pad_column(item->ui_timer, 3, LV_PART_MAIN);
    lv_obj_set_flex_flow(item->ui_timer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(item->ui_timer, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(item->ui_timer, LV_OBJ_FLAG_SCROLLABLE);

    item->ui_timer_time_left = lv_label_create(item->ui_timer);
    lv_obj_set_flex_grow(item->ui_timer_time_left, 1);
    lv_label_set_long_mode(item->ui_timer_time_left, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(item->ui_timer_time_left, ZSW_FONT_TITLE, LV_PART_MAIN);
    lv_obj_set_style_text_color(item->ui_timer_time_left, ZSW_COLOR_TEXT_DIM, LV_PART_MAIN);
    set_time_text(item->ui_timer_time_left, timer.hour, timer.min, timer.sec);

    item->ui_start_pause_button = create_item_button(item->ui_timer, LV_SYMBOL_PLAY, color,
                                                     ui_event_start_pause_pressed, timer.timer_id);
    if (timer.type == TYPE_TIMER) {
        create_item_button(item->ui_timer, LV_SYMBOL_REFRESH, ZSW_COLOR_SURFACE_HIGH, ui_event_reset_pressed,
                           timer.timer_id);
    }
    lv_obj_t *delete_button = create_item_button(item->ui_timer, LV_SYMBOL_TRASH, ZSW_COLOR_SURFACE_HIGH,
                                                 ui_event_delete_pressed, timer.timer_id);
    lv_obj_set_style_text_color(lv_obj_get_child(delete_button, 0), ZSW_COLOR_RED, LV_PART_MAIN);

    update_empty_label(page);
    zsw_ui_slide_in(item->ui_timer, 0);
}

static void create_key(lv_obj_t *parent, const char *text, uint32_t key, lv_color_t color, int column, int row)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, KEY_WIDTH, KEY_HEIGHT);
    lv_obj_align(button, LV_ALIGN_TOP_MID, (column - 1) * (KEY_WIDTH + KEY_GAP),
                 KEYPAD_TOP + row * (KEY_HEIGHT + KEY_GAP));
    lv_obj_set_style_bg_color(button, color, LV_PART_MAIN);
    lv_obj_set_style_pad_all(button, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(button, ui_event_picker_button_pressed, LV_EVENT_CLICKED, (void *)(uintptr_t)key);

    lv_obj_t *label = zsw_ui_label_create(button, text, ZSW_FONT_BODY, ZSW_COLOR_TEXT);
    lv_obj_center(label);
}

// The number pad where the time of a new timer or alarm is typed
static void create_timer_popup(lv_obj_t *parent)
{
    static const char *const digits[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

    assert(root_page != NULL);

    // Opaque and full screen: the page under it is not drawn while it is open
    ui_time_popup = zsw_ui_screen_create(parent, TIMER_COLOR);
    lv_obj_add_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE);

    ui_picker_new_alarm_time = zsw_ui_label_create(ui_time_popup, "00:00:00", ZSW_FONT_HEADLINE, ZSW_COLOR_TEXT);
    lv_obj_align(ui_picker_new_alarm_time, LV_ALIGN_TOP_MID, 0, 22);

    for (int i = 0; i < ARRAY_SIZE(digits); i++) {
        create_key(ui_time_popup, digits[i], i + 1, ZSW_COLOR_SURFACE_HIGH, i % 3, i / 3);
    }
    create_key(ui_time_popup, LV_SYMBOL_BACKSPACE, KEY_BACKSPACE, ZSW_COLOR_SURFACE, 0, 3);
    create_key(ui_time_popup, "0", 0, ZSW_COLOR_SURFACE_HIGH, 1, 3);
    create_key(ui_time_popup, LV_SYMBOL_OK, KEY_ADD, ZSW_COLOR_GREEN, 2, 3);
}

static void ui_event_open_popup(lv_event_t *e)
{
    LV_UNUSED(e);
    num_picker_digits = 0;
    memset(picker_digits, 0, sizeof(picker_digits));
    lv_label_set_text(ui_picker_new_alarm_time, "00:00:00");
    lv_obj_remove_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN);
}

static ui_timer_type_t get_current_page_type(void)
{
    return lv_tileview_get_tile_active(tv) == lv_obj_get_parent(alarm_page.tile) ? TYPE_ALARM : TYPE_TIMER;
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
    uint32_t value = (uint32_t)(uintptr_t)lv_event_get_user_data(e);

    if (value == KEY_ADD) {
        lv_obj_add_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN);
        // Nothing typed: the check mark only closes the number pad
        if (num_picker_digits > 0) {
            on_timer_created_cb(picker_digits[5] * 10 + picker_digits[4], picker_digits[3] * 10 + picker_digits[2],
                                picker_digits[1] * 10 + picker_digits[0], get_current_page_type());
        }
        return;
    }

    if (value == KEY_BACKSPACE) {
        if (num_picker_digits == 0) {
            // Nothing left to remove: close the number pad
            lv_obj_add_flag(ui_time_popup, LV_OBJ_FLAG_HIDDEN);
            return;
        }
        shift_out_number(picker_digits, sizeof(picker_digits));
        num_picker_digits--;
    } else if ((num_picker_digits < sizeof(picker_digits)) && !(value == 0 && num_picker_digits == 0)) {
        shift_in_number(picker_digits, sizeof(picker_digits), value);
        num_picker_digits++;
    }

    lv_label_set_text_fmt(ui_picker_new_alarm_time, "%02d:%02d:%02d", picker_digits[5] * 10 + picker_digits[4],
                          picker_digits[3] * 10 + picker_digits[2], picker_digits[1] * 10 + picker_digits[0]);
}

static void ui_event_start_pause_pressed(lv_event_t *e)
{
    on_timer_event_cb(TIMER_EVT_START_PAUSE_RESUME, (uint32_t)(uintptr_t)lv_event_get_user_data(e));
}

static void ui_event_reset_pressed(lv_event_t *e)
{
    on_timer_event_cb(TIMER_EVT_RESET, (uint32_t)(uintptr_t)lv_event_get_user_data(e));
}

static void ui_event_delete_pressed(lv_event_t *e)
{
    on_timer_event_cb(TIMER_EVT_DELETE, (uint32_t)(uintptr_t)lv_event_get_user_data(e));
}

static void on_tileview_change(lv_event_t *e)
{
    LV_UNUSED(e);
    zsw_ui_page_dots_set_page(ui_page_indicator, get_current_page_type() == TYPE_ALARM ? 1 : 0);
}
