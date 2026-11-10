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

#include <zephyr/kernel.h>
#include <lvgl.h>
#include <lvgl_private.h>
#include "living_sky_blend.h"

static lv_draw_sw_custom_blend_handler_t *registered;

static bool is_plain_multiply(const lv_draw_task_t *t, const lv_draw_sw_blend_dsc_t *dsc)
{
    return dsc->src_buf && dsc->blend_mode == LV_BLEND_MODE_MULTIPLY &&
           dsc->src_color_format == LV_COLOR_FORMAT_RGB565 && dsc->opa >= LV_OPA_MAX &&
           (dsc->mask_buf == NULL || dsc->mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) &&
           t->target_layer->color_format == LV_COLOR_FORMAT_RGB565;
}

// Gives the same result as LVGL's own multiply.
static void multiply_rgb565(lv_draw_task_t *t, const lv_draw_sw_blend_dsc_t *dsc)
{
    lv_layer_t *layer = t->target_layer;
    lv_area_t area;

    if (!lv_area_intersect(&area, dsc->blend_area, &t->clip_area) ||
        !lv_area_intersect(&area, &area, dsc->src_area)) {
        return;
    }

    int32_t w = lv_area_get_width(&area);
    int32_t h = lv_area_get_height(&area);
    uint32_t dest_stride = layer->draw_buf->header.stride;
    uint8_t *dest = lv_draw_layer_go_to_xy(layer, area.x1 - layer->buf_area.x1, area.y1 - layer->buf_area.y1);
    const uint8_t *src = (const uint8_t *)dsc->src_buf + dsc->src_stride * (area.y1 - dsc->src_area->y1) +
                         (area.x1 - dsc->src_area->x1) * sizeof(uint16_t);

    for (int32_t y = 0; y < h; y++) {
        uint16_t *d = (uint16_t *)dest;
        const uint16_t *s = (const uint16_t *)src;

        for (int32_t x = 0; x < w; x++) {
            uint32_t sc = s[x];
            uint32_t dc = d[x];

            // Pure white counts as 1, like in LVGL.
            if (sc == 0xFFFF) {
                continue;
            }
            uint32_t r = ((dc >> 11) * (sc >> 11)) >> 5;
            uint32_t g = (((dc >> 5) & 0x3F) * ((sc >> 5) & 0x3F)) >> 6;
            uint32_t b = ((dc & 0x1F) * (sc & 0x1F)) >> 5;
            d[x] = (uint16_t)((r << 11) | (g << 5) | b);
        }
        dest += dest_stride;
        src += dsc->src_stride;
    }
}

static void blend_handler(lv_draw_task_t *t, const lv_draw_sw_blend_dsc_t *dsc)
{
    if (is_plain_multiply(t, dsc)) {
        multiply_rgb565(t, dsc);
        return;
    }

    // LVGL calls a registered handler for everything, so hide it while LVGL does the blend.
    // There is only one SW draw unit, so nothing else blends meanwhile.
    registered->handler = NULL;
    lv_draw_sw_blend(t, dsc);
    registered->handler = blend_handler;
}

void living_sky_blend_register(void)
{
    lv_draw_sw_custom_blend_handler_t handler = {
        .dest_cf = LV_COLOR_FORMAT_RGB565,
        .handler = blend_handler,
    };
    lv_draw_sw_custom_blend_handler_t *it;

    if (registered || !lv_draw_sw_register_blend_handler(&handler)) {
        return;
    }

    // LVGL keeps its own copy of the handler.
    LV_LL_READ(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, it) {
        if (it->dest_cf == LV_COLOR_FORMAT_RGB565) {
            registered = it;
            break;
        }
    }
    __ASSERT(registered, "Blend handler not found after registration");
}

void living_sky_blend_unregister(void)
{
    if (registered) {
        lv_draw_sw_unregister_blend_handler(LV_COLOR_FORMAT_RGB565);
        registered = NULL;
    }
}
