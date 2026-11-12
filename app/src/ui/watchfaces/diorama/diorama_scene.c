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

#include "diorama_scene.h"
#include "diorama_terrain.h"

#define SIZE                    DIORAMA_SIZE
#define LAKE_Y                  DIORAMA_LAKE_Y
#define NUM_LAYERS              DIORAMA_NUM_LAYERS
#define MARGIN                  DIORAMA_TERRAIN_MARGIN

/* Rows where the sky reaches its middle and its horizon color */
#define SKY_MID_Y               88
#define SKY_HORIZON_Y           150

/* The sun and the moon travel between these columns */
#define ORBIT_X1                34
#define ORBIT_X2                206
/* Row of the middle of the sun at noon: in the depth of winter and in high summer */
#define SUN_PEAK_LOW_Y          108
#define SUN_PEAK_HIGH_Y         91
#define MOON_PEAK_Y             100
/* The least a body rises over where it comes up */
#define ORBIT_MIN_LIFT          12
/* A body this far past the end of its arc, of 256, is gone */
#define ORBIT_GONE              31
/* Rows a body sinks for a whole arc past the end, and the most it sinks */
#define ORBIT_SINK              500
#define ORBIT_MAX_SINK          60
#define SUN_RADIUS              10
#define MOON_RADIUS             8
#define MOON_GLOW_RADIUS        26

/* Steps from the middle to the edge that the light around the sun and the moon is known at */
#define GLOW_STEPS              48
/* Light weaker than this, of 256, never changes a pixel */
#define GLOW_MIN_VISIBLE        4

/* The northern lights: curtains that reach this many rows up from their foot, and fade below it */
#define AURORA_BANDS            2
#define AURORA_ROWS             40
#define AURORA_FOOT             5
/* No curtain reaches below this row */
#define AURORA_LAST_ROW         103
#define AURORA_PERIOD_MS        300
/* The stars have to be this bright, of 255, before the northern lights show */
#define AURORA_MIN_STARS        64

#define SMOKE_PUFFS             10
#define SMOKE_PERIOD_MS         250

/* The water is darker where the steps and the battery are shown, from this row down */
#define SHADE_Y                 174
#define SHADE_COLOR             0x060C18
#define LAMP_COLOR              0xFFD98A

/* Rows a cloud reaches above and below its base line */
#define CLOUD_ABOVE             32
#define CLOUD_BELOW             12
/* The fog lies in this many rows above the lake */
#define FOG_ROWS                70
/* The ripples push a row of the lake at most this many pixels sideways */
#define MAX_RIPPLE              3
/* Rows that are rendered at a time: their colors are worked out first */
#define CHUNK_ROWS              24

#define NUM_STARS               44
#define MAX_CLOUDS              5
#define MAX_PARTICLES           40
#define RAIN_LENGTH             9
#define RAIN_SPEED              260     /* Pixels per second */
#define SNOW_SPEED              26
#define CLOUD_SPEED             300     /* 1/256 pixels per second */
#define RIPPLE_PERIOD_MS        170
#define TWINKLE_PERIOD_MS       350
#define TWINKLE_STARS           3

/* How far the layers move for a full tilt, and how far they sink before they rise */
static const int8_t parallax_x[NUM_LAYERS] = { 2, 5, 9, 14 };
static const int8_t parallax_y[NUM_LAYERS] = { 0, 1, 2, 4 };
static const uint8_t rise_drop[NUM_LAYERS] = { 26, 38, 50, 70 };
/* How much of the haze the foot of each layer takes on, of 256 */
static const uint8_t layer_mist[NUM_LAYERS] = { 184, 148, 92, 36 };
/* The heath and the shore are browner than the forest: a color and how much of it, of 256 */
static const uint32_t layer_tone[NUM_LAYERS] = { 0, 0, 0x8A6A3A, 0x5A4630 };
static const uint8_t layer_tone_mix[NUM_LAYERS] = { 0, 0, 56, 51 };

typedef struct {
    uint32_t sky[3];            /* Top, middle and horizon */
    uint32_t layer[NUM_LAYERS];
    uint32_t haze;
    uint32_t snow;
    uint32_t cloud_light;
    uint32_t cloud_shade;
    uint32_t lake;
    uint32_t sun;
    uint8_t glow;               /* Strength of the light around the sun */
    uint8_t glow_radius;
    uint8_t stars;
} palette_t;

#define NUM_KEYS                6

/* The palette is known at these heights of the sun (tenths of a degree) and blended between */
static const int16_t key_elevation[NUM_KEYS] = { -180, -80, -30, 8, 70, 260 };

static const palette_t keys[NUM_KEYS] = {
    {
        /* Night */
        .sky = { 0x040716, 0x0A1130, 0x18224E },
        .layer = { 0x1A2446, 0x121A38, 0x0B1128, 0x060A18 },
        .haze = 0x25325E, .snow = 0x56658F,
        .cloud_light = 0x2A3559, .cloud_shade = 0x1A2242,
        .lake = 0x070C1E, .sun = 0xFF6A3A,
        .glow = 0, .glow_radius = 80, .stars = 255,
    },
    {
        /* The first or last light */
        .sky = { 0x090E2C, 0x20275A, 0x5C4676 },
        .layer = { 0x2B2D5B, 0x1D2046, 0x121430, 0x09091C },
        .haze = 0x57477A, .snow = 0x7C7AAA,
        .cloud_light = 0x4A4274, .cloud_shade = 0x2A2850,
        .lake = 0x0B0E26, .sun = 0xFF6A3A,
        .glow = 110, .glow_radius = 96, .stars = 170,
    },
    {
        /* Dawn and dusk */
        .sky = { 0x1A2A60, 0x6B4F8E, 0xF28C5A },
        .layer = { 0x4B4476, 0x33305C, 0x1F1D40, 0x100F26 },
        .haze = 0xB8718C, .snow = 0xCDA2BA,
        .cloud_light = 0xF09A8A, .cloud_shade = 0x8A5A88,
        .lake = 0x161638, .sun = 0xFF7440,
        .glow = 190, .glow_radius = 104, .stars = 40,
    },
    {
        /* The sun on the horizon */
        .sky = { 0x3A5FA8, 0xE28C90, 0xFFC26C },
        .layer = { 0x7C6190, 0x56456F, 0x33294A, 0x1A1530 },
        .haze = 0xF2A48A, .snow = 0xFFDCCB,
        .cloud_light = 0xFFD0B0, .cloud_shade = 0xD88A90,
        .lake = 0x23284E, .sun = 0xFFA850,
        .glow = 215, .glow_radius = 92, .stars = 0,
    },
    {
        /* The golden hour */
        .sky = { 0x4684D8, 0x9AC0EA, 0xFFE2AE },
        .layer = { 0x8192BA, 0x5D6F96, 0x35526A, 0x1D3340 },
        .haze = 0xEADCCC, .snow = 0xFFF5E8,
        .cloud_light = 0xFFF4E4, .cloud_shade = 0xE0C4B8,
        .lake = 0x23466E, .sun = 0xFFDE96,
        .glow = 160, .glow_radius = 70, .stars = 0,
    },
    {
        /* Day */
        .sky = { 0x2C7CE0, 0x6CB0F2, 0xBEE2FA },
        .layer = { 0x80A4CE, 0x5B86AA, 0x2E6C5A, 0x1B4A3C },
        .haze = 0xD0E7F6, .snow = 0xFFFFFF,
        .cloud_light = 0xFFFFFF, .cloud_shade = 0xD3E2F2,
        .lake = 0x1C5290, .sun = 0xFFF8E0,
        .glow = 120, .glow_radius = 46, .stars = 0,
    },
};

