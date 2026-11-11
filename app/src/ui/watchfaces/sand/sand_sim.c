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

#include <string.h>

#include "sand_sim.h"

#define GRID                SAND_GRID_SIZE

/* Grains in the pile at the end of the day: it then fills about the lower third of the display. */
#define GRAINS_PER_DAY      2600
#define MINUTES_PER_DAY     (24 * 60)

/* Sand that is owed comes in from the top, one grain every this many steps. */
#define TRICKLE_EVERY_STEPS 2
#define TRICKLE_SPREAD      3
#define TRICKLE_CATCH_UP    120
/* How far from the middle the sand of the day so far is dropped: a wide, low dune */
#define POUR_SPREAD         34

#define SHAKE_STEPS         16

/* Dust is gone after about this many steps, a bit more than half way down to the pile. */
#define DUST_LIFE_STEPS     28

/* In-plane gravity below this (0.2 g) means the watch lies flat: keep the last direction. */
#define GRAVITY_MIN         26

typedef struct {
    /* Four bits per cell */
    uint8_t cells[GRID * GRID / 2];
    /* First column inside the round display for each row. The last is its mirror. */
    uint8_t row_first[GRID];
    uint8_t dirty_first[SAND_NUM_BANDS];
    uint8_t dirty_last[SAND_NUM_BANDS];
    /*
     * The box where something moved in the last step. Only cells in it, and next to it, can
     * move in the next step, so that is all a step looks at. Empty when x0 > x1.
     */
    uint8_t active_x0;
    uint8_t active_y0;
    uint8_t active_x1;
    uint8_t active_y1;
    uint8_t next_x0;
    uint8_t next_y0;
    uint8_t next_x1;
    uint8_t next_y1;
    uint32_t rng;
    uint32_t step_count;
    /* Grains poured so far today, and how many of them have not come in yet */
    int16_t poured;
    int16_t owed;
    int16_t minute_of_day;
    int8_t gravity_x;
    int8_t gravity_y;
    /* The neighbour sand falls to, and the two it slides to when that one is taken. */
    int8_t fall[3][2];
    uint8_t shake_steps;
    bool blowing_away;
} sand_world_t;

static sand_world_t world;

static const int8_t directions[8][2] = {
    { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 },
};

static uint32_t random_next(void)
{
    /* xorshift32 */
    world.rng ^= world.rng << 13;
    world.rng ^= world.rng >> 17;
    world.rng ^= world.rng << 5;

    return world.rng;
}

static inline bool inside(int x, int y)
{
    return y >= 0 && y < GRID && x >= world.row_first[y] && x <= GRID - 1 - world.row_first[y];
}

static inline uint8_t get_cell(int x, int y)
{
    int index = y * GRID + x;

    return (world.cells[index >> 1] >> ((index & 1) * 4)) & 0xF;
}

static void mark_active(int x, int y)
{
    if (x < world.next_x0) {
        world.next_x0 = x;
    }
    if (x > world.next_x1) {
        world.next_x1 = x;
    }
    if (y < world.next_y0) {
        world.next_y0 = y;
    }
    if (y > world.next_y1) {
        world.next_y1 = y;
    }
}

static void set_cell(int x, int y, uint8_t cell)
{
    int index = y * GRID + x;
    int shift = (index & 1) * 4;
    int band = y / SAND_BAND_ROWS;

    world.cells[index >> 1] = (world.cells[index >> 1] & ~(0xF << shift)) | (cell << shift);

    if (x < world.dirty_first[band]) {
        world.dirty_first[band] = x;
    }
    if (x > world.dirty_last[band]) {
        world.dirty_last[band] = x;
    }
    mark_active(x, y);
}

static inline bool is_empty(int x, int y)
{
    return inside(x, y) && get_cell(x, y) == SAND_CELL_EMPTY;
}

static void activate_all(void)
{
    world.next_x0 = 0;
    world.next_y0 = 0;
    world.next_x1 = GRID - 1;
    world.next_y1 = GRID - 1;
}

