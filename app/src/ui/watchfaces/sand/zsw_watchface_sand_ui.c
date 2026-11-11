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

#include <lvgl.h>
#include <misc/lv_area_private.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ui/utils/zsw_ui_utils.h"
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"
#include "applications/watchface/watchface_app.h"
#include "sensors/zsw_imu.h"
#include "sand_sim.h"
#include "zsw_watchface_sand.h"

LOG_MODULE_REGISTER(watchface_sand, LOG_LEVEL_INF);

#define FACE_SIZE               (SAND_GRID_SIZE * SAND_CELL_PX)

/* While the sand moves a frame is two steps of the world. At rest only gravity is checked. */
#define FRAME_PERIOD_MS         40
#define REST_PERIOD_MS          250
#define STEPS_PER_FRAME         2
/* Frames without movement before going to rest */
#define REST_AFTER_FRAMES       4
/* The accelerometer is read every this many frames while the sand moves */
#define GRAVITY_EVERY_FRAMES    3

/*
 * How the accelerometer axes map to the display: which axis points right and which down
 * when looking at the watch, as +1 or -1. TODO: only a guess until tried on the hardware.
 */
#define ACCEL_TO_RIGHT(x, y)    (x)
#define ACCEL_TO_DOWN(x, y)     (-(y))
#define ACCEL_1G                9.81f

#define COLOR_DATE              0x9FAEE0
/* The grains where the sand meets the sky catch the light */
#define COLOR_SAND_TOP          0xFFFFFF

ZSW_LV_IMG_DECLARE(face_sand_preview);
ZSW_LV_IMG_DECLARE(face_sand_sky);
ZSW_LV_IMG_DECLARE(face_sand_penguin_walk_a_right);
ZSW_LV_IMG_DECLARE(face_sand_penguin_walk_a_left);
ZSW_LV_IMG_DECLARE(face_sand_penguin_walk_b_right);
ZSW_LV_IMG_DECLARE(face_sand_penguin_walk_b_left);
ZSW_LV_IMG_DECLARE(face_sand_penguin_slide_right);
ZSW_LV_IMG_DECLARE(face_sand_penguin_slide_left);
ZSW_LV_IMG_DECLARE(face_sand_penguin_fallen_right);
ZSW_LV_IMG_DECLARE(face_sand_penguin_fallen_left);

static void reset_cached_values(void);

/*
 * The grains are snow. One of four shades each: the base, darker, deep shadow and lighter.
 * The digits are the same snow, a little brighter so that the time stands out.
 */
static const uint32_t sand_colors[SAND_NUM_SHADES] = { 0xD3DFF5, 0xB4C5E6, 0x8DA2CE, 0xF2F6FF };
static const uint32_t digit_colors[SAND_NUM_SHADES] = { 0xFFFFFF, 0xE6EEFB, 0xCBD8F0, 0xFFFFFF };

/* Snow falls slowly all the time. Few flakes: each one is a small redraw of its own. */
#define NUM_FLAKES              7
#define FLAKE_PERIOD_MS         110
#define FLAKE_SIZE              2
#define COLOR_FLAKE             0xAFC0E6

typedef struct {
    int16_t x;
    int16_t y;
} flake_t;

/*
 * A penguin crosses the snow now and then, and when the face is tapped. He waddles on the
 * flat and uphill, slides on his belly downhill, and now and then trips and ends up head
 * first in the snow. Positions are in 1/16 pixel, the point where he touches the snow.
 */
#define PENGUIN_FP              16
#define PENGUIN_WIDTH           32
#define PENGUIN_HEIGHT          28
#define PENGUIN_WALK_SPEED      (PENGUIN_FP + 2)
#define PENGUIN_SLIDE_SPEED     (2 * PENGUIN_FP + 8)
#define PENGUIN_SPEED_MAX       (4 * PENGUIN_FP)
#define PENGUIN_GRAVITY         14
#define PENGUIN_JUMP            (5 * PENGUIN_FP)
/* The ground has to fall away this fast for him to leave it */
#define PENGUIN_DROP_FP         (6 * PENGUIN_FP)
/* He looks this far ahead to tell a slope: it has to drop this much to slide, and less than half to get up */
#define PENGUIN_LOOK_AHEAD_PX   10
#define PENGUIN_SLOPE_PX        5
#define PENGUIN_WADDLE_FRAMES   5
#define PENGUIN_SPRAY_EVERY     3
/* He trips about once in this many frames, which is about once per crossing, and when a jump ends on a slope */
#define PENGUIN_TRIP_ONE_IN     200
#define PENGUIN_FALLEN_FRAMES   32
/* How deep his head goes into the snow */
#define PENGUIN_FALLEN_SINK_PX  6
/* Rest ticks (REST_PERIOD_MS each): he comes about every this many, and soon after the face wakes */
#define PENGUIN_RANDOM_TICKS    70
#define PENGUIN_WAKE_TICKS      5