static const uint8_t bayer[4][4] = {
    { 0, 8, 2, 10 },
    { 12, 4, 14, 6 },
    { 3, 11, 1, 9 },
    { 15, 7, 13, 5 },
};

/* One period of a sine, -127 to 127 */
static const int8_t wave[32] = {
    0, 25, 49, 71, 90, 106, 117, 125, 127, 125, 117, 106, 90, 71, 49, 25,
    0, -25, -49, -71, -90, -106, -117, -125, -127, -125, -117, -106, -90, -71, -49, -25,
};

/* How a curtain of the northern lights fades on the way up, of 255 */
static const uint8_t aurora_fade[AURORA_ROWS + 1] = {
    255, 245, 235, 225, 215, 206, 197, 187, 178, 170, 161, 152, 144, 136, 128, 120, 113, 105, 98, 91, 84,
    77, 71, 65, 59, 53, 48, 42, 37, 32, 28, 23, 19, 16, 12, 9, 6, 4, 2, 1, 0,
};

/* Brightness of a star at each twinkle level, of 32 */
static const uint8_t star_levels[4] = { 10, 18, 25, 32 };

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t level;
} star_t;

typedef struct {
    int32_t x;                  /* 1/256 pixels */
    int16_t y;
    uint8_t shape;
} cloud_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t sway;
} particle_t;

/* What a column of the land is drawn with: the layers, and the snow on the farthest one */
#define COLOR_SNOW              NUM_LAYERS
#define NUM_COLORS              (NUM_LAYERS + 1)
/* The farthest layer shows as up to four spans: snow, rock, a field of snow and rock again */
#define MAX_SPANS               (NUM_LAYERS + 3)

/*
 * What stands in a column of the display: the rows where something new comes into view, from
 * the top, and what it is drawn with (three bits each). The last row is never reached.
 */
typedef struct {
    uint8_t y[MAX_SPANS + 1];
    uint32_t colors;
} column_t;

static struct {
    diorama_view_t view;
    bool view_valid;
    palette_t pal;
    uint16_t sun565;
    uint16_t moon565;
    uint16_t lake565;
    uint16_t glint565;
    uint16_t haze565;
    uint16_t cloud_light565;
    uint16_t cloud_shade565;
    uint16_t particle565;
    uint16_t smoke565;
    uint16_t sprite565[DIORAMA_NUM_SPRITE_COLORS];
    /* Of 32. A color that is not shown now has 0. */
    uint8_t sprite_alpha[DIORAMA_NUM_SPRITE_COLORS];
    /* How much daylight there is, of 256, for the things with a color of their own */
    uint16_t light;
    /* Rows of the middle of the sun where it comes up and where it goes down */
    int16_t rise_y;
    int16_t set_y;
    int16_t sun_x;
    int16_t sun_y;
    int16_t moon_x;
    int16_t moon_y;
    uint8_t glow;
    bool sun_disc;
    bool moon_up;
    uint8_t moon_alpha;
    uint8_t star_alpha;
    uint8_t fog;                /* Of 32, at the shore */
    int8_t off_x[NUM_LAYERS];
    int16_t off_y[NUM_LAYERS];
    column_t columns[SIZE];
    /* Top edge of the nearest layer: what stands further back is behind it */
    uint8_t near_top[SIZE];
    /* First row with land in it, and first row without sky */
    uint8_t land_top;
    uint8_t sky_bottom;
    uint8_t sun_falloff[GLOW_STEPS + 1];
    uint8_t moon_falloff[GLOW_STEPS + 1];
    uint8_t sun_reach;
    uint8_t moon_reach;
    star_t stars[NUM_STARS];
    cloud_t clouds[MAX_CLOUDS];
    uint8_t num_clouds;
    particle_t particles[MAX_PARTICLES];
    uint8_t num_particles;
    /* Strength of the northern lights, and the foot and strength of each curtain in every column */
    uint8_t aurora;
    uint8_t aurora_base[AURORA_BANDS][SIZE];
    uint8_t aurora_alpha[AURORA_BANDS][SIZE];
    uint16_t aurora565[AURORA_ROWS + 1];
    /* The rows each curtain is in */
    int16_t aurora_first[AURORA_BANDS];
    int16_t aurora_last[AURORA_BANDS];
    uint32_t aurora_step;
    uint32_t smoke;
    uint32_t time_ms;
    uint32_t ripple;
    uint32_t twinkle;
    uint32_t rng;
} s;

static uint32_t rnd(void)
{
    s.rng = s.rng * 1664525u + 1013904223u;

    return s.rng >> 8;
}

static int rnd_range(int lo, int hi)
{
    return lo + (int)(rnd() % (uint32_t)(hi - lo + 1));
}

static inline int clamp(int v, int lo, int hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

/* a with t of 256 parts of b */
static uint32_t mix(uint32_t a, uint32_t b, int t)
{
    uint32_t rb = (((a & 0xFF00FF) * (256 - t) + (b & 0xFF00FF) * t) >> 8) & 0xFF00FF;
    uint32_t g = (((a & 0x00FF00) * (256 - t) + (b & 0x00FF00) * t) >> 8) & 0x00FF00;

    return rb | g;
}

static inline uint16_t to565(uint32_t c)
{
    return (uint16_t)(((c >> 8) & 0xF800) | ((c >> 5) & 0x07E0) | ((c >> 3) & 0x001F));
}

/* bg with a of 32 parts of fg */
static inline uint16_t blend565(uint16_t fg, uint16_t bg, int a)
{
    uint32_t f = (fg | ((uint32_t)fg << 16)) & 0x07E0F81F;
    uint32_t b = (bg | ((uint32_t)bg << 16)) & 0x07E0F81F;
    uint32_t r = (b + (((f - b) * a) >> 5)) & 0x07E0F81F;

    return (uint16_t)(r | (r >> 16));
}

/* bg with a of 256 parts of fg. What a pixel cannot hold is rounded up when d, 0 to 255, is high. */
static inline uint16_t blend565_fine(uint16_t fg, uint16_t bg, unsigned int a, unsigned int d)
{
    /* Blue and red apart, so that each has room for the 8 bits of a */
    uint32_t f = (fg & 0x001Fu) | ((uint32_t)(fg & 0xF800u) << 5);
    uint32_t b = (bg & 0x001Fu) | ((uint32_t)(bg & 0xF800u) << 5);
    uint32_t rb = ((f * a + b * (256 - a) + (d | (d << 16))) >> 8) & 0x001F001Fu;
    uint32_t g = (((fg >> 5) & 0x3Fu) * a + ((bg >> 5) & 0x3Fu) * (256 - a) + d) >> 8;

    return (uint16_t)((rb & 0x1F) | ((rb >> 5) & 0xF800) | (g << 5));
}

/* The sine of phase, where 65536 is a full turn: -127 to 127 */
static int isin(uint32_t phase)
{
    unsigned int i = (phase >> 11) & 31;
    int frac = (int)(phase & 2047);

    return wave[i] + (wave[(i + 1) & 31] - wave[i]) * frac / 2048;
}

/*
 * The color c as the four pixels that repeat along row y. The gradients are too smooth for
 * the 5 and 6 bits of a pixel, an ordered dither hides the steps.
 */
static void dither_row(uint32_t c, int y, uint16_t out[4])
{
    unsigned int r = (c >> 16) & 0xFF;
    unsigned int g = (c >> 8) & 0xFF;
    unsigned int b = c & 0xFF;
    /* A channel that is already at its highest stays there */
    unsigned int r_low = r >= 0xF8 ? 0 : r & 7;
    unsigned int g_low = g >= 0xFC ? 0 : g & 3;
    unsigned int b_low = b >= 0xF8 ? 0 : b & 7;
    uint16_t base = (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));

    for (int i = 0; i < 4; i++) {
        unsigned int d = bayer[y & 3][i];

        /* What a pixel cannot hold decides how many of the pixels are one step brighter */
        out[i] = (uint16_t)(base + (r_low + (d >> 1) > 7 ? 0x0800 : 0) + (g_low + (d >> 2) > 3 ? 0x0020 : 0) +
                            (b_low + (d >> 1) > 7 ? 0x0001 : 0));
    }
}

