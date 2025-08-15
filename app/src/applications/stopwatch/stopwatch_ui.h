#pragma once

#include <inttypes.h>
#include <lvgl.h>

typedef enum {
    STOPWATCH_STATE_STOPPED,
    STOPWATCH_STATE_RUNNING,
    STOPWATCH_STATE_PAUSED
} stopwatch_state_t;

typedef void (*stopwatch_event_cb_t)(void);

void stopwatch_ui_show(lv_obj_t *root, stopwatch_event_cb_t start_cb, stopwatch_event_cb_t pause_cb,
                       stopwatch_event_cb_t reset_cb, stopwatch_event_cb_t lap_cb);
void stopwatch_ui_remove(void);
void stopwatch_ui_update_time(uint32_t elapsed_ms);
void stopwatch_ui_update_state(stopwatch_state_t state);
void stopwatch_ui_add_lap_time(uint32_t lap_time, uint32_t total_time);
