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

#include "snow_sim.h"
#include "snow_glyphs.h"

#define GRID                SNOW_GRID_SIZE

/* Grains in the pile. Snow that is lost or left over in a time change is evened out afterwards. */
#define PILE_GRAINS         1250
/* About what four digits take. Poured on top at the start, as the first digits are built from the pile. */
#define FIRST_DIGITS_GRAINS 1000

/* The time is four digits and a colon: HH:MM, a bit above the middle so the pile has room. */
#define NUM_SLOTS           4
#define DIGIT_GAP           3
#define COLON_WIDTH         10
#define TIME_WIDTH          (NUM_SLOTS * SNOW_GLYPH_WIDTH + 2 * DIGIT_GAP + COLON_WIDTH)
#define TIME_X              ((GRID - TIME_WIDTH) / 2)
#define TIME_Y              28
#define COLON_DOT_SIZE      4
#define COLON_DOT_1_Y       7
#define COLON_DOT_2_Y       17
#define COLON_X             (TIME_X + 2 * SNOW_GLYPH_WIDTH + DIGIT_GAP + (COLON_WIDTH - COLON_DOT_SIZE) / 2)

/* A flying grain takes this many steps, and the build of new digits waits for the old to fall away. */
#define FLIGHT_STEPS        18
#define CRUMBLE_STEPS       12
#define BUILD_DELAY_STEPS   18
#define MAX_LAUNCH_PER_STEP 12
#define FLIGHT_SWIRL_PX     16

#define SHAKE_STEPS         18
#define EVEN_OUT_MAX_STEPS  600
#define EVEN_OUT_PER_STEP   4
/* How far up a grain is thrown by snow_sim_kick() */
#define KICK_HEIGHT         3

/* In-plane gravity below this (0.2 g) means the watch lies flat: keep the last direction. */
#define GRAVITY_MIN         26

#define NO_DIGIT            0xFF
#define FLYING_FREE         0xFF

typedef struct {
    uint8_t from_x;
    uint8_t from_y;
    uint8_t to_x;
    uint8_t to_y;
    uint8_t step;       /* FLYING_FREE when unused */
    uint8_t shade;
} flying_grain_t;

typedef struct {
    uint8_t digit;      /* The digit shown, or being built */
    uint8_t build_row;  /* Next glyph row to build, from the bottom. SNOW_GLYPH_HEIGHT when done. */
    uint8_t build_col;
    uint8_t crumble_steps;  /* Steps left of letting go of the previous digit */
} slot_t;

typedef struct {
    /* Four bits per cell: type in the low two, shade in the high two */
    uint8_t cells[GRID * GRID / 2];
    /* First column inside the round display for each row. The last is its mirror. */
    uint8_t row_first[GRID];
    uint8_t dirty_first[SNOW_NUM_BANDS];
    uint8_t dirty_last[SNOW_NUM_BANDS];
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
    flying_grain_t flying[SNOW_MAX_FLYING];
    slot_t slots[NUM_SLOTS];
    uint32_t rng;
    uint32_t step_count;
    int16_t snow_count;
    int8_t gravity_x;
    int8_t gravity_y;
    /* The neighbour snow falls to, and the two it slides to when that one is taken. */
    int8_t fall[3][2];
    uint8_t build_delay;
    uint8_t shake_steps;
    uint8_t next_slot;
    uint16_t even_out_steps;
} snow_world_t;

static snow_world_t world;

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

static uint8_t random_shade(void)
{
    static const uint8_t shades[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 3, 3 };

    /* Mostly the base shade, some darker, a few very dark and some lighter grains */
    return shades[random_next() & 0xF];
}

static inline bool inside(int x, int y)
{
    return y >= 0 && y < GRID && x >= world.row_first[y] && x <= GRID - 1 - world.row_first[y];
}

/* A cell inside the display. The first and last row have none. */
static void random_cell(int *x, int *y)
{
    int first;

    *y = 1 + random_next() % (GRID - 2);
    first = world.row_first[*y];
    *x = first + random_next() % (GRID - 2 * first);
}

static inline uint8_t get_cell(int x, int y)
{
    int index = y * GRID + x;

    return (world.cells[index >> 1] >> ((index & 1) * 4)) & 0xF;
}

