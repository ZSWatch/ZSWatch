#pragma once

#include "sensors/zsw_pressure_sensor.h"

struct pressure_event {
    float pressure;
    float temperature;
};