/* The color of the sand that falls at a time of day: a new one every two hours. */
static uint8_t color_at(int minute_of_day)
{
    return 1 + (minute_of_day * SAND_NUM_COLORS / MINUTES_PER_DAY) % SAND_NUM_COLORS;
}

static int grains_at(int minute_of_day)
{
    return minute_of_day * GRAINS_PER_DAY / MINUTES_PER_DAY;
}

/* Picks the neighbour to fall to from the gravity vector, and the two next to it to slide to. */
static void update_fall_directions(void)
{
    int gx = world.shake_steps > 0 ? -world.gravity_x : world.gravity_x;
    int gy = world.shake_steps > 0 ? -world.gravity_y : world.gravity_y;
    int ax = gx < 0 ? -gx : gx;
    int ay = gy < 0 ? -gy : gy;
    int main;

    /* The eight neighbours are 45 degrees apart, so each covers 22.5 degrees to either side: tan = 0.414 */
    if (ax * 1000 < ay * 414) {
        main = gy > 0 ? 0 : 4;
    } else if (ay * 1000 < ax * 414) {
        main = gx > 0 ? 2 : 6;
    } else if (gx > 0) {
        main = gy > 0 ? 1 : 3;
    } else {
        main = gy > 0 ? 7 : 5;
    }

    int left = (main + 7) % 8;
    int right = (main + 1) % 8;
    /* The side that gravity leans to goes first */
    int lean = gx * directions[main][1] - gy * directions[main][0];
    int first = lean > 0 ? left : right;
    int second = lean > 0 ? right : left;

    world.fall[0][0] = directions[main][0];
    world.fall[0][1] = directions[main][1];
    world.fall[1][0] = directions[first][0];
    world.fall[1][1] = directions[first][1];
    world.fall[2][0] = directions[second][0];
    world.fall[2][1] = directions[second][1];
}

static void move_grain(int x, int y, uint8_t cell)
{
    uint32_t random = random_next();
    bool dust = cell == SAND_CELL_DUST;
    /* Straight gravity has no side to prefer: pick one at random so that piles grow evenly */
    int swap = (world.fall[0][0] == 0 || world.fall[0][1] == 0) ? (random & 1) : 0;
    int first = 0;

    if (world.blowing_away && (random >> 8) % 12 == 0) {
        set_cell(x, y, SAND_CELL_EMPTY);
        return;
    }

    if (dust) {
        if ((random >> 8) % DUST_LIFE_STEPS == 0) {
            set_cell(x, y, SAND_CELL_EMPTY);
            return;
        }
        /* Dust drifts: it takes any of the three ways down, and rests every other step */
        first = (random >> 1) % 3;
    } else if (world.shake_steps > 0) {
        /* Thrown up: any of the three ways, so that the pile bursts into a cloud */
        first = (random >> 1) % 3;
    }

    for (int i = first; i < 3; i++) {
        int choice = i == 0 ? 0 : (swap ? 3 - i : i);
        int nx = x + world.fall[choice][0];
        int ny = y + world.fall[choice][1];

        if (!is_empty(nx, ny)) {
            continue;
        }

        /*
         * Now and then a grain hangs back a step, or falling sand would move as one solid piece.
         * It still counts as moving, or it would be left hanging when all around it has come to rest.
         */
        if (((random >> 4) & 7) == 0 || (dust && ((random >> 7) & 1))) {
            mark_active(x, y);
        } else {
            set_cell(x, y, SAND_CELL_EMPTY);
            set_cell(nx, ny, cell);
        }
        return;
    }

    if (dust) {
        /* Dust that lands is gone, it never piles up */
        set_cell(x, y, SAND_CELL_EMPTY);
    } else if (world.blowing_away) {
        mark_active(x, y);
    }
}

/*
 * Cells are visited from the side the sand falls to, so that a grain always moves into a cell
 * that was already visited and is not moved twice in a step.
 */
