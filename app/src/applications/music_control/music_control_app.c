#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zsw_clock.h>
#include <zephyr/zbus/zbus.h>

#ifdef CONFIG_BLE_USES_AMS
#include "ble/ble_ams.h"
#endif

#include "music_control_ui.h"
#include "ble/ble_comm.h"
#include "events/ble_data_event.h"
#include "managers/zsw_app_manager.h"

// Functions needed for all applications
static void music_control_app_start(lv_obj_t *root, lv_group_t *group);
static void music_control_app_stop(void);

static void timer_callback(lv_timer_t *timer);
static void on_music_ui_evt_music(music_control_ui_evt_type_t evt_type);
static void zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void handle_update_ui(struct k_work *item);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(music_app_ble_comm_lis, zbus_ble_comm_data_callback);

static K_WORK_DEFINE(update_ui_work, handle_update_ui);
static ble_comm_music_info_t last_music_info;
static ble_comm_music_state_t last_music_state = {.position = -1};

LV_IMG_DECLARE(music);

static application_t app = {
    .name = "Music",
    .icon = &music,
    .start_func = music_control_app_start,
    .stop_func = music_control_app_stop
};

static lv_timer_t *progress_timer;
static int progress_seconds;
static bool running;
static bool playing;

static void music_control_app_start(lv_obj_t *root, lv_group_t *group)
{
    progress_timer = lv_timer_create(timer_callback, 1000,  NULL);
    music_control_ui_show(root, on_music_ui_evt_music);
    running = true;
    handle_update_ui(NULL);
}

static void music_control_app_stop(void)
{
    lv_timer_del(progress_timer);
    running = false;
    music_control_ui_remove();
}

static void on_music_ui_evt_music(music_control_ui_evt_type_t evt_type)
{
    uint8_t buf[50];
    int msg_len = 0;

#if defined(CONFIG_BLE_USES_GADGETBRIDGE)

    switch (evt_type) {
        case MUSIC_CONTROL_UI_CLOSE:
            zsw_app_manager_app_close_request(&app);
            break;
        case MUSIC_CONTROL_UI_PLAY:
            msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"music\", \"n\": %s} \n", "play");
            break;
        case MUSIC_CONTROL_UI_PAUSE:
            msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"music\", \"n\": %s} \n", "pause");
            break;
        case MUSIC_CONTROL_UI_NEXT_TRACK:
            msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"music\", \"n\": %s} \n", "next");
            break;
        case MUSIC_CONTROL_UI_PREV_TRACK:
            msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"music\", \"n\": %s} \n", "previous");
            break;
    }
    if (msg_len > 0) {
        ble_comm_send(buf, msg_len);
    }

#elif defined(CONFIG_BLE_USES_AMS)

    switch (evt_type) {
        case MUSIC_CONTROL_UI_CLOSE:
            zsw_app_manager_app_close_request(&app);
            break;
        case MUSIC_CONTROL_UI_PLAY:
            ble_ams_play_pause();
            break;
        case MUSIC_CONTROL_UI_PAUSE:
            ble_ams_play_pause();
            break;
        case MUSIC_CONTROL_UI_NEXT_TRACK:
            ble_ams_next_track();
            break;
        case MUSIC_CONTROL_UI_PREV_TRACK:
            ble_ams_previous_track();
            break;
    }

#endif // CONFIG_BLE_USES_AMS
}

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    // Need to context switch to not get stack overflow.
    // We are here in host bluetooth thread.
    const struct ble_data_event *event = zbus_chan_const_msg(chan);
    if (event->data.type == BLE_COMM_DATA_TYPE_MUSTIC_INFO) {
        memcpy(&last_music_info, &event->data.data.music_info, sizeof(ble_comm_music_info_t));
        k_work_submit(&update_ui_work);
    } else if (event->data.type == BLE_COMM_DATA_TYPE_MUSTIC_STATE) {
        memcpy(&last_music_state, &event->data.data.music_state, sizeof(ble_comm_music_state_t));
        k_work_submit(&update_ui_work);
    }
}

static void handle_update_ui(struct k_work *item)
{
    char buf[5 * MAX_MUSIC_FIELD_LENGTH];
    if (running) {
        if (strlen(last_music_info.track_name) > 0) {
            snprintf(buf, sizeof(buf), "Track: %s", last_music_info.track_name);
            progress_seconds = 0;
            music_control_ui_music_info(last_music_info.track_name, last_music_info.artist);
            music_control_ui_set_track_progress(0);
        }

        if (last_music_state.position >= 0) {
            music_control_ui_set_music_state(last_music_state.playing,
                                             (((float)last_music_state.position / (float)last_music_info.duration)) * 100,
                                             last_music_state.shuffle);
            progress_seconds = last_music_state.position;
            playing = last_music_state.playing;
        }
    }
}

static void timer_callback(lv_timer_t *timer)
{
    struct tm *time = zsw_clock_get_time();
    music_control_ui_set_time(time->tm_hour, time->tm_min, time->tm_sec);
    if (playing) {
        progress_seconds++;
        music_control_ui_set_track_progress((((float)progress_seconds / (float)last_music_info.duration)) * 100);
    }
}

static int music_control_app_add(void)
{
    zsw_app_manager_add_application(&app);
    running = false;

    return 0;
}

SYS_INIT(music_control_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