typedef enum {
    PENGUIN_WALKING,
    PENGUIN_SLIDING,
    PENGUIN_FALLEN,
} penguin_state_t;

typedef struct {
    lv_obj_t *img;
    int32_t x;
    int32_t y;
    int32_t dy;
    int8_t dir;
    bool active;
    bool airborne;
    /* In the air because of a tap, not because the ground dropped away */
    bool jumped;
    penguin_state_t state;
    /* Frames in the current state */
    uint16_t frames;
    const void *shown;
} penguin_t;

static lv_obj_t *root_page;
static lv_obj_t *sand_obj;
static lv_obj_t *ui_date;
static lv_timer_t *frame_timer;
static lv_timer_t *flake_timer;
static flake_t flakes[NUM_FLAKES];
static penguin_t penguin;
static uint8_t penguin_wait_ticks;
static bool face_active;
static bool paused;
static uint8_t rest_frames;
static uint8_t gravity_frames;
static bool gravity_overridden;

static int last_minute = -1;
static int last_hour = -1;
static int last_day = -1;

static void fill_cells(lv_layer_t *layer, lv_draw_rect_dsc_t *dsc, const lv_area_t *origin, int x_first, int x_last,
                       int y, uint32_t color)
{
    lv_area_t area = {
        .x1 = origin->x1 + x_first * SAND_CELL_PX,
        .y1 = origin->y1 + y * SAND_CELL_PX,
        .x2 = origin->x1 + (x_last + 1) * SAND_CELL_PX - 1,
        .y2 = origin->y1 + (y + 1) * SAND_CELL_PX - 1,
    };

    dsc->bg_color = lv_color_hex(color);
    lv_draw_rect(layer, dsc, &area);
}

static void flake_area(const flake_t *flake, const lv_area_t *origin, lv_area_t *area)
{
    area->x1 = origin->x1 + flake->x;
    area->y1 = origin->y1 + flake->y;
    area->x2 = area->x1 + FLAKE_SIZE - 1;
    area->y2 = area->y1 + FLAKE_SIZE - 1;
}

/*
 * Each row is drawn as runs of grains in the base shade, with the grains of other shades on
 * top one by one, and a lighter one where the sand meets the sky. That is far fewer
 * rectangles than one per grain.
 */
