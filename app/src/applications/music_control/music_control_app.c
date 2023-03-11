#include <music_control/music_control_ui.h>
#include <application_manager.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <events/ble_data_event.h>
#include <clock.h>
#include <ble_comm.h>
#include <zephyr/zbus/zbus.h>


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
static ble_comm_cb_data_t last_music_update;

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
static int track_duration;

static void music_control_app_start(lv_obj_t *root, lv_group_t *group)
{
    progress_timer = lv_timer_create(timer_callback, 1000,  NULL);
    music_control_ui_show(root, on_music_ui_evt_music);
    running = true;
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

    switch (evt_type) {
        case MUSIC_CONTROL_UI_CLOSE:
            application_manager_app_close_request(&app);
            break;
        case MUSIC_CONTROL_UI_PLAY:
            msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"music\", \"n\": %s} \n", "play");
            playing = true;
            break;
        case MUSIC_CONTROL_UI_PAUSE:
            msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"music\", \"n\": %s} \n", "pause");
            playing = false;
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
}

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    // Need to context switch to not get stack overflow.
    // We are here in host bluetooth thread.
    struct ble_data_event *event = zbus_chan_msg(chan);
    memcpy(&last_music_update, &event->data, sizeof(ble_comm_cb_data_t));
    k_work_submit(&update_ui_work);
}

static void handle_update_ui(struct k_work *item)
{
    char buf[5 * MAX_MUSIC_FIELD_LENGTH];
    if (running) {
        if (last_music_update.type == BLE_COMM_DATA_TYPE_MUSTIC_INFO) {
            snprintf(buf, sizeof(buf), "Track: %s",
                     last_music_update.data.music_info.track_name);
            progress_seconds = 0;
            track_duration = last_music_update.data.music_info.duration;
            music_control_ui_music_info(last_music_update.data.music_info.track_name, last_music_update.data.music_info.artist);
            music_control_ui_set_track_progress(0);
            playing = true;
        }

        if (last_music_update.type == BLE_COMM_DATA_TYPE_MUSTIC_STATE) {
            music_control_ui_set_music_state(last_music_update.data.music_state.playing,
                                             (((float)last_music_update.data.music_state.position / (float)track_duration)) * 100, last_music_update.data.music_state.shuffle);
            progress_seconds = last_music_update.data.music_state.position;
            playing = last_music_update.data.music_state.playing;
        }
    }
}

static void timer_callback(lv_timer_t *timer)
{
    struct tm *time = clock_get_time();
    music_control_ui_set_time(time->tm_hour, time->tm_min, time->tm_sec);
    if (playing) {
        progress_seconds++;
        music_control_ui_set_track_progress((((float)progress_seconds / (float)track_duration)) * 100);
    }
}

static int music_control_app_add(const struct device *arg)
{
    application_manager_add_application(&app);
    running = false;

    return 0;
}

SYS_INIT(music_control_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
