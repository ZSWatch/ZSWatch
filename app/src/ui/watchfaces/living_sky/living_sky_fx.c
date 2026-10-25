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

#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <misc/cache/instance/lv_image_cache.h>

#include "living_sky_fx.h"

LOG_MODULE_REGISTER(living_sky_fx, LOG_LEVEL_INF);

#define FX_SIZE             240
// Particle positions and speeds are in 1/16 px.
#define FP_SHIFT            4
#define FP(px)              ((px) << FP_SHIFT)
#define FRAME_PERIOD_MS     33

#define MAX_PARTICLES       36
#define MAX_BLOBS           4

#define NIGHT_START_MIN     (19 * 60 + 45)
#define NIGHT_END_MIN       (6 * 60)

#define SUN_X               62
#define SUN_Y               56
#define FOG_BAND_WIDTH      170

typedef struct {
    int16_t x;
    int16_t y;
    int8_t vx;
    int8_t vy;
    uint8_t size;
    uint8_t phase;
} fx_particle_t;

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

static struct {
    lv_obj_t *obj;
    lv_timer_t *timer;
    living_sky_weather_t weather;
    uint8_t intensity;          // 1 (light) - 3 (heavy)
    int16_t wind_x;             // Horizontal wind in km/h, positive blows to the right
    uint16_t minute_of_day;
    bool night;
    uint8_t num_particles;
    uint8_t num_blobs;
    fx_particle_t particles[MAX_PARTICLES];
    fx_particle_t blobs[MAX_BLOBS];
    uint32_t start_tick;
    uint32_t duration_ms;
    uint16_t frame;
    uint16_t next_flash_frame;
    lv_opa_t flash_opa;
    int8_t bolt_x;
    uint32_t rng;
} fx = {
    .weather = LIVING_SKY_WEATHER_CLEAR,
    .intensity = 1,
    .minute_of_day = 12 * 60,
    .rng = 0x2545F491,
};

static uint32_t fx_rand(uint32_t max)
{
    // xorshift32
    fx.rng ^= fx.rng << 13;
    fx.rng ^= fx.rng >> 17;
    fx.rng ^= fx.rng << 5;
    return max ? fx.rng % max : 0;
}

static int32_t fx_rand_range(int32_t min, int32_t max)
{
    return min + (int32_t)fx_rand(max - min + 1);
}

static int32_t sin_scaled(int32_t angle, int32_t amplitude)
{
    return (lv_trigo_sin((int16_t)(angle % 360)) * amplitude) / LV_TRIGO_SIN_MAX;
}

static bool weather_has_stars(void)
{
    return fx.night && (fx.weather == LIVING_SKY_WEATHER_CLEAR ||
                        (fx.weather == LIVING_SKY_WEATHER_CLOUDS && fx.intensity < 3));
}

static bool weather_has_sun(void)
{
    return !fx.night && (fx.weather == LIVING_SKY_WEATHER_CLEAR ||
                         (fx.weather == LIVING_SKY_WEATHER_CLOUDS && fx.intensity == 1));
}

static int8_t cloud_speed(void)
{
    // Slow drift even without wind so the clouds are seen moving during the burst,
    // but slow enough that they don't leave the sky before the burst ends.
    int32_t v = fx.wind_x / 3 + (fx.wind_x >= 0 ? 3 : -3);
    return (int8_t)CLAMP(v, -24, 24);
}

static void spawn_rain_drop(fx_particle_t *p, bool anywhere)
{
    p->vy = (int8_t)fx_rand_range(88, 127);
    p->vx = (int8_t)CLAMP(fx.wind_x * 3, -40, 40);
    p->size = (uint8_t)fx_rand_range(8, 16);
    // Spawn wider than the screen on the upwind side so slanted rain covers the whole face.
    p->x = (int16_t)FP(fx_rand_range(p->vx > 0 ? -40 : 20, p->vx < 0 ? FX_SIZE + 40 : FX_SIZE - 20));
    p->y = (int16_t)(anywhere ? FP(fx_rand_range(0, FX_SIZE)) : -FP(fx_rand_range(p->size, 40)));
}

