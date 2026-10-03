#pragma once

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

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