static void sand_draw_cb(lv_event_t *e)
{
    lv_layer_t *layer = lv_event_get_layer(e);
    lv_draw_rect_dsc_t dsc;
    lv_area_t coords;
    lv_area_t clip;
    sand_flying_t flying[SAND_MAX_FLYING];

    lv_obj_get_coords(sand_obj, &coords);
    if (!lv_area_intersect(&clip, &layer->_clip_area, &coords)) {
        return;
    }

    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_opa = LV_OPA_COVER;

    /* The falling snow first, so that it is behind the pile and the digits */
    dsc.bg_color = lv_color_hex(COLOR_FLAKE);
    for (int i = 0; i < NUM_FLAKES; i++) {
        lv_area_t area;

        flake_area(&flakes[i], &coords, &area);
        if (lv_area_is_on(&area, &clip)) {
            lv_draw_rect(layer, &dsc, &area);
        }
    }

    int x0 = (clip.x1 - coords.x1) / SAND_CELL_PX;
    int x1 = (clip.x2 - coords.x1) / SAND_CELL_PX;
    int y0 = (clip.y1 - coords.y1) / SAND_CELL_PX;
    int y1 = (clip.y2 - coords.y1) / SAND_CELL_PX;

    for (int y = y0; y <= y1; y++) {
        int run_start = 0;
        sand_cell_type_t run_type = SAND_CELL_EMPTY;

        for (int x = x0; x <= x1 + 1; x++) {
            sand_cell_type_t type = x <= x1 ? SAND_CELL_TYPE(sand_sim_cell(x, y)) : SAND_CELL_EMPTY;

            if (type == run_type) {
                continue;
            }
            if (run_type != SAND_CELL_EMPTY) {
                fill_cells(layer, &dsc, &coords, run_start, x - 1, y,
                           run_type == SAND_CELL_SAND ? sand_colors[0] : digit_colors[0]);
            }
            run_start = x;
            run_type = type;
        }

        for (int x = x0; x <= x1; x++) {
            uint8_t cell = sand_sim_cell(x, y);
            sand_cell_type_t type = SAND_CELL_TYPE(cell);
            uint8_t shade = SAND_CELL_SHADE(cell);

            if (type == SAND_CELL_SAND && (y == 0 || SAND_CELL_TYPE(sand_sim_cell(x, y - 1)) == SAND_CELL_EMPTY)) {
                fill_cells(layer, &dsc, &coords, x, x, y, COLOR_SAND_TOP);
            } else if (shade != 0 && type != SAND_CELL_EMPTY) {
                fill_cells(layer, &dsc, &coords, x, x, y, type == SAND_CELL_SAND ? sand_colors[shade] : digit_colors[shade]);
            }
        }
    }

    /* Grains in the air heat up to the color of the digits on their way */
    int num_flying = sand_sim_get_flying(flying, ARRAY_SIZE(flying));

    for (int i = 0; i < num_flying; i++) {
        lv_area_t area = {
            .x1 = coords.x1 + flying[i].x - SAND_CELL_PX / 2,
            .y1 = coords.y1 + flying[i].y - SAND_CELL_PX / 2,
        };

        area.x2 = area.x1 + SAND_CELL_PX - 1;
        area.y2 = area.y1 + SAND_CELL_PX - 1;
        dsc.bg_color = lv_color_hex(digit_colors[flying[i].shade]);
        lv_draw_rect(layer, &dsc, &area);
    }
}

static void invalidate_dirty(void)
{
    lv_area_t coords;

    lv_obj_get_coords(sand_obj, &coords);
    for (int band = 0; band < SAND_NUM_BANDS; band++) {
        int x_first;
        int x_last;

        if (!sand_sim_pop_dirty(band, &x_first, &x_last)) {
            continue;
        }

        /* One row more below: a grain is lighter when the cell above it is empty */
        lv_area_t area = {
            .x1 = coords.x1 + x_first * SAND_CELL_PX,
            .y1 = coords.y1 + band * SAND_BAND_ROWS * SAND_CELL_PX,
            .x2 = coords.x1 + (x_last + 1) * SAND_CELL_PX - 1,
                        .y2 = coords.y1 + ((band + 1) * SAND_BAND_ROWS + 1) * SAND_CELL_PX - 1,
        };

        lv_obj_invalidate_area(sand_obj, &area);
    }
}

static void wake_sand(void)
{
    rest_frames = 0;
    if (frame_timer) {
        lv_timer_set_period(frame_timer, FRAME_PERIOD_MS);
    }
}

/* @return true if the sand now falls another way. */
static bool read_gravity(void)
{
    float x;
    float y;
    float z;

    if (gravity_overridden || zsw_imu_fetch_accel_f(&x, &y, &z) != 0) {
        return false;
    }

    return sand_sim_set_gravity((int)(ACCEL_TO_RIGHT(x, y) * 127.0f / ACCEL_1G),
                                (int)(ACCEL_TO_DOWN(x, y) * 127.0f / ACCEL_1G));
}

static void flake_respawn(flake_t *flake, bool anywhere)
{
    flake->x = lv_rand(30, FACE_SIZE - 30);
    flake->y = anywhere ? lv_rand(0, FACE_SIZE / 2) : 0;
}

/* Each flake sinks a pixel or two and sways, until it meets the snow and a new one starts at the top. */
static void flake_timer_cb(lv_timer_t *timer)
{
    lv_area_t coords;
    lv_area_t area;

    ARG_UNUSED(timer);

    lv_obj_get_coords(sand_obj, &coords);
    for (int i = 0; i < NUM_FLAKES; i++) {
        flake_t *flake = &flakes[i];

        flake_area(flake, &coords, &area);
        lv_obj_invalidate_area(sand_obj, &area);

        flake->y += 1 + (i & 1);
        flake->x += (int)lv_rand(0, 2) - 1;
        if (flake->y >= FACE_SIZE - FLAKE_SIZE || flake->x < 0 || flake->x >= FACE_SIZE - FLAKE_SIZE ||
            SAND_CELL_TYPE(sand_sim_cell(flake->x / SAND_CELL_PX, flake->y / SAND_CELL_PX)) == SAND_CELL_SAND) {
            flake_respawn(flake, false);
        }

        flake_area(flake, &coords, &area);
        lv_obj_invalidate_area(sand_obj, &area);
    }
}

