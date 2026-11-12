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

#include <lvgl.h>
#include <misc/lv_area_private.h>
#include <draw/lv_draw_private.h>
#include <core/lv_obj_event_private.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/zbus/zbus.h>

#include "ui/utils/zsw_ui_utils.h"
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"
#include "applications/watchface/watchface_app.h"
#include "events/ble_event.h"
#include "sensors/zsw_imu.h"
#include "zsw_clock.h"
#include "zsw_weather.h"
#include "diorama_scene.h"
#include "diorama_sky.h"
#include "zsw_watchface_diorama.h"

LOG_MODULE_REGISTER(watchface_diorama, LOG_LEVEL_INF);

#define FRAME_PERIOD_MS         40
/* Every this many frames the accelerometer is read */
#define TILT_EVERY_FRAMES       2
/* Tilting the watch this far from flat moves the layers all the way, of 127 for upright */
#define TILT_FULL               70
/* The tilt has to change by this much, of 127, for the layers to follow */
#define TILT_MIN_CHANGE         4
/*
 * How the accelerometer axes map to the display: x to the right and y down, for a watch that
 * lies flat with the display up.
 */
#define ACCEL_TO_RIGHT(x, y)    (x)
#define ACCEL_TO_DOWN(x, y)     (-(y))
#define ACCEL_1G                9.81f

/* When the display turns on the layers rise, and the sky runs through the last hours */
#define WAKE_MS                 900
#define WAKE_REPLAY_MIN         240

#define SETTINGS_PLACE          "diorama/place"
/* A new position is only stored when it is this far (degrees) from the stored one */
#define PLACE_MIN_MOVE          0.5f

#define COLOR_TEXT              0xFFFFFF
#define COLOR_SHADOW            0x000000

ZSW_LV_IMG_DECLARE(face_diorama_preview);

typedef struct {
    float latitude;
    float longitude;
} place_t;

static void reset_cached_values(void);
static void on_zbus_ble_data(const struct zbus_channel *chan);
static void store_place_work(struct k_work *work);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(diorama_ble_comm_lis, on_zbus_ble_data);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, diorama_ble_comm_lis, 1);
static K_WORK_DEFINE(place_work, store_place_work);

static lv_obj_t *root_page;
static lv_obj_t *scene_obj;
static lv_obj_t *ui_time;
static lv_obj_t *ui_time_shadow;
static lv_obj_t *ui_date;
static lv_obj_t *ui_temperature;
static lv_obj_t *ui_steps;
static lv_obj_t *ui_battery;
static lv_obj_t *ui_sun_event;
static lv_timer_t *frame_timer;
static bool face_active;
static bool paused;

/* Where the watch is, as last told by the phone */
static place_t place;
static bool has_place;
static bool place_loaded;

static diorama_view_t view;
static diorama_sky_events_t sun_events;
static uint32_t wake_time;
static bool waking;
static uint8_t tilt_frames;
static int16_t tilt_x;
static int16_t tilt_y;
static int8_t tilt_target_x;
static int8_t tilt_target_y;

static struct {
    bool enabled;
    int minute_of_day;
    int tilt_x;
    int tilt_y;
} override;

static int last_minute = -1;
static int last_hour = -1;
static int last_day = -1;
static int last_steps = -1;
static int last_battery = -1;
static int last_temperature = INT32_MIN;

#if LV_USE_OS != LV_OS_NONE
#error "The Diorama watchface draws into the layer from the event and needs LVGL without a draw thread"
#endif

/* The landscape is rendered straight into the draw buffer, it is the background of the face */
static void scene_draw_cb(lv_event_t *e)
{
    lv_layer_t *layer = lv_event_get_layer(e);
    lv_area_t coords;
    lv_area_t clip;

    lv_obj_get_coords(scene_obj, &coords);
    if (!lv_area_intersect(&clip, &layer->_clip_area, &coords)) {
        return;
    }
    if (layer->color_format != LV_COLOR_FORMAT_RGB565) {
        LV_LOG_WARN("Can't draw the landscape into this layer");
        return;
    }

    diorama_scene_render(clip.x1 - coords.x1, clip.y1 - coords.y1, clip.x2 - coords.x1, clip.y2 - coords.y1,
                         lv_draw_layer_go_to_xy(layer, clip.x1 - layer->buf_area.x1, clip.y1 - layer->buf_area.y1),
                         layer->draw_buf->header.stride);
}

