#pragma once

#include <stdbool.h>

#include "managers/zsw_power_manager.h"

struct activity_state_event {
    zsw_power_manager_state_t state;
};
