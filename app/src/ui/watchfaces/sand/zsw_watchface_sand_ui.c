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

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ui/utils/zsw_ui_utils.h"
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"
#include "applications/watchface/watchface_app.h"
#include "sensors/zsw_imu.h"
#include "sand_sim.h"
#include "zsw_watchface_sand.h"

LOG_MODULE_REGISTER(watchface_sand, LOG_LEVEL_INF);

#define FACE_SIZE               (SAND_GRID_SIZE * SAND_CELL_PX)

/* While the sand moves a frame is two steps of the world. At rest only gravity is checked. */
#define FRAME_PERIOD_MS         40
#define REST_PERIOD_MS          250
#define STEPS_PER_FRAME         2
/* Frames without movement before going to rest */
#define REST_AFTER_FRAMES       8
/* The accelerometer is read every this many frames while the sand moves */
#define GRAVITY_EVERY_FRAMES    3

/*
 * How the accelerometer axes map to the display: which axis points right and which down
 * when looking at the watch, as +1 or -1. TODO: only a guess until tried on the hardware.
 */
#define ACCEL_TO_RIGHT(x, y)    (x)
#define ACCEL_TO_DOWN(x, y)     (-(y))
#define ACCEL_1G                9.81f

#define COLOR_DATE              0xB98A46

ZSW_LV_IMG_DECLARE(face_sand_preview);

static void reset_cached_values(void);

/* Base, darker and lighter grain of loose sand, of the digits, and of grains in the air */
static const uint32_t sand_colors[SAND_NUM_SHADES] = { 0xD9952B, 0xA9700F, 0xF2BE5E };
static const uint32_t digit_colors[SAND_NUM_SHADES] = { 0xFFF0D2, 0xEBCF9C, 0xFFFFFF };

static lv_obj_t *root_page;
static lv_obj_t *sand_obj;
static lv_obj_t *ui_date;
static lv_timer_t *frame_timer;
static bool face_active;
static bool paused;
static uint8_t rest_frames;
static uint8_t gravity_frames;
static bool gravity_overridden;

static int last_minute = -1;
static int last_hour = -1;
static int last_day = -1;

static void fill_cells(lv_layer_t *layer, lv_draw_rect_dsc_t *dsc, const lv_area_t *origin, int x_first, int x_last,
                       int y, uint32_t color)
{
    lv_area_t area = {
        .x1 = origin->x1 + x_first * SAND_CELL_PX,
        .y1 = origin->y1 + y * SAND_CELL_PX,
        .x2 = origin->x1 + (x_last + 1) * SAND_CELL_PX - 1,
        .y2 = origin->y1 + (y + 1) * SAND_CELL_PX - 1,
    };

    dsc->bg_color = lv_color_hex(color);
    lv_draw_rect(layer, dsc, &area);
}

/*
 * Each row is drawn as runs of grains in the base shade, with the darker and lighter grains
 * on top one by one. That is far fewer rectangles than one per grain.
 */
