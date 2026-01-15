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

lv_font_t * montserrat_14_full;
extern lv_font_t montserrat_14_full_data;
lv_font_t * montserrat_10;
extern lv_font_t montserrat_10_data;
lv_font_t * montserrat_12;
extern lv_font_t montserrat_12_data;
lv_font_t * montserrat_14;
extern lv_font_t montserrat_14_data;
lv_font_t * montserrat_18;
extern lv_font_t montserrat_18_data;
lv_font_t * montserrat_24;
extern lv_font_t montserrat_24_data;
lv_font_t * montserrat_28;
extern lv_font_t montserrat_28_data;
lv_font_t * montserrat_48;
extern lv_font_t montserrat_48_data;

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
const void * heart;
extern const void * heart_data;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t music_track_name;
lv_subject_t music_artist_name;
lv_subject_t music_time;
lv_subject_t music_progress;
lv_subject_t music_playing;
lv_subject_t app_picker_page_index;
lv_subject_t app_picker_page_count;
lv_subject_t app_picker_folder_open;
lv_subject_t app_picker_folder_title;
lv_subject_t hr_bpm;
lv_subject_t hr_confidence;
lv_subject_t hr_spo2;
lv_subject_t hr_spo2_confidence;
lv_subject_t hr_rr_interval;
lv_subject_t hr_rr_confidence;
lv_subject_t hr_skin_contact;
lv_subject_t hr_activity;
lv_subject_t hr_bpm_text;
lv_subject_t hr_confidence_text;
lv_subject_t hr_spo2_text;
lv_subject_t hr_rr_text;
lv_subject_t hr_activity_text;
lv_subject_t hr_skin_text;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lvgl_editor_init_gen(const char * asset_path)
{
    char buf[256];

    /*----------------
     * Global styles
     *----------------*/

    /*----------------
     * Fonts
     *----------------*/

    /* get font 'montserrat_14_full' from a C array */
    montserrat_14_full = &montserrat_14_full_data;
    /* get font 'montserrat_10' from a C array */
    montserrat_10 = &montserrat_10_data;
    /* get font 'montserrat_12' from a C array */
    montserrat_12 = &montserrat_12_data;
    /* get font 'montserrat_14' from a C array */
    montserrat_14 = &montserrat_14_data;
    /* get font 'montserrat_18' from a C array */
    montserrat_18 = &montserrat_18_data;
    /* get font 'montserrat_24' from a C array */
    montserrat_24 = &montserrat_24_data;
    /* get font 'montserrat_28' from a C array */
    montserrat_28 = &montserrat_28_data;
    /* get font 'montserrat_48' from a C array */
    montserrat_48 = &montserrat_48_data;


    /*----------------
     * Images
     *----------------*/
    icon_play = &icon_play_data;
    icon_pause = &icon_pause_data;
    icon_skip_forward = &icon_skip_forward_data;
    icon_skip_back = &icon_skip_back_data;
    heart = &heart_data;

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
    lv_subject_init_int(&app_picker_page_index, 0);
    lv_subject_init_int(&app_picker_page_count, 1);
    lv_subject_init_int(&app_picker_folder_open, 0);
    static char app_picker_folder_title_buf[UI_SUBJECT_STRING_LENGTH];
    static char app_picker_folder_title_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&app_picker_folder_title,
                           app_picker_folder_title_buf,
                           app_picker_folder_title_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Folder"
                          );
    lv_subject_init_int(&hr_bpm, 72);
    lv_subject_init_int(&hr_confidence, 78);
    lv_subject_init_int(&hr_spo2, 98);
    lv_subject_init_int(&hr_spo2_confidence, 85);
    lv_subject_init_int(&hr_rr_interval, 850);
    lv_subject_init_int(&hr_rr_confidence, 72);
    lv_subject_init_int(&hr_skin_contact, 3);
    lv_subject_init_int(&hr_activity, 2);
    static char hr_bpm_text_buf[UI_SUBJECT_STRING_LENGTH];
    static char hr_bpm_text_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&hr_bpm_text,
                           hr_bpm_text_buf,
                           hr_bpm_text_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "72"
                          );
    static char hr_confidence_text_buf[UI_SUBJECT_STRING_LENGTH];
    static char hr_confidence_text_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&hr_confidence_text,
                           hr_confidence_text_buf,
                           hr_confidence_text_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "78%"
                          );
    static char hr_spo2_text_buf[UI_SUBJECT_STRING_LENGTH];
    static char hr_spo2_text_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&hr_spo2_text,
                           hr_spo2_text_buf,
                           hr_spo2_text_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "98%"
                          );
    static char hr_rr_text_buf[UI_SUBJECT_STRING_LENGTH];
    static char hr_rr_text_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&hr_rr_text,
                           hr_rr_text_buf,
                           hr_rr_text_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "850"
                          );
    static char hr_activity_text_buf[UI_SUBJECT_STRING_LENGTH];
    static char hr_activity_text_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&hr_activity_text,
                           hr_activity_text_buf,
                           hr_activity_text_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Walk"
                          );
    static char hr_skin_text_buf[UI_SUBJECT_STRING_LENGTH];
    static char hr_skin_text_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&hr_skin_text,
                           hr_skin_text_buf,
                           hr_skin_text_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "On Skin"
                          );

    /*----------------
     * Translations
     *----------------*/