static void mark_dirty(int x, int y)
{
    int band = y / SNOW_BAND_ROWS;

    if (x < world.dirty_first[band]) {
        world.dirty_first[band] = x;
    }
    if (x > world.dirty_last[band]) {
        world.dirty_last[band] = x;
    }
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

static void activate_all(void)
{
    world.next_x0 = 0;
    world.next_y0 = 0;
    world.next_x1 = GRID - 1;
    world.next_y1 = GRID - 1;
}

static void set_cell(int x, int y, uint8_t cell)
{
    int index = y * GRID + x;
    int shift = (index & 1) * 4;

    world.cells[index >> 1] = (world.cells[index >> 1] & ~(0xF << shift)) | (cell << shift);
    mark_dirty(x, y);
    mark_active(x, y);
}

static inline bool is_empty(int x, int y)
{
    return inside(x, y) && SNOW_CELL_TYPE(get_cell(x, y)) == SNOW_CELL_EMPTY;
}

static int slot_x(int slot)
{
    int x = TIME_X + slot * SNOW_GLYPH_WIDTH;

    if (slot >= 1) {
        x += DIGIT_GAP;
    }
    if (slot >= 2) {
        x += COLON_WIDTH;
    }
    if (slot >= 3) {
        x += DIGIT_GAP;
    }

    return x;
}

static inline bool glyph_bit(int digit, int col, int row)
{
    return (snow_glyphs[digit][row] >> (SNOW_GLYPH_WIDTH - 1 - col)) & 1;
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
    /* Straight gravity has no side to prefer: pick one at random so that piles grow evenly */
    int swap = (world.fall[0][0] == 0 || world.fall[0][1] == 0) ? (random & 1) : 0;
    /* Thrown up by a shake: any of the three ways, so that the pile bursts into a cloud */
    int first = world.shake_steps > 0 ? (random >> 1) % 3 : 0;

    for (int i = first; i < 3; i++) {
        int choice = i == 0 ? 0 : (swap ? 3 - i : i);
        int nx = x + world.fall[choice][0];
        int ny = y + world.fall[choice][1];

        if (!is_empty(nx, ny)) {
            continue;
        }

        /*
         * Now and then a grain hangs back a step, or falling snow would move as one solid piece.
         * It still counts as moving, or it would be left hanging when all around it has come to rest.
         */
        if (((random >> 4) & 7) == 0) {
            mark_active(x, y);
        } else {
            set_cell(x, y, SNOW_CELL_EMPTY);
            set_cell(nx, ny, cell);
        }
        return;
    }
}

/*
 * Cells are visited from the side the snow falls to, so that a grain always moves into a cell
 * that was already visited and is not moved twice in a step.
 */
static void step_snow(void)
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

                if (SNOW_CELL_TYPE(cell) == SNOW_CELL_SNOW) {
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

                if (SNOW_CELL_TYPE(cell) == SNOW_CELL_SNOW) {
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

/*
 * A grain of snow with nothing on top of it, to lift from the pile: the first one met when
 * following the way the snow falls from a random place on the edge.
 */
static bool find_surface_grain(int *grain_x, int *grain_y)
{
    int fx = world.fall[0][0];
    int fy = world.fall[0][1];

    for (int i = 0; i < 4; i++) {
        int x;
        int y;
        bool above_empty = true;

        random_cell(&x, &y);
        while (inside(x - fx, y - fy)) {
            x -= fx;
            y -= fy;
        }

        for (; inside(x, y); x += fx, y += fy) {
            snow_cell_type_t type = SNOW_CELL_TYPE(get_cell(x, y));

            if (type == SNOW_CELL_SNOW && above_empty) {
                *grain_x = x;
                *grain_y = y;
                return true;
            }
            above_empty = type == SNOW_CELL_EMPTY;
        }
    }

    return false;
}

/* Finds the next grain missing in the digit of a slot, bottom row first, and steps past it. */
static bool next_build_cell(slot_t *slot, int *col, int *row)
{
    while (slot->build_row < SNOW_GLYPH_HEIGHT) {
        int glyph_row = SNOW_GLYPH_HEIGHT - 1 - slot->build_row;

        while (slot->build_col < SNOW_GLYPH_WIDTH) {
            /* Every other row goes the other way, like sweeping back and forth */
            int glyph_col = (slot->build_row & 1) ? SNOW_GLYPH_WIDTH - 1 - slot->build_col : slot->build_col;

            slot->build_col++;
            if (glyph_bit(slot->digit, glyph_col, glyph_row)) {
                *col = glyph_col;
                *row = glyph_row;
                return true;
            }
        }
        slot->build_col = 0;
        slot->build_row++;
    }

    return false;
}

static bool building(void)
{
    for (int i = 0; i < NUM_SLOTS; i++) {
        if (world.slots[i].digit != NO_DIGIT && world.slots[i].build_row < SNOW_GLYPH_HEIGHT) {
            return true;
        }
    }

    return false;
}

static void launch_grain(flying_grain_t *grain)
{
    for (int i = 0; i < NUM_SLOTS; i++) {
        int index = (world.next_slot + i) % NUM_SLOTS;
        slot_t *slot = &world.slots[index];
        int col;
        int row;
        int from_x;
        int from_y;

        if (slot->digit == NO_DIGIT || !next_build_cell(slot, &col, &row)) {
            continue;
        }

        grain->to_x = slot_x(index) + col;
        grain->to_y = TIME_Y + row;
        if (find_surface_grain(&from_x, &from_y)) {
            grain->shade = SNOW_CELL_SHADE(get_cell(from_x, from_y));
            set_cell(from_x, from_y, SNOW_CELL_EMPTY);
            world.snow_count--;
        } else {
            /* The pile is out of reach: the grain comes in from the edge below its place */
            from_x = grain->to_x;
            from_y = GRID - 1;
            grain->shade = random_shade();
        }
        grain->from_x = from_x;
        grain->from_y = from_y;
        grain->step = 0;
        world.next_slot = (index + 1) % NUM_SLOTS;
        return;
    }
}

static void flying_position(const flying_grain_t *grain, int *x, int *y)
{
    /* Ease out: fast off the pile, slow into place */
    int left = FLIGHT_STEPS - grain->step;
    int progress = 256 - (left * left * 256) / (FLIGHT_STEPS * FLIGHT_STEPS);
    /* A bow to one side or the other, largest halfway, makes the grains swirl up */
    int swirl = (progress * (256 - progress) * FLIGHT_SWIRL_PX) >> 14;
    int center = SNOW_CELL_PX / 2;

    *x = ((grain->from_x * (256 - progress) + grain->to_x * progress) * SNOW_CELL_PX >> 8) + center;
    *y = ((grain->from_y * (256 - progress) + grain->to_y * progress) * SNOW_CELL_PX >> 8) + center;
    *x += ((grain->to_x + grain->to_y) & 1) ? swirl : -swirl;
}

static void mark_flying_dirty(const flying_grain_t *grain)
{
    int x;
    int y;

    flying_position(grain, &x, &y);
    /* It is drawn one cell large around its position, so it can touch four cells */
    for (int i = 0; i < 4; i++) {
        int cell_x = (x + ((i & 1) ? 1 : -1)) / SNOW_CELL_PX;
        int cell_y = (y + ((i & 2) ? 1 : -1)) / SNOW_CELL_PX;

        if (cell_x >= 0 && cell_x < GRID && cell_y >= 0 && cell_y < GRID) {
            mark_dirty(cell_x, cell_y);
        }
    }
}

static bool step_flying(void)
{
    bool active = false;
    int launches = 0;
    bool launch = world.build_delay == 0 && building();

    for (int i = 0; i < SNOW_MAX_FLYING; i++) {
        flying_grain_t *grain = &world.flying[i];

        if (grain->step == FLYING_FREE) {
            if (launch && launches < MAX_LAUNCH_PER_STEP) {
                launch_grain(grain);
                launches++;
            }
            if (grain->step == FLYING_FREE) {
                continue;
            }
        }

        active = true;
        mark_flying_dirty(grain);
        grain->step++;
        if (grain->step >= FLIGHT_STEPS) {
            if (SNOW_CELL_TYPE(get_cell(grain->to_x, grain->to_y)) == SNOW_CELL_SNOW) {
                /* Loose snow was in the way and becomes part of the digit */
                world.snow_count--;
            }
            set_cell(grain->to_x, grain->to_y, SNOW_CELL_DIGIT | (grain->shade << 2));
            grain->step = FLYING_FREE;
        } else {
            mark_flying_dirty(grain);
        }
    }

    return active;
}

/* One grain off the top of the pile, or one onto it. */
static void even_out_grain(void)
{
    int x;
    int y;

    if (world.snow_count > PILE_GRAINS) {
        if (find_surface_grain(&x, &y)) {
            set_cell(x, y, SNOW_CELL_EMPTY);
            world.snow_count--;
        }
        return;
    }

    /*
     * A grain is put straight on top of the pile, as if it had fallen in from the edge the snow
     * falls away from. Letting it fall all the way would keep the world awake for many steps per
     * grain. Grains that would land on a digit are not added.
     */
    random_cell(&x, &y);
    while (inside(x - world.fall[0][0], y - world.fall[0][1])) {
        x -= world.fall[0][0];
        y -= world.fall[0][1];
    }
    if (is_empty(x, y)) {
        while (is_empty(x + world.fall[0][0], y + world.fall[0][1])) {
            x += world.fall[0][0];
            y += world.fall[0][1];
        }
        if (!inside(x + world.fall[0][0], y + world.fall[0][1]) ||
            SNOW_CELL_TYPE(get_cell(x + world.fall[0][0], y + world.fall[0][1])) == SNOW_CELL_SNOW) {
            set_cell(x, y, SNOW_CELL_SNOW | (random_shade() << 2));
            world.snow_count++;
        }
    }
}

/* Digits differ in size, so the pile drifts. Add or remove a few grains a step until it is back. */
static bool even_out_pile(void)
{
    if (world.snow_count == PILE_GRAINS) {
        world.even_out_steps = 0;
        return false;
    }

    /* Never keep the world awake for this: give up if there is no grain to take or no room to add */
    if (++world.even_out_steps > EVEN_OUT_MAX_STEPS) {
        world.snow_count = PILE_GRAINS;
        return false;
    }

    for (int i = 0; i < EVEN_OUT_PER_STEP && world.snow_count != PILE_GRAINS; i++) {
        even_out_grain();
    }

    return true;
}

/*
 * Lets go of the grains of an old digit, a random quarter of them per step so that it crumbles
 * instead of falling in one piece. The last step takes what is left.
 */
static void crumble_slot(int slot, bool all)
{
    int x0 = slot_x(slot);

    for (int row = 0; row < SNOW_GLYPH_HEIGHT; row++) {
        for (int col = 0; col < SNOW_GLYPH_WIDTH; col++) {
            uint8_t cell = get_cell(x0 + col, TIME_Y + row);

            if (SNOW_CELL_TYPE(cell) == SNOW_CELL_DIGIT && (all || (random_next() & 3) == 0)) {
                set_cell(x0 + col, TIME_Y + row, SNOW_CELL_SNOW | (SNOW_CELL_SHADE(cell) << 2));
                world.snow_count++;
            }
        }
    }
}

static bool step_crumble(void)
{
    bool active = false;

    for (int i = 0; i < NUM_SLOTS; i++) {
        if (world.slots[i].crumble_steps > 0) {
            world.slots[i].crumble_steps--;
            crumble_slot(i, world.slots[i].crumble_steps == 0);
            active = true;
        }
    }

    return active;
}

static void release_slot(int slot)
{
    int x0 = slot_x(slot);

    /* Grains still on their way to this digit would land in the wrong one */
    for (int i = 0; i < SNOW_MAX_FLYING; i++) {
        flying_grain_t *grain = &world.flying[i];

        if (grain->step != FLYING_FREE && grain->to_x >= x0 && grain->to_x < x0 + SNOW_GLYPH_WIDTH) {
            mark_flying_dirty(grain);
            grain->step = FLYING_FREE;
        }
    }

    world.slots[slot].crumble_steps = CRUMBLE_STEPS;
}

void snow_sim_init(uint32_t seed)
{
    /* Half a cell is left free at the edge, the display is not a perfect circle of cells */
    const int radius_sq_x4 = (GRID - 1) * (GRID - 1);
    const int total = PILE_GRAINS + FIRST_DIGITS_GRAINS;
    int poured = 0;

    memset(&world, 0, sizeof(world));
    world.rng = seed | 1;
    world.gravity_y = 127;
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

    for (int i = 0; i < SNOW_MAX_FLYING; i++) {
        world.flying[i].step = FLYING_FREE;
    }
    for (int i = 0; i < NUM_SLOTS; i++) {
        world.slots[i].digit = NO_DIGIT;
    }

    for (int y = GRID - 1; y >= 0 && poured < total; y--) {
        for (int x = world.row_first[y]; x <= GRID - 1 - world.row_first[y] && poured < total; x++) {
            set_cell(x, y, SNOW_CELL_SNOW | (random_shade() << 2));
            poured++;
        }
    }
    world.snow_count = poured;

    for (int dot = 0; dot < 2; dot++) {
        int y = TIME_Y + (dot == 0 ? COLON_DOT_1_Y : COLON_DOT_2_Y);

        for (int i = 0; i < COLON_DOT_SIZE * COLON_DOT_SIZE; i++) {
            set_cell(COLON_X + i % COLON_DOT_SIZE, y + i / COLON_DOT_SIZE, SNOW_CELL_DIGIT | (random_shade() << 2));
        }
    }

    update_fall_directions();
}

bool snow_sim_set_gravity(int x, int y)
{
    int8_t before_x = world.fall[0][0];
    int8_t before_y = world.fall[0][1];

    if (x * x + y * y < GRAVITY_MIN * GRAVITY_MIN) {
        return false;
    }

    world.gravity_x = x > 127 ? 127 : (x < -127 ? -127 : x);
    world.gravity_y = y > 127 ? 127 : (y < -127 ? -127 : y);
    update_fall_directions();
    if (before_x == world.fall[0][0] && before_y == world.fall[0][1]) {
        /* Still the same way: nothing that was at rest can move */
        return false;
    }

    activate_all();

    return true;
}

void snow_sim_set_time(int hour, int minute)
{
    const uint8_t digits[NUM_SLOTS] = { hour / 10 % 10, hour % 10, minute / 10 % 10, minute % 10 };
    bool released = false;

    for (int i = 0; i < NUM_SLOTS; i++) {
        if (world.slots[i].digit == digits[i]) {
            continue;
        }
        if (world.slots[i].digit != NO_DIGIT) {
            release_slot(i);
            released = true;
        }
        world.slots[i].digit = digits[i];
        world.slots[i].build_row = 0;
        world.slots[i].build_col = 0;
    }

    if (released) {
        world.build_delay = BUILD_DELAY_STEPS;
    }
}

void snow_sim_shake(void)
{
    world.shake_steps = SHAKE_STEPS;
    update_fall_directions();
    activate_all();
}

bool snow_sim_step(void)
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

    if (world.build_delay > 0) {
        world.build_delay--;
    }

    active = step_crumble();
    active |= step_flying();
    if (next_active_box()) {
        step_snow();
        active = true;
    }
    active |= world.shake_steps > 0 || world.build_delay > 0 || building();
    if (!active) {
        active = even_out_pile();
    }

    return active;
}

uint8_t snow_sim_cell(int x, int y)
{
    return get_cell(x, y);
}

int snow_sim_get_flying(snow_flying_t *grains, int max)
{
    int count = 0;

    for (int i = 0; i < SNOW_MAX_FLYING && count < max; i++) {
        if (world.flying[i].step != FLYING_FREE) {
            int x;
            int y;

            flying_position(&world.flying[i], &x, &y);
            grains[count].x = x;
            grains[count].y = y;
            grains[count].shade = world.flying[i].shade;
            count++;
        }
    }

    return count;
}

bool snow_sim_pop_dirty(int band, int *x_first, int *x_last)
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

int snow_sim_surface(int x)
{
    int top = GRID;
    int y;

    if (x < 0 || x >= GRID) {
        return GRID;
    }

    y = GRID - 1 - world.row_first[x];

    /* From the edge at the bottom of the column and up, as long as it is snow */
    for (; y >= 0 && inside(x, y) && SNOW_CELL_TYPE(get_cell(x, y)) == SNOW_CELL_SNOW; y--) {
        top = y;
    }

    return top;
}

void snow_sim_kick(int x)
{
    int y = snow_sim_surface(x);

    if (y < GRID && is_empty(x, y - KICK_HEIGHT)) {
        uint8_t cell = get_cell(x, y);

        set_cell(x, y, SNOW_CELL_EMPTY);
        set_cell(x, y - KICK_HEIGHT, cell);
    }
}

bool snow_sim_is_level(void)
{
    return world.fall[0][0] == 0 && world.fall[0][1] == 1 && world.shake_steps == 0;
}
