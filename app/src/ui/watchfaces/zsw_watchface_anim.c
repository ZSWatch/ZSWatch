/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

#include "zsw_watchface_anim.h"

#define RISE_DISTANCE 24

static void sweep_exec(void *hand, int32_t rotation)
{
    lv_image_set_rotation(hand, rotation);
}

void zsw_watchface_anim_reset_hand(lv_obj_t *hand)
{
    lv_anim_delete(hand, sweep_exec);
    lv_image_set_rotation(hand, 0);
}

void zsw_watchface_anim_sweep(lv_obj_t *hand, int32_t rotation, uint32_t delay_ms)
{
    lv_anim_t anim;

    lv_anim_delete(hand, sweep_exec);
    lv_image_set_rotation(hand, 0);
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, hand);
    lv_anim_set_exec_cb(&anim, sweep_exec);
    lv_anim_set_values(&anim, 0, rotation);
    lv_anim_set_duration(&anim, ZSW_WATCHFACE_ANIM_SWEEP_MS);
    lv_anim_set_delay(&anim, delay_ms);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_start(&anim);
}

void zsw_watchface_anim_set_rotation(lv_obj_t *hand, int32_t rotation)
{
    lv_anim_t *anim = lv_anim_get(hand, sweep_exec);

    if (anim) {
        // The time ticked during the sweep.
        anim->end_value = rotation;
    } else {
        lv_image_set_rotation(hand, rotation);
    }
}

// Text opacity, as object opacity would render the label into a layer each frame.
static void rise_exec(void *label, int32_t progress)
{
    lv_obj_set_style_translate_y(label, RISE_DISTANCE - (RISE_DISTANCE * progress) / LV_OPA_COVER, 0);
    lv_obj_set_style_text_opa(label, progress, 0);
}

void zsw_watchface_anim_reset_label(lv_obj_t *label)
{
    lv_anim_delete(label, rise_exec);
    rise_exec(label, LV_OPA_TRANSP);
}

void zsw_watchface_anim_rise(lv_obj_t *label, uint32_t delay_ms)
{
    lv_anim_t anim;

    zsw_watchface_anim_reset_label(label);
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, label);
    lv_anim_set_exec_cb(&anim, rise_exec);
    lv_anim_set_values(&anim, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_duration(&anim, ZSW_WATCHFACE_ANIM_RISE_MS);
    lv_anim_set_delay(&anim, delay_ms);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_start(&anim);
}

static void fill_exec(void *arc, int32_t value)
{
    lv_arc_set_value(arc, value);
}

void zsw_watchface_anim_fill(lv_obj_t *arc, uint32_t delay_ms)
{
    lv_anim_t anim;
    int32_t value = lv_arc_get_value(arc);

    lv_anim_delete(arc, fill_exec);
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, arc);
    lv_anim_set_exec_cb(&anim, fill_exec);
    lv_anim_set_values(&anim, lv_arc_get_min_value(arc), value);
    lv_anim_set_duration(&anim, ZSW_WATCHFACE_ANIM_SWEEP_MS);
    lv_anim_set_delay(&anim, delay_ms);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_start(&anim);
}

void zsw_watchface_anim_set_arc(lv_obj_t *arc, int32_t value)
{
    lv_anim_t *anim = lv_anim_get(arc, fill_exec);

    if (anim) {
        anim->end_value = value;
    } else {
        lv_arc_set_value(arc, value);
    }
}
