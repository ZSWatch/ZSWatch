#pragma once
#include <stdbool.h>
typedef enum { ZSW_VIBRATION_PATTERN_FIND } zsw_vibration_pattern_t;
int zsw_vibration_run_pattern_loop(zsw_vibration_pattern_t pattern);
bool zsw_vibration_stop_find(void);
