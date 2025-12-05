/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zsw_clock.h>
#include <zephyr/zbus/zbus.h>
#include <stdio.h>

#include "lvgl_editor_gen.h"
#include "music_app_gen.h"
#include "ble/ble_comm.h"
#include "events/ble_event.h"
#include "events/music_event.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

// Functions needed for all applications
static void music_control_app_start(lv_obj_t *root, lv_group_t *group);
static void music_control_app_stop(void);

static void timer_callback(lv_timer_t *timer);
static void zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void handle_update_ui(struct k_work *item);
static void app_on_ui_available(void);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);

ZBUS_CHAN_DECLARE(music_control_data_chan);
ZBUS_LISTENER_DEFINE(music_app_ble_comm_lis, zbus_ble_comm_data_callback);

static K_WORK_DEFINE(update_ui_work, handle_update_ui);
static ble_comm_music_info_t last_music_info;
static ble_comm_music_state_t last_music_state = {.position = -1};

ZSW_LV_IMG_DECLARE(music);

static application_t app = {
    .name = "Music",
    .icon = ZSW_LV_IMG_USE(music),
    .start_func = music_control_app_start,
    .stop_func = music_control_app_stop,
    .ui_available_func = app_on_ui_available,
    .category = ZSW_APP_CATEGORY_ROOT,
};

static lv_timer_t *progress_timer;
static int progress_seconds;
static bool playing;
static lv_obj_t *music_ui_root;

// Event callbacks referenced in the generated UI code
void music_on_prev_clicked(lv_event_t *e)
{
    LV_UNUSED(e);
    struct music_event music_event = {
        .control_type = MUSIC_CONTROL_UI_PREV_TRACK,
    };
    zbus_chan_pub(&music_control_data_chan, &music_event, K_MSEC(50));
}

void music_on_play_pause_clicked(lv_event_t *e)
{
    LV_UNUSED(e);
    // Toggle playing state via subject
    int current_playing = lv_subject_get_int(&music_playing);
    lv_subject_set_int(&music_playing, current_playing ? 0 : 1);
    playing = !current_playing;

    // Send event to BLE
    struct music_event music_event = {
        .control_type = current_playing ? MUSIC_CONTROL_UI_PAUSE : MUSIC_CONTROL_UI_PLAY,
    };
    zbus_chan_pub(&music_control_data_chan, &music_event, K_MSEC(50));
}

void music_on_next_clicked(lv_event_t *e)
{
    LV_UNUSED(e);
    struct music_event music_event = {
        .control_type = MUSIC_CONTROL_UI_NEXT_TRACK,
    };
    zbus_chan_pub(&music_control_data_chan, &music_event, K_MSEC(50));
}

static void music_control_app_start(lv_obj_t *root, lv_group_t *group)
{
    progress_timer = lv_timer_create(timer_callback, 1000, NULL);

    // Create the music app UI - images come from globals.xml
    music_ui_root = music_app_create(root);

    handle_update_ui(NULL);
}

static void music_control_app_stop(void)
{
    lv_timer_del(progress_timer);
    lv_obj_del(music_ui_root);
}

static void app_on_ui_available(void)
{
    // When UI becomes available, update it with the latest info.
    handle_update_ui(NULL);
}

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    // Need to context switch to not get stack overflow.
    // We are here in host bluetooth thread.
    const struct ble_data_event *event = zbus_chan_const_msg(chan);
    if (event->data.type == BLE_COMM_DATA_TYPE_MUSIC_INFO) {
        memcpy(&last_music_info, &event->data.data.music_info, sizeof(ble_comm_music_info_t));
        k_work_submit(&update_ui_work);
    } else if (event->data.type == BLE_COMM_DATA_TYPE_MUSIC_STATE) {
        memcpy(&last_music_state, &event->data.data.music_state, sizeof(ble_comm_music_state_t));
        k_work_submit(&update_ui_work);
    }
}

static void handle_update_ui(struct k_work *item)
{
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        if (strlen(last_music_info.track_name) > 0) {
            progress_seconds = 0;

            // Update subjects instead of calling UI functions
            lv_subject_copy_string(&music_track_name, last_music_info.track_name);
            lv_subject_copy_string(&music_artist_name, last_music_info.artist);
            lv_subject_set_int(&music_progress, 0);
        }

        if (last_music_state.position >= 0 && last_music_info.duration > 0) {
            int progress_percent = (((float)last_music_state.position / (float)last_music_info.duration)) * 100;

            // Update subjects
            lv_subject_set_int(&music_playing, last_music_state.playing ? 1 : 0);
            lv_subject_set_int(&music_progress, progress_percent);

            progress_seconds = last_music_state.position;
            playing = last_music_state.playing;
        }
    }
}

static void timer_callback(lv_timer_t *timer)
{
    zsw_timeval_t time;
    zsw_clock_get_time(&time);

    // Update time via subject
    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", time.tm.tm_hour, time.tm.tm_min);
    lv_subject_copy_string(&music_time, time_str);

    if (playing && last_music_info.duration > 0) {
        progress_seconds++;
        int progress_percent = (((float)progress_seconds / (float)last_music_info.duration)) * 100;
        lv_subject_set_int(&music_progress, progress_percent);
    }
}

static int music_control_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(music_control_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
