/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <compass/compass_ui.h>
#include <lvgl.h>
#include "assert.h"
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"

#define COMPASS_COLOR           lv_color_hex(0xE5484D)
/* Width of the north marker on the ring, in degrees */
#define NORTH_MARKER_WIDTH      14

static lv_obj_t *root_page = NULL;

static lv_obj_t *north_marker;
static lv_obj_t *direction_label;
static lv_obj_t *compass_label;
static int shown_heading;

static on_start_calibraion_cb_t start_cal;

static void calibrate_button_event_cb(lv_event_t *e)
{
    if (start_cal) {
        start_cal();
    }
}

static void create_ui(lv_obj_t *compass_panel)
{
    lv_obj_t *cal_btn;
    lv_obj_t *cal_btn_label;

    // North is a short piece of the ring that moves around the edge. Moving it only
    // redraws the piece, where rotating an arrow image redraws the image on every change.
    north_marker = zsw_ui_ring_create(compass_panel, COMPASS_COLOR);
    lv_arc_set_rotation(north_marker, 0);

    zsw_ui_title_create(compass_panel, "COMPASS", ZSW_COLOR_TEXT_DIM);

    direction_label = zsw_ui_label_create(compass_panel, "", ZSW_FONT_HEADLINE, COMPASS_COLOR);
    lv_obj_align(direction_label, LV_ALIGN_CENTER, 0, -50);

    compass_label = zsw_ui_label_create(compass_panel, "-", ZSW_FONT_NUMBER, ZSW_COLOR_TEXT);
    lv_obj_align(compass_label, LV_ALIGN_CENTER, 0, 0);

    cal_btn = lv_btn_create(compass_panel);
    lv_obj_set_style_pad_hor(cal_btn, 14, LV_PART_MAIN);
    lv_obj_set_style_pad_ver(cal_btn, 7, LV_PART_MAIN);
    lv_obj_align(cal_btn, LV_ALIGN_CENTER, 0, 56);
    lv_obj_set_style_bg_color(cal_btn, ZSW_COLOR_SURFACE_HIGH, LV_PART_MAIN | LV_STATE_DEFAULT);
    cal_btn_label = lv_label_create(cal_btn);
    lv_label_set_text(cal_btn_label, "Calibrate");
    lv_obj_add_event_cb(cal_btn, calibrate_button_event_cb, LV_EVENT_CLICKED, NULL);
}

void compass_ui_show(lv_obj_t *root, on_start_calibraion_cb_t start_cal_cb)
{
    assert(root_page == NULL);

    root_page = zsw_ui_screen_create(root, COMPASS_COLOR);
    start_cal = start_cal_cb;
    shown_heading = -1;

    create_ui(root_page);
}

void compass_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void compass_ui_set_heading(double heading)
{
    static const char *const directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    int degrees = ((int)(heading + 0.5)) % 360;

    // The heading is updated often, but mostly within the same degree
    if (degrees == shown_heading) {
        return;
    }
    shown_heading = degrees;

    // 12 o'clock is 270 degrees for the arc, and north is to the left of it when heading east
    int north = (270 - degrees + 360) % 360;

    lv_arc_set_angles(north_marker, (north - NORTH_MARKER_WIDTH / 2 + 360) % 360, (north + NORTH_MARKER_WIDTH / 2) % 360);
    lv_label_set_text_fmt(compass_label, "%d°", degrees);
    lv_label_set_text_static(direction_label, directions[((degrees * 2 + 45) / 90) % 8]);
}
