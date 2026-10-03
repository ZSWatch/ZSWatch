#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "zsw_clock_timezone.h"

static void check(float offset, const char *expected, int hour, int minute)
{
    char tz[16];
    assert(zsw_clock_format_timezone(offset, tz, sizeof(tz)));
    assert(strcmp(tz, expected) == 0);
    assert(setenv("TZ", tz, 1) == 0);
    tzset();
    const time_t utc_noon = 1767268800; /* 2026-01-01 12:00:00 UTC */
    const struct tm *local = localtime(&utc_noon);
    assert(local != NULL);
    assert(local->tm_hour == hour && local->tm_min == minute);
}

int main(void)
{
    check(2.0f, "UTC-02:00", 14, 0);
    check(0.0f, "UTC-00:00", 12, 0); /* Reset a previously nonzero timezone. */
    check(5.5f, "UTC-05:30", 17, 30);
    check(5.75f, "UTC-05:45", 17, 45);
    check(12.75f, "UTC-12:45", 0, 45);
    check(13.75f, "UTC-13:45", 1, 45);
    check(-3.5f, "UTC+03:30", 8, 30);
    check(-12.0f, "UTC+12:00", 0, 0);
    check(14.0f, "UTC-14:00", 2, 0);
    char tz[16];
    assert(!zsw_clock_format_timezone(NAN, tz, sizeof(tz)));
    assert(!zsw_clock_format_timezone(INFINITY, tz, sizeof(tz)));
    assert(!zsw_clock_format_timezone(25.0f, tz, sizeof(tz)));
    assert(!zsw_clock_format_timezone(-25.0f, tz, sizeof(tz)));
    assert(!zsw_clock_format_timezone(2.0f, tz, 3));
    assert(!zsw_clock_format_timezone(2.0f, NULL, 16));
    return 0;
}