static int isqrt(int v)
{
    int r = 0;

    while ((r + 1) * (r + 1) <= v) {
        r++;
    }

    return r;
}

/* A color without its hue, slightly blue: what is left of it under a covered sky */
static uint32_t overcast(uint32_t c, int amount)
{
    int l = (((c >> 16) & 0xFF) * 77 + ((c >> 8) & 0xFF) * 150 + (c & 0xFF) * 29) >> 8;
    uint32_t grey = ((uint32_t)(l * 200 / 256) << 16) | ((uint32_t)(l * 215 / 256) << 8) | (uint32_t)(l * 236 / 256);

    return mix(c, grey, amount);
}

static void blend_palette(int elevation, palette_t *pal)
{
    int k = 0;
    int t;

    if (elevation <= key_elevation[0]) {
        *pal = keys[0];
        return;
    }
    if (elevation >= key_elevation[NUM_KEYS - 1]) {
        *pal = keys[NUM_KEYS - 1];
        return;
    }
    while (elevation > key_elevation[k + 1]) {
        k++;
    }
    t = (elevation - key_elevation[k]) * 256 / (key_elevation[k + 1] - key_elevation[k]);

    const palette_t *a = &keys[k];
    const palette_t *b = &keys[k + 1];

    for (int i = 0; i < 3; i++) {
        pal->sky[i] = mix(a->sky[i], b->sky[i], t);
    }
    for (int i = 0; i < NUM_LAYERS; i++) {
        pal->layer[i] = mix(a->layer[i], b->layer[i], t);
    }
    pal->haze = mix(a->haze, b->haze, t);
    pal->snow = mix(a->snow, b->snow, t);
    pal->cloud_light = mix(a->cloud_light, b->cloud_light, t);
    pal->cloud_shade = mix(a->cloud_shade, b->cloud_shade, t);
    pal->lake = mix(a->lake, b->lake, t);
    pal->sun = mix(a->sun, b->sun, t);
    pal->glow = (uint8_t)(a->glow + (b->glow - a->glow) * t / 256);
    pal->glow_radius = (uint8_t)(a->glow_radius + (b->glow_radius - a->glow_radius) * t / 256);
    pal->stars = (uint8_t)(a->stars + (b->stars - a->stars) * t / 256);
}

/*
 * A body comes up over the skyline at the left, stands highest at peak_y in the middle of its
 * arc and goes down behind the skyline at the right.
 */
static void orbit_position(int arc, int peak_y, int16_t *x, int16_t *y, bool *up)
{
    int p = clamp(arc, 0, 256);
    int out = arc < p ? p - arc : arc - p;
    int lift = (s.rise_y + s.set_y) / 2 - peak_y;
    int sink = out * ORBIT_SINK / 256;

    if (lift < ORBIT_MIN_LIFT) {
        lift = ORBIT_MIN_LIFT;
    }
    *x = (int16_t)(ORBIT_X1 + p * (ORBIT_X2 - ORBIT_X1) / 256);
    *y = (int16_t)(s.rise_y + (s.set_y - s.rise_y) * p / 256 - isin((uint32_t)p * 128) * lift / 127 +
                   (sink < ORBIT_MAX_SINK ? sink : ORBIT_MAX_SINK));
    *up = arc != DIORAMA_ARC_NONE && out < ORBIT_GONE;
}

/* Row of the skyline in a column of the display, with the watch held flat */
static int skyline(int x)
{
    int top = LAKE_Y;

    for (int l = 0; l < NUM_LAYERS; l++) {
        if (diorama_layer_top[l][MARGIN + x] < top) {
            top = diorama_layer_top[l][MARGIN + x];
        }
    }

    return top;
}

/* A color of its own in the light of the moment: darker at night, and a little of the haze */
static uint32_t lit(uint32_t c)
{
    uint32_t dim = ((((c & 0xFF00FF) * s.light) >> 8) & 0xFF00FF) | ((((c & 0x00FF00) * s.light) >> 8) & 0x00FF00);

    return mix(dim, s.pal.haze, 31 + 26 * (256 - s.light) / 256);
}

/* The colors of what stands in the landscape. The palette holds the layers without their tone. */
static void sprite_colors(int cover)
{
    bool dark = s.view.sun_elevation < 20;
    uint32_t crown = mix(s.pal.layer[2], lit(0xA8CC6E), 141);
    uint32_t grass = mix(s.pal.layer[2], lit(0x7FA85A), 115);

    for (int i = 0; i < DIORAMA_NUM_SPRITE_COLORS; i++) {
        const diorama_sprite_color_t *entry = &diorama_sprite_colors[i];
        uint32_t color = 0;
        int alpha = 32;

        switch (entry->kind) {
            case DIORAMA_COLOR_WINDOW:
                color = dark ? LAMP_COLOR : overcast(lit(entry->rgb), cover);
                break;
            case DIORAMA_COLOR_CROWN:
                color = crown;
                break;
            case DIORAMA_COLOR_CROWN_SHADE:
                color = mix(crown, s.pal.layer[2], 115);
                break;
            case DIORAMA_COLOR_GRASS:
                color = mix(grass, s.pal.layer[3], entry->param * 256 / 12);
                break;
            case DIORAMA_COLOR_STONE:
                color = mix(s.pal.layer[3], lit(0xC9BFA8), entry->param);
                break;
            case DIORAMA_COLOR_LAMP_GLOW:
                color = LAMP_COLOR;
                alpha = dark ? entry->param : 0;
                break;
            default:
                color = overcast(lit(entry->rgb), cover);
                break;
        }
        s.sprite565[i] = to565(color);
        s.sprite_alpha[i] = (uint8_t)alpha;
    }
    s.smoke565 = to565(mix(s.pal.haze, 0xFFFFFF, 102));
}

/* Where the curtains of the northern lights hang now, and how strong they are in each column */
static void build_aurora(void)
{
    uint32_t t = s.aurora_step * AURORA_PERIOD_MS;
    uint32_t sway = (uint32_t)((uint64_t)t * 2608 / 1000);
    uint32_t wander = (uint32_t)((uint64_t)t * 4172 / 1000);
    uint32_t pulse = (uint32_t)((uint64_t)t * 6258 / 1000);

    for (int band = 0; band < AURORA_BANDS; band++) {
        int highest = SIZE;
        int lowest = 0;

        for (int x = 0; x < SIZE; x++) {
            int base = 52 + band * 26 + (13 * isin((uint32_t)x * 313 + sway + (uint32_t)band * 20861) +
                                         6 * isin((uint32_t)x * 887 - wander)) / 127;
            int strength = 115 + 141 * isin((uint32_t)x * 522 + pulse + (uint32_t)band * 10430) / 127;
            /* Rays: every other column has a brightness of its own */
            uint32_t hash = ((uint32_t)(x >> 1) * 2654435761u) ^ ((uint32_t)band * 0x9E3779B9u);
            int rays = 154 + (int)(hash >> 24) * 102 / 256;

            if (strength < 0) {
                strength = 0;
            }
            if (band > 0) {
                strength = strength * 141 / 256;
            }
            s.aurora_base[band][x] = (uint8_t)base;
            s.aurora_alpha[band][x] = (uint8_t)(((strength * rays) >> 8) * s.aurora >> 9);
            highest = base < highest ? base : highest;
            lowest = base > lowest ? base : lowest;
        }
        s.aurora_first[band] = (int16_t)(highest - AURORA_ROWS);
        s.aurora_last[band] = (int16_t)(lowest + AURORA_FOOT - 1);
    }
}

