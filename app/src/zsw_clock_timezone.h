#pragma once

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

/** CTS Local Time Information: signed UTC quarter-hours plus DST quarter-hours. */
static inline bool zsw_clock_cts_offset(const uint8_t info[2], float *hours)
{
    int zone = (int8_t)info[0];
    int dst = info[1];
    if (zone < -48 || zone > 56 ||
        (dst != 0 && dst != 2 && dst != 4 && dst != 8)) {
        return false; // Unknown/reserved offsets must not become UTC.
    }
    *hours = (zone + dst) / 4.0f;
    return true;
}

/** Phone offsets are east of UTC; POSIX TZ offsets use the opposite sign. */
static inline bool zsw_clock_format_timezone(float offset_hours, char *tz, size_t capacity)
{
    if (!isfinite(offset_hours) || offset_hours < -24.0f || offset_hours > 24.0f ||
        tz == NULL || capacity == 0) {
        return false;
    }

    int minutes = (int)(offset_hours * 60.0f + (offset_hours < 0.0f ? -0.5f : 0.5f));
    int magnitude = minutes < 0 ? -minutes : minutes;
    int length = snprintf(tz, capacity, "UTC%c%02d:%02d", minutes < 0 ? '+' : '-',
                          magnitude / 60, magnitude % 60);
    return length >= 0 && (size_t)length < capacity;
}
