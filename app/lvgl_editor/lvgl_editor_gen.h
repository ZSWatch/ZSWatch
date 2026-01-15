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

extern lv_font_t * montserrat_10;

extern lv_font_t * montserrat_12;

extern lv_font_t * montserrat_14;

extern lv_font_t * montserrat_18;

extern lv_font_t * montserrat_24;

extern lv_font_t * montserrat_28;

extern lv_font_t * montserrat_48;

/*----------------
 * Images
 *----------------*/

extern const void * icon_play;
extern const void * icon_pause;
extern const void * icon_skip_forward;
extern const void * icon_skip_back;
extern const void * heart;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t music_track_name;
extern lv_subject_t music_artist_name;
extern lv_subject_t music_time;
extern lv_subject_t music_progress;
extern lv_subject_t music_playing;
extern lv_subject_t app_picker_page_index;
extern lv_subject_t app_picker_page_count;
extern lv_subject_t app_picker_folder_open;
extern lv_subject_t app_picker_folder_title;
extern lv_subject_t hr_bpm;
extern lv_subject_t hr_confidence;
extern lv_subject_t hr_spo2;
extern lv_subject_t hr_spo2_confidence;
extern lv_subject_t hr_rr_interval;
extern lv_subject_t hr_rr_confidence;
extern lv_subject_t hr_skin_contact;
extern lv_subject_t hr_activity;
extern lv_subject_t hr_bpm_text;
extern lv_subject_t hr_confidence_text;
extern lv_subject_t hr_spo2_text;
extern lv_subject_t hr_rr_text;
extern lv_subject_t hr_activity_text;
extern lv_subject_t hr_skin_text;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

void app_picker_on_app_clicked(lv_event_t * e);
void app_picker_on_nav_left_clicked(lv_event_t * e);
void app_picker_on_nav_right_clicked(lv_event_t * e);
void app_picker_on_folder_close_clicked(lv_event_t * e);
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
#include "components/app_picker/app_picker_gen.h"
#include "components/hr_app/hr_app_debug_gen.h"
#include "components/hr_app/hr_app_graph_gen.h"
#include "components/hr_app/hr_app_overview_gen.h"
#include "components/music_app/music_app_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_EDITOR_GEN_H*/