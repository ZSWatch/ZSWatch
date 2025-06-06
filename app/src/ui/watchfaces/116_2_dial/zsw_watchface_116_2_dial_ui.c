
// File generated by bin2lvgl
// developed by fbiego.
// https://github.com/fbiego
// modified by Daniel Kampert.
// https://github.com/kampi
// Watchface: 116_2_dial

#include <lvgl.h>

#include <zephyr/logging/log.h>

#include "ui/zsw_ui.h"
#include "applications/watchface/watchface_app.h"

LOG_MODULE_REGISTER(watchface_116_2_dial, LOG_LEVEL_WRN);

static lv_obj_t *face_116_2_dial;
static lv_obj_t *face_116_2_dial = NULL;
static watchface_app_evt_listener ui_116_2_dial_evt_cb;

static int last_date = -1;
static int last_day = -1;
static int last_month = -1;
static int last_year = -1;
static int last_weekday = -1;
static int last_hour = -1;
static int last_minute = -1;

static int last_steps = -1;
static int last_distance = -1;
static int last_kcal = -1;

static lv_obj_t *face_116_2_dial_0_384;
static lv_obj_t *face_116_2_dial_1_59716;
static lv_obj_t *face_116_2_dial_2_59716;
static lv_obj_t *face_116_2_dial_3_62316;
static lv_obj_t *face_116_2_dial_4_62316;
static lv_obj_t *face_116_2_dial_5_114030;
static lv_obj_t *face_116_2_dial_6_114030;
static lv_obj_t *face_116_2_dial_8_58492;
static lv_obj_t *face_116_2_dial_18_162424;

ZSW_LV_IMG_DECLARE(face_116_2_dial_0_384_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_1_59716_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_1_59716_1);
ZSW_LV_IMG_DECLARE(face_116_2_dial_1_59716_2);
ZSW_LV_IMG_DECLARE(face_116_2_dial_1_59716_3);
ZSW_LV_IMG_DECLARE(face_116_2_dial_1_59716_4);
ZSW_LV_IMG_DECLARE(face_116_2_dial_1_59716_5);
ZSW_LV_IMG_DECLARE(face_116_2_dial_1_59716_6);
ZSW_LV_IMG_DECLARE(face_116_2_dial_1_59716_7);
ZSW_LV_IMG_DECLARE(face_116_2_dial_1_59716_8);
ZSW_LV_IMG_DECLARE(face_116_2_dial_1_59716_9);
ZSW_LV_IMG_DECLARE(face_116_2_dial_3_62316_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_3_62316_1);
ZSW_LV_IMG_DECLARE(face_116_2_dial_3_62316_2);
ZSW_LV_IMG_DECLARE(face_116_2_dial_3_62316_3);
ZSW_LV_IMG_DECLARE(face_116_2_dial_3_62316_4);
ZSW_LV_IMG_DECLARE(face_116_2_dial_3_62316_5);
ZSW_LV_IMG_DECLARE(face_116_2_dial_3_62316_6);
ZSW_LV_IMG_DECLARE(face_116_2_dial_3_62316_7);
ZSW_LV_IMG_DECLARE(face_116_2_dial_3_62316_8);
ZSW_LV_IMG_DECLARE(face_116_2_dial_3_62316_9);
ZSW_LV_IMG_DECLARE(face_116_2_dial_5_114030_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_5_114030_1);
ZSW_LV_IMG_DECLARE(face_116_2_dial_5_114030_2);
ZSW_LV_IMG_DECLARE(face_116_2_dial_5_114030_3);
ZSW_LV_IMG_DECLARE(face_116_2_dial_5_114030_4);
ZSW_LV_IMG_DECLARE(face_116_2_dial_5_114030_5);
ZSW_LV_IMG_DECLARE(face_116_2_dial_5_114030_6);
ZSW_LV_IMG_DECLARE(face_116_2_dial_5_114030_7);
ZSW_LV_IMG_DECLARE(face_116_2_dial_5_114030_8);
ZSW_LV_IMG_DECLARE(face_116_2_dial_5_114030_9);
ZSW_LV_IMG_DECLARE(face_116_2_dial_8_58492_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_9_157828_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_10_156106_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_11_153152_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_12_151838_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_13_154678_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_14_165314_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_15_60830_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_16_150496_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_17_159548_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_17_159548_1);
ZSW_LV_IMG_DECLARE(face_116_2_dial_17_159548_2);
ZSW_LV_IMG_DECLARE(face_116_2_dial_17_159548_3);
ZSW_LV_IMG_DECLARE(face_116_2_dial_17_159548_4);
ZSW_LV_IMG_DECLARE(face_116_2_dial_17_159548_5);
ZSW_LV_IMG_DECLARE(face_116_2_dial_17_159548_6);
ZSW_LV_IMG_DECLARE(face_116_2_dial_18_162424_0);
ZSW_LV_IMG_DECLARE(face_116_2_dial_18_162424_1);
ZSW_LV_IMG_DECLARE(face_116_2_dial_18_162424_2);
ZSW_LV_IMG_DECLARE(face_116_2_dial_18_162424_3);
ZSW_LV_IMG_DECLARE(face_116_2_dial_18_162424_4);
ZSW_LV_IMG_DECLARE(face_116_2_dial_18_162424_5);
ZSW_LV_IMG_DECLARE(face_116_2_dial_18_162424_6);
ZSW_LV_IMG_DECLARE(face_116_2_dial_preview_0);