static void spawn_snow_flake(fx_particle_t *p, bool anywhere)
{
    p->vy = (int8_t)fx_rand_range(8, 22);
    p->vx = (int8_t)CLAMP(fx.wind_x, -24, 24);
    p->size = (uint8_t)fx_rand_range(2, 5);
    p->phase = (uint8_t)fx_rand(255);
    p->x = (int16_t)FP(fx_rand_range(-20, FX_SIZE + 20));
    p->y = (int16_t)(anywhere ? FP(fx_rand_range(0, FX_SIZE)) : -FP(fx_rand_range(4, 30)));
}

static void spawn_star(fx_particle_t *p)
{
    int32_t x, y;

    // Keep stars out of the moon glow.
    do {
        x = fx_rand_range(24, FX_SIZE - 24);
        y = fx_rand_range(16, 170);
    } while (abs(x - SUN_X) < 26 && abs(y - SUN_Y) < 26);
    p->x = (int16_t)FP(x);
    p->y = (int16_t)FP(y);
    p->size = (uint8_t)(fx_rand(4) == 0 ? 3 : fx_rand_range(1, 2));
    p->phase = (uint8_t)fx_rand(255);
    p->vy = (int8_t)fx_rand_range(4, 12);    // Twinkle speed
}

static void spawn_cloud(fx_particle_t *p, int index, int count)
{
    bool storm = fx.weather == LIVING_SKY_WEATHER_RAIN || fx.weather == LIVING_SKY_WEATHER_THUNDER ||
                 fx.weather == LIVING_SKY_WEATHER_SNOW;

    // Spread the clouds over the sky part above the time, staggered so they overlap less.
    p->size = (uint8_t)(storm ? fx_rand_range(22, 28) : fx_rand_range(16, 24));
    p->x = (int16_t)FP(40 + index * (160 / count) + fx_rand_range(0, 30));
    p->y = (int16_t)FP((storm ? 22 : 30) + (index & 1) * 26 + fx_rand_range(0, 8));
    p->vx = cloud_speed();
    p->vx = (int8_t)(p->vx + (p->vx > 0 ? index : -index));
}

static void spawn_fog_band(fx_particle_t *p, int index)
{
    p->size = (uint8_t)fx_rand_range(20, 30);
    p->x = (int16_t)FP(fx_rand_range(-FOG_BAND_WIDTH / 2, FX_SIZE - FOG_BAND_WIDTH / 2));
    p->y = (int16_t)FP(50 + index * 45);
    // Neighbouring bands drift in opposite directions for a slow rolling look.
    p->vx = (int8_t)((index & 1) ? -fx_rand_range(3, 6) : fx_rand_range(3, 6));
}

static void spawn_all(void)
{
    fx.num_particles = 0;
    fx.num_blobs = 0;
    fx.flash_opa = 0;

    switch (fx.weather) {
        case LIVING_SKY_WEATHER_THUNDER:
        case LIVING_SKY_WEATHER_RAIN:
            fx.num_particles = MIN(MAX_PARTICLES, 10 + 8 * fx.intensity);
            fx.num_blobs = 3;
            for (int i = 0; i < fx.num_particles; i++) {
                spawn_rain_drop(&fx.particles[i], true);
            }
            break;
        case LIVING_SKY_WEATHER_SNOW:
            fx.num_particles = MIN(MAX_PARTICLES, 8 + 6 * fx.intensity);
            fx.num_blobs = 2;
            for (int i = 0; i < fx.num_particles; i++) {
                spawn_snow_flake(&fx.particles[i], true);
            }
            break;
        case LIVING_SKY_WEATHER_FOG:
            fx.num_blobs = MAX_BLOBS;
            for (int i = 0; i < fx.num_blobs; i++) {
                spawn_fog_band(&fx.blobs[i], i);
            }
            return;
        case LIVING_SKY_WEATHER_CLOUDS:
            fx.num_blobs = MIN(MAX_BLOBS, 1 + fx.intensity);
            break;
        case LIVING_SKY_WEATHER_CLEAR:
        default:
            break;
    }

    if (weather_has_stars()) {
        fx.num_particles = fx.weather == LIVING_SKY_WEATHER_CLEAR ? 22 : 12;
        for (int i = 0; i < fx.num_particles; i++) {
            spawn_star(&fx.particles[i]);
        }
    }

    for (int i = 0; i < fx.num_blobs; i++) {
        spawn_cloud(&fx.blobs[i], i, fx.num_blobs);
    }
}