/* The height in pixels of the snow under a point, or -1 where there is none. */
static int ground_at(int x_px)
{
    int surface = sand_sim_surface(x_px / SAND_CELL_PX);

    return surface < SAND_GRID_SIZE ? surface * SAND_CELL_PX : -1;
}

static void penguin_stop(void)
{
    penguin.active = false;
    lv_obj_add_flag(penguin.img, LV_OBJ_FLAG_HIDDEN);
}

static void penguin_set_state(penguin_state_t state)
{
    penguin.state = state;
    penguin.frames = 0;
}

/* Moves the image to where he is, in the pose that fits what he is doing. */
static void penguin_show(void)
{
    bool right = penguin.dir > 0;
    const void *src;
    int sink = 0;

    if (penguin.state == PENGUIN_FALLEN) {
        src = right ? ZSW_LV_IMG_USE(face_sand_penguin_fallen_right) : ZSW_LV_IMG_USE(face_sand_penguin_fallen_left);
        sink = PENGUIN_FALLEN_SINK_PX;
    } else if (penguin.state == PENGUIN_SLIDING || penguin.airborne) {
        src = right ? ZSW_LV_IMG_USE(face_sand_penguin_slide_right) : ZSW_LV_IMG_USE(face_sand_penguin_slide_left);
    } else if ((penguin.frames / PENGUIN_WADDLE_FRAMES) & 1) {
        src = right ? ZSW_LV_IMG_USE(face_sand_penguin_walk_b_right) : ZSW_LV_IMG_USE(face_sand_penguin_walk_b_left);
    } else {
        src = right ? ZSW_LV_IMG_USE(face_sand_penguin_walk_a_right) : ZSW_LV_IMG_USE(face_sand_penguin_walk_a_left);
    }

    if (src != penguin.shown) {
        penguin.shown = src;
        lv_image_set_src(penguin.img, src);
    }
    lv_obj_set_pos(penguin.img, penguin.x / PENGUIN_FP - PENGUIN_WIDTH / 2,
                   penguin.y / PENGUIN_FP - PENGUIN_HEIGHT + sink);
}

/* He starts at the end of the snow that is higher up, to get a slide downhill. */
static void penguin_start(void)
{
    int left = -1;
    int right = -1;

    if (penguin.active || !sand_sim_is_level()) {
        return;
    }

    for (int x = PENGUIN_WIDTH / 2; x < FACE_SIZE - PENGUIN_WIDTH / 2 && left < 0; x += SAND_CELL_PX) {
        if (ground_at(x) >= 0) {
            left = x;
        }
    }
    for (int x = FACE_SIZE - PENGUIN_WIDTH / 2; x > left && right < 0; x -= SAND_CELL_PX) {
        if (ground_at(x) >= 0) {
            right = x;
        }
    }
    if (left < 0 || right - left < 3 * PENGUIN_WIDTH) {
        /* Not enough snow to walk on */
        return;
    }

    penguin.dir = ground_at(left) <= ground_at(right) ? 1 : -1;
    penguin.x = (penguin.dir > 0 ? left : right) * PENGUIN_FP;
    penguin.y = ground_at(penguin.x / PENGUIN_FP) * PENGUIN_FP;
    penguin.dy = 0;
    penguin.airborne = false;
    penguin.jumped = false;
    penguin.active = true;
    penguin_set_state(PENGUIN_WALKING);
    penguin_show();
    lv_obj_remove_flag(penguin.img, LV_OBJ_FLAG_HIDDEN);
    wake_sand();
}

/* A tap while he is out makes him jump. */
static void penguin_jump(void)
{
    if (!penguin.airborne && penguin.state != PENGUIN_FALLEN) {
        penguin.airborne = true;
        penguin.jumped = true;
        penguin.dy = -PENGUIN_JUMP;
    }
}

