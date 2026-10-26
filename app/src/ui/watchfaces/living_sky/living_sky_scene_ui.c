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

#include <zephyr/sys/util.h>
#include <misc/cache/instance/lv_image_cache.h>
#include "living_sky_scene_ui.h"

#define FP_SHIFT LIVING_SKY_FP_SHIFT
#define FOG_BAND_WIDTH LIVING_SKY_FOG_BAND_WIDTH

typedef struct {
    uint16_t minute;
    uint32_t tint;
} sky_key_t;

// Multiply tints: white leaves the background as it is, other colours darken and tint it.
// Multiply never lightens, so the background keeps its contrast.
#define SKY_NIGHT(min)  { min, 0x98A4D0 }
#define SKY_DAY(min)    { min, 0xFFFFFF }

static const sky_key_t sky_keys[] = {
    SKY_NIGHT(0),
    SKY_NIGHT(5 * 60),
    { 6 * 60 + 30, 0xFFD8C0 },    // Dawn
    SKY_DAY(8 * 60 + 30),
    SKY_DAY(16 * 60 + 30),
    { 18 * 60 + 30, 0xFFC4A8 },   // Dusk
    SKY_NIGHT(20 * 60 + 30),
    SKY_NIGHT(24 * 60),
};

// The tint is drawn as a small tiled image, as LVGL only applies blend modes to images.
#define TINT_TILE_SIZE      8
static uint16_t tint_tile_px[TINT_TILE_SIZE * TINT_TILE_SIZE];
static lv_image_dsc_t tint_tile = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_RGB565,
        .w = TINT_TILE_SIZE,
        .h = TINT_TILE_SIZE,
        .stride = TINT_TILE_SIZE * sizeof(uint16_t),
    },
    .data_size = sizeof(tint_tile_px),
    .data = (const uint8_t *)tint_tile_px,
};

static int32_t sin_scaled(int32_t angle, int32_t amplitude)
{
    return (lv_trigo_sin((int16_t)(angle % 360)) * amplitude) / LV_TRIGO_SIN_MAX;
}

static lv_color_t sky_tint(const living_sky_scene_t *scene)
{
    int i;

    for (i = 0; i < (int)ARRAY_SIZE(sky_keys) - 2; i++) {
        if (scene->minute_of_day < sky_keys[i + 1].minute) {
            break;
        }
    }

    const sky_key_t *a = &sky_keys[i];
    const sky_key_t *b = &sky_keys[i + 1];
    // mix = 255 gives a, 0 gives b
    uint8_t mix = 255 - (uint8_t)(((scene->minute_of_day - a->minute) * 255) / (b->minute - a->minute));
    lv_color_t tint = lv_color_mix(lv_color_hex(a->tint), lv_color_hex(b->tint), mix);

    // Darken and grey the sky under heavy weather, fog only a little.
    if (scene->weather != ZSW_WEATHER_CLEAR && !(scene->weather == ZSW_WEATHER_CLOUDS && scene->intensity == 1)) {
        bool fog = scene->weather == ZSW_WEATHER_FOG;

        tint = lv_color_mix(lv_color_hex(fog ? 0xDCE0E4 : 0xC0C6D0), tint, 160);
    }

    return tint;
}

static void draw_circle(lv_layer_t *layer, lv_draw_rect_dsc_t *dsc, int32_t x, int32_t y, int32_t r)
{
    lv_area_t a = { x - r, y - r, x + r - 1, y + r - 1 };
    lv_draw_rect(layer, dsc, &a);
}

static void draw_sky(const living_sky_scene_t *scene, lv_layer_t *layer, const lv_area_t *coords)
{
    lv_color_t tint = sky_tint(scene);
    uint16_t tint_px = lv_color_to_u16(tint);
    lv_draw_image_dsc_t dsc;

    // Plain day: leave the background untouched (multiply by white would still darken by 1 LSB).
    if (tint.red >= 0xF8 && tint.green >= 0xFC && tint.blue >= 0xF8) {
        return;
    }

    if (tint_tile_px[0] != tint_px) {
        for (int i = 0; i < (int)ARRAY_SIZE(tint_tile_px); i++) {
            tint_tile_px[i] = tint_px;
        }
        lv_image_cache_drop(&tint_tile);
    }

    // One flat colour multiplied in: no per row variation, so no stripes on RGB565.
    lv_draw_image_dsc_init(&dsc);
    dsc.src = &tint_tile;
    dsc.tile = 1;
    dsc.blend_mode = LV_BLEND_MODE_MULTIPLY;
    lv_draw_image(layer, &dsc, coords);
}

