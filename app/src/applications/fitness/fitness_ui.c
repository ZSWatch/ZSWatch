#include "fitness_ui.h"
#include "ui/zsw_ui.h"
#include <lvgl.h>
#include <assert.h>

static lv_obj_t *root_page = NULL;
static lv_obj_t *ui_step_progress_label = NULL;
static lv_obj_t *ui_weekly_chart = NULL;
static lv_obj_t *ui_step_goal_arc = NULL;
static lv_chart_series_t *ui_weekly_chart_series_1 = NULL;
static char **chart_bar_names = NULL;

static void event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *chart = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_invalidate(chart);
    } else if (code == LV_EVENT_DRAW_POST_END) {
        lv_layer_t *draw_layer = lv_event_get_layer(e);
        if (!draw_layer) {
            return;
        }

        lv_chart_series_t *ser = lv_chart_get_series_next(chart, NULL);
        if (!ser) {
            return;
        }
        int num_points = lv_chart_get_point_count(chart);

        for (int id = 0; id < num_points; id++) {
            lv_point_t p;
            lv_chart_get_point_pos_by_id(chart, ser, id, &p);

            char buf[16];
            lv_snprintf(buf, sizeof(buf), "%s", chart_bar_names[id]);

            // Draw the day of week above each bar
            lv_draw_label_dsc_t draw_label_dsc;
            lv_draw_label_dsc_init(&draw_label_dsc);
            draw_label_dsc.color = zsw_color_red();
            draw_label_dsc.align = LV_TEXT_ALIGN_CENTER;
            draw_label_dsc.text = buf;
            draw_label_dsc.font = &lv_font_montserrat_12;

            lv_area_t a;
            lv_area_t obj_coords;
            lv_obj_get_coords(chart, &obj_coords);
            a.x1 = obj_coords.x1 + p.x - 15;
            a.x2 = obj_coords.x1 + p.x + 17;
            a.y1 = obj_coords.y1;
            a.y2 = obj_coords.y1 + 20;

            lv_draw_label(draw_layer, &draw_label_dsc, &a);

            // Draw a vertical line to separate the bars
            lv_draw_rect_dsc_t draw_rect_dsc;
            lv_draw_rect_dsc_init(&draw_rect_dsc);
            draw_rect_dsc.bg_opa = LV_OPA_TRANSP;
            draw_rect_dsc.border_color = zsw_color_gray();
            draw_rect_dsc.border_width = 1;
            draw_rect_dsc.border_side = LV_BORDER_SIDE_RIGHT;

            // Note these values are not dynamic, so needs change if graph size changes.
            a.y1 = obj_coords.y1 + 20 - 30;
            a.y2 = obj_coords.y1 + 20 + 99;

            // Don't draw a line after the last bar
            if (id != num_points - 1) {
                lv_draw_rect(draw_layer, &draw_rect_dsc, &a);
            }
        }

        static int32_t id = LV_CHART_POINT_NONE;
        if (lv_chart_get_pressed_point(chart) != LV_CHART_POINT_NONE) {
            id = lv_chart_get_pressed_point(chart);
        }
        if (id != LV_CHART_POINT_NONE) {
            // Draw a rectangle with the value of the point
            // Keep point value drawn until another bar is clicked.
            lv_point_t p;
            lv_chart_get_point_pos_by_id(chart, ser, id, &p);

            lv_coord_t *y_array = lv_chart_get_y_array(chart, ser);
            lv_coord_t value = y_array[id];

            char buf[16];
            lv_snprintf(buf, sizeof(buf), LV_SYMBOL_DUMMY"%d", value);

            lv_draw_rect_dsc_t draw_rect_dsc;
            lv_draw_rect_dsc_init(&draw_rect_dsc);
            draw_rect_dsc.bg_color = lv_color_white();
            draw_rect_dsc.bg_opa = LV_OPA_70;
            draw_rect_dsc.radius = 5;
            draw_rect_dsc.bg_image_src = buf;
            draw_rect_dsc.bg_image_recolor = lv_color_black();

            lv_area_t a;
            lv_area_t obj_coords;
            lv_obj_get_coords(chart, &obj_coords);
            a.x1 = obj_coords.x1 + p.x - 20;
            a.x2 = obj_coords.x1 + p.x + 20;
            a.y1 = obj_coords.y1 + p.y - 30;
            a.y2 = obj_coords.y1 + p.y - 10;

            lv_draw_rect(draw_layer, &draw_rect_dsc, &a);
        }
    }
}