/* Head first into the snow, with a puff of it thrown up around him. */
static void penguin_trip(void)
{
    int column = penguin.x / PENGUIN_FP / SAND_CELL_PX;

    penguin_set_state(PENGUIN_FALLEN);
    penguin.dy = 0;
    for (int i = -5; i <= 5; i += 2) {
        sand_sim_kick(column + i);
    }
}

/* @return How much lower the snow is a bit ahead of him, in pixels. Negative is uphill. */
static int penguin_slope(void)
{
    int here = ground_at(penguin.x / PENGUIN_FP);
    int ahead = ground_at(penguin.x / PENGUIN_FP + penguin.dir * PENGUIN_LOOK_AHEAD_PX);

    return (here < 0 || ahead < 0) ? 0 : ahead - here;
}

/* One frame of the penguin. */
static void penguin_step(void)
{
    int ground;
    int speed;
    int slope;

    if (!penguin.active) {
        return;
    }

    penguin.frames++;

    if (!sand_sim_is_level()) {
        /* The watch was turned and the snow is sliding away under him */
        penguin_stop();
        return;
    }

    if (penguin.state == PENGUIN_FALLEN) {
        /* He stays where he fell for a while, then gets up and goes on */
        ground = ground_at(penguin.x / PENGUIN_FP);
        if (ground < 0) {
            penguin_stop();
            return;
        }
        penguin.y = ground * PENGUIN_FP;
        if (penguin.frames >= PENGUIN_FALLEN_FRAMES) {
            penguin_set_state(PENGUIN_WALKING);
        }
        penguin_show();
        return;
    }

    slope = penguin_slope();
    if (!penguin.airborne) {
        if (penguin.state == PENGUIN_WALKING && slope >= PENGUIN_SLOPE_PX) {
            penguin_set_state(PENGUIN_SLIDING);
        } else if (penguin.state == PENGUIN_SLIDING && slope < PENGUIN_SLOPE_PX / 2) {
            penguin_set_state(PENGUIN_WALKING);
        }
    }

    /* Faster the steeper it is */
    speed = penguin.state == PENGUIN_SLIDING ?
            LV_MIN(PENGUIN_SLIDE_SPEED + slope * PENGUIN_FP / 4, PENGUIN_SPEED_MAX) : PENGUIN_WALK_SPEED;
    penguin.x += penguin.dir * speed;

    ground = ground_at(penguin.x / PENGUIN_FP);
    if (ground < 0) {
        /* Out of snow: he has crossed */
        penguin_stop();
        return;
    }
    ground *= PENGUIN_FP;

    if (penguin.airborne) {
        penguin.dy += PENGUIN_GRAVITY;
        penguin.y += penguin.dy;
        if (penguin.y >= ground) {
            penguin.y = ground;
            penguin.airborne = false;
            if (penguin.jumped && (slope >= PENGUIN_SLOPE_PX || slope <= -PENGUIN_SLOPE_PX)) {
                /* A jump that lands on a slope never ends well */
                penguin_trip();
            } else {
                penguin.dy = 0;
                sand_sim_kick(penguin.x / PENGUIN_FP / SAND_CELL_PX);
            }
            penguin.jumped = false;
        }
    } else {
        int fall = ground - penguin.y;

        if (fall > PENGUIN_DROP_FP) {
            /* The ground dropped away under him */
            penguin.airborne = true;
        } else {
            penguin.y = ground;
            penguin.dy = fall;
            if (penguin.frames > PENGUIN_WADDLE_FRAMES * 4 && lv_rand(0, PENGUIN_TRIP_ONE_IN - 1) == 0) {
                penguin_trip();
            } else if (penguin.state == PENGUIN_SLIDING && penguin.frames % PENGUIN_SPRAY_EVERY == 0) {
                sand_sim_kick(penguin.x / PENGUIN_FP / SAND_CELL_PX - penguin.dir * 7);
            }
        }
    }

    penguin_show();
}