static void draw_sun(const living_sky_scene_t *scene, lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_rect_dsc_t dsc;
    lv_draw_line_dsc_t line;
    int32_t pulse = sin_scaled(scene->frame * 6, 3);
    int32_t cx = ox + LIVING_SKY_SUN_X;
    int32_t cy = oy + LIVING_SKY_SUN_Y;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = lv_color_hex(0xFFE6A0);
    dsc.bg_opa = 40;
    draw_circle(layer, &dsc, cx, cy, 30 + pulse);
    dsc.bg_opa = 70;
    draw_circle(layer, &dsc, cx, cy, 21 + pulse / 2);

    lv_draw_line_dsc_init(&line);
    line.color = lv_color_hex(0xFFE9A8);
    line.opa = 170;
    line.width = 2;
    line.round_start = 1;
    line.round_end = 1;
    for (int i = 0; i < 8; i++) {
        int32_t angle = i * 45 + scene->frame / 2;
        line.p1.x = cx + sin_scaled(angle + 90, 18);
        line.p1.y = cy + sin_scaled(angle, 18);
        line.p2.x = cx + sin_scaled(angle + 90, 25 + pulse);
        line.p2.y = cy + sin_scaled(angle, 25 + pulse);
        lv_draw_line(layer, &line);
    }

    dsc.bg_color = lv_color_hex(0xFFD54A);
    dsc.bg_opa = LV_OPA_COVER;
    draw_circle(layer, &dsc, cx, cy, 12);
}

static void draw_moon(const living_sky_scene_t *scene, lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_rect_dsc_t dsc;
    int32_t cx = ox + LIVING_SKY_SUN_X;
    int32_t cy = oy + LIVING_SKY_SUN_Y;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = lv_color_hex(0xDCE4FF);
    dsc.bg_opa = 25 + sin_scaled(scene->frame * 4, 10);
    draw_circle(layer, &dsc, cx, cy, 24);
    dsc.bg_opa = 45;
    draw_circle(layer, &dsc, cx, cy, 16);

    dsc.bg_color = lv_color_hex(0xF2EFD9);
    dsc.bg_opa = LV_OPA_COVER;
    draw_circle(layer, &dsc, cx, cy, 11);

    // Craters
    dsc.bg_color = lv_color_hex(0xB8B4A0);
    dsc.bg_opa = 90;
    draw_circle(layer, &dsc, cx - 3, cy - 3, 3);
    draw_circle(layer, &dsc, cx + 4, cy + 3, 2);
    draw_circle(layer, &dsc, cx - 2, cy + 5, 1);
}

static void draw_stars(const living_sky_scene_t *scene, lv_layer_t *layer, int32_t ox, int32_t oy, bool playing)
{
    lv_draw_rect_dsc_t dsc;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = lv_color_white();

    for (int i = 0; i < scene->num_particles; i++) {
        const living_sky_particle_t *p = &scene->particles[i];
        int32_t x = ox + (p->x >> FP_SHIFT);
        int32_t y = oy + (p->y >> FP_SHIFT);
        lv_area_t a = { x, y, x + p->size - 1, y + p->size - 1 };

        dsc.bg_opa = (lv_opa_t)(150 + sin_scaled(p->phase + scene->frame * p->vy, 100));
        lv_draw_rect(layer, &dsc, &a);
    }

    // One shooting star early in the burst on clear nights.
    if (scene->weather == ZSW_WEATHER_CLEAR && playing && scene->frame > 20 && scene->frame < 36) {
        lv_draw_line_dsc_t line;
        int32_t t = scene->frame - 20;

        lv_draw_line_dsc_init(&line);
        line.color = lv_color_white();
        line.width = 2;
        line.round_end = 1;
        line.opa = (lv_opa_t)(255 - t * 14);
        line.p2.x = ox + 150 - t * 7;
        line.p2.y = oy + 30 + t * 4;
        line.p1.x = line.p2.x + 24;
        line.p1.y = line.p2.y - 14;
        lv_draw_line(layer, &line);
    }
}

static void draw_clouds(const living_sky_scene_t *scene, lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_rect_dsc_t dsc;
    bool dark = scene->weather == ZSW_WEATHER_RAIN || scene->weather == ZSW_WEATHER_THUNDER;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    if (dark) {
        dsc.bg_color = lv_color_hex(scene->night ? 0x2C3240 : 0x5A6578);
    } else {
        dsc.bg_color = lv_color_hex(scene->night ? 0x7A8398 : 0xEEF3F8);
    }
    dsc.bg_opa = 225;

    for (int i = 0; i < scene->num_blobs; i++) {
        const living_sky_particle_t *b = &scene->blobs[i];
        int32_t x = ox + (b->x >> FP_SHIFT);
        int32_t y = oy + (b->y >> FP_SHIFT);
        int32_t r = b->size;
        lv_area_t base = { x - (r * 3) / 2, y, x + (r * 3) / 2, y + (r * 2) / 3 };

        lv_draw_rect(layer, &dsc, &base);
        draw_circle(layer, &dsc, x - r / 3, y + r / 8, (r * 3) / 4);
        draw_circle(layer, &dsc, x + r / 2, y + r / 4, r / 2);
    }
}