/*
 * The light is strongest in the middle and gone at the edge.
 *
 * @return the number of steps from the middle that the light can be seen at.
 */
static uint8_t build_falloff(uint8_t *falloff, int strength)
{
    int reach = 0;

    for (int i = 0; i <= GLOW_STEPS; i++) {
        int t = 255 - i * 255 / GLOW_STEPS;
        int a = ((t * t) >> 8) * strength >> 8;

        falloff[i] = (uint8_t)(a > 255 ? 255 : a);
        if (a >= GLOW_MIN_VISIBLE) {
            reach = i + 1;
        }
    }

    return (uint8_t)reach;
}

/* Works out, for every column, what is in front at which row */
static void build_columns(void)
{
    int land_top = SIZE - 1;
    int sky_bottom = 0;

    for (int x = 0; x < SIZE; x++) {
        column_t *column = &s.columns[x];
        uint8_t rows[NUM_LAYERS];
        uint8_t layers[NUM_LAYERS];
        int n = 0;
        int k = 0;
        int front = 255;

        /* From the nearest: a layer shows where it reaches above everything in front of it */
        for (int l = NUM_LAYERS - 1; l >= 0; l--) {
            int top = clamp(diorama_layer_top[l][MARGIN + s.off_x[l] + x] + s.off_y[l], 0, 254);

            if (l == NUM_LAYERS - 1) {
                s.near_top[x] = (uint8_t)top;
            }
            if (top < front) {
                rows[n] = (uint8_t)top;
                layers[n] = (uint8_t)l;
                front = top;
                n++;
            }
        }
        column->colors = 0;
        for (int i = n - 1; i >= 0; i--) {
            if (layers[i] == 0) {
                /* The farthest layer has snow on its top edge and in a field further down */
                int below = i > 0 ? rows[i - 1] : 255;
                int column_x = MARGIN + s.off_x[0] + x;
                int cap_end = rows[i] + diorama_snow[column_x];
                int field = rows[i] + diorama_snow_field[column_x];
                int field_end = field + diorama_snow_field_rows[column_x];

                if (cap_end > rows[i]) {
                    column->y[k] = rows[i];
                    column->colors |= (uint32_t)COLOR_SNOW << (k * 3);
                    k++;
                    if (cap_end >= below) {
                        continue;
                    }
                    rows[i] = (uint8_t)cap_end;
                }
                if (field_end > field && field < below) {
                    /* Rock, then the field, and rock again below it if it shows */
                    column->y[k] = rows[i];
                    k++;
                    column->y[k] = (uint8_t)field;
                    column->colors |= (uint32_t)COLOR_SNOW << (k * 3);
                    k++;
                    if (field_end >= below) {
                        continue;
                    }
                    rows[i] = (uint8_t)field_end;
                }
            }
            column->y[k] = rows[i];
            column->colors |= (uint32_t)layers[i] << (k * 3);
            k++;
        }
        for (; k <= MAX_SPANS; k++) {
            column->y[k] = 255;
        }
        land_top = front < land_top ? front : land_top;
        sky_bottom = front > sky_bottom ? front : sky_bottom;
    }
    s.land_top = (uint8_t)land_top;
    s.sky_bottom = (uint8_t)(sky_bottom < LAKE_Y ? sky_bottom : LAKE_Y);
}

static void particle_spawn(particle_t *p, bool anywhere)
{
    p->x = (int16_t)rnd_range(-20, SIZE + 20);
    p->y = (int16_t)(anywhere ? rnd_range(-RAIN_LENGTH, SIZE) : -RAIN_LENGTH - rnd_range(0, 30));
    p->sway = (uint8_t)rnd_range(0, 31);
}

static void setup_weather(const diorama_view_t *view)
{
    static const uint8_t cloud_rows[MAX_CLOUDS] = { 92, 80, 98, 72, 86 };
    int clouds = 0;
    int particles = 0;
    int intensity = clamp(view->intensity, 1, 3);

    switch (view->weather) {
        case DIORAMA_WEATHER_CLOUDS:
            clouds = intensity + 1 + (intensity == 3);
            break;
        case DIORAMA_WEATHER_RAIN:
            clouds = 4;
            particles = 10 + intensity * 10;
            break;
        case DIORAMA_WEATHER_SNOW:
            clouds = 4;
            particles = 10 + intensity * 10;
            break;
        case DIORAMA_WEATHER_FOG:
            clouds = 2;
            break;
        default:
            break;
    }

    s.num_clouds = (uint8_t)clouds;
    for (int i = 0; i < clouds; i++) {
        s.clouds[i].shape = (uint8_t)(i % DIORAMA_NUM_CLOUD_SHAPES);
        s.clouds[i].x = (i * SIZE / clouds + rnd_range(-20, 20)) * 256;
        s.clouds[i].y = cloud_rows[i];
    }
    s.num_particles = (uint8_t)particles;
    for (int i = 0; i < particles; i++) {
        particle_spawn(&s.particles[i], true);
    }
}

void diorama_scene_init(uint32_t seed)
{
    memset(&s, 0, sizeof(s));
    s.rng = seed | 1;

    for (int i = 0; i < NUM_STARS; i++) {
        int x;
        int y;

        /* Only where the round display shows them, and above the mountains */
        do {
            x = rnd_range(4, SIZE - 5);
            y = rnd_range(4, 128);
        } while ((x - SIZE / 2) * (x - SIZE / 2) + (y - SIZE / 2) * (y - SIZE / 2) > 116 * 116);
        s.stars[i].x = (uint8_t)x;
        s.stars[i].y = (uint8_t)y;
        s.stars[i].level = (uint8_t)rnd_range(0, 3);
    }

    /* The upper edge of the sun shows over the skyline when it rises */
    s.rise_y = (int16_t)(skyline(ORBIT_X1) + SUN_RADIUS);
    s.set_y = (int16_t)(skyline(ORBIT_X2) + SUN_RADIUS);

    /* Green at the foot of a curtain of the northern lights, violet at its top */
    for (int i = 0; i <= AURORA_ROWS; i++) {
        s.aurora565[i] = to565(mix(0x46FF96, 0x965AFF, clamp(i * 256 * 13 / (AURORA_ROWS * 10), 0, 256)));
    }
}