static void sand_draw_cb(lv_event_t *e)
{
    lv_layer_t *layer = lv_event_get_layer(e);
    lv_draw_rect_dsc_t dsc;
    lv_area_t coords;
    lv_area_t clip;
    sand_flying_t flying[SAND_MAX_FLYING];

    lv_obj_get_coords(sand_obj, &coords);
    if (!lv_area_intersect(&clip, &layer->_clip_area, &coords)) {
        return;
    }

    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_opa = LV_OPA_COVER;

    int x0 = (clip.x1 - coords.x1) / SAND_CELL_PX;
    int x1 = (clip.x2 - coords.x1) / SAND_CELL_PX;
    int y0 = (clip.y1 - coords.y1) / SAND_CELL_PX;
    int y1 = (clip.y2 - coords.y1) / SAND_CELL_PX;

    for (int y = y0; y <= y1; y++) {
        int run_start = 0;
        sand_cell_type_t run_type = SAND_CELL_EMPTY;

        for (int x = x0; x <= x1 + 1; x++) {
            sand_cell_type_t type = x <= x1 ? SAND_CELL_TYPE(sand_sim_cell(x, y)) : SAND_CELL_EMPTY;

            if (type == run_type) {
                continue;
            }
            if (run_type != SAND_CELL_EMPTY) {
                fill_cells(layer, &dsc, &coords, run_start, x - 1, y,
                           run_type == SAND_CELL_SAND ? sand_colors[0] : digit_colors[0]);
            }
            run_start = x;
            run_type = type;
        }

        for (int x = x0; x <= x1; x++) {
            uint8_t cell = sand_sim_cell(x, y);
            uint8_t shade = SAND_CELL_SHADE(cell);

            if (shade != 0 && SAND_CELL_TYPE(cell) != SAND_CELL_EMPTY) {
                fill_cells(layer, &dsc, &coords, x, x, y,
                           SAND_CELL_TYPE(cell) == SAND_CELL_SAND ? sand_colors[shade] : digit_colors[shade]);
            }
        }
    }

    /* Grains in the air heat up to the color of the digits on their way */
    int num_flying = sand_sim_get_flying(flying, ARRAY_SIZE(flying));

    for (int i = 0; i < num_flying; i++) {
        lv_area_t area = {
            .x1 = coords.x1 + flying[i].x - SAND_CELL_PX / 2,
            .y1 = coords.y1 + flying[i].y - SAND_CELL_PX / 2,
        };

        area.x2 = area.x1 + SAND_CELL_PX - 1;
        area.y2 = area.y1 + SAND_CELL_PX - 1;
        dsc.bg_color = lv_color_hex(digit_colors[flying[i].shade]);
        lv_draw_rect(layer, &dsc, &area);
    }
}

static void invalidate_dirty(void)
{
    lv_area_t coords;

    lv_obj_get_coords(sand_obj, &coords);
    for (int band = 0; band < SAND_NUM_BANDS; band++) {
        int x_first;
        int x_last;

        if (!sand_sim_pop_dirty(band, &x_first, &x_last)) {
            continue;
        }

        lv_area_t area = {
            .x1 = coords.x1 + x_first * SAND_CELL_PX,
            .y1 = coords.y1 + band * SAND_BAND_ROWS * SAND_CELL_PX,
            .x2 = coords.x1 + (x_last + 1) * SAND_CELL_PX - 1,
                        .y2 = coords.y1 + (band + 1) * SAND_BAND_ROWS * SAND_CELL_PX - 1,
        };

        lv_obj_invalidate_area(sand_obj, &area);
    }
}

static void wake_sand(void)
{
    rest_frames = 0;
    if (frame_timer) {
        lv_timer_set_period(frame_timer, FRAME_PERIOD_MS);
    }
}

/* @return true if the sand now falls another way. */
static bool read_gravity(void)
{
    float x;
    float y;
    float z;

    if (gravity_overridden || zsw_imu_fetch_accel_f(&x, &y, &z) != 0) {
        return false;
    }

    return sand_sim_set_gravity((int)(ACCEL_TO_RIGHT(x, y) * 127.0f / ACCEL_1G),
                                (int)(ACCEL_TO_DOWN(x, y) * 127.0f / ACCEL_1G));
}

