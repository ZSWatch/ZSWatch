/**
 * @file lvgl_editor_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "lvgl_editor_gen.h"

#if LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Fonts
 *----------------*/

/*----------------
 * Images
 *----------------*/

const void * icon_play;
extern const void * icon_play_data;
const void * icon_pause;
extern const void * icon_pause_data;
const void * icon_skip_forward;
extern const void * icon_skip_forward_data;
const void * icon_skip_back;
extern const void * icon_skip_back_data;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t music_track_name;
lv_subject_t music_artist_name;
lv_subject_t music_time;
lv_subject_t music_progress;
lv_subject_t music_playing;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lvgl_editor_init_gen(const char * asset_path)
{
    /*----------------
     * Global styles
     *----------------*/

    /*----------------
     * Fonts
     *----------------*/


    /*----------------
     * Images
     *----------------*/
    icon_play = &icon_play_data;
    icon_pause = &icon_pause_data;
    icon_skip_forward = &icon_skip_forward_data;
    icon_skip_back = &icon_skip_back_data;

    /*----------------
     * Subjects
     *----------------*/
    static char music_track_name_buf[UI_SUBJECT_STRING_LENGTH];
    static char music_track_name_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&music_track_name,
                           music_track_name_buf,
                           music_track_name_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "No music playing..."
                          );
    static char music_artist_name_buf[UI_SUBJECT_STRING_LENGTH];
    static char music_artist_name_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&music_artist_name,
                           music_artist_name_buf,
                           music_artist_name_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Artist..."
                          );
    static char music_time_buf[UI_SUBJECT_STRING_LENGTH];
    static char music_time_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&music_time,
                           music_time_buf,
                           music_time_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "00:00"
                          );
    lv_subject_init_int(&music_progress, 0);
    lv_subject_init_int(&music_playing, 0);

    /*----------------
     * Translations
     *----------------*/

#if LV_USE_XML
    /* Register widgets */

    /* Register fonts */

    /* Register subjects */
    lv_xml_register_subject(NULL, "music_track_name", &music_track_name);
    lv_xml_register_subject(NULL, "music_artist_name", &music_artist_name);
    lv_xml_register_subject(NULL, "music_time", &music_time);
    lv_xml_register_subject(NULL, "music_progress", &music_progress);
    lv_xml_register_subject(NULL, "music_playing", &music_playing);

    /* Register callbacks */
    lv_xml_register_event_cb(NULL, "music_on_prev_clicked", music_on_prev_clicked);
    lv_xml_register_event_cb(NULL, "music_on_play_pause_clicked", music_on_play_pause_clicked);
    lv_xml_register_event_cb(NULL, "music_on_next_clicked", music_on_next_clicked);
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "icon_play", icon_play);
    lv_xml_register_image(NULL, "icon_pause", icon_pause);
    lv_xml_register_image(NULL, "icon_skip_forward", icon_skip_forward);
    lv_xml_register_image(NULL, "icon_skip_back", icon_skip_back);
#endif

#if LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manaully from XML using lv_xml_create() */
#endif
}

/* Callbacks */
#if defined(LV_EDITOR_PREVIEW)
void __attribute__((weak)) music_on_prev_clicked(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("music_on_prev_clicked was called\n");
}
void __attribute__((weak)) music_on_play_pause_clicked(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("music_on_play_pause_clicked was called\n");
}
void __attribute__((weak)) music_on_next_clicked(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("music_on_next_clicked was called\n");
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/
