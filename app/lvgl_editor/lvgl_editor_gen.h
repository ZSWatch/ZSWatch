/**
 * @file lvgl_editor_gen.h
 */

#ifndef LVGL_EDITOR_GEN_H
#define LVGL_EDITOR_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

#define UNIT_SM 6

#define UNIT_MD 12

#define UNIT_LG 18

#define UNIT_XL 24

#define OPA_MUTED lv_pct(20)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Fonts
 *----------------*/

extern lv_font_t * montserrat_14_full;

/*----------------
 * Images
 *----------------*/

extern const void * icon_play;
extern const void * icon_pause;
extern const void * icon_skip_forward;
extern const void * icon_skip_back;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t music_track_name;
extern lv_subject_t music_artist_name;
extern lv_subject_t music_time;
extern lv_subject_t music_progress;
extern lv_subject_t music_playing;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

void music_on_prev_clicked(lv_event_t * e);
void music_on_play_pause_clicked(lv_event_t * e);
void music_on_next_clicked(lv_event_t * e);

/**
 * Initialize the component library
 */

void lvgl_editor_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widget and components of this library*/
#include "components/music_app/music_app_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_EDITOR_GEN_H*/