static void create_step_chart(lv_obj_t *ui_root_container, uint16_t max_samples)
{
    ui_step_progress_label = lv_label_create(ui_root_container);
    lv_obj_set_width(ui_step_progress_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_step_progress_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_step_progress_label, 0);
    lv_obj_set_y(ui_step_progress_label, -90);
    lv_obj_set_align(ui_step_progress_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(ui_step_progress_label, zsw_color_blue(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_step_progress_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(ui_step_progress_label, "- / 10000");

    ui_weekly_chart = lv_chart_create(ui_root_container);
    lv_obj_set_width(ui_weekly_chart, 231);
    lv_obj_set_height(ui_weekly_chart, 120);
    lv_obj_set_x(ui_weekly_chart, 0);
    lv_obj_set_y(ui_weekly_chart, 10);
    lv_obj_set_align(ui_weekly_chart, LV_ALIGN_CENTER);
    lv_chart_set_type(ui_weekly_chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(ui_weekly_chart, 7);
    lv_chart_set_range(ui_weekly_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 12000);
    lv_chart_set_range(ui_weekly_chart, LV_CHART_AXIS_SECONDARY_Y, 0, 0);
    lv_chart_set_div_line_count(ui_weekly_chart, 0, 0);

    // Create a scale for the Y axis (steps)
    lv_obj_t *scale_y = lv_scale_create(ui_root_container);
    lv_scale_set_mode(scale_y, LV_SCALE_MODE_VERTICAL_LEFT);
    lv_scale_set_range(scale_y, 0, 12000);
    lv_scale_set_label_show(scale_y, true);
    lv_obj_set_height(scale_y, 120);
    lv_obj_align_to(scale_y, ui_weekly_chart, LV_ALIGN_OUT_LEFT_MID, -4, 0);

    ui_weekly_chart_series_1 = lv_chart_add_series(ui_weekly_chart, zsw_color_blue(),
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
    lv_obj_set_style_arc_color(ui_step_goal_arc, lv_color_hex(0xffd147), LV_PART_INDICATOR | LV_STATE_DEFAULT);
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
    lv_label_set_text(ui_last_7_days_title_label, "Steps per day");

    lv_obj_add_flag(ui_weekly_chart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_obj_add_event_cb(ui_weekly_chart, event_cb, LV_EVENT_ALL, NULL);
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

void fitness_ui_set_weekly_steps(uint16_t *samples, char **weekday_names, uint16_t num_samples)
{
    assert(ui_weekly_chart_series_1 != NULL);

    chart_bar_names = weekday_names;

    for (int i = 0; i < num_samples; i++) {
        lv_chart_set_next_value(ui_weekly_chart, ui_weekly_chart_series_1, samples[i]);
    }
    lv_label_set_text_fmt(ui_step_progress_label, "%d / %d", samples[num_samples - 1], 10000);
    lv_obj_set_style_text_align(ui_step_progress_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_arc_set_value(ui_step_goal_arc, samples[num_samples - 1]);
}

void fitness_ui_set_daily_steps(uint32_t steps)
{
    lv_label_set_text_fmt(ui_step_progress_label, "%d / %d", steps, 10000);
    lv_obj_set_style_text_align(ui_step_progress_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_arc_set_value(ui_step_goal_arc, steps);
}

void fitness_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}