static void frame_timer_cb(lv_timer_t *timer)
{
    bool moving = false;

    ARG_UNUSED(timer);

    if (rest_frames >= REST_AFTER_FRAMES) {
        /* At rest: only look for the wrist turning, and now and then send the penguin out */
        if (read_gravity()) {
            wake_sand();
        } else if (penguin_wait_ticks > 0 && --penguin_wait_ticks == 0) {
            penguin_wait_ticks = lv_rand(PENGUIN_RANDOM_TICKS / 2, PENGUIN_RANDOM_TICKS * 3 / 2);
            penguin_start();
        }
        return;
    }

    if (++gravity_frames >= GRAVITY_EVERY_FRAMES) {
        gravity_frames = 0;
        read_gravity();
    }

    penguin_step();
    for (int i = 0; i < STEPS_PER_FRAME; i++) {
        moving |= sand_sim_step();
    }
    moving |= penguin.active;
    invalidate_dirty();

    rest_frames = moving ? 0 : rest_frames + 1;
    if (rest_frames >= REST_AFTER_FRAMES) {
        lv_timer_set_period(frame_timer, REST_PERIOD_MS);
    }
}

static void update_timer_state(void)
{
    if (!frame_timer) {
        return;
    }

    if (face_active && !paused) {
        lv_timer_resume(frame_timer);
        lv_timer_resume(flake_timer);
    } else {
        lv_timer_pause(frame_timer);
        lv_timer_pause(flake_timer);
    }
}

static void root_clicked_cb(lv_event_t *e)
{
    if (!face_active || paused) {
        return;
    }

    if (lv_event_get_code(e) == LV_EVENT_LONG_PRESSED) {
        /* A long press throws the snow up */
        sand_sim_shake();
        wake_sand();
    } else if (penguin.active) {
        penguin_jump();
    } else {
        penguin_start();
    }
}

