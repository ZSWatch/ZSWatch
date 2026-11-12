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

/* The sun and the moon travel between these columns, and rise from this row */
#define ORBIT_X1                34
#define ORBIT_X2                206
#define ORBIT_HORIZON_Y         142
#define ORBIT_HEIGHT            52
/* Elevation (tenths of a degree) drawn at the top of the orbit: higher looks the same */
#define ORBIT_MAX_ELEVATION     600
#define SUN_RADIUS              10
#define MOON_RADIUS             8
#define MOON_GLOW_RADIUS        26

/* Steps from the middle to the edge that the light around the sun and the moon is known at */
#define GLOW_STEPS              48
/* Light weaker than this, of 256, never changes a pixel */
#define GLOW_MIN_VISIBLE        4

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
static const uint8_t layer_mist[NUM_LAYERS] = { 185, 150, 95, 40 };

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

/*
 * What stands in a column of the display: the rows where something new comes into view, from
 * the top, and what it is drawn with (three bits each). The last row is never reached.
 */
typedef struct {
    uint8_t y[NUM_COLORS + 1];
    uint16_t colors;
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
    int16_t sun_x;
    int16_t sun_y;
    int16_t moon_x;
    int16_t moon_y;
    uint8_t glow;
    bool sun_disc;
    uint8_t moon_alpha;
    uint8_t star_alpha;
    uint8_t fog;                /* Of 32, at the shore */
    int8_t off_x[NUM_LAYERS];
    int16_t off_y[NUM_LAYERS];
    column_t columns[SIZE];
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

static void orbit_position(int elevation, int pos, int16_t *x, int16_t *y)
{
    *x = (int16_t)(ORBIT_X1 + pos * (ORBIT_X2 - ORBIT_X1) / 255);
    *y = (int16_t)(ORBIT_HORIZON_Y - clamp(elevation, -ORBIT_MAX_ELEVATION, ORBIT_MAX_ELEVATION) *
                   ORBIT_HEIGHT / ORBIT_MAX_ELEVATION);
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
                /* The top of the farthest layer is snow, down to the next layer at the most */
                int below = i > 0 ? rows[i - 1] : 255;
                int snow_end = rows[i] + diorama_snow[MARGIN + s.off_x[0] + x];

                if (snow_end > rows[i]) {
                    column->y[k] = rows[i];
                    column->colors |= (uint16_t)(COLOR_SNOW << (k * 3));
                    k++;
                    if (snow_end >= below) {
                        continue;
                    }
                    rows[i] = (uint8_t)snow_end;
                }
            }
            column->y[k] = rows[i];
            column->colors |= (uint16_t)(layers[i] << (k * 3));
            k++;
        }
        for (; k <= NUM_COLORS; k++) {
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
}

