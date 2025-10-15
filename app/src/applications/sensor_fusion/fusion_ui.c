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

#include <sensor_fusion/fusion_ui.h>
#include <lvgl.h>
#include "assert.h"
#include <math.h>

static lv_obj_t *root_page = NULL;

static lv_obj_t *roll_label;
static lv_obj_t *pitch_label;
static lv_obj_t *yaw_label;
static lv_obj_t *stats_label;
static lv_obj_t *btn_zero;

static on_close_cb_t close_callback;
static fusion_ui_zero_cb_t zero_cb_fn = NULL;

// Quaternion used for rendering
static float q_w = 1.0f;
static float q_x = 0.0f;
static float q_y = 0.0f;
static float q_z = 0.0f;

static void cube_draw_event(lv_event_t *e);
static void zero_btn_event_cb(lv_event_t *e);

void fusion_ui_show(lv_obj_t *root, on_close_cb_t close_cb,
                    fusion_ui_zero_cb_t zero_cb)
{
    assert(root_page == NULL);

    close_callback = close_cb;
    zero_cb_fn = zero_cb;

    root_page = lv_obj_create(root);
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(root_page, 0, 0);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_center(root_page);
    lv_obj_add_event_cb(root_page, cube_draw_event, LV_EVENT_DRAW_MAIN, NULL);

    // Labels for euler overlay
    roll_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(roll_label, lv_color_white(), 0);
    lv_obj_align(roll_label, LV_ALIGN_LEFT_MID, 8, -24);
    lv_label_set_text(roll_label, "Roll: 0°");

    pitch_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(pitch_label, lv_color_white(), 0);
    lv_obj_align(pitch_label, LV_ALIGN_LEFT_MID, 8, -8);
    lv_label_set_text(pitch_label, "Pitch: 0°");

    yaw_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(yaw_label, lv_color_white(), 0);
    lv_obj_align(yaw_label, LV_ALIGN_LEFT_MID, 8, 8);
    lv_label_set_text(yaw_label, "Yaw: 0°");

    // Stats overlay
    stats_label = lv_label_create(root_page);
    lv_obj_set_style_text_color(stats_label, lv_color_white(), 0);
    lv_obj_align(stats_label, LV_ALIGN_TOP_MID, 0, 16);
    lv_label_set_text(stats_label, "R: -- Hz");

    // Zero button
    btn_zero = lv_btn_create(root_page);
    lv_obj_set_size(btn_zero, 80, 28);

    lv_obj_align(btn_zero, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(btn_zero, zero_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *zero_lbl = lv_label_create(btn_zero);
    lv_label_set_text(zero_lbl, "Zero");
    lv_obj_center(zero_lbl);
}

void fusion_ui_remove(void)
{
    if (!root_page) {
        return;
    }
    lv_obj_del(root_page);
    root_page = NULL;
}

void fusion_ui_set_values(int32_t x, int32_t y, int32_t z)
{
    if (!root_page) {
        return;
    }
    lv_label_set_text_fmt(roll_label, "Roll: %d°", x);
    lv_label_set_text_fmt(pitch_label, "Pitch: %d°", y);
    lv_label_set_text_fmt(yaw_label, "Yaw: %d°", z);
}

void fusion_ui_set_quaternion(float w, float x, float y, float z)
{
    q_w = w;
    q_x = x;
    q_y = y;
    q_z = z;
    if (root_page) {
        lv_obj_invalidate(root_page);
    }
}

void fusion_ui_set_stats(float render_hz)
{
    if (!root_page || !stats_label) {
        return;
    }
    bool valid = render_hz > 0.5f;
    if (valid) {
        lv_label_set_text_fmt(stats_label, "R: %.0f Hz", render_hz);
    } else {
        lv_label_set_text(stats_label, "R: -- Hz");
    }
}

static inline void quat_to_rotm(float w, float x, float y, float z, float R[3][3])
{
    // Normalize to avoid drift artifacts
    float n = sqrtf(w * w + x * x + y * y + z * z);
    if (n > 0.0f) {
        w /= n;
        x /= n;
        y /= n;
        z /= n;
    }
    float xx = x * x, yy = y * y, zz = z * z;
    float xy = x * y, xz = x * z, yz = y * z;
    float wx = w * x, wy = w * y, wz = w * z;
    R[0][0] = 1.0f - 2.0f * (yy + zz);
    R[0][1] = 2.0f * (xy - wz);
    R[0][2] = 2.0f * (xz + wy);
    R[1][0] = 2.0f * (xy + wz);
    R[1][1] = 1.0f - 2.0f * (xx + zz);
    R[1][2] = 2.0f * (yz - wx);
    R[2][0] = 2.0f * (xz - wy);
    R[2][1] = 2.0f * (yz + wx);
    R[2][2] = 1.0f - 2.0f * (xx + yy);
}

static void cube_draw_event(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_layer_t *layer = lv_event_get_layer(e);
    if (!obj || !layer) {
        return;
    }

    const lv_coord_t w = lv_obj_get_content_width(obj);
    const lv_coord_t h = lv_obj_get_content_height(obj);
    const float cx = w * 0.5f;
    const float cy = h * 0.5f;

    // Define cube vertices in object space (side length 2*sz)
    const float sz = 0.9f; // unit size
    const float verts[8][3] = {
        {-sz, -sz, -sz}, { sz, -sz, -sz}, { sz,  sz, -sz}, {-sz,  sz, -sz},
        {-sz, -sz,  sz}, { sz, -sz,  sz}, { sz,  sz,  sz}, {-sz,  sz,  sz}
    };
    const uint8_t edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // back face
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // front face
        {0, 4}, {1, 5}, {2, 6}, {3, 7} // connectors
    };

    float Rm[3][3];
    quat_to_rotm(q_w, q_x, q_y, q_z, Rm);

    // Perspective parameters
    const float d = 3.0f;      // camera distance
    const float scale = (w < h ? w : h) * 0.42f; // pixel scale
    const float f = scale;     // focal length scale
    const float z_bias = 0.0f; // optional bias

    // Draw edges
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_white();
    line_dsc.width = 2;
    line_dsc.opa = LV_OPA_COVER;
    line_dsc.round_start = 1;
    line_dsc.round_end = 1;

    // Compute projected origin
    float r0x = 0.0f, r0y = 0.0f, r0z = 0.0f;
    float dz0 = r0z + d;
    lv_point_precise_t p0 = { .x = cx + f * (r0x / dz0), .y = cy - f * (r0y / dz0) };

    // Axis triad in RGB
    const float al = sz * 1.2f;
    const float axes[3][3] = {{al, 0, 0}, {0, al, 0}, {0, 0, al}};
    const lv_color_t axis_colors[3] = { LV_COLOR_MAKE(255, 0, 0), LV_COLOR_MAKE(0, 255, 0), LV_COLOR_MAKE(0, 140, 255) };
    for (int ai = 0; ai < 3; ai++) {
        float ax = axes[ai][0], ay = axes[ai][1], az = axes[ai][2];
        float rax = Rm[0][0] * ax + Rm[0][1] * ay + Rm[0][2] * az;
        float ray = Rm[1][0] * ax + Rm[1][1] * ay + Rm[1][2] * az;
        float raz = Rm[2][0] * ax + Rm[2][1] * ay + Rm[2][2] * az;
        float dz = raz + d;
        if (dz < 0.1f) {
            dz = 0.1f;
        }
        float px = cx + f * (rax / dz);
        float py = cy - f * (ray / dz);
        line_dsc.color = axis_colors[ai];
        line_dsc.p1 = p0;
        line_dsc.p2.x = px;
        line_dsc.p2.y = py;
        lv_draw_line(layer, &line_dsc);
    }

    for (int i = 0; i < 12; i++) {
        int a = edges[i][0];
        int b = edges[i][1];
        float ax = verts[a][0], ay = verts[a][1], az = verts[a][2];
        float bx = verts[b][0], by = verts[b][1], bz = verts[b][2];

        // Rotate
        float rax = Rm[0][0] * ax + Rm[0][1] * ay + Rm[0][2] * az;
        float ray = Rm[1][0] * ax + Rm[1][1] * ay + Rm[1][2] * az;
        float raz = Rm[2][0] * ax + Rm[2][1] * ay + Rm[2][2] * az + z_bias;
        float rbx = Rm[0][0] * bx + Rm[0][1] * by + Rm[0][2] * bz;
        float rby = Rm[1][0] * bx + Rm[1][1] * by + Rm[1][2] * bz;
        float rbz = Rm[2][0] * bx + Rm[2][1] * by + Rm[2][2] * bz + z_bias;

        // Project
        float dz_a = raz + d;
        float dz_b = rbz + d;
        if (dz_a < 0.1f) {
            dz_a = 0.1f;
        }
        if (dz_b < 0.1f) {
            dz_b = 0.1f;
        }
        float px1 = cx + f * (rax / dz_a);
        float py1 = cy - f * (ray / dz_a);
        float px2 = cx + f * (rbx / dz_b);
        float py2 = cy - f * (rby / dz_b);

        line_dsc.p1.x = px1;
        line_dsc.p1.y = py1;
        line_dsc.p2.x = px2;
        line_dsc.p2.y = py2;
        lv_draw_line(layer, &line_dsc);
    }
}

static void zero_btn_event_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (zero_cb_fn) {
        zero_cb_fn();
    }
}
