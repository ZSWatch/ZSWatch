#include "fitness_ui.h"
#include "ui/zsw_ui.h"
#include <lvgl.h>

static lv_obj_t *root_page = NULL;
static lv_obj_t *ui_step_progress_label = NULL;
static lv_obj_t *ui_weekly_chart = NULL;
static lv_obj_t *ui_step_goal_arc = NULL;
static lv_chart_series_t *ui_weekly_chart_series_1 = NULL;

static void create_step_chart(lv_obj_t *ui_root_container, uint16_t max_samples)
{
    lv_obj_t *ui_step_goal_chart_line = lv_obj_create(ui_root_container);
    lv_obj_set_height(ui_step_goal_chart_line, 5);
    lv_obj_set_width(ui_step_goal_chart_line, lv_pct(100));
    lv_obj_set_x(ui_step_goal_chart_line, 0);
    lv_obj_set_y(ui_step_goal_chart_line, -27);
    lv_obj_set_align(ui_step_goal_chart_line, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_step_goal_chart_line, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_step_goal_chart_line, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_step_goal_chart_line, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_blend_mode(ui_step_goal_chart_line, LV_BLEND_MODE_NORMAL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_step_goal_chart_line, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_step_goal_chart_line, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_step_goal_chart_line, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_step_goal_chart_line, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_step_goal_chart_line, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_step_progress_label = lv_label_create(ui_root_container);
    lv_obj_set_width(ui_step_progress_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_step_progress_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_step_progress_label, 0);
    lv_obj_set_y(ui_step_progress_label, -90);
    lv_obj_set_align(ui_step_progress_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_step_progress_label, "1234/10000");

    ui_weekly_chart = lv_chart_create(ui_root_container);
    lv_obj_set_width(ui_weekly_chart, 231);
    lv_obj_set_height(ui_weekly_chart, 127);
    lv_obj_set_x(ui_weekly_chart, 0);
    lv_obj_set_y(ui_weekly_chart, 10);
    lv_obj_set_align(ui_weekly_chart, LV_ALIGN_CENTER);
    lv_chart_set_type(ui_weekly_chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(ui_weekly_chart, 7);
    lv_chart_set_range(ui_weekly_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 12000);
    lv_chart_set_range(ui_weekly_chart, LV_CHART_AXIS_SECONDARY_Y, 0, 0);
    lv_chart_set_div_line_count(ui_weekly_chart, 0, 0);
    lv_chart_set_axis_tick(ui_weekly_chart, LV_CHART_AXIS_PRIMARY_X, 1, 1, 1, 1, false, 50);
    lv_chart_set_axis_tick(ui_weekly_chart, LV_CHART_AXIS_PRIMARY_Y, 1, 1, 1, 1, false, 50);
    lv_chart_set_axis_tick(ui_weekly_chart, LV_CHART_AXIS_SECONDARY_Y, 1, 1, 1, 1, false, 25);
    ui_weekly_chart_series_1 = lv_chart_add_series(ui_weekly_chart, lv_color_hex(0x9E3939),
                                                   LV_CHART_AXIS_PRIMARY_Y);

    lv_obj_set_style_bg_color(ui_weekly_chart, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_weekly_chart, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_weekly_chart, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_weekly_chart, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_weekly_chart, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_step_goal_arc = lv_arc_create(ui_root_container);
    lv_obj_set_width(ui_step_goal_arc, lv_pct(100));
    lv_obj_set_height(ui_step_goal_arc, lv_pct(100));
    lv_obj_set_align(ui_step_goal_arc, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_step_goal_arc, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK |
                      LV_OBJ_FLAG_CLICK_FOCUSABLE);      /// Flags
    lv_arc_set_range(ui_step_goal_arc, 0, 10000);
    lv_arc_set_value(ui_step_goal_arc, 2000);
    lv_arc_set_bg_angles(ui_step_goal_arc, 0, 359);
    lv_arc_set_rotation(ui_step_goal_arc, 270);
    lv_obj_set_style_pad_left(ui_step_goal_arc, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_step_goal_arc, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_step_goal_arc, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_step_goal_arc, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_step_goal_arc, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_step_goal_arc, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_step_goal_arc, lv_color_hex(0x5F6571), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_step_goal_arc, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_step_goal_arc, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_pad_left(ui_step_goal_arc, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_step_goal_arc, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_step_goal_arc, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_step_goal_arc, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_step_goal_arc, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_step_goal_arc, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui_step_goal_arc, lv_color_hex(0x00921A), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_step_goal_arc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_step_goal_arc, 4, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_step_goal_arc, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_step_goal_arc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    lv_obj_t *ui_last_7_days_title_label = lv_label_create(ui_root_container);
    lv_obj_set_width(ui_last_7_days_title_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_last_7_days_title_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_last_7_days_title_label, 0);
    lv_obj_set_y(ui_last_7_days_title_label, -20);
    lv_obj_set_align(ui_last_7_days_title_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_last_7_days_title_label, "Last 7 days");
}

void fitness_ui_show(lv_obj_t *root, uint16_t max_samples)
{
    assert(root_page == NULL);

    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));

    lv_obj_set_width(root_page, lv_pct(100));
    lv_obj_set_height(root_page, lv_pct(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_set_style_pad_all(root_page, 0, LV_PART_MAIN);

    create_step_chart(root_page, max_samples);
}

void fitness_ui_set_weekly_steps(uint16_t *samples, uint16_t num_samples)
{
    assert(ui_weekly_chart_series_1 != NULL);

    for (int i = 0; i < num_samples; i++) {
        lv_chart_set_next_value(ui_weekly_chart, ui_weekly_chart_series_1, samples[i]);
    }
    lv_label_set_text_fmt(ui_step_progress_label, "%d / %d", samples[num_samples - 1], 10000);
    lv_arc_set_value(ui_step_goal_arc, samples[num_samples - 1]);
}

void fitness_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}