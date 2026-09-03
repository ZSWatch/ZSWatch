#pragma once

#include <lvgl.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float altitude;          // Current altitude in meters
    float raw_pressure;      // Current raw pressure in hPa
    float *history_data;     // Pointer to historical altitude data (24 samples)
    uint8_t start_idx;       // Starting index for the circular buffer of history data
    bool is_calibrated;      // Calibration status
    bool has_data;           // Flag indicating if historical data is available
} altimeter_ui_data_t;

#define HISTORY_SAMPLES 24

void altimeter_ui_show(lv_obj_t *root);
void altimeter_ui_remove(void);
void altimeter_ui_update(altimeter_ui_data_t *data);