static void classify_weather(int code, living_sky_weather_t *weather, uint8_t *intensity)
{
    *intensity = 2;

    switch (code / 100) {
        case 2:
            *weather = LIVING_SKY_WEATHER_THUNDER;
            *intensity = 3;
            break;
        case 3:
            *weather = LIVING_SKY_WEATHER_RAIN;
            *intensity = 1;
            break;
        case 5:
            if (code == 511) {
                *weather = LIVING_SKY_WEATHER_SNOW;
            } else {
                *weather = LIVING_SKY_WEATHER_RAIN;
                if (code == 500 || code == 520) {
                    *intensity = 1;
                } else if (code != 501 && code != 521) {
                    *intensity = 3;
                }
            }
            break;
        case 6:
            *weather = LIVING_SKY_WEATHER_SNOW;
            if (code == 600 || code == 620) {
                *intensity = 1;
            } else if (code == 602 || code == 622) {
                *intensity = 3;
            }
            break;
        case 7:
            *weather = LIVING_SKY_WEATHER_FOG;
            break;
        case 8:
            if (code == 800) {
                *weather = LIVING_SKY_WEATHER_CLEAR;
            } else {
                *weather = LIVING_SKY_WEATHER_CLOUDS;
                *intensity = code == 801 ? 1 : (code == 802 ? 2 : 3);
            }
            break;
        default:
            *weather = LIVING_SKY_WEATHER_CLEAR;
            break;
    }
}

static void step_particles(void)
{
    for (int i = 0; i < fx.num_particles; i++) {
        fx_particle_t *p = &fx.particles[i];

        switch (fx.weather) {
            case LIVING_SKY_WEATHER_RAIN:
            case LIVING_SKY_WEATHER_THUNDER:
                p->x += p->vx;
                p->y += p->vy;
                if (p->y > FP(FX_SIZE + p->size)) {
                    spawn_rain_drop(p, false);
                }
                break;
            case LIVING_SKY_WEATHER_SNOW:
                p->x += p->vx;
                p->y += p->vy;
                if (p->y > FP(FX_SIZE + 4) || p->x < -FP(30) || p->x > FP(FX_SIZE + 30)) {
                    spawn_snow_flake(p, false);
                }
                break;
            default:
                // Stars don't move, they twinkle based on the frame counter.
                break;
        }
    }

    for (int i = 0; i < fx.num_blobs; i++) {
        fx_particle_t *b = &fx.blobs[i];
        int32_t half_width = fx.weather == LIVING_SKY_WEATHER_FOG ? FOG_BAND_WIDTH / 2 : b->size * 2;

        b->x += b->vx;
        if (b->vx > 0 && b->x > FP(FX_SIZE + half_width)) {
            b->x = (int16_t) - FP(half_width);
        } else if (b->vx < 0 && b->x < -FP(half_width)) {
            b->x = (int16_t)FP(FX_SIZE + half_width);
        }
    }
}

static void step_lightning(void)
{
    if (fx.weather != LIVING_SKY_WEATHER_THUNDER) {
        return;
    }

    if (fx.flash_opa > 0) {
        fx.flash_opa = fx.flash_opa > 40 ? fx.flash_opa - 40 : 0;
    } else if (fx.frame >= fx.next_flash_frame) {
        fx.flash_opa = 200;
        fx.bolt_x = (int8_t)fx_rand_range(-90, 10);
        fx.next_flash_frame = fx.frame + fx_rand_range(45, 90);
    }
}

static void fx_timer_cb(lv_timer_t *timer)
{
    ARG_UNUSED(timer);

    fx.frame++;
    step_particles();
    step_lightning();
    lv_obj_invalidate(fx.obj);

    if (lv_tick_elaps(fx.start_tick) >= fx.duration_ms) {
        living_sky_fx_stop();
    }
}