bool diorama_scene_set_view(const diorama_view_t *view)
{
    int cover = 0;
    int fog = 0;
    int8_t off_x[NUM_LAYERS];
    int16_t off_y[NUM_LAYERS];
    bool moved = false;

    for (int i = 0; i < NUM_LAYERS; i++) {
        /* The nearer layers come up later */
        int rise = clamp((view->rise - i * 24) * 255 / (255 - (NUM_LAYERS - 1) * 24), 0, 255);

        off_x[i] = (int8_t)(view->tilt_x * parallax_x[i] / 127);
        off_y[i] = (int16_t)(view->tilt_y * parallax_y[i] / 127 + (255 - rise) * rise_drop[i] / 255);
        moved |= off_x[i] != s.off_x[i] || off_y[i] != s.off_y[i];
    }
    /* A tilt too small to move a layer by a pixel changes nothing */
    if (s.view_valid && !moved && view->sun_elevation == s.view.sun_elevation && view->sun_pos == s.view.sun_pos &&
        view->moon_elevation == s.view.moon_elevation && view->moon_pos == s.view.moon_pos &&
        view->moon_lit == s.view.moon_lit && view->moon_waxing == s.view.moon_waxing &&
        view->weather == s.view.weather && view->intensity == s.view.intensity) {
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
    if (view->weather == DIORAMA_WEATHER_SNOW) {
        /* Snow on the ground too */
        for (int i = 0; i < NUM_LAYERS; i++) {
            s.pal.layer[i] = mix(s.pal.layer[i], s.pal.snow, 70 - i * 12);
        }
    }
    s.fog = (uint8_t)fog;
    s.glow = (uint8_t)(s.pal.glow * (256 - cover) / 256);
    s.sun_disc = cover < 140;
    s.star_alpha = (uint8_t)(s.pal.stars * (256 - cover) / 256);
    s.moon_alpha = (uint8_t)((12 + s.pal.stars * 20 / 255) * (256 - cover * 2 / 3) / 256);

    s.sun565 = to565(s.pal.sun);
    s.moon565 = to565(0xE6ECFF);
    s.lake565 = to565(s.pal.lake);
    s.glint565 = to565(mix(s.pal.sky[2], 0xFFFFFF, 90));
    s.haze565 = to565(s.pal.haze);
    s.cloud_light565 = to565(s.pal.cloud_light);
    s.cloud_shade565 = to565(s.pal.cloud_shade);
    s.particle565 = to565(view->weather == DIORAMA_WEATHER_SNOW ? 0xFFFFFF : mix(s.pal.sky[2], 0xFFFFFF, 130));

    orbit_position(view->sun_elevation, view->sun_pos, &s.sun_x, &s.sun_y);
    orbit_position(view->moon_elevation, view->moon_pos, &s.moon_x, &s.moon_y);

    memcpy(s.off_x, off_x, sizeof(s.off_x));
    memcpy(s.off_y, off_y, sizeof(s.off_y));
    build_columns();
    s.sun_reach = build_falloff(s.sun_falloff, s.glow);
    s.moon_reach = build_falloff(s.moon_falloff, s.moon_alpha * view->moon_lit / 64);

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
    /* Parts of 32 of the color of the lake that the row takes on */
    uint8_t tint;
} row_t;

static inline uint16_t tinted(uint16_t color, const row_t *row)
{
    return row->tint ? blend565(s.lake565, color, row->tint) : color;
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
        a = (falloff[((uint32_t)here * inv) >> 16] + (dither[x & 3] >> 1)) >> 3;
        if (here < soft2) {
            int edge = 32 * (soft2 - here) / (soft2 - disc2);

            a = a > edge ? a : edge;
        }
        if (a > 0) {
            row->dst[x - x1] = blend565(color, row->dst[x - x1], a > 32 ? 32 : a);
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
    dither_row(mix(color, s.pal.lake, row->tint * 8), row->y, sky);
    fill_pattern(row->dst, x1, x2 - x1 + 1, sky);

    if (s.star_alpha > 0) {
        draw_stars(row, x1, x2);
    }
    if (s.glow > 0) {
        draw_glow(row, x1, x2, s.sun_x, s.sun_y, s.pal.glow_radius, s.sun_falloff, s.sun_reach,
                  s.sun_disc ? SUN_RADIUS : 0, s.sun565);
    }
    if (s.moon_alpha > 0 && s.moon_y < ORBIT_HORIZON_Y + MOON_RADIUS) {
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
        dither_row(mix(mix(s.pal.layer[l], s.pal.haze, mist), s.pal.lake, row->tint * 8), row->y, colors[l]);
        if (l == 0) {
            dither_row(mix(s.pal.snow, s.pal.lake, row->tint * 8), row->y, colors[COLOR_SNOW]);
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
                row->tint = (uint8_t)(depth == 0 ? 20 : 9 + depth * 14 / (SIZE - LAKE_Y));
            } else {
                row->src_y = row->y;
                row->shift = 0;
                row->tint = 0;
            }
            if (row->src_y < s.sky_bottom) {
                render_sky_row(row, x1, x2);
            }
        }

        if (lake) {
            render_mirrored_land(rows, num_rows, x1, x2);
            for (int r = 0; r < num_rows; r++) {
                draw_glint(&rows[r], x1, x2);
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