#if CONFIG_LV_COLOR_DEPTH_16 != 1
#error "CONFIG_LV_COLOR_DEPTH_16 should be 16 bit for watchfaces"
#endif
#if CONFIG_LV_COLOR_16_SWAP != 1
#error "CONFIG_LV_COLOR_16_SWAP should be 1 for watchfaces"
#endif

const void *face_116_2_dial_1_59716_group[] = {
    ZSW_LV_IMG_USE(face_116_2_dial_1_59716_0),
    ZSW_LV_IMG_USE(face_116_2_dial_1_59716_1),
    ZSW_LV_IMG_USE(face_116_2_dial_1_59716_2),
    ZSW_LV_IMG_USE(face_116_2_dial_1_59716_3),
    ZSW_LV_IMG_USE(face_116_2_dial_1_59716_4),
    ZSW_LV_IMG_USE(face_116_2_dial_1_59716_5),
    ZSW_LV_IMG_USE(face_116_2_dial_1_59716_6),
    ZSW_LV_IMG_USE(face_116_2_dial_1_59716_7),
    ZSW_LV_IMG_USE(face_116_2_dial_1_59716_8),
    ZSW_LV_IMG_USE(face_116_2_dial_1_59716_9),
};
const void *face_116_2_dial_3_62316_group[] = {
    ZSW_LV_IMG_USE(face_116_2_dial_3_62316_0),
    ZSW_LV_IMG_USE(face_116_2_dial_3_62316_1),
    ZSW_LV_IMG_USE(face_116_2_dial_3_62316_2),
    ZSW_LV_IMG_USE(face_116_2_dial_3_62316_3),
    ZSW_LV_IMG_USE(face_116_2_dial_3_62316_4),
    ZSW_LV_IMG_USE(face_116_2_dial_3_62316_5),
    ZSW_LV_IMG_USE(face_116_2_dial_3_62316_6),
    ZSW_LV_IMG_USE(face_116_2_dial_3_62316_7),
    ZSW_LV_IMG_USE(face_116_2_dial_3_62316_8),
    ZSW_LV_IMG_USE(face_116_2_dial_3_62316_9),
};
const void *face_116_2_dial_5_114030_group[] = {
    ZSW_LV_IMG_USE(face_116_2_dial_5_114030_0),
    ZSW_LV_IMG_USE(face_116_2_dial_5_114030_1),
    ZSW_LV_IMG_USE(face_116_2_dial_5_114030_2),
    ZSW_LV_IMG_USE(face_116_2_dial_5_114030_3),
    ZSW_LV_IMG_USE(face_116_2_dial_5_114030_4),
    ZSW_LV_IMG_USE(face_116_2_dial_5_114030_5),
    ZSW_LV_IMG_USE(face_116_2_dial_5_114030_6),
    ZSW_LV_IMG_USE(face_116_2_dial_5_114030_7),
    ZSW_LV_IMG_USE(face_116_2_dial_5_114030_8),
    ZSW_LV_IMG_USE(face_116_2_dial_5_114030_9),
};
const void *face_116_2_dial_weather[] = {
    ZSW_LV_IMG_USE(face_116_2_dial_8_58492_0),
    ZSW_LV_IMG_USE(face_116_2_dial_9_157828_0),
    ZSW_LV_IMG_USE(face_116_2_dial_10_156106_0),
    ZSW_LV_IMG_USE(face_116_2_dial_11_153152_0),
    ZSW_LV_IMG_USE(face_116_2_dial_12_151838_0),
    ZSW_LV_IMG_USE(face_116_2_dial_13_154678_0),
    ZSW_LV_IMG_USE(face_116_2_dial_14_165314_0),
    ZSW_LV_IMG_USE(face_116_2_dial_15_60830_0),
    ZSW_LV_IMG_USE(face_116_2_dial_16_150496_0),
};
const void *face_116_2_dial_17_159548_group[] = {
    ZSW_LV_IMG_USE(face_116_2_dial_17_159548_0),
    ZSW_LV_IMG_USE(face_116_2_dial_17_159548_1),
    ZSW_LV_IMG_USE(face_116_2_dial_17_159548_2),
    ZSW_LV_IMG_USE(face_116_2_dial_17_159548_3),
    ZSW_LV_IMG_USE(face_116_2_dial_17_159548_4),
    ZSW_LV_IMG_USE(face_116_2_dial_17_159548_5),
    ZSW_LV_IMG_USE(face_116_2_dial_17_159548_6),
};
const void *face_116_2_dial_18_162424_group[] = {
    ZSW_LV_IMG_USE(face_116_2_dial_18_162424_0),
    ZSW_LV_IMG_USE(face_116_2_dial_18_162424_1),
    ZSW_LV_IMG_USE(face_116_2_dial_18_162424_2),
    ZSW_LV_IMG_USE(face_116_2_dial_18_162424_3),
    ZSW_LV_IMG_USE(face_116_2_dial_18_162424_4),
    ZSW_LV_IMG_USE(face_116_2_dial_18_162424_5),
    ZSW_LV_IMG_USE(face_116_2_dial_18_162424_6),
};