static lv_color_t sky_tint(void)
{
    int i;

    for (i = 0; i < (int)ARRAY_SIZE(sky_keys) - 2; i++) {
        if (fx.minute_of_day < sky_keys[i + 1].minute) {
            break;
        }
    }

    const sky_key_t *a = &sky_keys[i];
    const sky_key_t *b = &sky_keys[i + 1];
    // mix = 255 gives a, 0 gives b
    uint8_t mix = 255 - (uint8_t)(((fx.minute_of_day - a->minute) * 255) / (b->minute - a->minute));
    lv_color_t tint = lv_color_mix(lv_color_hex(a->tint), lv_color_hex(b->tint), mix);

    // Darken and grey the sky under heavy weather, fog only a little.
    if (fx.weather != LIVING_SKY_WEATHER_CLEAR && !(fx.weather == LIVING_SKY_WEATHER_CLOUDS && fx.intensity == 1)) {
        bool fog = fx.weather == LIVING_SKY_WEATHER_FOG;

        tint = lv_color_mix(lv_color_hex(fog ? 0xDCE0E4 : 0xC0C6D0), tint, 160);
    }

    return tint;
}

static void draw_circle(lv_layer_t *layer, lv_draw_rect_dsc_t *dsc, int32_t x, int32_t y, int32_t r)
{
    lv_area_t a = { x - r, y - r, x + r - 1, y + r - 1 };
    lv_draw_rect(layer, dsc, &a);
}

