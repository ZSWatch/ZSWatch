#pragma once

#include "sensors/zsw_magnetometer.h"

struct magnetometer_event {
    float x;
    float y;
    float z;
};