bool diorama_scene_set_view(const diorama_view_t *view, int *first_row)
{
    int cover = 0;
    int fog = 0;
    int8_t off_x[NUM_LAYERS];
    int16_t off_y[NUM_LAYERS];
    bool moved = false;
    bool sky_same;
    bool sun_up;
    int light;
    int land_top = s.land_top;

    for (int i = 0; i < NUM_LAYERS; i++) {
        /* The nearer layers come up later */
        int rise = clamp((view->rise - i * 24) * 255 / (255 - (NUM_LAYERS - 1) * 24), 0, 255);

        off_x[i] = (int8_t)(view->tilt_x * parallax_x[i] / 127);
        off_y[i] = (int16_t)(view->tilt_y * parallax_y[i] / 127 + (255 - rise) * rise_drop[i] / 255);
        moved |= off_x[i] != s.off_x[i] || off_y[i] != s.off_y[i];
    }
    sky_same = s.view_valid && view->sun_elevation == s.view.sun_elevation && view->sun_arc == s.view.sun_arc &&
               view->sun_height == s.view.sun_height && view->moon_arc == s.view.moon_arc &&
               view->moon_lit == s.view.moon_lit && view->moon_waxing == s.view.moon_waxing &&
               view->weather == s.view.weather && view->intensity == s.view.intensity;
    /* A tilt too small to move a layer by a pixel changes nothing */
    if (sky_same && !moved) {
        s.view = *view;
        return false;
    }
    if (!s.view_valid || view->weather != s.view.weather || view->intensity != s.view.intensity) {
        setup_weather(view);
    }
    s.view = *view;
    s.view_valid = true;

    blend_palette(view->sun_elevation, &s.pal);

    /* How much the weather takes away of the colors and of the sun */
    switch (view->weather) {
        case DIORAMA_WEATHER_CLOUDS:
            cover = view->intensity >= 3 ? 150 : (view->intensity == 2 ? 60 : 0);
            break;
        case DIORAMA_WEATHER_RAIN:
        case DIORAMA_WEATHER_SNOW:
            cover = 190;
            break;
        case DIORAMA_WEATHER_FOG:
            cover = 150;
            fog = 22;
            break;
        default:
            break;
    }
    if (cover > 0) {
        for (int i = 0; i < 3; i++) {
            s.pal.sky[i] = overcast(s.pal.sky[i], cover);
        }
        for (int i = 0; i < NUM_LAYERS; i++) {
            s.pal.layer[i] = overcast(s.pal.layer[i], cover * 2 / 3);
        }
        s.pal.haze = overcast(s.pal.haze, cover);
        s.pal.lake = overcast(s.pal.lake, cover * 2 / 3);
        s.pal.cloud_light = mix(overcast(s.pal.cloud_light, cover), s.pal.sky[0], cover / 3);
        s.pal.cloud_shade = mix(overcast(s.pal.cloud_shade, cover), s.pal.sky[0], cover / 2);
    }
    light = clamp((view->sun_elevation + 100) * 256 / 250, 0, 256);
    s.light = (uint16_t)(51 + 205 * ((light * light * (768 - 2 * light)) >> 16) / 256);
    sprite_colors(cover * 2 / 3);
    for (int i = 0; i < NUM_LAYERS; i++) {
        if (layer_tone_mix[i]) {
            s.pal.layer[i] = mix(s.pal.layer[i], lit(layer_tone[i]), layer_tone_mix[i]);
        }
    }
    if (view->weather == DIORAMA_WEATHER_SNOW) {
        /* Snow on the ground too */
        for (int i = 0; i < NUM_LAYERS; i++) {
            s.pal.layer[i] = mix(s.pal.layer[i], s.pal.snow, 70 - i * 12);
        }
    }
    s.fog = (uint8_t)fog;
    s.glow = (uint8_t)(s.pal.glow * (256 - cover) / 256);
    s.star_alpha = (uint8_t)(s.pal.stars * (256 - cover) / 256);
    s.moon_alpha = (uint8_t)((14 + s.pal.stars * 18 / 255) * (256 - cover * 2 / 3) / 256);
    s.aurora = (uint8_t)(s.star_alpha > AURORA_MIN_STARS ?
                         (s.star_alpha - AURORA_MIN_STARS) * 255 / (255 - AURORA_MIN_STARS) : 0);

    s.sun565 = to565(s.pal.sun);
    s.moon565 = to565(0xE6ECFF);
    s.lake565 = to565(s.pal.lake);
    s.glint565 = to565(mix(s.pal.sky[2], 0xFFFFFF, 90));
    s.haze565 = to565(s.pal.haze);
    s.cloud_light565 = to565(s.pal.cloud_light);
    s.cloud_shade565 = to565(s.pal.cloud_shade);
    s.particle565 = to565(view->weather == DIORAMA_WEATHER_SNOW ? 0xFFFFFF : mix(s.pal.sky[2], 0xFFFFFF, 130));

    orbit_position(view->sun_arc, SUN_PEAK_LOW_Y - (SUN_PEAK_LOW_Y - SUN_PEAK_HIGH_Y) * view->sun_height / 255, &s.sun_x,
                   &s.sun_y, &sun_up);
    orbit_position(view->moon_arc, MOON_PEAK_Y, &s.moon_x, &s.moon_y, &s.moon_up);
    s.sun_disc = sun_up && cover < 140;

    memcpy(s.off_x, off_x, sizeof(s.off_x));
    memcpy(s.off_y, off_y, sizeof(s.off_y));
    build_columns();
    s.sun_reach = build_falloff(s.sun_falloff, s.glow);
    s.moon_reach = build_falloff(s.moon_falloff, s.moon_alpha * view->moon_lit / 64);
    if (s.aurora > 0) {
        build_aurora();
    }
    /* When only the land moved, the sky above it is as it was */
    *first_row = sky_same ? (land_top < s.land_top ? land_top : s.land_top) : 0;

    return true;
}

/* The pixels pattern[0..3] repeated from column x on */
static void fill_pattern(uint16_t *dst, int x, int n, const uint16_t pattern[4])
{
    uint16_t a = pattern[x & 3];
    uint16_t b = pattern[(x + 1) & 3];
    uint16_t c = pattern[(x + 2) & 3];
    uint16_t d = pattern[(x + 3) & 3];

    while (n >= 4) {
        dst[0] = a;
        dst[1] = b;
        dst[2] = c;
        dst[3] = d;
        dst += 4;
        n -= 4;
    }
    if (n > 0) {
        dst[0] = a;
    }
    if (n > 1) {
        dst[1] = b;
    }
    if (n > 2) {
        dst[2] = c;
    }
}

/* A row of the display, and the row of the scene that it shows */
typedef struct {
    uint16_t *dst;              /* The pixel of column x1 */
    int16_t y;
    /* The row of the scene. On the lake that is a row of the land, upside down. */
    int16_t src_y;
    /* Column x of the display shows column x + shift of the scene: the ripples of the lake */
    int8_t shift;
    /* Parts of 256 of the color of the lake that the row takes on */
    uint8_t tint;
    /* Parts of 256 that the water is darkened by, behind what is shown on it */
    uint8_t shade;
} row_t;

/* A color of the scene as it shows in a row: the water takes on the color of the lake */
static uint32_t row_color(uint32_t c, const row_t *row)
{
    if (row->tint) {
        c = mix(c, s.pal.lake, row->tint);
        if (row->shade) {
            c = mix(c, SHADE_COLOR, row->shade);
        }
    }

    return c;
}

static inline uint16_t tinted(uint16_t color, const row_t *row)
{
    if (!row->tint) {
        return color;
    }
    color = blend565(s.lake565, color, row->tint >> 3);

    return row->shade ? blend565(0x0063 /* SHADE_COLOR */, color, row->shade >> 3) : color;
}

/*
 * Light around a point of the sky, and optionally a disc in its middle. Only drawn where the
 * sky shows: half of the light of a low sun is behind the mountains.
 */