#if LV_USE_XML
    /* Register widgets */

    /* Register fonts */
    lv_xml_register_font(NULL, "montserrat_14_full", montserrat_14_full);
    lv_xml_register_font(NULL, "montserrat_10", montserrat_10);
    lv_xml_register_font(NULL, "montserrat_12", montserrat_12);
    lv_xml_register_font(NULL, "montserrat_14", montserrat_14);
    lv_xml_register_font(NULL, "montserrat_18", montserrat_18);
    lv_xml_register_font(NULL, "montserrat_24", montserrat_24);
    lv_xml_register_font(NULL, "montserrat_28", montserrat_28);
    lv_xml_register_font(NULL, "montserrat_48", montserrat_48);

    /* Register subjects */
    lv_xml_register_subject(NULL, "music_track_name", &music_track_name);
    lv_xml_register_subject(NULL, "music_artist_name", &music_artist_name);
    lv_xml_register_subject(NULL, "music_time", &music_time);
    lv_xml_register_subject(NULL, "music_progress", &music_progress);
    lv_xml_register_subject(NULL, "music_playing", &music_playing);
    lv_xml_register_subject(NULL, "app_picker_page_index", &app_picker_page_index);
    lv_xml_register_subject(NULL, "app_picker_page_count", &app_picker_page_count);
    lv_xml_register_subject(NULL, "app_picker_folder_open", &app_picker_folder_open);
    lv_xml_register_subject(NULL, "app_picker_folder_title", &app_picker_folder_title);
    lv_xml_register_subject(NULL, "hr_bpm", &hr_bpm);
    lv_xml_register_subject(NULL, "hr_confidence", &hr_confidence);
    lv_xml_register_subject(NULL, "hr_spo2", &hr_spo2);
    lv_xml_register_subject(NULL, "hr_spo2_confidence", &hr_spo2_confidence);
    lv_xml_register_subject(NULL, "hr_rr_interval", &hr_rr_interval);
    lv_xml_register_subject(NULL, "hr_rr_confidence", &hr_rr_confidence);
    lv_xml_register_subject(NULL, "hr_skin_contact", &hr_skin_contact);
    lv_xml_register_subject(NULL, "hr_activity", &hr_activity);
    lv_xml_register_subject(NULL, "hr_bpm_text", &hr_bpm_text);
    lv_xml_register_subject(NULL, "hr_confidence_text", &hr_confidence_text);
    lv_xml_register_subject(NULL, "hr_spo2_text", &hr_spo2_text);
    lv_xml_register_subject(NULL, "hr_rr_text", &hr_rr_text);
    lv_xml_register_subject(NULL, "hr_activity_text", &hr_activity_text);
    lv_xml_register_subject(NULL, "hr_skin_text", &hr_skin_text);

    /* Register callbacks */
    lv_xml_register_event_cb(NULL, "app_picker_on_app_clicked", app_picker_on_app_clicked);
    lv_xml_register_event_cb(NULL, "app_picker_on_nav_left_clicked", app_picker_on_nav_left_clicked);
    lv_xml_register_event_cb(NULL, "app_picker_on_nav_right_clicked", app_picker_on_nav_right_clicked);
    lv_xml_register_event_cb(NULL, "app_picker_on_folder_close_clicked", app_picker_on_folder_close_clicked);
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
    lv_xml_register_image(NULL, "heart", heart);
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
void __attribute__((weak)) app_picker_on_app_clicked(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("app_picker_on_app_clicked was called\n");
}
void __attribute__((weak)) app_picker_on_nav_left_clicked(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("app_picker_on_nav_left_clicked was called\n");
}
void __attribute__((weak)) app_picker_on_nav_right_clicked(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("app_picker_on_nav_right_clicked was called\n");
}
void __attribute__((weak)) app_picker_on_folder_close_clicked(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("app_picker_on_folder_close_clicked was called\n");
}
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