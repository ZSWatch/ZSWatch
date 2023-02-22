#include <watchface_ui.h>
#include <lvgl.h>
#include <general_ui.h>

#define SMALL_WATCHFACE_CENTER_OFFSET 38

#define USE_SECOND_HAND

const lv_img_dsc_t *get_icon_from_weather_code(int code);

static lv_obj_t *root_page = NULL;

static lv_obj_t *clock_meter;
static lv_meter_indicator_t *indic_min;
static lv_meter_indicator_t *indic_hour;
#ifdef USE_SECOND_HAND
static lv_meter_indicator_t *indic_second;
#endif

static lv_obj_t *battery_label;
static lv_obj_t *battery_arc;

static lv_obj_t *hrm_label;
static lv_obj_t *hrm_arc;

static lv_obj_t *step_label;
static lv_obj_t *step_arc;

static lv_obj_t *ble_symbol;

static lv_obj_t *weather_temperature;
static lv_obj_t *weather_icon;

static lv_obj_t *notification_icon;
static lv_obj_t *notification_text;

static lv_obj_t *date_label;
static lv_obj_t *day_label;

static void tick_draw_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t *dsc = lv_event_get_param(e);
        if (dsc->type == LV_METER_DRAW_PART_TICK && dsc->text != NULL) {
            if (dsc->value > 0) {
                if ((dsc->value / 5) % 3 == 0) {
                    dsc->label_dsc->opa = LV_OPA_90;
                    dsc->text_length = snprintf(dsc->text, 16, "%d", dsc->value / 5);
                } else {
                    dsc->label_dsc->opa = LV_OPA_60;
                    dsc->text_length = snprintf(dsc->text, 16, "%d", dsc->value / 5);
                }
            } else {
                dsc->text[0] = '\0';
                dsc->text_length = 0;
            }
        }
    }
}