static void draw_glow(const row_t *row, int x1, int x2, int cx, int cy, int radius, const uint8_t *falloff, int reach,
                      int disc_radius, uint16_t color)
{
    int dy = row->src_y - cy;
    int r2 = radius * radius;
    /* The edge of the disc fades over two pixels */
    int disc2 = disc_radius > 0 ? (disc_radius - 1) * (disc_radius - 1) : 0;
    int soft2 = disc_radius > 0 ? (disc_radius + 1) * (disc_radius + 1) : 0;
    uint32_t inv = ((uint32_t)GLOW_STEPS << 16) / (uint32_t)r2;
    const uint8_t *dither = bayer[row->y & 3];
    const column_t *column;
    const column_t *column_last;
    int d2;
    int half;
    int from;
    int to;

    /* Only as far out as the light can be seen */
    half = r2 * reach / GLOW_STEPS - dy * dy;
    if (half <= 0) {
        return;
    }
    half = isqrt(half);
    color = tinted(color, row);
    cx -= row->shift;
    from = cx - half > x1 ? cx - half : x1;
    to = cx + half < x2 ? cx + half : x2;
    if (from > to) {
        return;
    }
    column = &s.columns[clamp(from + row->shift, 0, SIZE - 1)];
    column_last = &s.columns[SIZE - 1];
    d2 = (from - cx) * (from - cx) + dy * dy;
    for (int x = from; x <= to; x++) {
        int dx = x - cx;
        int a;
        int here = d2;
        bool sky = row->src_y < column->y[0];

        d2 += 2 * dx + 1;
        if (x + row->shift >= 0 && column < column_last) {
            column++;
        }
        if (!sky) {
            continue;
        }
        if (here < disc2) {
            row->dst[x - x1] = color;
            continue;
        }
        a = falloff[((uint32_t)here * inv) >> 16];
        if (here < soft2) {
            int edge = 256 * (soft2 - here) / (soft2 - disc2);

            a = a > edge ? a : edge;
        }
        if (a > 0) {
            /* The light fades too slowly for the steps of a pixel: the dither hides them */
            row->dst[x - x1] = blend565_fine(color, row->dst[x - x1], (unsigned int)a, dither[x & 3] * 16u + 8u);
        }
    }
}

static void draw_moon(const row_t *row, int x1, int x2)
{
    int dy = row->src_y - s.moon_y;
    uint16_t color = tinted(s.moon565, row);
    int half;
    int edge;

    if (dy < -MOON_RADIUS || dy > MOON_RADIUS) {
        return;
    }
    half = isqrt(MOON_RADIUS * MOON_RADIUS - dy * dy);
    /* The edge of the shadow is half an ellipse: at the far edge for a new moon */
    edge = half * (255 - 2 * s.view.moon_lit) / 255;
    for (int dx = -half; dx <= half; dx++) {
        int x = s.moon_x + dx - row->shift;
        bool lit = s.view.moon_waxing ? dx >= edge : dx <= -edge;

        if (x >= x1 && x <= x2) {
            row->dst[x - x1] = blend565(color, row->dst[x - x1], lit ? s.moon_alpha : s.moon_alpha / 7);
        }
    }
}

static void draw_stars(const row_t *row, int x1, int x2)
{
    uint16_t color = tinted(0xFFFF, row);

    for (int i = 0; i < NUM_STARS; i++) {
        const star_t *star = &s.stars[i];
        int x = star->x - row->shift;

        if (star->y == row->src_y && x >= x1 && x <= x2) {
            row->dst[x - x1] = blend565(color, row->dst[x - x1], star_levels[star->level] * s.star_alpha / 255);
        }
    }
}

static void draw_aurora(const row_t *row, int x1, int x2)
{
    const uint8_t *dither = bayer[row->y & 3];
    int y = row->src_y;

    for (int band = 0; band < AURORA_BANDS; band++) {
        const uint8_t *base = s.aurora_base[band];
        const uint8_t *alpha = s.aurora_alpha[band];

        if (y > s.aurora_last[band] || y < s.aurora_first[band]) {
            continue;
        }
        for (int x = x1; x <= x2; x++) {
            int column = clamp(x + row->shift, 0, SIZE - 1);
            int up = base[column] - y;
            int a;

            if (alpha[column] == 0 || up > AURORA_ROWS || up <= -AURORA_FOOT) {
                continue;
            }
            a = (up > 0 ? aurora_fade[up] : 255 + up * 255 / AURORA_FOOT) * alpha[column] >> 8;
            if (a > 2) {
                uint16_t color = s.aurora565[up > 0 ? up : 0];

                row->dst[x - x1] = blend565_fine(row->tint ? tinted(color, row) : color, row->dst[x - x1],
                                                 (unsigned int)a, dither[x & 3] * 16u + 8u);
            }
        }
    }
}

static void draw_clouds(const row_t *row, int x1, int x2)
{
    uint16_t light = tinted(s.cloud_light565, row);
    uint16_t shade = tinted(s.cloud_shade565, row);

    for (int i = 0; i < s.num_clouds; i++) {
        const cloud_t *cloud = &s.clouds[i];
        const diorama_cloud_shape_t *shape = &diorama_cloud_shapes[cloud->shape];
        int rel = row->src_y - cloud->y;
        int left = (cloud->x >> 8) - row->shift;

        if (rel < -CLOUD_ABOVE || rel > CLOUD_BELOW || left > x2 || left + shape->width <= x1) {
            continue;
        }
        for (int col = 0; col < shape->width; col++) {
            int x = left + col;
            int top = shape->top[col];

            if (x >= x1 && x <= x2 && top > 0 && rel >= -top && rel <= shape->bottom[col]) {
                row->dst[x - x1] = rel * 10 > -3 * top ? shade : light;
            }
        }
    }
}

/* The sky of a row, with all that is in it. The land is drawn over it afterwards. */
static void render_sky_row(const row_t *row, int x1, int x2)
{
    int y = row->src_y;
    uint32_t color;
    uint16_t sky[4];

    if (y < SKY_MID_Y) {
        color = mix(s.pal.sky[0], s.pal.sky[1], clamp(y, 0, SKY_MID_Y) * 256 / SKY_MID_Y);
    } else {
        color = mix(s.pal.sky[1], s.pal.sky[2],
                    clamp(y - SKY_MID_Y, 0, SKY_HORIZON_Y - SKY_MID_Y) * 256 / (SKY_HORIZON_Y - SKY_MID_Y));
    }
    dither_row(row_color(color, row), row->y, sky);
    fill_pattern(row->dst, x1, x2 - x1 + 1, sky);

    if (s.star_alpha > 0) {
        draw_stars(row, x1, x2);
    }
    if (s.aurora > 0) {
        draw_aurora(row, x1, x2);
    }
    if (s.glow > 0) {
        draw_glow(row, x1, x2, s.sun_x, s.sun_y, s.pal.glow_radius, s.sun_falloff, s.sun_reach,
                  s.sun_disc ? SUN_RADIUS : 0, s.sun565);
    }
    if (s.moon_alpha > 0 && s.moon_up) {
        draw_glow(row, x1, x2, s.moon_x, s.moon_y, MOON_GLOW_RADIUS, s.moon_falloff, s.moon_reach, 0, s.moon565);
        draw_moon(row, x1, x2);
    }
    if (s.num_clouds > 0) {
        draw_clouds(row, x1, x2);
    }
}

/* The colors of the land in a row: every layer, and the snow, as the four pixels that repeat */
static void land_colors(const row_t *row, uint16_t colors[NUM_COLORS][4])
{
    for (int l = 0; l < NUM_LAYERS; l++) {
        int top_row = diorama_layer_min_top[l];
        int below = row->src_y - s.off_y[l] - top_row;
        int mist;

        if (below < 0) {
            /* The layer does not reach up to this row */
            continue;
        }
        /* Haze gathers at the foot of each layer, which sets it apart from the one in front */
        mist = clamp(below * 256 / (LAKE_Y - top_row), 0, 256) * layer_mist[l] >> 8;
        if (s.fog > 0 && row->tint > 0 && row->src_y > LAKE_Y - FOG_ROWS) {
            /* The fog on the shore shows in the water too */
            mist += (256 - mist) * s.fog * (row->src_y - (LAKE_Y - FOG_ROWS)) / (FOG_ROWS * 32);
        }
        dither_row(row_color(mix(s.pal.layer[l], s.pal.haze, mist), row), row->y, colors[l]);
        if (l == 0) {
            dither_row(row_color(mix(s.pal.snow, s.pal.haze, mist / 2), row), row->y, colors[COLOR_SNOW]);
        }
    }
}