static void step_sand(void)
{
    int fx = world.fall[0][0];
    int fy = world.fall[0][1];
    bool reverse = world.step_count & 1;
    int x0 = world.active_x0;
    int x1 = world.active_x1;
    int y0 = world.active_y0;
    int y1 = world.active_y1;

    if (fy != 0) {
        for (int i = y0; i <= y1; i++) {
            int y = fy > 0 ? y1 - (i - y0) : i;
            int first = world.row_first[y] > x0 ? world.row_first[y] : x0;
            int last = GRID - 1 - world.row_first[y] < x1 ? GRID - 1 - world.row_first[y] : x1;
            bool right_to_left = fx > 0 || (fx == 0 && reverse);

            for (int j = first; j <= last; j++) {
                int x = right_to_left ? last - (j - first) : j;
                uint8_t cell = get_cell(x, y);

                if (cell != SAND_CELL_EMPTY) {
                    move_grain(x, y, cell);
                }
            }
        }
    } else {
        for (int i = x0; i <= x1; i++) {
            int x = fx > 0 ? x1 - (i - x0) : i;

            for (int j = y0; j <= y1; j++) {
                int y = reverse ? y1 - (j - y0) : j;

                if (!inside(x, y)) {
                    continue;
                }

                uint8_t cell = get_cell(x, y);

                if (cell != SAND_CELL_EMPTY) {
                    move_grain(x, y, cell);
                }
            }
        }
    }
}

/* Takes the box of what moved in the last step, grown by one cell, as what to look at now. */
static bool next_active_box(void)
{
    if (world.next_x0 > world.next_x1) {
        world.active_x0 = 1;
        world.active_x1 = 0;
        return false;
    }

    world.active_x0 = world.next_x0 > 0 ? world.next_x0 - 1 : 0;
    world.active_y0 = world.next_y0 > 0 ? world.next_y0 - 1 : 0;
    world.active_x1 = world.next_x1 < GRID - 1 ? world.next_x1 + 1 : GRID - 1;
    world.active_y1 = world.next_y1 < GRID - 1 ? world.next_y1 + 1 : GRID - 1;

    world.next_x0 = GRID - 1;
    world.next_y0 = GRID - 1;
    world.next_x1 = 0;
    world.next_y1 = 0;

    return true;
}

/* A grain of the sand that is owed comes in at the edge the sand falls away from. */
static void trickle(void)
{
    int x = GRID / 2 + (int)(random_next() % (2 * TRICKLE_SPREAD + 1)) - TRICKLE_SPREAD;
    int y = GRID / 2;

    while (inside(x - world.fall[0][0], y - world.fall[0][1])) {
        x -= world.fall[0][0];
        y -= world.fall[0][1];
    }

    /* If the pile is in the way the grain is dropped, or the world would wait for room for ever */
    if (is_empty(x, y)) {
        set_cell(x, y, color_at(world.minute_of_day));
    }
    world.owed--;
}

/*
 * Pours the sand of the day so far without showing it fall: each grain is dropped near the
 * middle and comes to rest as it would have, so the layers get the shape of a real pile.
 */
static void pour_day_so_far(void)
{
    int grains = grains_at(world.minute_of_day);

    for (int i = 0; i < grains; i++) {
        /* The sum of two random numbers: most grains near the middle, few far out */
        int x = GRID / 2 + (int)(random_next() % POUR_SPREAD) + (int)(random_next() % POUR_SPREAD) - (POUR_SPREAD - 1);
        int y = world.row_first[x] + 1;
        uint8_t color = color_at(i * MINUTES_PER_DAY / GRAINS_PER_DAY);

        if (!is_empty(x, y)) {
            continue;
        }

        while (true) {
            int side = (random_next() & 1) ? 1 : -1;

            if (is_empty(x, y + 1)) {
                y++;
            } else if (is_empty(x + side, y + 1)) {
                x += side;
                y++;
            } else if (is_empty(x - side, y + 1)) {
                x -= side;
                y++;
            } else {
                break;
            }
        }
        set_cell(x, y, color);
    }

    world.poured = grains;
    world.owed = 0;
}

