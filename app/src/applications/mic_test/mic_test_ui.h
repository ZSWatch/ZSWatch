#pragma once

#include <inttypes.h>
#include <lvgl.h>

typedef void(*on_mic_test_ui_event_cb_t)(void);

void mic_test_ui_show(lv_obj_t *root, on_mic_test_ui_event_cb_t toggle_cb);

void mic_test_ui_remove(void);

void mic_test_ui_set_status(const char *status);

void mic_test_ui_toggle_button_state(void);

void mic_test_ui_update_spectrum(const uint8_t *magnitudes, size_t num_bars);

typedef enum {
    SPECTRUM_MODE_CIRCULAR = 0,
    SPECTRUM_MODE_LINEAR = 1,
    SPECTRUM_MODE_COUNT
} spectrum_mode_t;