static void add_clock(lv_obj_t *parent)
{
    clock_meter = lv_meter_create(parent);
    lv_obj_set_style_bg_opa(clock_meter, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(clock_meter, 0, LV_PART_MAIN);
    lv_obj_set_size(clock_meter, 240, 240);
    lv_obj_center(clock_meter);

    /*Create another scale for the hours. It's only visual and contains only major ticks*/
    lv_meter_scale_t *scale_hour = lv_meter_add_scale(clock_meter);
    lv_meter_set_scale_ticks(clock_meter, scale_hour, 61, 1, 10, lv_palette_main(LV_PALETTE_BLUE_GREY));
    lv_meter_set_scale_range(clock_meter, scale_hour, 0, 60, 360, 270);
    lv_meter_set_scale_major_ticks(clock_meter, scale_hour, 5, 2, 20, lv_color_black(), 10); /*Every tick is major*/
#ifdef USE_SECOND_HAND
    lv_meter_scale_t *scale_second = lv_meter_add_scale(clock_meter);
    lv_obj_set_style_pad_all(clock_meter, 0, LV_PART_MAIN);
    lv_meter_set_scale_ticks(clock_meter, scale_second, 61, 1, 10, lv_palette_main(LV_PALETTE_BLUE_GREY));
    lv_meter_set_scale_range(clock_meter, scale_second, 0, 60, 360, 270);
#endif

    /* Create a scale for the minutes */
    /* 61 ticks in a 360 degrees range (the last and the first line overlaps) */
    lv_meter_scale_t *scale_min = lv_meter_add_scale(clock_meter);
    lv_obj_set_style_border_color(clock_meter, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_meter_set_scale_ticks(clock_meter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_BLUE_GREY));
    lv_meter_set_scale_range(clock_meter, scale_min, 0, 60, 360, 270);


    lv_obj_add_event_cb(clock_meter, tick_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_remove_style(clock_meter, NULL, LV_PART_INDICATOR);

    LV_IMG_DECLARE(minute_hand)
    LV_IMG_DECLARE(hour_hand)

    indic_hour = lv_meter_add_needle_img(clock_meter, scale_hour, &hour_hand, 31, 9);
    indic_min = lv_meter_add_needle_img(clock_meter, scale_min, &minute_hand, 31, 9);
#ifdef USE_SECOND_HAND
    LV_IMG_DECLARE(second_hand)
    indic_second = lv_meter_add_needle_img(clock_meter, scale_second, &second_hand, 12, 2);
#endif
}

static void add_battery_indicator(lv_obj_t *parent)
{
    battery_arc = lv_arc_create(parent);
    lv_arc_set_rotation(battery_arc, 270);
    lv_arc_set_bg_angles(battery_arc, 0, 360);
    lv_arc_set_range(battery_arc, 0, 100); // 0-100% battery
    lv_obj_remove_style(battery_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(battery_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(battery_arc, 3, LV_PART_MAIN);
    lv_obj_set_style_arc_width(battery_arc, 3, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(battery_arc, lv_palette_main(LV_PALETTE_DEEP_ORANGE), LV_PART_INDICATOR);

    lv_obj_set_size(battery_arc, 50, 50);
    lv_obj_align_to(battery_arc, parent, LV_ALIGN_CENTER, -SMALL_WATCHFACE_CENTER_OFFSET, -SMALL_WATCHFACE_CENTER_OFFSET);

    LV_IMG_DECLARE(voltage);

    lv_obj_t *charge_icon = lv_img_create(parent);
    lv_img_set_src(charge_icon, &voltage);
    lv_obj_align_to(charge_icon, battery_arc, LV_ALIGN_CENTER, 0, 9);

    battery_label = lv_label_create(parent);
    lv_label_set_text(battery_label, "-%");
    lv_obj_align_to(battery_label, battery_arc, LV_ALIGN_CENTER, 0, -9);
}

static void add_pulse_indicator(lv_obj_t *parent)
{
    hrm_arc = lv_arc_create(parent);
    lv_arc_set_rotation(hrm_arc, 270);
    lv_arc_set_bg_angles(hrm_arc, 0, 360);
    lv_arc_set_range(hrm_arc, 0, 220); // 220 max hrm
    lv_obj_remove_style(hrm_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(hrm_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(hrm_arc, 3, LV_PART_MAIN);
    lv_obj_set_style_arc_width(hrm_arc, 3, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(hrm_arc, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);

    lv_obj_set_size(hrm_arc, 50, 50);
    lv_obj_align_to(hrm_arc, parent, LV_ALIGN_CENTER, -SMALL_WATCHFACE_CENTER_OFFSET, SMALL_WATCHFACE_CENTER_OFFSET);

    LV_IMG_DECLARE(heart_beat);

    lv_obj_t *charge_icon = lv_img_create(parent);
    lv_img_set_src(charge_icon, &heart_beat);
    lv_obj_align_to(charge_icon, hrm_arc, LV_ALIGN_CENTER, 0, 9);

    hrm_label = lv_label_create(parent);
    lv_label_set_text(hrm_label, "-");
    lv_obj_align_to(hrm_label, hrm_arc, LV_ALIGN_CENTER, 0, -9);
}

static void add_step_indicator(lv_obj_t *parent)
{
    step_arc = lv_arc_create(parent);
    lv_arc_set_rotation(step_arc, 270);
    lv_arc_set_bg_angles(step_arc, 0, 360);
    lv_arc_set_range(step_arc, 0, 10000); // 10000 daily step goal
    lv_obj_remove_style(step_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(step_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(step_arc, 3, LV_PART_MAIN);
    lv_obj_set_style_arc_width(step_arc, 3, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(step_arc, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_PART_INDICATOR);

    lv_obj_set_size(step_arc, 50, 50);
    lv_obj_align_to(step_arc, parent, LV_ALIGN_CENTER, SMALL_WATCHFACE_CENTER_OFFSET, SMALL_WATCHFACE_CENTER_OFFSET);

    LV_IMG_DECLARE(walk);

    lv_obj_t *charge_icon = lv_img_create(parent);
    lv_img_set_src(charge_icon, &walk);
    lv_obj_align_to(charge_icon, step_arc, LV_ALIGN_CENTER, 0, 9);

    step_label = lv_label_create(parent);
    lv_label_set_text(step_label, "-");
    lv_obj_align_to(step_label, step_arc, LV_ALIGN_CENTER, 0, -9);
}

static void add_ble_connected_indicator(lv_obj_t *parent)
{
    static lv_style_t color_style;

    ble_symbol = lv_label_create(parent);
    lv_label_set_text(ble_symbol, LV_SYMBOL_BLUETOOTH);
    lv_obj_align_to(ble_symbol, parent, LV_ALIGN_CENTER, 0, SMALL_WATCHFACE_CENTER_OFFSET);

    lv_style_init(&color_style);
    lv_style_set_text_color(&color_style, lv_color_hex(0x0082FC));
    lv_obj_add_style(ble_symbol, &color_style, 0);
    watchface_set_ble_connected(false);
}

static void add_notification_indicator(lv_obj_t *parent)
{
    static lv_style_t color_style;
    LV_IMG_DECLARE(notification_black);

    notification_icon = lv_img_create(parent);
    lv_img_set_src(notification_icon, &notification_black);
    lv_obj_align_to(notification_icon, parent, LV_ALIGN_CENTER, 10, -SMALL_WATCHFACE_CENTER_OFFSET - 25);

    notification_text = lv_label_create(parent);
    lv_label_set_text(notification_text, "-");
    lv_obj_align_to(notification_text, parent, LV_ALIGN_CENTER, -10, -SMALL_WATCHFACE_CENTER_OFFSET - 25);

    lv_style_init(&color_style);
    lv_style_set_text_color(&color_style, lv_color_black());
    lv_obj_add_style(notification_text, &color_style, 0);
    watchface_set_num_notifcations(0);
}

static void add_weather_data(lv_obj_t *parent)
{
    weather_temperature = lv_label_create(parent);
    lv_label_set_text(weather_temperature, "5");
    lv_obj_align_to(weather_temperature, parent, LV_ALIGN_CENTER, -15, SMALL_WATCHFACE_CENTER_OFFSET + 33);

    weather_icon = lv_img_create(weather_temperature);
    lv_obj_set_size(weather_icon, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align_to(weather_icon, weather_temperature, LV_ALIGN_OUT_RIGHT_MID, 10, -13);

    lv_obj_add_flag(weather_temperature, LV_OBJ_FLAG_HIDDEN);
}

static void add_date(lv_obj_t *parent)
{
    date_label = lv_label_create(parent);
    lv_label_set_text(date_label, "1");
    lv_obj_align_to(date_label, parent, LV_ALIGN_CENTER, SMALL_WATCHFACE_CENTER_OFFSET + 25, 7);

    day_label = lv_label_create(parent);
    lv_obj_set_style_text_color(day_label, lv_color_make(0xFF, 0xB7, 0x22), LV_PART_MAIN);
    lv_label_set_text(day_label, "MON");
    lv_obj_align_to(day_label, parent, LV_ALIGN_CENTER, SMALL_WATCHFACE_CENTER_OFFSET + 25, -7);
}

void watchface_init(void)
{
    lv_obj_clean(lv_scr_act());
}

void watchface_show(void)
{
    if (root_page != NULL) {
        lv_obj_clear_flag(root_page, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    root_page = lv_obj_create(lv_scr_act());
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, 240, 240);
    lv_obj_align(root_page, LV_ALIGN_CENTER, 0, 0);
    add_battery_indicator(root_page);
    add_pulse_indicator(root_page);
    add_step_indicator(root_page);
    add_ble_connected_indicator(root_page);
    add_notification_indicator(root_page);
    add_weather_data(root_page);
    add_date(root_page);
    add_clock(root_page);
}

void watchface_remove(void)
{
    lv_obj_add_flag(root_page, LV_OBJ_FLAG_HIDDEN);
}

void watchface_set_battery_percent(int32_t percent, int32_t value)
{
    if (!root_page) {
        return;
    }
    char buf[5];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%dmV", value);
    lv_arc_set_value(battery_arc, percent);
    lv_label_set_text(battery_label, buf);
    lv_obj_align_to(battery_label, battery_arc, LV_ALIGN_CENTER, 0, -9);
}

void watchface_set_hrm(int32_t value)
{
    if (!root_page) {
        return;
    }
    char buf[5];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%d", value);
    lv_arc_set_value(hrm_arc, value);
    lv_label_set_text(hrm_label, buf);
    lv_obj_align_to(hrm_label, hrm_arc, LV_ALIGN_CENTER, 0, -9);
}

void watchface_set_step(int32_t value)
{
    if (!root_page) {
        return;
    }
    char buf[6];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%d", value);
    lv_arc_set_value(step_arc, value);
    lv_label_set_text(step_label, buf);
    lv_obj_align_to(step_label, step_arc, LV_ALIGN_CENTER, 0, -9);
}

void watchface_set_time(int32_t hour, int32_t minute, int32_t second)
{

    if (!root_page) {
        return;
    }
    // Hour hand is split into 60 steps, use minutes to progress the hour
    // hand between full hours.
    int hour_offset;
    hour = hour % 12;
    hour_offset = hour * 5;
    hour_offset += minute / 10;
    if (hour_offset >= 60) {
        hour_offset = 60;
    }
    lv_meter_set_indicator_end_value(clock_meter, indic_min, minute);
    lv_meter_set_indicator_end_value(clock_meter, indic_hour, hour_offset);
#ifdef USE_SECOND_HAND
    lv_meter_set_indicator_end_value(clock_meter, indic_second, second);
#endif
}

void watchface_set_num_notifcations(int32_t value)
{
    char not_text_buf[2]; // 0-9 and \0
    if (value > 0) {
        if (value < 10) {
            snprintf(not_text_buf, 2, "%d", value);
        } else {
            snprintf(not_text_buf, 2, "*");
        }
        lv_label_set_text(notification_text, not_text_buf);
        lv_obj_clear_flag(notification_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(notification_text, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(notification_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(notification_text, LV_OBJ_FLAG_HIDDEN);
    }
}

void watchface_set_ble_connected(bool connected)
{
    if (connected) {
        lv_obj_clear_flag(ble_symbol, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ble_symbol, LV_OBJ_FLAG_HIDDEN);
    }
}

void watchface_set_weather(int8_t temperature, int weather_code)
{
    char buf[10];
    snprintf(buf, sizeof(buf), "%d°", temperature);
    lv_label_set_text(weather_temperature, buf);
    const lv_img_dsc_t *icon = get_icon_from_weather_code(weather_code);
    lv_img_set_src(weather_icon, icon);
    lv_obj_clear_flag(weather_temperature, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(weather_icon, LV_OBJ_FLAG_HIDDEN);
}

void watchface_set_date(int day_of_week, int date)
{
    char buf[10];
    char *days[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

    snprintf(buf, sizeof(buf), "%s", days[day_of_week]);
    lv_label_set_text(day_label, buf);
    snprintf(buf, sizeof(buf), "%d", date);
    lv_label_set_text(date_label, buf);
}

LV_IMG_DECLARE(stormy);
LV_IMG_DECLARE(snowy);
LV_IMG_DECLARE(rainy);
LV_IMG_DECLARE(snowy);
LV_IMG_DECLARE(foggy);
LV_IMG_DECLARE(sunny);
LV_IMG_DECLARE(partly_cloudy);
LV_IMG_DECLARE(cloudy);
LV_IMG_DECLARE(unknown);

const lv_img_dsc_t *get_icon_from_weather_code(int code)
{
    int code_group = code / 100;

    switch (code_group) {
        case 2:
            return &stormy;
        case 3:
            return &cloudy;
        case 5:
            switch (code) {
                case 511:
                    return &snowy;
                default:
                    return &rainy;
            }
        case 6:
            return &snowy;
        case 7:
            return &foggy;
        case 8:
            switch (code) {
                case 800:
                    // TODO if day sunny else moon
                    return &sunny;
                case 801:
                    return &partly_cloudy;
                case 802:
                    return &partly_cloudy;
                default:
                    return &cloudy;
            }
        default: {
            printf("Unhandled weather code: %d", code);
            return &unknown;
        }
    }
}
