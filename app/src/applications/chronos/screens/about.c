#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/logging/log.h>

#include "../chronos_ui.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(chronos_about, CONFIG_ZSW_BLE_LOG_LEVEL);

ZSW_LV_IMG_DECLARE(image_chronos_icon);

ZSW_LV_IMG_DECLARE(ui_img_previous_png);
ZSW_LV_IMG_DECLARE(ui_img_play_png);
ZSW_LV_IMG_DECLARE(ui_img_forward_png);
ZSW_LV_IMG_DECLARE(ui_img_volume_up_png);
ZSW_LV_IMG_DECLARE(ui_img_volume_down_png);
ZSW_LV_IMG_DECLARE(ui_img_radar_png);


static lv_obj_t *ui_appversion;
static lv_obj_t *ui_appcode;

static lv_obj_t *ui_batterybar;
static lv_obj_t *ui_batterytext;

static lv_obj_t *ui_findimage;
static lv_obj_t *ui_findtext;
static bool find_active;

static lv_anim_t rotate;


void rotate_image_cb(void *var, int32_t v)
{
    lv_img_set_angle(var, v);
}

void rotate_anim_deleted_cb(lv_anim_t *anim)
{
    find_active = false;
    lv_label_set_text(ui_findtext, "Start");
    lv_obj_set_style_img_recolor_opa(ui_findimage, 155, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_img_set_angle(ui_findimage, 2700);
    ble_chronos_find_phone(false);
}

void start_rotate_anim()
{
    lv_anim_init(&rotate);
    lv_anim_set_exec_cb(&rotate, rotate_image_cb);
    lv_anim_set_var(&rotate, ui_findimage);
    lv_anim_set_time(&rotate, 5000);
    lv_anim_set_values(&rotate, 2700, 2700 + 3600);
    lv_anim_set_repeat_count(&rotate, 2);
    lv_anim_set_deleted_cb(&rotate, rotate_anim_deleted_cb);
    lv_anim_start(&rotate);

    lv_label_set_text(ui_findtext, "Stop");
    lv_obj_set_style_img_recolor_opa(ui_findimage, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    ble_chronos_find_phone(true);
}

void stop_rotate_anim()
{
    lv_anim_del(ui_findimage, rotate_image_cb);
}

void music_button_event_cb(lv_event_t *e)
{
    chronos_control_t code = (chronos_control_t)lv_event_get_user_data(e);
    ble_chronos_music_control(code);
}

void find_button_event_cb(lv_event_t *e)
{
    find_active = !find_active;
    if (find_active) {
        start_rotate_anim();
    } else {
        stop_rotate_anim();
    }

}

void chronos_ui_about_init(lv_obj_t *page)
{

    chronos_ui_add_app_title(page, "About", ZSW_LV_IMG_USE(image_chronos_icon));


    lv_obj_t *ui_aboutpanel = lv_obj_create(page);
    lv_obj_set_width(ui_aboutpanel, lv_pct(100));
    lv_obj_set_height(ui_aboutpanel, LV_SIZE_CONTENT);    /// 240
    lv_obj_set_align(ui_aboutpanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_aboutpanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_aboutpanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_aboutpanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_aboutpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_aboutpanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_aboutpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_aboutpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_aboutpanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_aboutpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_apppanel = lv_obj_create(ui_aboutpanel);
    lv_obj_remove_style_all(ui_apppanel);
    lv_obj_set_width(ui_apppanel, 200);
    lv_obj_set_height(ui_apppanel, LV_SIZE_CONTENT);    /// 40
    lv_obj_set_align(ui_apppanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_apppanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_apppanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_apppanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_pad_row(ui_apppanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_apppanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_appname = lv_label_create(ui_apppanel);
    lv_obj_set_width(ui_appname, 200);
    lv_obj_set_height(ui_appname, LV_SIZE_CONTENT);    /// 1
    lv_label_set_text(ui_appname, "Chronos App");
    lv_obj_set_style_text_align(ui_appname, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_appname, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_appcont = lv_obj_create(ui_apppanel);
    lv_obj_remove_style_all(ui_appcont);
    lv_obj_set_width(ui_appcont, 200);
    lv_obj_set_height(ui_appcont, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_appcont, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_appcont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_appversion = lv_label_create(ui_appcont);
    lv_obj_set_width(ui_appversion, 100);
    lv_obj_set_height(ui_appversion, LV_SIZE_CONTENT);    /// 1
    lv_label_set_text(ui_appversion, "--");
    lv_obj_set_style_text_align(ui_appversion, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_appversion, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_appcode = lv_label_create(ui_appcont);
    lv_obj_set_width(ui_appcode, 100);
    lv_obj_set_height(ui_appcode, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_appcode, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(ui_appcode, "v-");
    lv_obj_set_style_text_align(ui_appcode, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_appcode, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_batterypanel = lv_obj_create(ui_aboutpanel);
    lv_obj_remove_style_all(ui_batterypanel);
    lv_obj_set_width(ui_batterypanel, 200);
    lv_obj_set_height(ui_batterypanel, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_batterypanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_batterypanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_batterypanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_batterypanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_color(ui_batterypanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_batterypanel, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_batterypanel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_batterypanel, LV_BORDER_SIDE_TOP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_batterypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_batterypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_batterypanel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_batterypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_batterypanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_batterypanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_batterylabel = lv_label_create(ui_batterypanel);
    lv_obj_set_width(ui_batterylabel, 200);
    lv_obj_set_height(ui_batterylabel, LV_SIZE_CONTENT);    /// 1
    lv_label_set_text(ui_batterylabel, "Phone Battery");
    lv_obj_set_style_text_align(ui_batterylabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_batterylabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_batterybar = lv_bar_create(ui_batterypanel);
    lv_bar_set_value(ui_batterybar, 0, LV_ANIM_OFF);
    lv_bar_set_start_value(ui_batterybar, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_batterybar, 200);
    lv_obj_set_height(ui_batterybar, 15);
    lv_obj_set_align(ui_batterybar, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(ui_batterybar, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_batterybar, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_batterybar, 50, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_batterybar, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_batterybar, lv_color_hex(0x0BFF27), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_batterybar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_batterytext = lv_label_create(ui_batterypanel);
    lv_obj_set_width(ui_batterytext, 200);
    lv_obj_set_height(ui_batterytext, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_batterytext, LV_ALIGN_BOTTOM_RIGHT);
    lv_label_set_text(ui_batterytext, "-%");
    lv_obj_set_style_text_font(ui_batterytext, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_controlpanel = lv_obj_create(page);
    lv_obj_set_width(ui_controlpanel, lv_pct(100));
    lv_obj_set_height(ui_controlpanel, LV_SIZE_CONTENT);    /// 240
    lv_obj_set_align(ui_controlpanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_controlpanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_controlpanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_controlpanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_controlpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_controlpanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_controlpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_controlpanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_controlpanel, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_controlpanel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_controlpanel, LV_BORDER_SIDE_TOP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_controlpanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_controlpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_musicpanel = lv_obj_create(ui_controlpanel);
    lv_obj_remove_style_all(ui_musicpanel);
    lv_obj_set_width(ui_musicpanel, 200);
    lv_obj_set_height(ui_musicpanel, LV_SIZE_CONTENT);    /// 40
    lv_obj_set_align(ui_musicpanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_musicpanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_musicpanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_musicpanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_pad_row(ui_musicpanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_musicpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_musiclabel = lv_label_create(ui_musicpanel);
    lv_obj_set_width(ui_musiclabel, 200);
    lv_obj_set_height(ui_musiclabel, LV_SIZE_CONTENT);    /// 1
    lv_label_set_text(ui_musiclabel, "Music Control");
    lv_obj_set_style_text_align(ui_musiclabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_musiclabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_playcontrols = lv_obj_create(ui_musicpanel);
    lv_obj_remove_style_all(ui_playcontrols);
    lv_obj_set_width(ui_playcontrols, 200);
    lv_obj_set_height(ui_playcontrols, 50);
    lv_obj_set_align(ui_playcontrols, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_playcontrols, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_playcontrols, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_playcontrols, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags


    lv_obj_t *ui_previousbutton = lv_obj_create(ui_playcontrols);
    lv_obj_remove_style_all(ui_previousbutton);
    lv_obj_set_width(ui_previousbutton, 50);
    lv_obj_set_height(ui_previousbutton, 50);
    lv_obj_set_align(ui_previousbutton, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_previousbutton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_previousbutton, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_previousbutton, ZSW_LV_IMG_USE(ui_img_previous_png), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_previousbutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_previousbutton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_previousbutton, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_previousbutton, 25, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_previousbutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_previousbutton, 200, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(ui_previousbutton, music_button_event_cb, LV_EVENT_CLICKED, (void *)CH_CONTROL_MUSIC_PREVIOUS);

    lv_obj_t *ui_playpausebutton = lv_obj_create(ui_playcontrols);
    lv_obj_remove_style_all(ui_playpausebutton);
    lv_obj_set_width(ui_playpausebutton, 50);
    lv_obj_set_height(ui_playpausebutton, 50);
    lv_obj_set_align(ui_playpausebutton, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_playpausebutton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_playpausebutton, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_playpausebutton, ZSW_LV_IMG_USE(ui_img_play_png), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_playpausebutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_playpausebutton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_playpausebutton, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_playpausebutton, 25, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_playpausebutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_playpausebutton, 200, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(ui_playpausebutton, music_button_event_cb, LV_EVENT_CLICKED, (void *)CH_CONTROL_MUSIC_TOGGLE);

    lv_obj_t *ui_nextbutton = lv_obj_create(ui_playcontrols);
    lv_obj_remove_style_all(ui_nextbutton);
    lv_obj_set_width(ui_nextbutton, 50);
    lv_obj_set_height(ui_nextbutton, 50);
    lv_obj_set_align(ui_nextbutton, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_nextbutton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_nextbutton, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_nextbutton, ZSW_LV_IMG_USE(ui_img_forward_png), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_nextbutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_nextbutton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_nextbutton, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_nextbutton, 25, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_nextbutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_nextbutton, 200, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(ui_nextbutton, music_button_event_cb, LV_EVENT_CLICKED, (void *)CH_CONTROL_MUSIC_NEXT);

    lv_obj_t *ui_volumecontrols = lv_obj_create(ui_musicpanel);
    lv_obj_remove_style_all(ui_volumecontrols);
    lv_obj_set_width(ui_volumecontrols, 200);
    lv_obj_set_height(ui_volumecontrols, 50);
    lv_obj_set_align(ui_volumecontrols, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_volumecontrols, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_volumecontrols, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_volumecontrols, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t *ui_volumedownbutton = lv_obj_create(ui_volumecontrols);
    lv_obj_remove_style_all(ui_volumedownbutton);
    lv_obj_set_width(ui_volumedownbutton, 50);
    lv_obj_set_height(ui_volumedownbutton, 50);
    lv_obj_set_align(ui_volumedownbutton, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_volumedownbutton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_volumedownbutton, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_volumedownbutton, ZSW_LV_IMG_USE(ui_img_volume_down_png),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_volumedownbutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_volumedownbutton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_volumedownbutton, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_volumedownbutton, 25, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_volumedownbutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_volumedownbutton, 200, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(ui_volumedownbutton, music_button_event_cb, LV_EVENT_CLICKED, (void *)CH_CONTROL_VOLUME_DOWN);

    lv_obj_t *ui_volumeupbutton = lv_obj_create(ui_volumecontrols);
    lv_obj_remove_style_all(ui_volumeupbutton);
    lv_obj_set_width(ui_volumeupbutton, 50);
    lv_obj_set_height(ui_volumeupbutton, 50);
    lv_obj_set_align(ui_volumeupbutton, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_volumeupbutton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_volumeupbutton, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_volumeupbutton, ZSW_LV_IMG_USE(ui_img_volume_up_png), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_volumeupbutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_volumeupbutton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_volumeupbutton, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_volumeupbutton, 25, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_volumeupbutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_volumeupbutton, 200, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(ui_volumeupbutton, music_button_event_cb, LV_EVENT_CLICKED, (void *)CH_CONTROL_VOLUME_UP);

    lv_obj_t *ui_findphonepanel = lv_obj_create(ui_controlpanel);
    lv_obj_remove_style_all(ui_findphonepanel);
    lv_obj_set_width(ui_findphonepanel, 200);
    lv_obj_set_height(ui_findphonepanel, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_findphonepanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_findphonepanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_findphonepanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_findphonepanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_color(ui_findphonepanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_findphonepanel, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_findphonepanel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_findphonepanel, LV_BORDER_SIDE_TOP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_findphonepanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_findphonepanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_findphonepanel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_findphonepanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_findphonepanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_findphonepanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_findlabel = lv_label_create(ui_findphonepanel);
    lv_obj_set_width(ui_findlabel, 200);
    lv_obj_set_height(ui_findlabel, LV_SIZE_CONTENT);    /// 1
    lv_label_set_text(ui_findlabel, "Find Phone");
    lv_obj_set_style_text_align(ui_findlabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_findlabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_findimage = lv_img_create(ui_findphonepanel);
    lv_img_set_src(ui_findimage, ZSW_LV_IMG_USE(ui_img_radar_png));
    lv_obj_set_width(ui_findimage, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_findimage, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_findimage, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_findimage, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_findimage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_angle(ui_findimage, 2700);
    lv_obj_set_style_img_recolor(ui_findimage, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_findimage, 155, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_findbutton = lv_obj_create(ui_findphonepanel);
    lv_obj_remove_style_all(ui_findbutton);
    lv_obj_set_width(ui_findbutton, 150);
    lv_obj_set_height(ui_findbutton, 40);
    lv_obj_set_align(ui_findbutton, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_findbutton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_findbutton, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_findbutton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_findbutton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_findbutton, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_findbutton, 20, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_findbutton, lv_color_hex(0xD0D0D0), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_findbutton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(ui_findbutton, find_button_event_cb, LV_EVENT_CLICKED, NULL);

    ui_findtext = lv_label_create(ui_findbutton);
    lv_obj_set_width(ui_findtext, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_findtext, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_findtext, LV_ALIGN_CENTER);
    lv_label_set_text(ui_findtext, "Start");
    lv_obj_set_style_text_font(ui_findtext, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_obj_t *ui_morepanel = lv_obj_create(ui_controlpanel);
    lv_obj_remove_style_all(ui_morepanel);
    lv_obj_set_width(ui_morepanel, 200);
    lv_obj_set_height(ui_morepanel, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_morepanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_morepanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_morepanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_morepanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_border_color(ui_morepanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_morepanel, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_morepanel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_morepanel, LV_BORDER_SIDE_TOP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_morepanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_morepanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_morepanel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_morepanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_morepanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_morepanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_morelabel = lv_label_create(ui_morepanel);
    lv_obj_set_width(ui_morelabel, 200);
    lv_obj_set_height(ui_morelabel, LV_SIZE_CONTENT);    /// 1
    lv_label_set_text(ui_morelabel, "More apps available on the right. Swipe to explore");
    lv_obj_set_style_text_font(ui_morelabel, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);


}

void chronos_ui_set_app_info(chronos_app_info_t info)
{
    lv_label_set_text(ui_appversion, info.version);
    lv_label_set_text_fmt(ui_appcode, "v%d", info.code);
}

void chronos_ui_set_phone_info(chronos_phone_info_t info)
{
    lv_bar_set_value(ui_batterybar, info.level, LV_ANIM_OFF);
    lv_label_set_text_fmt(ui_batterytext, info.state == 1 ? "%d%% - Charging" : "%d%%", info.level);
}