/* Smoke from the chimney of the cottage: a few puffs that sway on the way up */
static void draw_smoke(const row_t *row, int x1, int x2)
{
    int up = DIORAMA_CHIMNEY_Y + s.off_y[DIORAMA_CHIMNEY_LAYER] - row->src_y;
    int puff = up / 2;
    int x;

    if (up < 0 || puff >= SMOKE_PUFFS || (up & 1)) {
        return;
    }
    /* Rounded down, also when the puff sways to the left */
    x = DIORAMA_CHIMNEY_X - s.off_x[DIORAMA_CHIMNEY_LAYER] - row->shift - 4 +
        (isin((uint32_t)puff * 8344 + s.smoke * 2898) * 5 / 2 + puff * 76 + 63 + 4 * 127) / 127;
    for (int i = 0; i < 2; i++) {
        if (x + i >= x1 && x + i <= x2) {
            row->dst[x + i - x1] = blend565(tinted(s.smoke565, row), row->dst[x + i - x1],
                                            i == 0 ? 18 - puff * 8 / 5 : 11 - puff);
        }
    }
}

/* What stands in the landscape, in a row of it: the birches, the cottage and its headland */
static void draw_sprites(const row_t *row, int x1, int x2)
{
    for (int g = 0; g < DIORAMA_NUM_SPRITE_GROUPS; g++) {
        const diorama_sprite_group_t *group = &diorama_sprite_groups[g];
        bool behind = group->layer < NUM_LAYERS - 1;
        int group_row;
        int offset;

        if (group->layer < 0) {
            continue;
        }
        group_row = row->src_y - s.off_y[group->layer] - group->first_row;
        if (group_row < 0 || group_row >= group->num_rows) {
            continue;
        }
        offset = MARGIN + s.off_x[group->layer] + row->shift;
        for (int i = group->rows[group_row]; i < group->rows[group_row + 1]; i++) {
            int x = (diorama_sprite_runs[i] & 0x1FF) - offset;
            int end = x + (diorama_sprite_runs[i] >> 9) - 1;
            int alpha = s.sprite_alpha[diorama_sprite_run_colors[i]];
            uint16_t color;

            x = x > x1 ? x : x1;
            end = end < x2 ? end : x2;
            if (alpha == 0 || x > end) {
                continue;
            }
            color = tinted(s.sprite565[diorama_sprite_run_colors[i]], row);
            for (; x <= end; x++) {
                if (behind && row->src_y >= s.near_top[clamp(x + row->shift, 0, SIZE - 1)]) {
                    continue;
                }
                row->dst[x - x1] = alpha >= 32 ? color : blend565(color, row->dst[x - x1], alpha);
            }
        }
    }
    draw_smoke(row, x1, x2);
}

/* What lies on the water in front of the land: the jetty and the boat */
static void draw_afloat(const row_t *row, int x1, int x2)
{
    /* Not before the land has come up */
    if (s.off_y[NUM_LAYERS - 1] > 8) {
        return;
    }
    for (int g = 0; g < DIORAMA_NUM_SPRITE_GROUPS; g++) {
        const diorama_sprite_group_t *group = &diorama_sprite_groups[g];
        int group_row = row->y - group->first_row;

        if (group->layer >= 0 || group_row < 0 || group_row >= group->num_rows) {
            continue;
        }
        for (int i = group->rows[group_row]; i < group->rows[group_row + 1]; i++) {
            /* Sideways it follows the shore it is moored at */
            int x = (diorama_sprite_runs[i] & 0x1FF) - MARGIN - s.off_x[NUM_LAYERS - 1];
            int end = x + (diorama_sprite_runs[i] >> 9) - 1;

            x = x > x1 ? x : x1;
            end = end < x2 ? end : x2;
            for (; x <= end; x++) {
                row->dst[x - x1] = s.sprite565[diorama_sprite_run_colors[i]];
            }
        }
    }
}

/*
 * The land of some rows that follow each other downwards. It is drawn a column at a time:
 * what stands in a column changes only a few times on the way down.
 */
static void render_land(const row_t *rows, int num_rows, int x1, int x2, int stride)
{
    uint16_t colors[CHUNK_ROWS][NUM_COLORS][4];
    int first = rows[0].src_y;
    int last = rows[num_rows - 1].src_y;

    if (last < s.land_top) {
        return;
    }
    for (int r = 0; r < num_rows; r++) {
        land_colors(&rows[r], colors[r]);
    }

    for (int x = x1; x <= x2; x++) {
        const column_t *column = &s.columns[x];
        int y = column->y[0] > first ? column->y[0] : first;
        unsigned int k = 0;
        unsigned int next;
        const uint16_t *color;
        uint16_t *dst;

        if (y > last) {
            continue;
        }
        while (column->y[k] <= y) {
            k++;
        }
        next = column->y[k];
        color = &colors[y - first][(column->colors >> ((k - 1) * 3)) & 7][x & 3];
        dst = (uint16_t *)((uint8_t *)rows[0].dst + (y - first) * stride) + (x - x1);
        for (; y <= last; y++) {
            if ((unsigned int)y >= next) {
                const uint16_t *row_colors = &colors[y - first][0][x & 3];

                do {
                    k++;
                } while (column->y[k] <= y);
                next = column->y[k];
                color = row_colors + ((column->colors >> ((k - 1) * 3)) & 7) * 4;
            }
            *dst = *color;
            color += NUM_COLORS * 4;
            dst = (uint16_t *)((uint8_t *)dst + stride);
        }
    }

    for (int r = 0; r < num_rows; r++) {
        draw_sprites(&rows[r], x1, x2);
    }

    if (s.fog > 0) {
        for (int r = 0; r < num_rows; r++) {
            int y = rows[r].y;
            int a = s.fog * (y - (LAKE_Y - FOG_ROWS)) / FOG_ROWS;

            for (int x = x1; x <= x2 && a > 0; x++) {
                rows[r].dst[x - x1] = blend565(s.haze565, rows[r].dst[x - x1], (a * 8 + (bayer[y & 3][x & 3] >> 1)) >> 3);
            }
        }
    }
}

/*
 * The land in some rows of the lake: upside down, so the rows of the scene go upwards, and
 * every row pushed sideways by the ripples.
 */
static void render_mirrored_land(const row_t *rows, int num_rows, int x1, int x2)
{
    uint16_t colors[CHUNK_ROWS][NUM_COLORS][4];
    unsigned int width = (unsigned int)(x2 - x1 + 1);

    if (rows[0].src_y < s.land_top) {
        return;
    }
    for (int r = 0; r < num_rows; r++) {
        land_colors(&rows[r], colors[r]);
    }

    for (int x = x1 - MAX_RIPPLE; x <= x2 + MAX_RIPPLE; x++) {
        const column_t *column = &s.columns[clamp(x, 0, SIZE - 1)];
        unsigned int k = 0;

        while (column->y[k] <= rows[0].src_y) {
            k++;
        }
        for (int r = 0; r < num_rows; r++) {
            unsigned int at = (unsigned int)(x - rows[r].shift - x1);

            while (k > 0 && column->y[k - 1] > rows[r].src_y) {
                k--;
            }
            if (k == 0) {
                /* Sky from here on up */
                break;
            }
            if (at < width) {
                rows[r].dst[at] = colors[r][(column->colors >> ((k - 1) * 3)) & 7][x & 3];
            }
        }
    }

    for (int r = 0; r < num_rows; r++) {
        draw_sprites(&rows[r], x1, x2);
    }
}