/* The landscape fills its object: nothing behind it needs to be drawn */
static void scene_cover_cb(lv_event_t *e)
{
    lv_cover_check_info_t *info = lv_event_get_param(e);
    lv_area_t coords;

    lv_obj_get_coords(scene_obj, &coords);
    if (info->res != LV_COVER_RES_MASKED && lv_area_is_in(info->area, &coords, 0)) {
        info->res = LV_COVER_RES_COVER;
    }
}

static void scene_dirty(int x1, int y1, int x2, int y2)
{
    lv_area_t coords;
    lv_area_t area;

    lv_obj_get_coords(scene_obj, &coords);
    area.x1 = coords.x1 + x1;
    area.y1 = coords.y1 + y1;
    area.x2 = coords.x1 + x2;
    area.y2 = coords.y1 + y2;
    lv_obj_invalidate_area(scene_obj, &area);
}

/* Where the sun and the moon are, minutes_ago before now */
static void compute_sky(int minutes_ago, diorama_sky_events_t *events)
{
    zsw_timeval_t now;
    struct tm local;
    struct tm utc;
    time_t epoch;
    diorama_sky_time_t when;

    zsw_clock_get_time(&now);
    zsw_timeval_to_tm(&now, &local);
    if (local.tm_year >= 1900) {
        local.tm_year -= 1900;
    }
    local.tm_isdst = -1;
    epoch = mktime(&local);
    gmtime_r(&epoch, &utc);

    when.year = local.tm_year + 1900;
    when.month = local.tm_mon + 1;
    when.day = local.tm_mday;
    when.minute_of_day = local.tm_hour * 60 + local.tm_min;
    when.utc_offset_min = when.minute_of_day - (utc.tm_hour * 60 + utc.tm_min);
    if (when.utc_offset_min > 14 * 60) {
        when.utc_offset_min -= 24 * 60;
    } else if (when.utc_offset_min < -12 * 60) {
        when.utc_offset_min += 24 * 60;
    }
    if (override.enabled) {
        when.minute_of_day = override.minute_of_day;
    }
    /* Going back over midnight keeps the date: a day makes no difference that can be seen */
    when.minute_of_day = (when.minute_of_day - minutes_ago + 24 * 60) % (24 * 60);
    when.has_place = has_place;
    when.latitude = place.latitude;
    when.longitude = place.longitude;

    diorama_sky_compute(&when, &view, events);
}

static void show_sun_event(void)
{
    int now = override.enabled ? override.minute_of_day : last_hour * 60 + last_minute;
    bool day;

    if (!has_place || sun_events.sunrise < 0 || last_hour < 0) {
        lv_label_set_text(ui_sun_event, "");
        return;
    }
    day = now >= sun_events.sunrise && now < sun_events.sunset;
    lv_label_set_text_fmt(ui_sun_event, "%s %02d:%02d", day ? "SUNSET" : "SUNRISE",
                          (day ? sun_events.sunset : sun_events.sunrise) / 60,
                          (day ? sun_events.sunset : sun_events.sunrise) % 60);
}

static void update_sky(void)
{
    compute_sky(0, &sun_events);
    show_sun_event();
}

static void apply_view(void)
{
    view.tilt_x = (int8_t)(tilt_x / 16);
    view.tilt_y = (int8_t)(tilt_y / 16);
    if (diorama_scene_set_view(&view)) {
        lv_obj_invalidate(scene_obj);
    }
}

static void start_wake(void)
{
    waking = true;
    wake_time = lv_tick_get();
    view.rise = 0;
    compute_sky(WAKE_REPLAY_MIN, NULL);
}

static void read_tilt(void)
{
    float x;
    float y;
    float z;

    if (override.enabled) {
        tilt_target_x = (int8_t)override.tilt_x;
        tilt_target_y = (int8_t)override.tilt_y;
    } else if (zsw_imu_fetch_accel_f(&x, &y, &z) == 0) {
        int new_x = CLAMP((int)(ACCEL_TO_RIGHT(x, y) * 127.0f * 127.0f / (ACCEL_1G * TILT_FULL)), -127, 127);
        int new_y = CLAMP((int)(ACCEL_TO_DOWN(x, y) * 127.0f * 127.0f / (ACCEL_1G * TILT_FULL)), -127, 127);

        /* A hand is never quite still: small changes are left out */
        if (abs(new_x - tilt_target_x) >= TILT_MIN_CHANGE) {
            tilt_target_x = (int8_t)new_x;
        }
        if (abs(new_y - tilt_target_y) >= TILT_MIN_CHANGE) {
            tilt_target_y = (int8_t)new_y;
        }
    }
}