void sand_sim_init(uint32_t seed)
{
    /* Half a cell is left free at the edge, the display is not a perfect circle of cells */
    const int radius_sq_x4 = (GRID - 1) * (GRID - 1);

    memset(&world, 0, sizeof(world));
    world.rng = seed | 1;
    world.gravity_y = 127;
    world.minute_of_day = -1;
    world.next_x0 = GRID - 1;
    world.next_y0 = GRID - 1;

    for (int y = 0; y < GRID; y++) {
        int dy = 2 * y - (GRID - 1);
        int x = 0;

        while (x < GRID / 2 && (2 * x - (GRID - 1)) * (2 * x - (GRID - 1)) + dy * dy > radius_sq_x4) {
            x++;
        }
        world.row_first[y] = x;
    }

    update_fall_directions();
}

bool sand_sim_set_gravity(int x, int y)
{
    int8_t before_x = world.fall[0][0];
    int8_t before_y = world.fall[0][1];
    int8_t new_x = x > 127 ? 127 : (x < -127 ? -127 : x);
    int8_t new_y = y > 127 ? 127 : (y < -127 ? -127 : y);

    if (x * x + y * y < GRAVITY_MIN * GRAVITY_MIN) {
        return false;
    }

    world.gravity_x = new_x;
    world.gravity_y = new_y;
    update_fall_directions();
    if (before_x == world.fall[0][0] && before_y == world.fall[0][1]) {
        /* Still the same way: nothing that was at rest can move */
        return false;
    }

    activate_all();

    return true;
}

void sand_sim_set_time(int hour, int minute)
{
    int minute_of_day = (hour * 60 + minute) % MINUTES_PER_DAY;
    bool first = world.minute_of_day < 0;
    bool new_day = minute_of_day < world.minute_of_day;

    world.minute_of_day = minute_of_day;

    if (first) {
        pour_day_so_far();
    } else if (new_day) {
        /* The pile blows away, and the new day starts from nothing */
        world.blowing_away = true;
        world.poured = 0;
        world.owed = grains_at(minute_of_day);
        activate_all();
    } else {
        int grains = grains_at(minute_of_day);

        world.owed += grains - world.poured;
        world.poured = grains;
    }
}

void sand_sim_dissolve(int cell_x, int cell_y, const uint16_t *rows, int width, int height)
{
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (((rows[row] >> (width - 1 - col)) & 1) && is_empty(cell_x + col, cell_y + row)) {
                set_cell(cell_x + col, cell_y + row, SAND_CELL_DUST);
            }
        }
    }
}

void sand_sim_shake(void)
{
    world.shake_steps = SHAKE_STEPS;
    update_fall_directions();
    activate_all();
}

bool sand_sim_step(void)
{
    bool active;

    world.step_count++;

    if (world.shake_steps > 0) {
        world.shake_steps--;
        if (world.shake_steps == 0) {
            update_fall_directions();
            activate_all();
        }
    }

    if (!world.blowing_away && world.step_count % TRICKLE_EVERY_STEPS == 0) {
        /* A thin stream, thicker when a lot is owed after the watch has been asleep for long */
        for (int i = world.owed / TRICKLE_CATCH_UP; i >= 0 && world.owed > 0; i--) {
            trickle();
        }
    }

    active = next_active_box();
    if (active) {
        step_sand();
    } else {
        world.blowing_away = false;
    }

    return active || world.shake_steps > 0 || world.owed > 0;
}

uint8_t sand_sim_cell(int x, int y)
{
    return get_cell(x, y);
}

bool sand_sim_pop_dirty(int band, int *x_first, int *x_last)
{
    if (world.dirty_first[band] > world.dirty_last[band]) {
        return false;
    }

    *x_first = world.dirty_first[band];
    *x_last = world.dirty_last[band];
    world.dirty_first[band] = GRID - 1;
    world.dirty_last[band] = 0;

    return true;
}
