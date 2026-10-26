/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

/*
 * Glanceable tiles, opened by swiping right on the watchface: weather, steps,
 * heart rate, music and battery. Tapping a tile opens its app.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "tiles_ui.h"
#include "ble/ble_comm.h"
#include "events/ble_event.h"
#include "events/battery_event.h"
#include "events/music_event.h"
#include "managers/zsw_app_manager.h"
#include "managers/zsw_hr_manager.h"
#include "sensors/zsw_imu.h"

LOG_MODULE_REGISTER(tiles_app, CONFIG_ZSW_TILES_APP_LOG_LEVEL);

#define REFRESH_INTERVAL_MS 1000
#define STEP_GOAL           10000

static void tiles_app_start(lv_obj_t *root, lv_group_t *group);
static void tiles_app_stop(void);
static void tiles_app_ui_available(void);
static void on_ble_data(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_CHAN_DECLARE(music_control_data_chan);
ZBUS_LISTENER_DEFINE(tiles_app_ble_lis, on_ble_data);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, tiles_app_ble_lis, 1);

static application_t app = {
    .name = "Tiles",
    .hidden = true,
    .start_func = tiles_app_start,
    .stop_func = tiles_app_stop,
    .ui_available_func = tiles_app_ui_available,
    .category = ZSW_APP_CATEGORY_SYSTEM,
};

/* Weather and music only arrive as events, so they are kept while the tiles are closed. */
static struct k_spinlock cache_lock;
static tiles_data_t cache;
static tiles_data_t data;
static lv_timer_t *refresh_timer;

static void on_ble_data(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);
    k_spinlock_key_t key = k_spin_lock(&cache_lock);

    switch (event->data.type) {
        case BLE_COMM_DATA_TYPE_WEATHER:
            cache.weather_valid = true;
            cache.temperature_c = event->data.data.weather.temperature_c;
            cache.weather_code = event->data.data.weather.weather_code;
            cache.humidity = event->data.data.weather.humidity;
            strncpy(cache.weather_text, event->data.data.weather.report_text, sizeof(cache.weather_text) - 1);
            break;
        case BLE_COMM_DATA_TYPE_MUSIC_INFO:
            cache.music_valid = event->data.data.music_info.track_name[0] != '\0';
            strncpy(cache.track, event->data.data.music_info.track_name, sizeof(cache.track) - 1);
            strncpy(cache.artist, event->data.data.music_info.artist, sizeof(cache.artist) - 1);
            break;
        case BLE_COMM_DATA_TYPE_MUSIC_STATE:
            cache.music_playing = event->data.data.music_state.playing;
            break;
        default:
            break;
    }
    k_spin_unlock(&cache_lock, key);
}

static void collect(void)
{
    k_spinlock_key_t key = k_spin_lock(&cache_lock);
    memcpy(&data, &cache, sizeof(data));
    k_spin_unlock(&cache_lock, key);

    data.step_goal = STEP_GOAL;
    if (zsw_imu_fetch_num_steps(&data.steps) != 0) {
        data.steps = 0;
    }

    struct battery_sample_event battery;
    if (zbus_chan_read(&battery_sample_data_chan, &battery, K_MSEC(10)) == 0) {
        data.battery_percent = battery.percent;
        data.battery_mv = battery.mV;
        data.battery_charging = battery.is_charging;
        float seconds = battery.is_charging ? battery.ttf : battery.tte;
        data.battery_minutes_left = battery.pmic_data_valid && seconds > 0 ? (int)(seconds / 60) : -1;
    }

#ifdef CONFIG_ZSW_HR_MANAGER
    struct zsw_hr_manager_snapshot snapshot;
    struct zsw_hr_history_point history[TILES_HR_POINTS];

    zsw_hr_manager_get_snapshot(&snapshot);
    data.hr_available = snapshot.status != ZSW_HR_STATUS_UNAVAILABLE;
    data.hr_bpm = snapshot.has_sample ? snapshot.sample.heart_rate_bpm : 0;
    data.hr_age_s = snapshot.has_sample ? (k_uptime_get() - snapshot.sample.timestamp_ms) / 1000 : 0;
    size_t num = zsw_hr_manager_get_history(history, ARRAY_SIZE(history));
    data.hr_num_points = num;
    for (size_t i = 0; i < num; i++) {
        data.hr_points[i] = history[i].bpm;
    }
#endif
}

static void refresh(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    collect();
    tiles_ui_update(&data);
}

static void on_open(tiles_tile_t tile)
{
    static const char *const apps[TILE_COUNT] = {
        [TILE_WEATHER] = "Weather",
        [TILE_ACTIVITY] = "Fitness",
        [TILE_HEART_RATE] = "HR",
        [TILE_MUSIC] = "Music",
        [TILE_BATTERY] = "Battery",
    };

    if (zsw_app_manager_switch_app(apps[tile]) != 0) {
        LOG_WRN("No app %s", apps[tile]);
    }
}

static void on_music(tiles_music_action_t action)
{
    struct music_event event;

    switch (action) {
        case TILES_MUSIC_PREVIOUS:
            event.control_type = MUSIC_CONTROL_UI_PREV_TRACK;
            break;
        case TILES_MUSIC_NEXT:
            event.control_type = MUSIC_CONTROL_UI_NEXT_TRACK;
            break;
        default:
            event.control_type = data.music_playing ? MUSIC_CONTROL_UI_PAUSE : MUSIC_CONTROL_UI_PLAY;
            // Shown at once, the phone confirms with a music state later.
            k_spinlock_key_t key = k_spin_lock(&cache_lock);
            cache.music_playing = !data.music_playing;
            k_spin_unlock(&cache_lock, key);
            break;
    }
    zbus_chan_pub(&music_control_data_chan, &event, K_MSEC(50));
    refresh(NULL);
}

static void tiles_app_start(lv_obj_t *root, lv_group_t *group)
{
    collect();
    tiles_ui_show(root, group, on_open, on_music, &data);
    refresh_timer = lv_timer_create(refresh, REFRESH_INTERVAL_MS, NULL);
}

static void tiles_app_stop(void)
{
    if (refresh_timer) {
        lv_timer_delete(refresh_timer);
        refresh_timer = NULL;
    }
    tiles_ui_remove();
}

static void tiles_app_ui_available(void)
{
    refresh(NULL);
}

static int tiles_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(tiles_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
