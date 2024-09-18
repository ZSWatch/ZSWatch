#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <lvgl.h>

#define TIMER_UI_MAX_TIMERS 10

typedef enum {
    TYPE_ALARM,
    TYPE_TIMER,
    TYPE_INVALID
} ui_timer_type_t;

typedef enum {
    TIMER_EVT_START_PAUSE_RESUME,
    TIMER_EVT_RESET,
    TIMER_EVT_DELETE,
} timer_event_type_t;

typedef enum {
    TIMER_STATE_PLAYING,
    TIMER_STATE_PAUSED,
    TIMER_STATE_STOPPED,
} timer_state_t;

typedef struct {
    bool used;
    uint32_t timer_id;
    uint32_t zsw_alarm_timer_id;
    uint32_t hour;
    uint32_t min;
    uint32_t sec;
    uint32_t remaining_hour;
    uint32_t remaining_min;
    uint32_t remaining_sec;
    timer_state_t state;
    ui_timer_type_t type;
} timer_app_timer_t;

typedef void(*on_timer_created)(uint32_t hour, uint32_t min, uint32_t sec, ui_timer_type_t type);

typedef void(*on_timer_event)(timer_event_type_t type, uint32_t timer_id);

void timer_ui_show(lv_obj_t *root, on_timer_created on_create, on_timer_event on_event);

void timer_ui_remove(void);

void timer_ui_add_timer(timer_app_timer_t timer);

void timer_ui_update_timer(timer_app_timer_t timer);

void timer_ui_set_time(int hour, int min, int second);

void timer_ui_remove_timer(timer_app_timer_t timer);


