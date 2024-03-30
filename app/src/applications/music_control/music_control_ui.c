#include <music_control/music_control_ui.h>
#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>

static lv_obj_t *root_page = NULL;
static lv_obj_t *track_name_label;
static lv_obj_t *artist_name_label;
static lv_obj_t *play_pause_button;
static lv_obj_t *next_button;
static lv_obj_t *prev_button;
static lv_obj_t *progress_arc;
static lv_obj_t *time_label;

static on_music_control_ui_event_cb_t ui_evt_callback;
static bool is_playing;

ZSW_LV_IMG_DECLARE(pause);
ZSW_LV_IMG_DECLARE(play);
ZSW_LV_IMG_DECLARE(next);
ZSW_LV_IMG_DECLARE(previous);

static void create_progress_arc(lv_obj_t *parent)
{
    progress_arc = lv_arc_create(parent);
    lv_arc_set_rotation(progress_arc, 270);
    lv_arc_set_bg_angles(progress_arc, 0, 360);
    lv_arc_set_range(progress_arc, 0, 100);
    lv_arc_set_value(progress_arc, 0);

    lv_obj_remove_style(progress_arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    lv_obj_clear_flag(progress_arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    lv_obj_set_style_arc_width(progress_arc, 3, LV_PART_MAIN); // Changes background arc width
    lv_obj_set_style_arc_width(progress_arc, 3, LV_PART_INDICATOR); // Changes set part width
    lv_obj_set_style_arc_color(progress_arc, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_INDICATOR);

    lv_obj_set_size(progress_arc, 240, 240);
    lv_obj_center(progress_arc);
}

static void play_event_click_cb(lv_event_t *e)
{
    lv_img_header_t header;
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_FOCUSED && obj != play_pause_button) {
        lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);
        lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN);
        lv_obj_set_style_border_color(obj, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN);
    } else if (code == LV_EVENT_DEFOCUSED && obj != play_pause_button) {
        lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN);
    } else if (code == LV_EVENT_RELEASED && obj != play_pause_button) {
        lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN);
    } else if (code == LV_EVENT_CLICKED) {
        if (obj == play_pause_button) {
            if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                ui_evt_callback(MUSIC_CONTROL_UI_PLAY);
            } else {
                ui_evt_callback(MUSIC_CONTROL_UI_PAUSE);
            }
            // Need to update the height after the image changes on imgbtn for some reason.
            lv_img_decoder_get_info(ZSW_LV_IMG_USE(play), &header);
            lv_obj_set_height(play_pause_button, header.h + 5);
        } else if (obj == prev_button) {
            ui_evt_callback(MUSIC_CONTROL_UI_PREV_TRACK);
        } else if (obj == next_button) {
            ui_evt_callback(MUSIC_CONTROL_UI_NEXT_TRACK);
        }
    }
}

static void create_buttons(lv_obj_t *parent)
{
    lv_img_header_t header;
    lv_group_t *group = lv_group_get_default();

    play_pause_button = lv_imgbtn_create(parent);
    lv_obj_clear_flag(play_pause_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_imgbtn_set_src(play_pause_button, LV_IMGBTN_STATE_RELEASED, NULL, ZSW_LV_IMG_USE(play), NULL);
    lv_imgbtn_set_src(play_pause_button, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, ZSW_LV_IMG_USE(pause), NULL);
    lv_img_decoder_get_info(ZSW_LV_IMG_USE(pause), &header);
    lv_obj_set_width(play_pause_button, header.w);
    lv_obj_set_height(play_pause_button, header.h + 5);
    lv_obj_add_flag(play_pause_button, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(play_pause_button, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_center(play_pause_button);
    lv_obj_add_event_cb(play_pause_button, play_event_click_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(group, play_pause_button);

    next_button = lv_imgbtn_create(parent);
    lv_obj_clear_flag(next_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_imgbtn_set_src(next_button, LV_IMGBTN_STATE_RELEASED, NULL, ZSW_LV_IMG_USE(next), NULL);
    lv_img_decoder_get_info(ZSW_LV_IMG_USE(next), &header);
    lv_obj_set_width(next_button, header.w);
    lv_obj_add_flag(next_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(next_button, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_align_to(next_button, play_pause_button, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(next_button, play_event_click_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(group, next_button);

    prev_button = lv_imgbtn_create(parent);
    lv_obj_clear_flag(prev_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_imgbtn_set_src(prev_button, LV_IMGBTN_STATE_RELEASED, NULL, ZSW_LV_IMG_USE(previous), NULL);
    lv_img_decoder_get_info(ZSW_LV_IMG_USE(previous), &header);
    lv_obj_set_width(prev_button, header.w);
    lv_obj_add_flag(prev_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(prev_button, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_align_to(prev_button, play_pause_button, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_obj_add_event_cb(prev_button, play_event_click_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(group, prev_button);

    // To get correct order when navigating the music control buttons they need to be removed and
    // re-add in the order we want.

    lv_group_focus_obj(play_pause_button);
}

void music_control_ui_show(lv_obj_t *root, on_music_control_ui_event_cb_t evt_cb)
{
    assert(root_page == NULL);
    ui_evt_callback = evt_cb;
    is_playing = false;

    // Create the root container
    root_page = lv_obj_create(root);

    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);

    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    create_progress_arc(root_page);

    create_buttons(root_page);

    track_name_label = lv_label_create(root_page);
    lv_obj_align(track_name_label, LV_ALIGN_TOP_MID, 0, 25);
    lv_obj_set_style_text_align(track_name_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(track_name_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(track_name_label, LV_PCT(50));
    lv_label_set_text(track_name_label, "No music playing...");
    lv_obj_set_style_anim_speed(track_name_label, 10, 0);

    artist_name_label = lv_label_create(root_page);
    lv_obj_align(artist_name_label, LV_ALIGN_TOP_MID, 0, 45);
    lv_obj_set_style_text_align(artist_name_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(artist_name_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(artist_name_label, LV_PCT(50));
    lv_label_set_text(artist_name_label, "Artist...");
    lv_obj_set_style_anim_speed(artist_name_label, 10, 0);

    time_label = lv_label_create(root_page);
    lv_obj_align(time_label, LV_ALIGN_TOP_MID, 0, -5);
    lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(time_label, LV_SIZE_CONTENT);
    lv_label_set_text(time_label, "...");
}

void music_control_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
    track_name_label = NULL;
}

void music_control_ui_music_info(char *track, char *artist)
{
    lv_label_set_text(track_name_label, track);
    lv_label_set_text(artist_name_label, artist);
}

void music_control_ui_set_track_progress(int percent_played)
{
    lv_arc_set_value(progress_arc, percent_played);
}

void music_control_ui_set_time(int hour, int min, int second)
{
    lv_label_set_text_fmt(time_label, "%02d:%02d", hour, min);
}

void music_control_ui_set_music_state(bool playing, int percent_played, bool shuffle)
{
    lv_img_header_t header;
    lv_arc_set_value(progress_arc, percent_played);
    if (playing != is_playing) {
        is_playing = playing;
        if (playing) {
            lv_imgbtn_set_state(play_pause_button, LV_IMGBTN_STATE_CHECKED_RELEASED);
        } else {
            lv_imgbtn_set_state(play_pause_button, LV_IMGBTN_STATE_RELEASED);
        }
        // Need to update the height after the image changes on imgbtn for some reason.
        lv_img_decoder_get_info(ZSW_LV_IMG_USE(play), &header);
        lv_obj_set_height(play_pause_button, header.h + 5);
    }
}
