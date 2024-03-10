#pragma once

#include <stdint.h>
#include <stdbool.h>

struct battery_sample_event {
    int mV;
    int percent;
    float temperature;
    float avg_current;
    float tte;
    float ttf;
    int status;
    int error;
    bool is_charging;
};