static int32_t getPlaceValue(int32_t num, int32_t place)
{
    int32_t divisor = 1;
    for (uint32_t i = 1; i < place; i++) {
        divisor *= 10;
    }
    return (num / divisor) % 10;
}

static int32_t setPlaceValue(int32_t num, int32_t place, int32_t newValue)
{
    int32_t divisor = 1;
    for (uint32_t i = 1; i < place; i++) {
        divisor *= 10;
    }
    return num - ((num / divisor) % 10 * divisor) + (newValue * divisor);
}

static void watchface_116_2_dial_remove(void)
{
    if (!face_116_2_dial) {
        return;
    }

    lv_obj_del(face_116_2_dial);
    face_116_2_dial = NULL;
}

static void watchface_116_2_dial_invalidate_cached(void)
{
    last_date = -1;
    last_day = -1;
    last_month = -1;
    last_year = -1;
    last_weekday = -1;
    last_hour = -1;
    last_minute = -1;
    last_steps = -1;
    last_distance = -1;
    last_kcal = -1;
}

static const void *watchface_116_2_dial_get_preview_img(void)
{
    return ZSW_LV_IMG_USE(face_116_2_dial_preview_0);
}

static void watchface_116_2_dial_set_datetime(int day_of_week, int date, int day, int month, int year, int weekday,
                                              int hour,
                                              int minute, int second, uint32_t usec, bool am, bool mode)
{
    if (!face_116_2_dial) {
        return;
    }

    // Month parameter is 0-11, but we want to show 1-12 in UI.
    month += 1;

    if (getPlaceValue(last_day, 1) != getPlaceValue(day, 1)) {
        last_day = setPlaceValue(last_day, 1, getPlaceValue(day, 1));
        lv_img_set_src(face_116_2_dial_1_59716, face_116_2_dial_1_59716_group[(day / 1) % 10]);
    }

    if (getPlaceValue(last_day, 2) != getPlaceValue(day, 2)) {
        last_day = setPlaceValue(last_day, 2, getPlaceValue(day, 2));
        lv_img_set_src(face_116_2_dial_2_59716, face_116_2_dial_1_59716_group[(day / 10) % 10]);
    }

    if (getPlaceValue(last_hour, 1) != getPlaceValue(hour, 1)) {
        last_hour = setPlaceValue(last_hour, 1, getPlaceValue(hour, 1));
        lv_img_set_src(face_116_2_dial_3_62316, face_116_2_dial_3_62316_group[(hour / 1) % 10]);
    }

    if (getPlaceValue(last_hour, 2) != getPlaceValue(hour, 2)) {
        last_hour = setPlaceValue(last_hour, 2, getPlaceValue(hour, 2));
        lv_img_set_src(face_116_2_dial_4_62316, face_116_2_dial_3_62316_group[(hour / 10) % 10]);
    }

    if (getPlaceValue(last_minute, 1) != getPlaceValue(minute, 1)) {
        last_minute = setPlaceValue(last_minute, 1, getPlaceValue(minute, 1));
        lv_img_set_src(face_116_2_dial_5_114030, face_116_2_dial_5_114030_group[(minute / 1) % 10]);
    }

    if (getPlaceValue(last_minute, 2) != getPlaceValue(minute, 2)) {
        last_minute = setPlaceValue(last_minute, 2, getPlaceValue(minute, 2));
        lv_img_set_src(face_116_2_dial_6_114030, face_116_2_dial_5_114030_group[(minute / 10) % 10]);
    }

    if (getPlaceValue(last_weekday, 1) != getPlaceValue(weekday, 1)) {
        last_weekday = setPlaceValue(last_weekday, 1, getPlaceValue(weekday, 1));
        lv_img_set_src(face_116_2_dial_18_162424, face_116_2_dial_18_162424_group[((weekday + 6) / 1) % 7]);
    }

}