static void frame_timer_cb(lv_timer_t *timer)
{
    ARG_UNUSED(timer);

    if (++tilt_frames >= TILT_EVERY_FRAMES) {
        tilt_frames = 0;
        read_tilt();
    }
    /* The layers follow the hand softly. Kept in 1/16 steps so that they come all the way. */
    tilt_x += (tilt_target_x * 16 - tilt_x) / 4;
    tilt_y += (tilt_target_y * 16 - tilt_y) / 4;

    if (waking) {
        uint32_t elapsed = lv_tick_elaps(wake_time);

        if (elapsed >= WAKE_MS) {
            waking = false;
            view.rise = 255;
            compute_sky(0, NULL);
        } else {
            /* Fast at first, then settling */
            int32_t left = 256 - (int32_t)(elapsed * 256 / WAKE_MS);
            int32_t eased = 256 - ((left * left >> 8) * left >> 8);

            view.rise = (uint8_t)CLAMP(eased, 0, 255);
            compute_sky(WAKE_REPLAY_MIN * (256 - eased) / 256, NULL);
        }
    }

    apply_view();
    diorama_scene_step(FRAME_PERIOD_MS, scene_dirty);
}

static void update_timer_state(void)
{
    if (!frame_timer) {
        return;
    }

    if (face_active && !paused) {
        lv_timer_resume(frame_timer);
    } else {
        lv_timer_pause(frame_timer);
    }
}

static int load_place_cb(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg, void *param)
{
    ARG_UNUSED(key);
    ARG_UNUSED(param);

    if (len == sizeof(place) && read_cb(cb_arg, &place, sizeof(place)) == sizeof(place)) {
        has_place = true;
    }

    return 0;
}

static void store_place_work(struct k_work *work)
{
    ARG_UNUSED(work);

    if (settings_save_one(SETTINGS_PLACE, &place, sizeof(place)) != 0) {
        LOG_WRN("Could not store the position");
    }
}

/* Runs in the thread that published the position: only take a copy */
static void on_zbus_ble_data(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);
    float latitude;
    float longitude;

    if (event->data.type != BLE_COMM_DATA_TYPE_GPS) {
        return;
    }
    latitude = (float)event->data.data.gps.lat;
    longitude = (float)event->data.data.gps.lon;
    if (!isfinite(latitude) || !isfinite(longitude) || latitude < -90.0f || latitude > 90.0f || longitude < -180.0f ||
        longitude > 180.0f) {
        return;
    }
    if (has_place && fabsf(latitude - place.latitude) < PLACE_MIN_MOVE &&
        fabsf(longitude - place.longitude) < PLACE_MIN_MOVE) {
        return;
    }
    place.latitude = latitude;
    place.longitude = longitude;
    has_place = true;
    k_work_submit(&place_work);
}

static lv_obj_t *info_label_create(lv_align_t align, int32_t x, int32_t y)
{
    lv_obj_t *label = zsw_ui_label_create(root_page, "", ZSW_FONT_TEXT, lv_color_hex(COLOR_TEXT));

    lv_obj_align(label, align, x, y);

    return label;
}