static void frame_timer_cb(lv_timer_t *timer)
{
    bool moving = false;

    ARG_UNUSED(timer);

    if (rest_frames >= REST_AFTER_FRAMES) {
        /* At rest: only look for the wrist turning */
        if (read_gravity()) {
            wake_sand();
        }
        return;
    }

    if (++gravity_frames >= GRAVITY_EVERY_FRAMES) {
        gravity_frames = 0;
        read_gravity();
    }

    for (int i = 0; i < STEPS_PER_FRAME; i++) {
        moving |= sand_sim_step();
    }
    invalidate_dirty();

    rest_frames = moving ? 0 : rest_frames + 1;
    if (rest_frames >= REST_AFTER_FRAMES) {
        lv_timer_set_period(frame_timer, REST_PERIOD_MS);
    }
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

static void root_clicked_cb(lv_event_t *e)
{
    ARG_UNUSED(e);

    if (face_active && !paused) {
        sand_sim_shake();
        wake_sand();
    }
}

static void watchface_show(lv_obj_t *parent, watchface_app_evt_listener evt_cb, zsw_settings_watchface_t *settings)
{
    ARG_UNUSED(evt_cb);
    ARG_UNUSED(settings);

    lv_obj_remove_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    root_page = lv_obj_create(parent);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_size(root_page, FACE_SIZE, FACE_SIZE);
    lv_obj_center(root_page);
    lv_obj_remove_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(root_page, lv_color_black(), LV_PART_MAIN);
    /* A tap on the face throws the sand up */
    lv_obj_add_flag(root_page, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(root_page, root_clicked_cb, LV_EVENT_SHORT_CLICKED, NULL);

    reset_cached_values();
    face_active = true;
    paused = false;
    gravity_frames = 0;

    sand_sim_init(k_cycle_get_32());

    sand_obj = lv_obj_create(root_page);
    lv_obj_remove_style_all(sand_obj);
    lv_obj_set_size(sand_obj, FACE_SIZE, FACE_SIZE);
    lv_obj_center(sand_obj);
    lv_obj_remove_flag(sand_obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(sand_obj, sand_draw_cb, LV_EVENT_DRAW_MAIN, NULL);

    ui_date = zsw_ui_label_create(root_page, "", ZSW_FONT_TEXT, lv_color_hex(COLOR_DATE));
    lv_obj_set_style_text_letter_space(ui_date, 1, LV_PART_MAIN);
    lv_obj_align(ui_date, LV_ALIGN_TOP_MID, 0, 34);

    frame_timer = lv_timer_create(frame_timer_cb, FRAME_PERIOD_MS, NULL);
    wake_sand();
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
    sand_obj = NULL;
}

static void watchface_set_display_active(bool active)
{
    if (!root_page) {
        return;
    }

    face_active = active;
    update_timer_state();
    if (active) {
        wake_sand();
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
        /* The digits that changed crumble and new ones rise from the pile */
        sand_sim_set_time(hour, minute);
        wake_sand();
    }

    if (date != last_day) {
        last_day = date;
        lv_label_set_text_fmt(ui_date, "%s %d %s", days[day_of_week % 7], date, months[month % 12]);
    }
}

/* The face shows the time and the date only, but the watchface app calls these without checking. */
static void watchface_set_battery_percent(int32_t percent, int32_t battery)
{
    ARG_UNUSED(percent);
    ARG_UNUSED(battery);
}

static void watchface_set_hrm(int32_t bpm, int32_t oxygen)
{
    ARG_UNUSED(bpm);
    ARG_UNUSED(oxygen);
}

static void watchface_set_step(int32_t steps, int32_t distance, int32_t kcal)
{
    ARG_UNUSED(steps);
    ARG_UNUSED(distance);
    ARG_UNUSED(kcal);
}

static void watchface_set_ble_connected(bool connected)
{
    ARG_UNUSED(connected);
}

static void watchface_set_num_notifcations(int32_t number)
{
    ARG_UNUSED(number);
}

static void watchface_set_weather(int8_t temperature, int weather_code, uint16_t humidity)
{
    ARG_UNUSED(temperature);
    ARG_UNUSED(weather_code);
    ARG_UNUSED(humidity);
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
}

static void watchface_ui_invalidate_cached(void)
{
    /* The time is not reset: that would crumble and rebuild digits that did not change */
    last_day = -1;
}

static const void *watchface_get_preview_img(void)
{
    return ZSW_LV_IMG_USE(face_sand_preview);
}

void zsw_watchface_sand_override_gravity(bool enable, int x, int y)
{
    gravity_overridden = enable;
    if (enable && root_page && sand_sim_set_gravity(x, y)) {
        wake_sand();
    }
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
    .name = "Sand",
};

static int watchface_init(void)
{
    watchface_app_register_ui(&ui_api);

    return 0;
}

SYS_INIT(watchface_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