static void watchface_show(lv_obj_t *parent, watchface_app_evt_listener evt_cb, zsw_settings_watchface_t *settings)
{
    ARG_UNUSED(evt_cb);
    ARG_UNUSED(settings);

    lv_obj_remove_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    root_page = lv_obj_create(parent);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_size(root_page, FACE_SIZE, FACE_SIZE);
    lv_obj_center(root_page);
    lv_obj_remove_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(root_page, lv_color_black(), LV_PART_MAIN);
    /* A night sky behind the sand, drawn straight from the resource filesystem */
    lv_obj_set_style_bg_image_src(root_page, ZSW_LV_IMG_USE(face_sand_sky), LV_PART_MAIN);
    /* A tap sends the penguin out, or makes him jump. A long press throws the snow up. */
    lv_obj_add_flag(root_page, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(root_page, root_clicked_cb, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_event_cb(root_page, root_clicked_cb, LV_EVENT_LONG_PRESSED, NULL);

    reset_cached_values();
    face_active = true;
    paused = false;
    gravity_frames = 0;

    /* The same seed at every start would make the penguin trip at the same place every time */
    lv_rand_set_seed(k_cycle_get_32());
    sand_sim_init(k_cycle_get_32());

    sand_obj = lv_obj_create(root_page);
    lv_obj_remove_style_all(sand_obj);
    lv_obj_set_size(sand_obj, FACE_SIZE, FACE_SIZE);
    lv_obj_center(sand_obj);
    lv_obj_remove_flag(sand_obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(sand_obj, sand_draw_cb, LV_EVENT_DRAW_MAIN, NULL);

    ui_date = zsw_ui_label_create(root_page, "", ZSW_FONT_TEXT, lv_color_hex(COLOR_DATE));
    lv_obj_set_style_text_letter_space(ui_date, 1, LV_PART_MAIN);
    lv_obj_align(ui_date, LV_ALIGN_TOP_MID, 0, 30);

    penguin.img = lv_image_create(root_page);
    penguin.active = false;
    penguin.shown = NULL;
    lv_obj_add_flag(penguin.img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(penguin.img, LV_OBJ_FLAG_CLICKABLE);
    penguin_wait_ticks = PENGUIN_WAKE_TICKS;

    for (int i = 0; i < NUM_FLAKES; i++) {
        flake_respawn(&flakes[i], true);
    }

    frame_timer = lv_timer_create(frame_timer_cb, FRAME_PERIOD_MS, NULL);
    flake_timer = lv_timer_create(flake_timer_cb, FLAKE_PERIOD_MS, NULL);
    wake_sand();
}

static void watchface_remove(void)
{
    if (!root_page) {
        return;
    }

    face_active = false;
    if (frame_timer) {
        lv_timer_delete(frame_timer);
        lv_timer_delete(flake_timer);
        frame_timer = NULL;
        flake_timer = NULL;
    }
    penguin.active = false;
    lv_obj_delete(root_page);
    root_page = NULL;
    sand_obj = NULL;
}

static void watchface_set_display_active(bool active)
{
    if (!root_page) {
        return;
    }

    face_active = active;
    update_timer_state();
    if (active) {
        /* Every other time the watch is looked at, the penguin comes by soon after */
        penguin_wait_ticks = lv_rand(0, 1) ? PENGUIN_WAKE_TICKS : PENGUIN_RANDOM_TICKS;
        wake_sand();
    }
}

static void watchface_set_paused(bool pause)
{
    paused = pause;
    update_timer_state();
}

static void watchface_set_datetime(int day_of_week, int date, int day, int month, int year, int weekday, int32_t hour,
                                   int32_t minute, int32_t second, uint32_t usec, bool am, bool mode)
{
    static const char *const days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
    static const char *const months[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

    ARG_UNUSED(day);
    ARG_UNUSED(year);
    ARG_UNUSED(weekday);
    ARG_UNUSED(second);
    ARG_UNUSED(usec);
    ARG_UNUSED(am);
    ARG_UNUSED(mode);

    if (!root_page) {
        return;
    }

    if (hour != last_hour || minute != last_minute) {
        last_hour = hour;
        last_minute = minute;
        /* The digits that changed crumble and new ones rise from the pile */
        sand_sim_set_time(hour, minute);
        wake_sand();
    }

    if (date != last_day) {
        last_day = date;
        lv_label_set_text_fmt(ui_date, "%s %d %s", days[day_of_week % 7], date, months[month % 12]);
    }
}

/* The face shows the time and the date only, but the watchface app calls these without checking. */
static void watchface_set_battery_percent(int32_t percent, int32_t battery)
{
    ARG_UNUSED(percent);
    ARG_UNUSED(battery);
}

static void watchface_set_hrm(int32_t bpm, int32_t oxygen)
{
    ARG_UNUSED(bpm);
    ARG_UNUSED(oxygen);
}

static void watchface_set_step(int32_t steps, int32_t distance, int32_t kcal)
{
    ARG_UNUSED(steps);
    ARG_UNUSED(distance);
    ARG_UNUSED(kcal);
}

static void watchface_set_ble_connected(bool connected)
{
    ARG_UNUSED(connected);
}

static void watchface_set_num_notifcations(int32_t number)
{
    ARG_UNUSED(number);
}

static void watchface_set_weather(int8_t temperature, int weather_code, uint16_t humidity)
{
    ARG_UNUSED(temperature);
    ARG_UNUSED(weather_code);
    ARG_UNUSED(humidity);
}

static void watchface_set_watch_env_sensors(int pressure)
{
    ARG_UNUSED(pressure);
}

static void watchface_set_bg(const void *bg_img)
{
    ARG_UNUSED(bg_img);
}

static void reset_cached_values(void)
{
    last_minute = -1;
    last_hour = -1;
    last_day = -1;
}

static void watchface_ui_invalidate_cached(void)
{
    /* The time is not reset: that would crumble and rebuild digits that did not change */
    last_day = -1;
}

static const void *watchface_get_preview_img(void)
{
    return ZSW_LV_IMG_USE(face_sand_preview);
}

void zsw_watchface_sand_override_gravity(bool enable, int x, int y)
{
    gravity_overridden = enable;
    if (enable && root_page && sand_sim_set_gravity(x, y)) {
        wake_sand();
    }
}

static watchface_ui_api_t ui_api = {
    .show = watchface_show,
    .remove = watchface_remove,
    .set_battery_percent = watchface_set_battery_percent,
    .set_hrm = watchface_set_hrm,
    .set_step = watchface_set_step,
    .set_ble_connected = watchface_set_ble_connected,
    .set_num_notifcations = watchface_set_num_notifcations,
    .set_weather = watchface_set_weather,
    .set_watch_env_sensors = watchface_set_watch_env_sensors,
    .set_watchface_bg = watchface_set_bg,
    .set_display_active = watchface_set_display_active,
    .set_paused = watchface_set_paused,
    .set_datetime = watchface_set_datetime,
    .ui_invalidate_cached = watchface_ui_invalidate_cached,
    .get_preview_img = watchface_get_preview_img,
    .name = "Sand",
};

static int watchface_init(void)
{
    watchface_app_register_ui(&ui_api);

    return 0;
}

SYS_INIT(watchface_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