static void draw_fog(const living_sky_scene_t *scene, lv_layer_t *layer, int32_t ox, int32_t oy)
{
    // Three stacked pills per band, wide and thin to narrow and thick, fake soft edges.
    // Horizontal gradients are not used as they need RGB888 blending support.
    static const uint8_t widths[] = { FOG_BAND_WIDTH, FOG_BAND_WIDTH * 3 / 4, FOG_BAND_WIDTH / 2 };
    lv_draw_rect_dsc_t dsc;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = lv_color_hex(0xD8DEE6);

    for (int i = 0; i < scene->num_blobs; i++) {
        const living_sky_particle_t *b = &scene->blobs[i];
        int32_t x = ox + (b->x >> FP_SHIFT);
        int32_t y = oy + (b->y >> FP_SHIFT);

        dsc.bg_opa = (lv_opa_t)(38 + sin_scaled(scene->frame * 3 + i * 90, 8));
        for (int j = 0; j < (int)ARRAY_SIZE(widths); j++) {
            int32_t half_h = (b->size * (j + 2)) / 6;
            lv_area_t a = { x - widths[j] / 2, y - half_h, x + widths[j] / 2, y + half_h };

            lv_draw_rect(layer, &dsc, &a);
        }
    }
}

static void draw_rain(const living_sky_scene_t *scene, lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_line_dsc_t line;

    lv_draw_line_dsc_init(&line);
    line.color = lv_color_hex(0xBFD8FF);
    line.opa = 180;
    line.width = scene->intensity >= 2 ? 2 : 1;

    for (int i = 0; i < scene->num_particles; i++) {
        const living_sky_particle_t *p = &scene->particles[i];

        line.p1.x = ox + (p->x >> FP_SHIFT);
        line.p1.y = oy + (p->y >> FP_SHIFT);
        line.p2.x = line.p1.x - (p->vx * p->size) / p->vy;
        line.p2.y = line.p1.y - p->size;
        lv_draw_line(layer, &line);
    }
}

static void draw_snow(const living_sky_scene_t *scene, lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_rect_dsc_t dsc;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = lv_color_white();
    dsc.bg_opa = 220;

    for (int i = 0; i < scene->num_particles; i++) {
        const living_sky_particle_t *p = &scene->particles[i];
        int32_t x = ox + (p->x >> FP_SHIFT) + sin_scaled(p->phase + scene->frame * 5, 4);
        int32_t y = oy + (p->y >> FP_SHIFT);
        lv_area_t a = { x, y, x + p->size - 1, y + p->size - 1 };

        lv_draw_rect(layer, &dsc, &a);
    }
}

static void draw_lightning(const living_sky_scene_t *scene, lv_layer_t *layer, const lv_area_t *coords)
{
    lv_draw_rect_dsc_t dsc;

    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = lv_color_hex(0xE8ECFF);
    dsc.bg_opa = scene->flash_opa / 2;
    lv_draw_rect(layer, &dsc, coords);

    if (scene->flash_opa > 100) {
        // Short bolt from the clouds, kept above the time.
        static const lv_point_t bolt[] = { {170, 30}, {160, 50}, {168, 52}, {156, 72}, {163, 73}, {152, 88} };
        lv_draw_line_dsc_t line;

        lv_draw_line_dsc_init(&line);
        line.color = lv_color_hex(0xFFF6C8);
        line.width = 3;
        line.opa = scene->flash_opa;
        line.round_start = 1;
        line.round_end = 1;
        for (int i = 0; i < (int)ARRAY_SIZE(bolt) - 1; i++) {
            line.p1.x = coords->x1 + scene->bolt_x + bolt[i].x;
            line.p1.y = coords->y1 + bolt[i].y;
            line.p2.x = coords->x1 + scene->bolt_x + bolt[i + 1].x;
            line.p2.y = coords->y1 + bolt[i + 1].y;
            lv_draw_line(layer, &line);
        }
    }
}

void living_sky_scene_ui_draw(lv_layer_t *layer, const lv_area_t *area,
                              const living_sky_scene_t *scene, bool playing)
{
    const lv_area_t coords = *area;
    draw_sky(scene, layer, &coords);

    if (living_sky_scene_has_sun(scene)) {
        draw_sun(scene, layer, coords.x1, coords.y1);
    }
    if (living_sky_scene_has_stars(scene)) {
        draw_stars(scene, layer, coords.x1, coords.y1, playing);
        draw_moon(scene, layer, coords.x1, coords.y1);
    }

    switch (scene->weather) {
        case ZSW_WEATHER_FOG:
            draw_fog(scene, layer, coords.x1, coords.y1);
            break;
        case ZSW_WEATHER_RAIN:
        case ZSW_WEATHER_THUNDER:
            draw_rain(scene, layer, coords.x1, coords.y1);
            draw_clouds(scene, layer, coords.x1, coords.y1);
            if (scene->flash_opa > 0) {
                draw_lightning(scene, layer, &coords);
            }
            break;
        case ZSW_WEATHER_SNOW:
            draw_clouds(scene, layer, coords.x1, coords.y1);
            draw_snow(scene, layer, coords.x1, coords.y1);
            break;
        case ZSW_WEATHER_CLOUDS:
            draw_clouds(scene, layer, coords.x1, coords.y1);
            break;
        default:
            break;
    }
}