/* Short streaks of light that drift over the water */
static void draw_glint(const row_t *row, int x1, int x2)
{
    uint32_t hash = (uint32_t)row->y * 2654435761u;

    if (((hash >> 12) % 3) == 0 && row->y > LAKE_Y + 2) {
        int length = 5 + (int)((hash >> 16) % 10);
        int drift = (int)(s.time_ms >> 8) * (((hash >> 20) & 1) ? 1 : -1);
        int start = (int)(((hash >> 4) + (uint32_t)(drift + 100000)) % (SIZE + 40)) - 20;

        for (int x = start > x1 ? start : x1; x < start + length && x <= x2; x++) {
            row->dst[x - x1] = blend565(s.glint565, row->dst[x - x1], 5);
        }
    }
}

static void draw_particles(int x1, int y1, int x2, int y2, uint16_t *dst, int stride)
{
    bool snow = s.view.weather == DIORAMA_WEATHER_SNOW;

    for (int i = 0; i < s.num_particles; i++) {
        const particle_t *p = &s.particles[i];
        int sway = snow ? wave[(p->sway + (p->y >> 2)) & 31] >> 5 : 0;
        int rows = snow ? 2 : RAIN_LENGTH;

        for (int rel = 0; rel < rows; rel++) {
            int y = p->y + rel;
            int x = snow ? p->x + sway : p->x - rel / 3;

            if (y < y1 || y > y2) {
                continue;
            }
            for (int px = x; px < x + (snow ? 2 : 1); px++) {
                if (px >= x1 && px <= x2) {
                    uint16_t *pixel = (uint16_t *)((uint8_t *)dst + (y - y1) * stride) + (px - x1);

                    *pixel = blend565(s.particle565, *pixel, snow ? 26 : 15);
                }
            }
        }
    }
}

/* How much darker a row of the lake is: from a bit below the shore, most where the text is */
static uint8_t lake_shade(int y)
{
    int below = y - SHADE_Y;

    if (below < 0) {
        return 0;
    }
    if (below < 20) {
        return (uint8_t)(below * 87 / 20);
    }

    return (uint8_t)(below < 50 ? 87 + (below - 20) * 20 / 30 : 107);
}

void diorama_scene_render(int x1, int y1, int x2, int y2, uint16_t *dst, int stride)
{
    row_t rows[CHUNK_ROWS];
    int y = y1;

    while (y <= y2) {
        bool lake = y >= LAKE_Y;
        /* Rows of the land and of the lake are not mixed, and no more rows than there are colors for */
        int end = lake ? y2 : (y2 < LAKE_Y - 1 ? y2 : LAKE_Y - 1);
        int num_rows = end - y + 1 > CHUNK_ROWS ? CHUNK_ROWS : end - y + 1;

        for (int r = 0; r < num_rows; r++) {
            row_t *row = &rows[r];

            row->y = (int16_t)(y + r);
            row->dst = (uint16_t *)((uint8_t *)dst + (y + r - y1) * stride);
            if (lake) {
                int depth = row->y - LAKE_Y;

                row->src_y = (int16_t)(LAKE_Y - 1 - depth * 5 / 4);
                row->shift = (int8_t)((wave[(row->y * 5 + (int)s.ripple * 3) & 31] * (depth / 24 + 1)) >> 7);
                /* The water is darkest right at the shore and far out */
                row->tint = (uint8_t)(depth == 0 ? 160 : 72 + depth * 112 / (SIZE - LAKE_Y));
                row->shade = lake_shade(row->y);
            } else {
                row->src_y = row->y;
                row->shift = 0;
                row->tint = 0;
                row->shade = 0;
            }
            if (row->src_y < s.sky_bottom) {
                render_sky_row(row, x1, x2);
            }
        }

        if (lake) {
            render_mirrored_land(rows, num_rows, x1, x2);
            for (int r = 0; r < num_rows; r++) {
                draw_glint(&rows[r], x1, x2);
                draw_afloat(&rows[r], x1, x2);
            }
        } else {
            render_land(rows, num_rows, x1, x2, stride);
        }
        y += num_rows;
    }

    if (s.num_particles > 0) {
        draw_particles(x1, y1, x2, y2, dst, stride);
    }
}

void diorama_scene_step(uint32_t elapsed_ms, diorama_dirty_cb_t dirty)
{
    uint32_t before = s.time_ms;

    s.time_ms += elapsed_ms;

    if (s.time_ms / RIPPLE_PERIOD_MS != before / RIPPLE_PERIOD_MS) {
        s.ripple = s.time_ms / RIPPLE_PERIOD_MS;
        dirty(0, LAKE_Y, SIZE - 1, SIZE - 1);
    }

    if (s.star_alpha > 0 && s.time_ms / TWINKLE_PERIOD_MS != before / TWINKLE_PERIOD_MS) {
        for (int i = 0; i < TWINKLE_STARS; i++) {
            star_t *star = &s.stars[rnd() % NUM_STARS];

            star->level = (uint8_t)rnd_range(0, 3);
            dirty(star->x, star->y, star->x, star->y);
        }
    }

    if (s.aurora > 0 && s.time_ms / AURORA_PERIOD_MS != before / AURORA_PERIOD_MS) {
        s.aurora_step = s.time_ms / AURORA_PERIOD_MS;
        build_aurora();
        dirty(0, 0, SIZE - 1, AURORA_LAST_ROW);
    }

    if (s.time_ms / SMOKE_PERIOD_MS != before / SMOKE_PERIOD_MS) {
        int x = DIORAMA_CHIMNEY_X - s.off_x[DIORAMA_CHIMNEY_LAYER];
        int y = DIORAMA_CHIMNEY_Y + s.off_y[DIORAMA_CHIMNEY_LAYER];

        s.smoke = s.time_ms / SMOKE_PERIOD_MS;
        if (y < LAKE_Y) {
            dirty(clamp(x - 4, 0, SIZE - 1), clamp(y - 2 * SMOKE_PUFFS, 0, SIZE - 1), clamp(x + 10, 0, SIZE - 1), y);
        }
    }

    for (int i = 0; i < s.num_clouds; i++) {
        cloud_t *cloud = &s.clouds[i];
        int width = diorama_cloud_shapes[cloud->shape].width;
        int old = cloud->x >> 8;

        cloud->x += (int32_t)(CLOUD_SPEED * elapsed_ms / 1000);
        if ((cloud->x >> 8) != old) {
            if ((cloud->x >> 8) > SIZE) {
                cloud->x = -width * 256;
            }
            dirty(old, cloud->y - CLOUD_ABOVE, (cloud->x >> 8) + width, cloud->y + CLOUD_BELOW);
        }
    }

    if (s.num_particles > 0) {
        bool snow = s.view.weather == DIORAMA_WEATHER_SNOW;
        int fall = (int)((snow ? SNOW_SPEED : RAIN_SPEED) * elapsed_ms / 1000);

        for (int i = 0; i < s.num_particles; i++) {
            particle_t *p = &s.particles[i];

            p->y = (int16_t)(p->y + fall + (snow ? (i & 1) : 0));
            if (!snow) {
                p->x = (int16_t)(p->x - fall / 3);
            }
            if (p->y >= SIZE) {
                particle_spawn(p, false);
            }
        }
        dirty(0, 0, SIZE - 1, SIZE - 1);
    }
}
