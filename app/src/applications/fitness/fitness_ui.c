#include "fitness_ui.h"
#include "ui/zsw_ui.h"
#include <lvgl.h>

static lv_obj_t *root_page = NULL;
static lv_obj_t *ui_title_label = NULL;
static lv_obj_t *ui_weekly_chart = NULL;
static lv_chart_series_t *ui_weekly_chart_series_1 = NULL;

static void create_step_chart(lv_obj_t *ui_root_container, uint16_t max_samples)
{
    //lv_obj_remove_style_all(ui_root_container);
    lv_obj_set_width(ui_root_container, lv_pct(100));
    lv_obj_set_height(ui_root_container, lv_pct(100));
    lv_obj_set_align(ui_root_container, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_title_label = lv_label_create(ui_root_container);
    lv_obj_set_width(ui_title_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_title_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_title_label, 0);
    lv_obj_set_y(ui_title_label, -90);
    lv_obj_set_align(ui_title_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_title_label, "Steps last 7 days");

    ui_weekly_chart = lv_chart_create(ui_root_container);
    lv_obj_set_width(ui_weekly_chart, 150);
    lv_obj_set_height(ui_weekly_chart, 127);
    lv_obj_set_x(ui_weekly_chart, 20);
    lv_obj_set_y(ui_weekly_chart, 0);
    lv_obj_set_align(ui_weekly_chart, LV_ALIGN_CENTER);
    lv_chart_set_type(ui_weekly_chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(ui_weekly_chart, max_samples);
    lv_chart_set_range(ui_weekly_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_range(ui_weekly_chart, LV_CHART_AXIS_SECONDARY_Y, 0, 0);
    lv_chart_set_div_line_count(ui_weekly_chart, 0, 0);
    lv_chart_set_axis_tick(ui_weekly_chart, LV_CHART_AXIS_PRIMARY_X, 5, 1, 7, 1, true, 50);
    lv_chart_set_axis_tick(ui_weekly_chart, LV_CHART_AXIS_PRIMARY_Y, 5, 1, 5, 1, true, 50);
    lv_chart_set_axis_tick(ui_weekly_chart, LV_CHART_AXIS_SECONDARY_Y, 1, 1, 1, 1, false, 25);
    ui_weekly_chart_series_1 = lv_chart_add_series(ui_weekly_chart, lv_color_hex(0x808080), LV_CHART_AXIS_PRIMARY_Y);
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

    create_step_chart(root_page, max_samples);
}

void fitness_ui_set_weekly_steps(uint16_t *samples, uint16_t num_samples)
{
    assert(ui_weekly_chart_series_1 != NULL);

    for (int i = 0; i < num_samples; i++) {
        lv_chart_set_next_value(ui_weekly_chart, ui_weekly_chart_series_1, samples[i]);
    }
}

void fitness_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}