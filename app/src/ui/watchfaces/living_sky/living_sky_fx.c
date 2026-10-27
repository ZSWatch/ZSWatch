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

#include "living_sky_fx.h"
#include "living_sky_scene_ui.h"

LOG_MODULE_REGISTER(living_sky_fx, LOG_LEVEL_INF);

#define FX_SIZE             LIVING_SKY_SIZE
// Particle positions and speeds are in 1/16 px.
#define FP_SHIFT            LIVING_SKY_FP_SHIFT
#define FP(px)              ((px) << FP_SHIFT)
#define FRAME_PERIOD_MS     33

#define MAX_PARTICLES       LIVING_SKY_MAX_PARTICLES
#define MAX_BLOBS           LIVING_SKY_MAX_BLOBS

#define NIGHT_START_MIN     (19 * 60 + 45)
#define NIGHT_END_MIN       (6 * 60)

#define FOG_BAND_WIDTH      LIVING_SKY_FOG_BAND_WIDTH

static struct {
    lv_obj_t *obj;
    lv_timer_t *timer;
    living_sky_scene_t scene;
    int16_t wind_x;
    uint32_t start_tick;
    uint32_t duration_ms;
    uint16_t next_flash_frame;
    uint32_t rng;
} fx = {
    .scene = {
        .weather = ZSW_WEATHER_CLEAR,
        .intensity = 1,
        .minute_of_day = 12 * 60,
    },
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

static int8_t cloud_speed(void)
{
    // Slow drift even without wind so the clouds are seen moving during the burst,
    // but slow enough that they don't leave the sky before the burst ends.
    int32_t v = fx.wind_x / 3 + (fx.wind_x >= 0 ? 3 : -3);
    return (int8_t)CLAMP(v, -24, 24);
}

static void spawn_rain_drop(living_sky_particle_t *p, bool anywhere)
{
    p->vy = (int8_t)fx_rand_range(88, 127);
    p->vx = (int8_t)CLAMP(fx.wind_x * 3, -40, 40);
    p->size = (uint8_t)fx_rand_range(8, 16);
    // Spawn wider than the screen on the upwind side so slanted rain covers the whole face.
    p->x = (int16_t)FP(fx_rand_range(p->vx > 0 ? -40 : 20, p->vx < 0 ? FX_SIZE + 40 : FX_SIZE - 20));
    p->y = (int16_t)(anywhere ? FP(fx_rand_range(0, FX_SIZE)) : -FP(fx_rand_range(p->size, 40)));
}

static void spawn_snow_flake(living_sky_particle_t *p, bool anywhere)
{
    p->vy = (int8_t)fx_rand_range(8, 22);
    p->vx = (int8_t)CLAMP(fx.wind_x, -24, 24);
    p->size = (uint8_t)fx_rand_range(2, 5);
    p->phase = (uint8_t)fx_rand(255);
    p->x = (int16_t)FP(fx_rand_range(-20, FX_SIZE + 20));
    p->y = (int16_t)(anywhere ? FP(fx_rand_range(0, FX_SIZE)) : -FP(fx_rand_range(4, 30)));
}

static void spawn_star(living_sky_particle_t *p)
{
    int32_t x, y;

    // Keep stars out of the moon glow.
    do {
        x = fx_rand_range(24, FX_SIZE - 24);
        y = fx_rand_range(16, 170);
    } while (abs(x - LIVING_SKY_SUN_X) < 26 && abs(y - LIVING_SKY_SUN_Y) < 26);
    p->x = (int16_t)FP(x);
    p->y = (int16_t)FP(y);
    p->size = (uint8_t)(fx_rand(4) == 0 ? 3 : fx_rand_range(1, 2));
    p->phase = (uint8_t)fx_rand(255);
    p->vy = (int8_t)fx_rand_range(4, 12);    // Twinkle speed
}

static void spawn_cloud(living_sky_particle_t *p, int index, int count)
{
    bool storm = fx.scene.weather == ZSW_WEATHER_RAIN || fx.scene.weather == ZSW_WEATHER_THUNDER ||
                 fx.scene.weather == ZSW_WEATHER_SNOW;

    // Spread the clouds over the sky part above the time, staggered so they overlap less.
    p->size = (uint8_t)(storm ? fx_rand_range(22, 28) : fx_rand_range(16, 24));
    p->x = (int16_t)FP(40 + index * (160 / count) + fx_rand_range(0, 30));
    p->y = (int16_t)FP((storm ? 22 : 30) + (index & 1) * 26 + fx_rand_range(0, 8));
    p->vx = cloud_speed();
    p->vx = (int8_t)(p->vx + (p->vx > 0 ? index : -index));
}

static void spawn_fog_band(living_sky_particle_t *p, int index)
{
    p->size = (uint8_t)fx_rand_range(20, 30);
    p->x = (int16_t)FP(fx_rand_range(-FOG_BAND_WIDTH / 2, FX_SIZE - FOG_BAND_WIDTH / 2));
    p->y = (int16_t)FP(50 + index * 45);
    // Neighbouring bands drift in opposite directions for a slow rolling look.
    p->vx = (int8_t)((index & 1) ? -fx_rand_range(3, 6) : fx_rand_range(3, 6));
}

static void spawn_all(void)
{
    fx.scene.num_particles = 0;
    fx.scene.num_blobs = 0;
    fx.scene.flash_opa = 0;

    switch (fx.scene.weather) {
        case ZSW_WEATHER_THUNDER:
        case ZSW_WEATHER_RAIN:
            fx.scene.num_particles = MIN(MAX_PARTICLES, 10 + 8 * fx.scene.intensity);
            fx.scene.num_blobs = 3;
            for (int i = 0; i < fx.scene.num_particles; i++) {
                spawn_rain_drop(&fx.scene.particles[i], true);
            }
            break;
        case ZSW_WEATHER_SNOW:
            fx.scene.num_particles = MIN(MAX_PARTICLES, 8 + 6 * fx.scene.intensity);
            fx.scene.num_blobs = 2;
            for (int i = 0; i < fx.scene.num_particles; i++) {
                spawn_snow_flake(&fx.scene.particles[i], true);
            }
            break;
        case ZSW_WEATHER_FOG:
            fx.scene.num_blobs = MAX_BLOBS;
            for (int i = 0; i < fx.scene.num_blobs; i++) {
                spawn_fog_band(&fx.scene.blobs[i], i);
            }
            return;
        case ZSW_WEATHER_CLOUDS:
            fx.scene.num_blobs = MIN(MAX_BLOBS, 1 + fx.scene.intensity);
            break;
        case ZSW_WEATHER_CLEAR:
        default:
            break;
    }

    if (living_sky_scene_has_stars(&fx.scene)) {
        fx.scene.num_particles = fx.scene.weather == ZSW_WEATHER_CLEAR ? 22 : 12;
        for (int i = 0; i < fx.scene.num_particles; i++) {
            spawn_star(&fx.scene.particles[i]);
        }
    }

    for (int i = 0; i < fx.scene.num_blobs; i++) {
        spawn_cloud(&fx.scene.blobs[i], i, fx.scene.num_blobs);
    }
}

static void step_particles(void)
{
    for (int i = 0; i < fx.scene.num_particles; i++) {
        living_sky_particle_t *p = &fx.scene.particles[i];

        switch (fx.scene.weather) {
            case ZSW_WEATHER_RAIN:
            case ZSW_WEATHER_THUNDER:
                p->x += p->vx;
                p->y += p->vy;
                if (p->y > FP(FX_SIZE + p->size)) {
                    spawn_rain_drop(p, false);
                }
                break;
            case ZSW_WEATHER_SNOW:
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

    for (int i = 0; i < fx.scene.num_blobs; i++) {
        living_sky_particle_t *b = &fx.scene.blobs[i];
        int32_t half_width = fx.scene.weather == ZSW_WEATHER_FOG ? FOG_BAND_WIDTH / 2 : b->size * 2;

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
    if (fx.scene.weather != ZSW_WEATHER_THUNDER) {
        return;
    }

    if (fx.scene.flash_opa > 0) {
        fx.scene.flash_opa = fx.scene.flash_opa > 40 ? fx.scene.flash_opa - 40 : 0;
    } else if (fx.scene.frame >= fx.next_flash_frame) {
        fx.scene.flash_opa = 200;
        fx.scene.bolt_x = (int8_t)fx_rand_range(-90, 10);
        fx.next_flash_frame = fx.scene.frame + fx_rand_range(45, 90);
    }
}

static void fx_timer_cb(lv_timer_t *timer)
{
    ARG_UNUSED(timer);

    fx.scene.frame++;
    step_particles();
    step_lightning();
    lv_obj_invalidate(fx.obj);

    if (lv_tick_elaps(fx.start_tick) >= fx.duration_ms) {
        living_sky_fx_stop();
    }
}

static void fx_draw_cb(lv_event_t *e)
{
    lv_area_t coords;
    lv_obj_get_coords(fx.obj, &coords);
    living_sky_scene_ui_draw(lv_event_get_layer(e), &coords, &fx.scene, fx.timer != NULL);
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
    fx.scene.frame = 0;
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
    zsw_weather_type_t weather;
    // Wind direction is where the wind comes from, so it blows towards direction + 180.
    int32_t wind_x = -sin_scaled(wind_direction, wind);

    zsw_weather_condition_t condition = zsw_weather_classify(weather_code);
    weather = condition.type == ZSW_WEATHER_UNKNOWN ? ZSW_WEATHER_CLEAR : condition.type;
    if (weather == ZSW_WEATHER_DRIZZLE) {
        weather = ZSW_WEATHER_RAIN;
    }
    uint8_t intensity = condition.intensity;
    wind_x = CLAMP(wind_x, -60, 60);

    if (weather == fx.scene.weather && intensity == fx.scene.intensity && wind_x == fx.wind_x) {
        return false;
    }

    LOG_INF("Weather %d -> effect %d intensity %d wind_x %d", weather_code, weather, intensity, wind_x);
    fx.scene.weather = weather;
    fx.scene.intensity = intensity;
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

    if (minute_of_day == fx.scene.minute_of_day) {
        return;
    }

    night = minute_of_day >= NIGHT_START_MIN || minute_of_day < NIGHT_END_MIN;
    fx.scene.minute_of_day = minute_of_day;
    if (night != fx.scene.night) {
        fx.scene.night = night;
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
    fx.scene.frame = 0;
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
    fx.scene.flash_opa = 0;
    if (fx.obj) {
        // Redraw once without the lightning flash, this frame then stays on screen.
        lv_obj_invalidate(fx.obj);
    }
    LOG_INF("Burst stop after %u frames", fx.scene.frame);
}

bool living_sky_fx_is_playing(void)
{
    return fx.timer != NULL;
}

zsw_weather_type_t living_sky_fx_get_weather(void)
{
    return fx.scene.weather;
}