static void draw_sky(lv_layer_t *layer, const lv_area_t *coords)
{
    lv_color_t tint = sky_tint();
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

static void draw_sun(lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_rect_dsc_t dsc;
    lv_draw_line_dsc_t line;
    int32_t pulse = sin_scaled(fx.frame * 6, 3);
    int32_t cx = ox + SUN_X;
    int32_t cy = oy + SUN_Y;

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
        int32_t angle = i * 45 + fx.frame / 2;
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

static void draw_moon(lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_rect_dsc_t dsc;
    int32_t cx = ox + SUN_X;
    int32_t cy = oy + SUN_Y;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = lv_color_hex(0xDCE4FF);
    dsc.bg_opa = 25 + sin_scaled(fx.frame * 4, 10);
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

static void draw_stars(lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_rect_dsc_t dsc;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = lv_color_white();

    for (int i = 0; i < fx.num_particles; i++) {
        const fx_particle_t *p = &fx.particles[i];
        int32_t x = ox + (p->x >> FP_SHIFT);
        int32_t y = oy + (p->y >> FP_SHIFT);
        lv_area_t a = { x, y, x + p->size - 1, y + p->size - 1 };

        dsc.bg_opa = (lv_opa_t)(150 + sin_scaled(p->phase + fx.frame * p->vy, 100));
        lv_draw_rect(layer, &dsc, &a);
    }

    // One shooting star early in the burst on clear nights.
    if (fx.weather == LIVING_SKY_WEATHER_CLEAR && fx.timer && fx.frame > 20 && fx.frame < 36) {
        lv_draw_line_dsc_t line;
        int32_t t = fx.frame - 20;

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

static void draw_clouds(lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_rect_dsc_t dsc;
    bool dark = fx.weather == LIVING_SKY_WEATHER_RAIN || fx.weather == LIVING_SKY_WEATHER_THUNDER;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    if (dark) {
        dsc.bg_color = lv_color_hex(fx.night ? 0x2C3240 : 0x5A6578);
    } else {
        dsc.bg_color = lv_color_hex(fx.night ? 0x7A8398 : 0xEEF3F8);
    }
    dsc.bg_opa = 225;

    for (int i = 0; i < fx.num_blobs; i++) {
        const fx_particle_t *b = &fx.blobs[i];
        int32_t x = ox + (b->x >> FP_SHIFT);
        int32_t y = oy + (b->y >> FP_SHIFT);
        int32_t r = b->size;
        lv_area_t base = { x - (r * 3) / 2, y, x + (r * 3) / 2, y + (r * 2) / 3 };

        lv_draw_rect(layer, &dsc, &base);
        draw_circle(layer, &dsc, x - r / 3, y + r / 8, (r * 3) / 4);
        draw_circle(layer, &dsc, x + r / 2, y + r / 4, r / 2);
    }
}

static void draw_fog(lv_layer_t *layer, int32_t ox, int32_t oy)
{
    // Three stacked pills per band, wide and thin to narrow and thick, fake soft edges.
    // Horizontal gradients are not used as they need RGB888 blending support.
    static const uint8_t widths[] = { FOG_BAND_WIDTH, FOG_BAND_WIDTH * 3 / 4, FOG_BAND_WIDTH / 2 };
    lv_draw_rect_dsc_t dsc;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = lv_color_hex(0xD8DEE6);

    for (int i = 0; i < fx.num_blobs; i++) {
        const fx_particle_t *b = &fx.blobs[i];
        int32_t x = ox + (b->x >> FP_SHIFT);
        int32_t y = oy + (b->y >> FP_SHIFT);

        dsc.bg_opa = (lv_opa_t)(38 + sin_scaled(fx.frame * 3 + i * 90, 8));
        for (int j = 0; j < (int)ARRAY_SIZE(widths); j++) {
            int32_t half_h = (b->size * (j + 2)) / 6;
            lv_area_t a = { x - widths[j] / 2, y - half_h, x + widths[j] / 2, y + half_h };

            lv_draw_rect(layer, &dsc, &a);
        }
    }
}

static void draw_rain(lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_line_dsc_t line;

    lv_draw_line_dsc_init(&line);
    line.color = lv_color_hex(0xBFD8FF);
    line.opa = 180;
    line.width = fx.intensity >= 2 ? 2 : 1;

    for (int i = 0; i < fx.num_particles; i++) {
        const fx_particle_t *p = &fx.particles[i];

        line.p1.x = ox + (p->x >> FP_SHIFT);
        line.p1.y = oy + (p->y >> FP_SHIFT);
        line.p2.x = line.p1.x - (p->vx * p->size) / p->vy;
        line.p2.y = line.p1.y - p->size;
        lv_draw_line(layer, &line);
    }
}

static void draw_snow(lv_layer_t *layer, int32_t ox, int32_t oy)
{
    lv_draw_rect_dsc_t dsc;

    lv_draw_rect_dsc_init(&dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = lv_color_white();
    dsc.bg_opa = 220;

    for (int i = 0; i < fx.num_particles; i++) {
        const fx_particle_t *p = &fx.particles[i];
        int32_t x = ox + (p->x >> FP_SHIFT) + sin_scaled(p->phase + fx.frame * 5, 4);
        int32_t y = oy + (p->y >> FP_SHIFT);
        lv_area_t a = { x, y, x + p->size - 1, y + p->size - 1 };

        lv_draw_rect(layer, &dsc, &a);
    }
}

static void draw_lightning(lv_layer_t *layer, const lv_area_t *coords)
{
    lv_draw_rect_dsc_t dsc;

    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = lv_color_hex(0xE8ECFF);
    dsc.bg_opa = fx.flash_opa / 2;
    lv_draw_rect(layer, &dsc, coords);

    if (fx.flash_opa > 100) {
        // Short bolt from the clouds, kept above the time.
        static const lv_point_t bolt[] = { {170, 30}, {160, 50}, {168, 52}, {156, 72}, {163, 73}, {152, 88} };
        lv_draw_line_dsc_t line;

        lv_draw_line_dsc_init(&line);
        line.color = lv_color_hex(0xFFF6C8);
        line.width = 3;
        line.opa = fx.flash_opa;
        line.round_start = 1;
        line.round_end = 1;
        for (int i = 0; i < (int)ARRAY_SIZE(bolt) - 1; i++) {
            line.p1.x = coords->x1 + fx.bolt_x + bolt[i].x;
            line.p1.y = coords->y1 + bolt[i].y;
            line.p2.x = coords->x1 + fx.bolt_x + bolt[i + 1].x;
            line.p2.y = coords->y1 + bolt[i + 1].y;
            lv_draw_line(layer, &line);
        }
    }
}

static void fx_draw_cb(lv_event_t *e)
{
    lv_layer_t *layer = lv_event_get_layer(e);
    lv_area_t coords;

    lv_obj_get_coords(fx.obj, &coords);
    draw_sky(layer, &coords);

    if (weather_has_sun()) {
        draw_sun(layer, coords.x1, coords.y1);
    }
    if (weather_has_stars()) {
        draw_stars(layer, coords.x1, coords.y1);
        draw_moon(layer, coords.x1, coords.y1);
    }

    switch (fx.weather) {
        case LIVING_SKY_WEATHER_FOG:
            draw_fog(layer, coords.x1, coords.y1);
            break;
        case LIVING_SKY_WEATHER_RAIN:
        case LIVING_SKY_WEATHER_THUNDER:
            draw_rain(layer, coords.x1, coords.y1);
            draw_clouds(layer, coords.x1, coords.y1);
            if (fx.flash_opa > 0) {
                draw_lightning(layer, &coords);
            }
            break;
        case LIVING_SKY_WEATHER_SNOW:
            draw_clouds(layer, coords.x1, coords.y1);
            draw_snow(layer, coords.x1, coords.y1);
            break;
        case LIVING_SKY_WEATHER_CLOUDS:
            draw_clouds(layer, coords.x1, coords.y1);
            break;
        default:
            break;
    }
}

lv_obj_t *living_sky_fx_create(lv_obj_t *parent)
{
    __ASSERT(fx.obj == NULL, "Only one living sky layer supported");

    fx.obj = lv_obj_create(parent);
    lv_obj_remove_style_all(fx.obj);
    lv_obj_set_size(fx.obj, FX_SIZE, FX_SIZE);
    lv_obj_center(fx.obj);
    lv_obj_remove_flag(fx.obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(fx.obj, fx_draw_cb, LV_EVENT_DRAW_MAIN, NULL);

    // xorshift must never be seeded with 0.
    fx.rng = (fx.rng ^ k_cycle_get_32()) | 1;
    fx.frame = 0;
    spawn_all();

    return fx.obj;
}

void living_sky_fx_delete(void)
{
    living_sky_fx_stop();
    fx.obj = NULL;
}

bool living_sky_fx_set_weather(int weather_code, uint16_t wind, uint16_t wind_direction)
{
    living_sky_weather_t weather;
    uint8_t intensity;
    // Wind direction is where the wind comes from, so it blows towards direction + 180.
    int32_t wind_x = -sin_scaled(wind_direction, wind);

    classify_weather(weather_code, &weather, &intensity);
    wind_x = CLAMP(wind_x, -60, 60);

    if (weather == fx.weather && intensity == fx.intensity && wind_x == fx.wind_x) {
        return false;
    }

    LOG_INF("Weather %d -> effect %d intensity %d wind_x %d", weather_code, weather, intensity, wind_x);
    fx.weather = weather;
    fx.intensity = intensity;
    fx.wind_x = (int16_t)wind_x;
    spawn_all();
    if (fx.obj) {
        lv_obj_invalidate(fx.obj);
    }

    return true;
}

void living_sky_fx_set_time(int hour, int minute)
{
    uint16_t minute_of_day = (uint16_t)((hour % 24) * 60 + minute % 60);
    bool night;

    if (minute_of_day == fx.minute_of_day) {
        return;
    }

    night = minute_of_day >= NIGHT_START_MIN || minute_of_day < NIGHT_END_MIN;
    fx.minute_of_day = minute_of_day;
    if (night != fx.night) {
        fx.night = night;
        spawn_all();
    }
    if (fx.obj) {
        lv_obj_invalidate(fx.obj);
    }
}

void living_sky_fx_play(uint32_t duration_ms)
{
    if (!fx.obj) {
        return;
    }

    fx.start_tick = lv_tick_get();
    fx.duration_ms = duration_ms;
    fx.frame = 0;
    fx.next_flash_frame = 12;

    if (!fx.timer) {
        fx.timer = lv_timer_create(fx_timer_cb, FRAME_PERIOD_MS, NULL);
        LOG_INF("Burst start (%u ms)", duration_ms);
    }
}

void living_sky_fx_stop(void)
{
    if (!fx.timer) {
        return;
    }

    lv_timer_delete(fx.timer);
    fx.timer = NULL;
    fx.flash_opa = 0;
    if (fx.obj) {
        // Redraw once without the lightning flash, this frame then stays on screen.
        lv_obj_invalidate(fx.obj);
    }
    LOG_INF("Burst stop after %u frames", fx.frame);
}

bool living_sky_fx_is_playing(void)
{
    return fx.timer != NULL;
}

living_sky_weather_t living_sky_fx_get_weather(void)
{
    return fx.weather;
}