static void watchface_show(lv_obj_t *parent, watchface_app_evt_listener evt_cb, zsw_settings_watchface_t *settings)
{
    ARG_UNUSED(evt_cb);
    ARG_UNUSED(settings);

    if (!place_loaded) {
        place_loaded = true;
        settings_load_subtree_direct(SETTINGS_PLACE, load_place_cb, NULL);
    }

    lv_obj_remove_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    root_page = lv_obj_create(parent);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_size(root_page, DIORAMA_SIZE, DIORAMA_SIZE);
    lv_obj_center(root_page);
    lv_obj_remove_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    reset_cached_values();
    face_active = true;
    paused = false;
    tilt_frames = 0;
    tilt_x = 0;
    tilt_y = 0;

    diorama_scene_init(k_cycle_get_32());

    scene_obj = lv_obj_create(root_page);
    lv_obj_remove_style_all(scene_obj);
    lv_obj_set_size(scene_obj, DIORAMA_SIZE, DIORAMA_SIZE);
    lv_obj_center(scene_obj);
    lv_obj_remove_flag(scene_obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(scene_obj, scene_draw_cb, LV_EVENT_DRAW_MAIN, NULL);
    lv_obj_add_event_cb(scene_obj, scene_cover_cb, LV_EVENT_COVER_CHECK, NULL);

    ui_date = info_label_create(LV_ALIGN_TOP_MID, 0, 16);
    lv_obj_set_style_text_letter_space(ui_date, 1, LV_PART_MAIN);

    /* A soft shadow keeps the time readable in front of clouds and a bright sky */
    ui_time_shadow = zsw_ui_label_create(root_page, "", ZSW_FONT_NUMBER, lv_color_hex(COLOR_SHADOW));
    lv_obj_set_style_text_opa(ui_time_shadow, LV_OPA_30, LV_PART_MAIN);
    lv_obj_align(ui_time_shadow, LV_ALIGN_TOP_MID, 1, 36);
    ui_time = zsw_ui_label_create(root_page, "", ZSW_FONT_NUMBER, lv_color_hex(COLOR_TEXT));
    lv_obj_align(ui_time, LV_ALIGN_TOP_MID, 0, 34);

    /* On the lake */
    ui_temperature = info_label_create(LV_ALIGN_TOP_MID, -58, 190);
    ui_steps = info_label_create(LV_ALIGN_TOP_MID, 0, 190);
    ui_battery = info_label_create(LV_ALIGN_TOP_MID, 58, 190);
    ui_sun_event = zsw_ui_label_create(root_page, "", ZSW_FONT_CAPTION, lv_color_hex(COLOR_TEXT));
    lv_obj_set_style_text_opa(ui_sun_event, LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_sun_event, 1, LV_PART_MAIN);
    lv_obj_align(ui_sun_event, LV_ALIGN_TOP_MID, 0, 212);

    view.weather = DIORAMA_WEATHER_CLEAR;
    view.intensity = 1;
    start_wake();
    apply_view();

    frame_timer = lv_timer_create(frame_timer_cb, FRAME_PERIOD_MS, NULL);
}

static void watchface_remove(void)
{
    if (!root_page) {
        return;
    }

    face_active = false;
    if (frame_timer) {
        lv_timer_delete(frame_timer);
        frame_timer = NULL;
    }
    lv_obj_delete(root_page);
    root_page = NULL;
    scene_obj = NULL;
}

static void watchface_set_display_active(bool active)
{
    if (!root_page) {
        return;
    }

    face_active = active;
    update_timer_state();
    if (active) {
        start_wake();
        apply_view();
    }
}

static void watchface_set_paused(bool pause)
{
    paused = pause;
    update_timer_state();
}

static void watchface_set_datetime(int day_of_week, int date, int day, int month, int year, int weekday, int32_t hour,
                                   int32_t minute, int32_t second, uint32_t usec, bool am, bool mode)
{
    static const char *const days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
    static const char *const months[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

    ARG_UNUSED(day);
    ARG_UNUSED(year);
    ARG_UNUSED(weekday);
    ARG_UNUSED(second);
    ARG_UNUSED(usec);
    ARG_UNUSED(am);
    ARG_UNUSED(mode);

    if (!root_page) {
        return;
    }

    if (hour != last_hour || minute != last_minute) {
        last_hour = hour;
        last_minute = minute;
        lv_label_set_text_fmt(ui_time, "%02d:%02d", (int)hour, (int)minute);
        lv_label_set_text_fmt(ui_time_shadow, "%02d:%02d", (int)hour, (int)minute);
        if (!waking) {
            update_sky();
            apply_view();
        } else {
            diorama_view_t unused = view;

            /* The sky is in the middle of its run, only the times of the sun are needed */
            compute_sky(0, &sun_events);
            view = unused;
            show_sun_event();
        }
    }

    if (date != last_day) {
        last_day = date;
        lv_label_set_text_fmt(ui_date, "%s %d %s", days[day_of_week % 7], date, months[month % 12]);
    }
}

static void watchface_set_battery_percent(int32_t percent, int32_t battery)
{
    ARG_UNUSED(battery);

    if (root_page && percent != last_battery) {
        last_battery = percent;
        lv_label_set_text_fmt(ui_battery, "%d%%", (int)percent);
    }
}

static void watchface_set_hrm(int32_t bpm, int32_t oxygen)
{
    ARG_UNUSED(bpm);
    ARG_UNUSED(oxygen);
}

static void watchface_set_step(int32_t steps, int32_t distance, int32_t kcal)
{
    ARG_UNUSED(distance);
    ARG_UNUSED(kcal);

    if (root_page && steps != last_steps) {
        last_steps = steps;
        lv_label_set_text_fmt(ui_steps, "%d", (int)steps);
    }
}

static void watchface_set_ble_connected(bool connected)
{
    ARG_UNUSED(connected);
}

static void watchface_set_num_notifcations(int32_t number)
{
    ARG_UNUSED(number);
}

static void set_weather(diorama_weather_t weather, int intensity)
{
    view.weather = weather;
    view.intensity = (uint8_t)intensity;
    apply_view();
}

static void watchface_set_weather(int8_t temperature, int weather_code, uint16_t humidity)
{
    zsw_weather_condition_t condition = zsw_weather_classify(weather_code);

    ARG_UNUSED(humidity);

    if (!root_page) {
        return;
    }

    if (temperature != last_temperature) {
        last_temperature = temperature;
        lv_label_set_text_fmt(ui_temperature, "%d°", temperature);
    }

    switch (condition.type) {
        case ZSW_WEATHER_CLOUDS:
            set_weather(DIORAMA_WEATHER_CLOUDS, condition.intensity);
            break;
        case ZSW_WEATHER_DRIZZLE:
            set_weather(DIORAMA_WEATHER_RAIN, 1);
            break;
        case ZSW_WEATHER_RAIN:
            set_weather(DIORAMA_WEATHER_RAIN, condition.intensity);
            break;
        case ZSW_WEATHER_THUNDER:
            set_weather(DIORAMA_WEATHER_RAIN, 3);
            break;
        case ZSW_WEATHER_SNOW:
            set_weather(DIORAMA_WEATHER_SNOW, condition.intensity);
            break;
        case ZSW_WEATHER_FOG:
            set_weather(DIORAMA_WEATHER_FOG, condition.intensity);
            break;
        default:
            set_weather(DIORAMA_WEATHER_CLEAR, 1);
            break;
    }
}

static void watchface_set_watch_env_sensors(int pressure)
{
    ARG_UNUSED(pressure);
}

static void watchface_set_bg(const void *bg_img)
{
    ARG_UNUSED(bg_img);
}

static void reset_cached_values(void)
{
    last_minute = -1;
    last_hour = -1;
    last_day = -1;
    last_steps = -1;
    last_battery = -1;
    last_temperature = INT32_MIN;
}

static void watchface_ui_invalidate_cached(void)
{
    reset_cached_values();
}

static const void *watchface_get_preview_img(void)
{
    return ZSW_LV_IMG_USE(face_diorama_preview);
}

void zsw_watchface_diorama_override(bool enable, int minute_of_day, int weather, int intensity, int tilt_x_new,
                                    int tilt_y_new)
{
    override.enabled = enable;
    override.minute_of_day = minute_of_day;
    override.tilt_x = tilt_x_new;
    override.tilt_y = tilt_y_new;
    if (!root_page) {
        return;
    }
    /* Back to clear weather until the next report comes */
    view.weather = enable ? (diorama_weather_t)weather : DIORAMA_WEATHER_CLEAR;
    view.intensity = enable ? (uint8_t)intensity : 1;
    if (!waking) {
        update_sky();
    }
    apply_view();
}

static watchface_ui_api_t ui_api = {
    .show = watchface_show,
    .remove = watchface_remove,
    .set_battery_percent = watchface_set_battery_percent,
    .set_hrm = watchface_set_hrm,
    .set_step = watchface_set_step,
    .set_ble_connected = watchface_set_ble_connected,
    .set_num_notifcations = watchface_set_num_notifcations,
    .set_weather = watchface_set_weather,
    .set_watch_env_sensors = watchface_set_watch_env_sensors,
    .set_watchface_bg = watchface_set_bg,
    .set_display_active = watchface_set_display_active,
    .set_paused = watchface_set_paused,
    .set_datetime = watchface_set_datetime,
    .ui_invalidate_cached = watchface_ui_invalidate_cached,
    .get_preview_img = watchface_get_preview_img,
    .name = "Diorama",
};

static int watchface_init(void)
{
    watchface_app_register_ui(&ui_api);

    return 0;
}

SYS_INIT(watchface_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
