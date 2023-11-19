#pragma once

#include "sensors/zsw_environment_sensor.h"

struct environment_event {
    float temperature;
    float humidity;
    float pressure;
    float iaq;
};