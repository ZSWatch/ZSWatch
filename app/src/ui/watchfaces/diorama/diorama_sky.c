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

#include <math.h>

#include "diorama_sky.h"

#define PI_F                    3.14159265f
#define DEG                     (PI_F / 180.0f)
#define SYNODIC_MONTH           29.530588f
/* Julian day number of the new moon of 6 January 2000, and the time of day it fell on */
#define NEW_MOON_JDN            2451550
#define NEW_MOON_DAY_FRACTION   0.76f
/* The widest half day the orbit is stretched over, so the sun still moves in a polar summer */
/* The upper edge of the sun shows at the horizon when its middle is this far below it */
#define SUNRISE_HEIGHT          (-0.833f * DEG)
#define MIN_HALF_DAY            (20.0f * DEG)

static int julian_day(int year, int month, int day)
{
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;

    return day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}

/* Hour angle in radians, -pi to pi, 0 when the body is highest */
static float wrap(float angle)
{
    while (angle > PI_F) {
        angle -= 2.0f * PI_F;
    }
    while (angle < -PI_F) {
        angle += 2.0f * PI_F;
    }

    return angle;
}

/* Hour angle at which a body of this declination is at the given height on its way up and down */
static float half_day(float latitude, float declination, float height, bool *rises)
{
    float c = (sinf(height) - sinf(latitude) * sinf(declination)) / (cosf(latitude) * cosf(declination));

    *rises = c > -1.0f && c < 1.0f;
    if (c >= 1.0f) {
        return 0.0f;
    }
    if (c <= -1.0f) {
        return PI_F;
    }

    return acosf(c);
}

static void place_body(float latitude, float declination, float hour_angle, int16_t *elevation, uint8_t *pos)
{
    bool rises;
    float h0 = half_day(latitude, declination, 0.0f, &rises);
    float sin_el = sinf(latitude) * sinf(declination) + cosf(latitude) * cosf(declination) * cosf(hour_angle);
    float p;

    if (h0 < MIN_HALF_DAY) {
        h0 = MIN_HALF_DAY;
    }
    p = 127.5f + hour_angle / h0 * 127.5f;
    *elevation = (int16_t)(asinf(sin_el) / DEG * 10.0f);
    *pos = (uint8_t)(p < 0.0f ? 0.0f : (p > 255.0f ? 255.0f : p));
}

void diorama_sky_compute(const diorama_sky_time_t *when, diorama_view_t *view, diorama_sky_events_t *events)
{
    int jdn = julian_day(when->year, when->month, when->day);
    int day_of_year = jdn - julian_day(when->year, 1, 1) + 1;
    float latitude = 0.0f;
    float declination = 0.0f;
    /* Minutes the sun is ahead of the clock */
    float ahead = 0.0f;
    float solar_minutes;
    float hour_angle;
    float age;
    float phase;

    if (when->has_place) {
        float b = 2.0f * PI_F * (float)(day_of_year - 81) / 364.0f;

        latitude = when->latitude * DEG;
        declination = -23.44f * DEG * cosf(2.0f * PI_F * (float)(day_of_year + 10) / 365.0f);
        /* The equation of time, and how far the place is from the meridian of its time zone */
        ahead = 9.87f * sinf(2.0f * b) - 7.53f * cosf(b) - 1.5f * sinf(b) +
                4.0f * (when->longitude - (float)when->utc_offset_min / 4.0f);
    }

    solar_minutes = (float)when->minute_of_day + ahead;
    hour_angle = wrap((solar_minutes / 4.0f - 180.0f) * DEG);
    place_body(latitude, declination, hour_angle, &view->sun_elevation, &view->sun_pos);

    if (events) {
        bool rises;
        float h0 = half_day(latitude, declination, SUNRISE_HEIGHT, &rises) / DEG * 4.0f;

        events->sunrise = rises ? ((int)(720.0f - h0 - ahead) + 1440) % 1440 : -1;
        events->sunset = rises ? ((int)(720.0f + h0 - ahead) + 1440) % 1440 : -1;
    }

    /* The moon trails the sun by its age, and stands high when the sun stands low */
    age = fmodf((float)(jdn - NEW_MOON_JDN) + ((float)(when->minute_of_day - when->utc_offset_min) / 1440.0f) + 0.5f -
                NEW_MOON_DAY_FRACTION, SYNODIC_MONTH);
    if (age < 0.0f) {
        age += SYNODIC_MONTH;
    }
    phase = 2.0f * PI_F * age / SYNODIC_MONTH;
    view->moon_lit = (uint8_t)((1.0f - cosf(phase)) * 127.5f);
    view->moon_waxing = age < SYNODIC_MONTH / 2.0f;
    place_body(latitude, declination * cosf(phase), wrap(hour_angle - phase), &view->moon_elevation, &view->moon_pos);
}