static void watchface_116_2_dial_set_step(int32_t steps, int32_t distance, int32_t kcal)
{
    if (!face_116_2_dial) {
        return;
    }

}

static void watchface_116_2_dial_set_hrm(int32_t bpm, int32_t oxygen)
{
    if (!face_116_2_dial) {
        return;
    }

}

static void watchface_116_2_dial_set_weather(int8_t temp, int icon)
{
    if (!face_116_2_dial) {
        return;
    }

    lv_img_set_src(face_116_2_dial_8_58492, face_116_2_dial_weather[icon % 8]);

}

static void watchface_116_2_dial_set_ble_connected(bool connected)
{
    if (!face_116_2_dial) {
        return;
    }

}

static void watchface_116_2_dial_set_battery_percent(int32_t percent, int32_t battery)
{
    if (!face_116_2_dial) {
        return;
    }

}

static void watchface_116_2_dial_set_num_notifcations(int32_t number)
{
    if (!face_116_2_dial) {
        return;
    }

}

static void watchface_116_2_dial_set_watch_env_sensors(int temperature, int humidity, int pressure, float iaq,
                                                       float co2)
{
    if (!face_116_2_dial) {
        return;
    }

}

void watchface_116_2_dial_show(lv_obj_t *parent, watchface_app_evt_listener evt_cb, zsw_settings_watchface_t *settings)
{
    ui_116_2_dial_evt_cb = evt_cb;

    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    face_116_2_dial = lv_obj_create(parent);
    watchface_116_2_dial_invalidate_cached();

    lv_obj_clear_flag(face_116_2_dial, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(face_116_2_dial, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(face_116_2_dial, LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_set_style_border_width(face_116_2_dial, 0, LV_PART_MAIN);
    lv_obj_set_size(face_116_2_dial, 240, 240);
    lv_obj_clear_flag(face_116_2_dial, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(face_116_2_dial, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(face_116_2_dial, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(face_116_2_dial, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(face_116_2_dial, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(face_116_2_dial, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(face_116_2_dial, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(face_116_2_dial, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    face_116_2_dial_0_384 = lv_img_create(face_116_2_dial);
    lv_img_set_src(face_116_2_dial_0_384, ZSW_LV_IMG_USE(face_116_2_dial_0_384_0));
    lv_obj_set_width(face_116_2_dial_0_384, LV_SIZE_CONTENT);
    lv_obj_set_height(face_116_2_dial_0_384, LV_SIZE_CONTENT);
    lv_obj_set_x(face_116_2_dial_0_384, 0);
    lv_obj_set_y(face_116_2_dial_0_384, 0);
    lv_obj_add_flag(face_116_2_dial_0_384, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(face_116_2_dial_0_384, LV_OBJ_FLAG_SCROLLABLE);

    face_116_2_dial_1_59716 = lv_img_create(face_116_2_dial);
    lv_img_set_src(face_116_2_dial_1_59716, ZSW_LV_IMG_USE(face_116_2_dial_1_59716_0));
    lv_obj_set_width(face_116_2_dial_1_59716, LV_SIZE_CONTENT);
    lv_obj_set_height(face_116_2_dial_1_59716, LV_SIZE_CONTENT);
    lv_obj_set_x(face_116_2_dial_1_59716, 59);
    lv_obj_set_y(face_116_2_dial_1_59716, 54);
    lv_obj_add_flag(face_116_2_dial_1_59716, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(face_116_2_dial_1_59716, LV_OBJ_FLAG_SCROLLABLE);

    face_116_2_dial_2_59716 = lv_img_create(face_116_2_dial);
    lv_img_set_src(face_116_2_dial_2_59716, ZSW_LV_IMG_USE(face_116_2_dial_1_59716_0));
    lv_obj_set_width(face_116_2_dial_2_59716, LV_SIZE_CONTENT);
    lv_obj_set_height(face_116_2_dial_2_59716, LV_SIZE_CONTENT);
    lv_obj_set_x(face_116_2_dial_2_59716, 49);
    lv_obj_set_y(face_116_2_dial_2_59716, 54);
    lv_obj_add_flag(face_116_2_dial_2_59716, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(face_116_2_dial_2_59716, LV_OBJ_FLAG_SCROLLABLE);

    face_116_2_dial_3_62316 = lv_img_create(face_116_2_dial);
    lv_img_set_src(face_116_2_dial_3_62316, ZSW_LV_IMG_USE(face_116_2_dial_3_62316_0));
    lv_obj_set_width(face_116_2_dial_3_62316, LV_SIZE_CONTENT);
    lv_obj_set_height(face_116_2_dial_3_62316, LV_SIZE_CONTENT);
    lv_obj_set_x(face_116_2_dial_3_62316, 144);
    lv_obj_set_y(face_116_2_dial_3_62316, 38);
    lv_obj_add_flag(face_116_2_dial_3_62316, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(face_116_2_dial_3_62316, LV_OBJ_FLAG_SCROLLABLE);

    face_116_2_dial_4_62316 = lv_img_create(face_116_2_dial);
    lv_img_set_src(face_116_2_dial_4_62316, ZSW_LV_IMG_USE(face_116_2_dial_3_62316_0));
    lv_obj_set_width(face_116_2_dial_4_62316, LV_SIZE_CONTENT);
    lv_obj_set_height(face_116_2_dial_4_62316, LV_SIZE_CONTENT);
    lv_obj_set_x(face_116_2_dial_4_62316, 87);
    lv_obj_set_y(face_116_2_dial_4_62316, 38);
    lv_obj_add_flag(face_116_2_dial_4_62316, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(face_116_2_dial_4_62316, LV_OBJ_FLAG_SCROLLABLE);

    face_116_2_dial_5_114030 = lv_img_create(face_116_2_dial);
    lv_img_set_src(face_116_2_dial_5_114030, ZSW_LV_IMG_USE(face_116_2_dial_5_114030_0));
    lv_obj_set_width(face_116_2_dial_5_114030, LV_SIZE_CONTENT);
    lv_obj_set_height(face_116_2_dial_5_114030, LV_SIZE_CONTENT);
    lv_obj_set_x(face_116_2_dial_5_114030, 169);
    lv_obj_set_y(face_116_2_dial_5_114030, 129);
    lv_obj_add_flag(face_116_2_dial_5_114030, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(face_116_2_dial_5_114030, LV_OBJ_FLAG_SCROLLABLE);

    face_116_2_dial_6_114030 = lv_img_create(face_116_2_dial);
    lv_img_set_src(face_116_2_dial_6_114030, ZSW_LV_IMG_USE(face_116_2_dial_5_114030_0));
    lv_obj_set_width(face_116_2_dial_6_114030, LV_SIZE_CONTENT);
    lv_obj_set_height(face_116_2_dial_6_114030, LV_SIZE_CONTENT);
    lv_obj_set_x(face_116_2_dial_6_114030, 112);
    lv_obj_set_y(face_116_2_dial_6_114030, 129);
    lv_obj_add_flag(face_116_2_dial_6_114030, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(face_116_2_dial_6_114030, LV_OBJ_FLAG_SCROLLABLE);

    face_116_2_dial_8_58492 = lv_img_create(face_116_2_dial);
    lv_img_set_src(face_116_2_dial_8_58492, ZSW_LV_IMG_USE(face_116_2_dial_8_58492_0));
    lv_obj_set_width(face_116_2_dial_8_58492, LV_SIZE_CONTENT);
    lv_obj_set_height(face_116_2_dial_8_58492, LV_SIZE_CONTENT);
    lv_obj_set_x(face_116_2_dial_8_58492, 41);
    lv_obj_set_y(face_116_2_dial_8_58492, 169);
    lv_obj_add_flag(face_116_2_dial_8_58492, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(face_116_2_dial_8_58492, LV_OBJ_FLAG_SCROLLABLE);

    face_116_2_dial_18_162424 = lv_img_create(face_116_2_dial);
    lv_img_set_src(face_116_2_dial_18_162424, ZSW_LV_IMG_USE(face_116_2_dial_18_162424_0));
    lv_obj_set_width(face_116_2_dial_18_162424, LV_SIZE_CONTENT);
    lv_obj_set_height(face_116_2_dial_18_162424, LV_SIZE_CONTENT);
    lv_obj_set_x(face_116_2_dial_18_162424, 43);
    lv_obj_set_y(face_116_2_dial_18_162424, 40);
    lv_obj_add_flag(face_116_2_dial_18_162424, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(face_116_2_dial_18_162424, LV_OBJ_FLAG_SCROLLABLE);

}

static watchface_ui_api_t ui_api = {
    .show = watchface_116_2_dial_show,
    .remove = watchface_116_2_dial_remove,
    .set_battery_percent = watchface_116_2_dial_set_battery_percent,
    .set_hrm = watchface_116_2_dial_set_hrm,
    .set_step = watchface_116_2_dial_set_step,
    .set_ble_connected = watchface_116_2_dial_set_ble_connected,
    .set_num_notifcations = watchface_116_2_dial_set_num_notifcations,
    .set_weather = watchface_116_2_dial_set_weather,
    .set_datetime = watchface_116_2_dial_set_datetime,
    .set_watch_env_sensors = watchface_116_2_dial_set_watch_env_sensors,
    .ui_invalidate_cached = watchface_116_2_dial_invalidate_cached,
    .get_preview_img = watchface_116_2_dial_get_preview_img,
    .name = "Sporty"
};

static int watchface_116_2_dial_init(void)
{
    watchface_app_register_ui(&ui_api);

    return 0;
}

SYS_INIT(watchface_116_2_dial_init, APPLICATION, WATCHFACE_UI_INIT_PRIO);
