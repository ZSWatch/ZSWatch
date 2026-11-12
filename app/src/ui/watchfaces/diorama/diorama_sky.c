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
/* The upper edge of the sun shows at the horizon when its middle is this far below it */
#define SUNRISE_HEIGHT          (-0.833f * DEG)
/* A sun that stands this high at noon, in degrees, is drawn at the top of its arc */
#define HIGH_NOON               45.0f
#define DAY_MINUTES             1440.0f

static int julian_day(int year, int month, int day)
{
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;

    return day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
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
    float half;
    float noon;
    bool rises;
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

    solar_minutes = fmodf((float)when->minute_of_day + ahead + DAY_MINUTES, DAY_MINUTES);
    hour_angle = (solar_minutes / 4.0f - 180.0f) * DEG;
    view->sun_elevation = (int16_t)(asinf(sinf(latitude) * sinf(declination) +
                                          cosf(latitude) * cosf(declination) * cosf(hour_angle)) / DEG * 10.0f);
    noon = (90.0f - fabsf(latitude - declination) / DEG) / HIGH_NOON;
    view->sun_height = (uint8_t)(noon < 0.0f ? 0.0f : (noon > 1.0f ? 255.0f : noon * 255.0f));

    /* Half the length of the day in minutes: the sun has the day and the moon has the night */
    half = half_day(latitude, declination, SUNRISE_HEIGHT, &rises) / DEG * 4.0f;
    if (!rises && half <= 0.0f) {
        /* The polar night: the moon goes round once a day and stands highest at midnight */
        view->sun_arc = DIORAMA_ARC_NONE;
        view->moon_arc = (int16_t)(fmodf(solar_minutes + DAY_MINUTES / 2.0f, DAY_MINUTES) * 256.0f / DAY_MINUTES);
    } else if (!rises) {
        /* The midnight sun: lowest at midnight */
        view->sun_arc = (int16_t)(solar_minutes * 256.0f / DAY_MINUTES);
        view->moon_arc = DIORAMA_ARC_NONE;
    } else {
        float night = DAY_MINUTES - 2.0f * half;

        view->sun_arc = (int16_t)((solar_minutes - (DAY_MINUTES / 2.0f - half)) * 256.0f / (2.0f * half));
        view->moon_arc = (int16_t)(fmodf(solar_minutes - (DAY_MINUTES / 2.0f + half) + DAY_MINUTES, DAY_MINUTES) * 256.0f /
                                   night);
    }

    if (events) {
        events->sunrise = rises ? ((int)(720.0f - half - ahead) + 1440) % 1440 : -1;
        events->sunset = rises ? ((int)(720.0f + half - ahead) + 1440) % 1440 : -1;
        events->polar_day = !rises && half > 0.0f;
    }

    /* How much of the moon is lit follows from the days since a known new moon */
    age = fmodf((float)(jdn - NEW_MOON_JDN) + ((float)(when->minute_of_day - when->utc_offset_min) / 1440.0f) + 0.5f -
                NEW_MOON_DAY_FRACTION, SYNODIC_MONTH);
    if (age < 0.0f) {
        age += SYNODIC_MONTH;
    }
    phase = 2.0f * PI_F * age / SYNODIC_MONTH;
    view->moon_lit = (uint8_t)((1.0f - cosf(phase)) * 127.5f);
    view->moon_waxing = age < SYNODIC_MONTH / 2.0f;
}
