#include "stopwatch_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include "ui/zsw_ui.h"
#include <lvgl.h>
#include <assert.h>
#include <stdio.h>

static void on_start_pause_clicked(lv_event_t *e);
static void on_reset_clicked(lv_event_t *e);
static void on_lap_clicked(lv_event_t *e);
static void format_time(uint32_t ms, char *buf, size_t buf_size);

typedef struct {
    lv_obj_t *root_page;
    lv_obj_t *time_label;
    lv_obj_t *start_pause_btn;
    lv_obj_t *reset_btn;
    lv_obj_t *lap_btn;
    lv_obj_t *lap_list;
    lv_obj_t *progress_arc;
    int lap_count;
} stopwatch_ui_t;

static stopwatch_ui_t ui = {0};

static stopwatch_event_cb_t start_cb = NULL;
static stopwatch_event_cb_t pause_cb = NULL;
static stopwatch_event_cb_t reset_cb = NULL;
static stopwatch_event_cb_t lap_cb = NULL;

static stopwatch_state_t current_state = STOPWATCH_STATE_STOPPED;

void stopwatch_ui_show(lv_obj_t *root, stopwatch_event_cb_t start_callback, stopwatch_event_cb_t pause_callback,
                       stopwatch_event_cb_t reset_callback, stopwatch_event_cb_t lap_callback)
{
    assert(ui.root_page == NULL);

    start_cb = start_callback;
    pause_cb = pause_callback;
    reset_cb = reset_callback;
    lap_cb = lap_callback;

    // Create the root container
    ui.root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(ui.root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(ui.root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(ui.root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(ui.root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(ui.root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui.root_page, 10, LV_PART_MAIN);

    // Progress arc around the screen perimeter (1 second is one lap)
    ui.progress_arc = lv_arc_create(ui.root_page);
    lv_obj_set_size(ui.progress_arc, LV_PCT(95), LV_PCT(95));
    lv_obj_center(ui.progress_arc);
    lv_arc_set_range(ui.progress_arc, 0, 1000);
    lv_arc_set_value(ui.progress_arc, 0);
    lv_arc_set_rotation(ui.progress_arc, 270);
    lv_arc_set_bg_angles(ui.progress_arc, 0, 360);
    lv_obj_set_style_arc_width(ui.progress_arc, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui.progress_arc, zsw_color_gray(), LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui.progress_arc, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ui.progress_arc, zsw_color_blue(), LV_PART_INDICATOR);
    lv_obj_remove_style(ui.progress_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ui.progress_arc, LV_OBJ_FLAG_CLICKABLE);

    // The stopwatch time label
    ui.time_label = lv_label_create(ui.root_page);
    lv_label_set_text(ui.time_label, "00:00.00");
    lv_obj_set_style_text_font(ui.time_label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui.time_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_align(ui.time_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_width(ui.time_label, 150); // Fixed width to prevent jumping
    lv_obj_align(ui.time_label, LV_ALIGN_CENTER, 0, -60);

    // Buttons container
    lv_obj_t *btn_container = lv_obj_create(ui.root_page);
    lv_obj_set_style_border_width(btn_container, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(btn_container, 0, LV_PART_MAIN);
    lv_obj_set_size(btn_container, 200, 50);
    lv_obj_align(btn_container, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Lap button
    ui.lap_btn = lv_btn_create(btn_container);
    lv_obj_set_size(ui.lap_btn, 45, 45);
    lv_obj_add_event_cb(ui.lap_btn, on_lap_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(ui.lap_btn, zsw_color_gray(), LV_PART_MAIN);
    lv_obj_set_style_radius(ui.lap_btn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_add_flag(ui.lap_btn, LV_OBJ_FLAG_HIDDEN); // Hidden unless running

    lv_obj_t *lap_label = lv_label_create(ui.lap_btn);
    lv_label_set_text(lap_label, LV_SYMBOL_LIST);
    lv_obj_set_style_text_color(lap_label, zsw_color_red(), LV_PART_MAIN);
    lv_obj_set_style_text_font(lap_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_center(lap_label);

    // Play/Pause button
    ui.start_pause_btn = lv_btn_create(btn_container);
    lv_obj_set_size(ui.start_pause_btn, 50, 50);
    lv_obj_add_event_cb(ui.start_pause_btn, on_start_pause_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(ui.start_pause_btn, zsw_color_gray(), LV_PART_MAIN);
    lv_obj_set_style_radius(ui.start_pause_btn, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    lv_obj_t *start_pause_label = lv_label_create(ui.start_pause_btn);
    lv_label_set_text(start_pause_label, LV_SYMBOL_PLAY);
    lv_obj_set_style_text_color(start_pause_label, zsw_color_red(), LV_PART_MAIN);
    lv_obj_set_style_text_font(start_pause_label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_center(start_pause_label);

    // Reset button
    ui.reset_btn = lv_btn_create(btn_container);
    lv_obj_set_size(ui.reset_btn, 45, 45);
    lv_obj_add_event_cb(ui.reset_btn, on_reset_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(ui.reset_btn, zsw_color_gray(), LV_PART_MAIN);
    lv_obj_set_style_radius(ui.reset_btn, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    lv_obj_t *reset_label = lv_label_create(ui.reset_btn);
    lv_label_set_text(reset_label, LV_SYMBOL_REFRESH);
    lv_obj_set_style_text_color(reset_label, zsw_color_red(), LV_PART_MAIN);
    lv_obj_set_style_text_font(reset_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_center(reset_label);

    // Lap times container
    ui.lap_list = lv_obj_create(ui.root_page);
    lv_obj_set_size(ui.lap_list, LV_PCT(85), 90);
    lv_obj_align(ui.lap_list, LV_ALIGN_BOTTOM_MID, 0, 5);
    lv_obj_set_style_bg_opa(ui.lap_list, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui.lap_list, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(ui.lap_list, 15, LV_PART_MAIN);
    lv_obj_set_flex_flow(ui.lap_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(ui.lap_list, 5, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(ui.lap_list, LV_SCROLLBAR_MODE_OFF);

    lv_group_focus_obj(ui.start_pause_btn);
}

void stopwatch_ui_remove(void)
{
    if (ui.root_page) {
        lv_obj_del(ui.root_page);
        memset(&ui, 0, sizeof(ui));
    }
}

void stopwatch_ui_update_time(uint32_t elapsed_ms)
{
    if (ui.time_label) {
        char time_str[16];
        format_time(elapsed_ms, time_str, sizeof(time_str));
        lv_label_set_text(ui.time_label, time_str);
    }

    // Update the spinning arc
    if (ui.progress_arc && current_state == STOPWATCH_STATE_RUNNING) {
        uint32_t total_seconds = elapsed_ms / 1000;
        uint32_t ms_in_current_second = elapsed_ms % 1000;

        // To make a nice animating effect, the arc fills in one second, then
        // goes back to the start of the next second and so on.
        if (total_seconds % 2 == 0) {
            // Clockwise
            lv_arc_set_mode(ui.progress_arc, LV_ARC_MODE_NORMAL);
            lv_arc_set_value(ui.progress_arc, ms_in_current_second);
        } else {
            // Counter-clockwise
            lv_arc_set_mode(ui.progress_arc, LV_ARC_MODE_REVERSE);
            lv_arc_set_value(ui.progress_arc, 1000 - ms_in_current_second);
        }
    }
}

void stopwatch_ui_update_state(stopwatch_state_t state)
{
    current_state = state;

    if (!ui.start_pause_btn) {
        return;
    }

    lv_obj_t *label = lv_obj_get_child(ui.start_pause_btn, 0);

    switch (state) {
        case STOPWATCH_STATE_STOPPED:
            lv_label_set_text(label, LV_SYMBOL_PLAY);
            lv_obj_add_flag(ui.lap_btn, LV_OBJ_FLAG_HIDDEN);
            if (ui.progress_arc) {
                lv_arc_set_value(ui.progress_arc, 0);
            }
            break;
        case STOPWATCH_STATE_RUNNING:
            lv_label_set_text(label, LV_SYMBOL_PAUSE);
            lv_obj_clear_flag(ui.lap_btn, LV_OBJ_FLAG_HIDDEN);
            break;
        case STOPWATCH_STATE_PAUSED:
            lv_label_set_text(label, LV_SYMBOL_PLAY);
            lv_obj_clear_flag(ui.lap_btn, LV_OBJ_FLAG_HIDDEN);
            break;
    }
}

void stopwatch_ui_add_lap_time(uint32_t lap_time, uint32_t total_time)
{
    if (!ui.lap_list) {
        return;
    }

    char time_str[16];
    format_time(lap_time, time_str, sizeof(time_str));
    ui.lap_count++;

    // Lap time label
    lv_obj_t *lap_item = lv_label_create(ui.lap_list);
    lv_label_set_text_fmt(lap_item, "L%d %s", ui.lap_count, time_str);
    lv_obj_set_style_text_color(lap_item, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(lap_item, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_width(lap_item, LV_PCT(100));
    lv_obj_set_style_text_align(lap_item, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    // Scroll to bottom to show latest lap
    lv_obj_scroll_to_y(ui.lap_list, LV_COORD_MAX, LV_ANIM_ON);
}

static void on_start_pause_clicked(lv_event_t *e)
{
    switch (current_state) {
        case STOPWATCH_STATE_STOPPED:
        case STOPWATCH_STATE_PAUSED:
            if (start_cb) {
                start_cb();
            }
            break;
        case STOPWATCH_STATE_RUNNING:
            if (pause_cb) {
                pause_cb();
            }
            break;
    }
}

static void on_reset_clicked(lv_event_t *e)
{
    if (reset_cb) {
        reset_cb();
    }

    // Clear lap list and reset lap counter
    if (ui.lap_list) {
        lv_obj_clean(ui.lap_list);
    }
    ui.lap_count = 0;
}

static void on_lap_clicked(lv_event_t *e)
{
    if (current_state == STOPWATCH_STATE_RUNNING && lap_cb) {
        lap_cb();
    }
}

static void format_time(uint32_t ms, char *buf, size_t buf_size)
{
    uint32_t total_seconds = ms / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    uint32_t centiseconds = (ms % 1000) / 10;

    snprintf(buf, buf_size, "%02u:%02u.%02u", minutes, seconds, centiseconds);
}
