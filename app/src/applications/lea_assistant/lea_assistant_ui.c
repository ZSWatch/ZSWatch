#include "lea_assistant_ui.h"

#include <stdio.h>
#include <string.h>

#include <zephyr/logging/log.h>

#include "managers/zsw_app_manager.h"
#include "ui/zsw_ui.h"

#define MAX_DEVICES_NUM       6
#define STEP_COUNT            3
#define SCAN_TIMEOUT_MS       30000
#define DEVICE_ROW_HEIGHT     40
#define DEVICE_LIST_HEIGHT    106
#define SUCCESS_RING_COUNT    3
#define SUCCESS_BAR_COUNT     5
#define SUCCESS_TIMER_MS      70
#define SUCCESS_RING_MIN_SIZE 58
#define SUCCESS_RING_RANGE    58
#define LEA_COLOR_PANEL       lv_color_hex(0x1e2028)
#define LEA_COLOR_PANEL_HI    lv_color_hex(0x242b34)
#define LEA_COLOR_TEXT_DIM    lv_color_hex(0xa9b0bf)
#define LEA_COLOR_MUTED       lv_color_hex(0x495060)
#define LEA_COLOR_MINT        lv_color_hex(0x87e6be)

LOG_MODULE_REGISTER(lea_assistant_ui, CONFIG_ZSW_LEA_ASSISTANT_APP_LOG_LEVEL);

typedef enum {
    LEA_UI_STAGE_SINK_SCAN,
    LEA_UI_STAGE_CONNECTING,
    LEA_UI_STAGE_SOURCE_SCAN,
    LEA_UI_STAGE_SYNCING,
    LEA_UI_STAGE_COMPLETE,
    LEA_UI_STAGE_ERROR,
} lea_ui_stage_t;

typedef struct devices_list {
    uint8_t num;
    lea_assistant_device_t device[MAX_DEVICES_NUM];
} devices_list_t;

static lv_obj_t *root_page;
static lv_obj_t *list_container;
static lv_obj_t *empty_label;
static lv_obj_t *stage_pill_label;
static lv_obj_t *step_subtitle_labels[STEP_COUNT];
static lv_obj_t *progress_dots[STEP_COUNT];
static lv_obj_t *success_rings[SUCCESS_RING_COUNT];
static lv_obj_t *success_bars[SUCCESS_BAR_COUNT];
static lv_timer_t *timeout_timer;
static lv_timer_t *success_timer;
static on_button_press_cb_t click_callback;
static on_close_cb_t close_callback;
static lea_ui_stage_t current_stage;
static uint8_t success_tick;
static char active_name[BT_NAME_LEN];
static char error_title[32];
static char error_message[64];

static devices_list_t source_list;
static devices_list_t sink_list;

LV_FONT_DECLARE(lv_font_montserrat_10);
LV_FONT_DECLARE(lv_font_montserrat_12);
LV_FONT_DECLARE(lv_font_montserrat_14);
LV_FONT_DECLARE(lv_font_montserrat_16);

static void stop_timeout_timer(void)
{
    if (timeout_timer != NULL) {
        lv_timer_del(timeout_timer);
        timeout_timer = NULL;
    }
}

static void stop_success_timer(void)
{
    if (success_timer != NULL) {
        lv_timer_del(success_timer);
        success_timer = NULL;
    }
}

static void reset_ui_refs(void)
{
    list_container = NULL;
    empty_label = NULL;
    stage_pill_label = NULL;

    for (int step_index = 0; step_index < STEP_COUNT; step_index++) {
        step_subtitle_labels[step_index] = NULL;
        progress_dots[step_index] = NULL;
    }

    for (int ring_index = 0; ring_index < SUCCESS_RING_COUNT; ring_index++) {
        success_rings[ring_index] = NULL;
    }

    for (int bar_index = 0; bar_index < SUCCESS_BAR_COUNT; bar_index++) {
        success_bars[bar_index] = NULL;
    }

    success_tick = 0;
}

static void clear_root_page(void)
{
    stop_timeout_timer();
    stop_success_timer();

    if (root_page != NULL) {
        lv_obj_del(root_page);
        root_page = NULL;
    }

    reset_ui_refs();
}

static void set_text(lv_obj_t *label, const char *text)
{
    if (label != NULL) {
        lv_label_set_text(label, text);
    }
}

static lv_obj_t *create_label(lv_obj_t *parent, const char *text, const lv_font_t *font, lv_color_t color)
{
    lv_obj_t *label = lv_label_create(parent);

    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    return label;
}

static void style_clean_container(lv_obj_t *obj, lv_color_t bg_color, lv_opa_t bg_opa)
{
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_color(obj, bg_color, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, bg_opa, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
}

static const char *stage_pill_text(void)
{
    switch (current_stage) {
    case LEA_UI_STAGE_SINK_SCAN:
    case LEA_UI_STAGE_CONNECTING:
        return "1/3";
    case LEA_UI_STAGE_SOURCE_SCAN:
        return "2/3";
    case LEA_UI_STAGE_SYNCING:
        return "3/3";
    case LEA_UI_STAGE_COMPLETE:
        return "Done";
    case LEA_UI_STAGE_ERROR:
    default:
        return "Error";
    }
}

static int active_step_index(void)
{
    switch (current_stage) {
    case LEA_UI_STAGE_SINK_SCAN:
    case LEA_UI_STAGE_CONNECTING:
        return 0;
    case LEA_UI_STAGE_SOURCE_SCAN:
        return 1;
    case LEA_UI_STAGE_SYNCING:
    case LEA_UI_STAGE_COMPLETE:
        return 2;
    case LEA_UI_STAGE_ERROR:
    default:
        return -1;
    }
}

static bool step_is_complete(int step_index)
{
    switch (current_stage) {
    case LEA_UI_STAGE_SOURCE_SCAN:
        return step_index == 0;
    case LEA_UI_STAGE_SYNCING:
        return step_index < 2;
    case LEA_UI_STAGE_COMPLETE:
        return true;
    default:
        return false;
    }
}

static const char *step_title(int step_index)
{
    static const char *const titles[STEP_COUNT] = {
        "Find sink",
        "Pick source",
        "Sync audio",
    };

    return titles[step_index];
}

static const char *step_subtitle(int step_index)
{
    switch (current_stage) {
    case LEA_UI_STAGE_SINK_SCAN:
        if (step_index == 0) {
            return sink_list.num > 0 ? "Tap receiver" : "Searching receivers";
        }
        return "Waiting";
    case LEA_UI_STAGE_CONNECTING:
        if (step_index == 0) {
            return active_name[0] != '\0' ? active_name : "Connecting";
        }
        return "Waiting";
    case LEA_UI_STAGE_SOURCE_SCAN:
        if (step_index == 0) {
            return "Sink connected";
        }
        if (step_index == 1) {
            return source_list.num > 0 ? "Tap broadcast" : "Searching sources";
        }
        return "Waiting";
    case LEA_UI_STAGE_SYNCING:
        if (step_index == 0) {
            return "Sink ready";
        }
        if (step_index == 1) {
            return active_name[0] != '\0' ? active_name : "Source selected";
        }
        return "Adding source";
    case LEA_UI_STAGE_COMPLETE:
        if (step_index == 0) {
            return "Sink ready";
        }
        if (step_index == 1) {
            return "Source ready";
        }
        return "Audio ready";
    case LEA_UI_STAGE_ERROR:
    default:
        return step_index == 0 ? "Stopped" : "Waiting";
    }
}

static void create_header(void)
{
    lv_obj_t *title = create_label(root_page, "Assistant", &lv_font_montserrat_14, lv_color_white());
    lv_obj_set_width(title, 112);
    lv_obj_align(title, LV_ALIGN_TOP_MID, -8, 18);

    lv_obj_t *pill = lv_obj_create(root_page);
    style_clean_container(pill, LEA_COLOR_PANEL_HI, LV_OPA_COVER);
    lv_obj_set_size(pill, 46, 22);
    lv_obj_set_style_radius(pill, 7, LV_PART_MAIN);
    lv_obj_align(pill, LV_ALIGN_TOP_RIGHT, -28, 16);

    stage_pill_label = create_label(pill, stage_pill_text(), &lv_font_montserrat_10, LEA_COLOR_TEXT_DIM);
    lv_obj_center(stage_pill_label);
}

static void create_progress_dots(void)
{
    int active_index = active_step_index();

    for (int step_index = 0; step_index < STEP_COUNT; step_index++) {
        lv_obj_t *dot = lv_obj_create(root_page);
        lv_color_t dot_color = LEA_COLOR_MUTED;

        if (current_stage == LEA_UI_STAGE_COMPLETE || step_is_complete(step_index)) {
            dot_color = zsw_color_blue();
        } else if (step_index == active_index) {
            dot_color = LEA_COLOR_MINT;
        }

        style_clean_container(dot, dot_color, LV_OPA_COVER);
        lv_obj_set_size(dot, 7, 7);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_align(dot, LV_ALIGN_TOP_MID, (step_index - 1) * 14, 42);
        progress_dots[step_index] = dot;
    }
}

static void create_active_step_card(void)
{
    int active_index = active_step_index();
    bool valid_step = active_index >= 0;
    lv_color_t accent_color = valid_step ? LEA_COLOR_MINT : zsw_color_red();

    if (current_stage == LEA_UI_STAGE_COMPLETE) {
        accent_color = zsw_color_blue();
    }

    lv_obj_t *card = lv_obj_create(root_page);
    style_clean_container(card, LEA_COLOR_PANEL_HI, LV_OPA_COVER);
    lv_obj_set_size(card, 166, 34);
    lv_obj_set_style_radius(card, 8, LV_PART_MAIN);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(card, accent_color, LV_PART_MAIN);
    lv_obj_align(card, LV_ALIGN_TOP_MID, 0, 54);

    lv_obj_t *number = lv_obj_create(card);
    style_clean_container(number, accent_color, LV_OPA_COVER);
    lv_obj_set_size(number, 22, 22);
    lv_obj_set_style_radius(number, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_align(number, LV_ALIGN_LEFT_MID, 8, 0);

    char number_text[2] = { valid_step ? '1' + active_index : '!', '\0' };
    lv_obj_t *number_label = create_label(number, number_text, &lv_font_montserrat_12, lv_color_black());
    lv_obj_center(number_label);

    lv_obj_t *title = create_label(card, valid_step ? step_title(active_index) : "Check link", &lv_font_montserrat_12,
                                   lv_color_white());
    lv_obj_set_width(title, 116);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 38, 5);

    int subtitle_index = valid_step ? active_index : 0;
    step_subtitle_labels[subtitle_index] = create_label(card, valid_step ? step_subtitle(active_index) : "Try again",
                                                        &lv_font_montserrat_10, LEA_COLOR_TEXT_DIM);
    lv_obj_set_width(step_subtitle_labels[subtitle_index], 116);
    lv_label_set_long_mode(step_subtitle_labels[subtitle_index], LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(step_subtitle_labels[subtitle_index], LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_align(step_subtitle_labels[subtitle_index], LV_ALIGN_TOP_LEFT, 38, 20);
}

static void create_steps(void)
{
    create_active_step_card();
}

static void set_empty_state_text(const char *text)
{
    if (empty_label != NULL) {
        lv_label_set_text(empty_label, text);
        lv_obj_center(empty_label);
    }
}

static void create_device_list(void)
{
    list_container = lv_obj_create(root_page);
    style_clean_container(list_container, lv_color_black(), LV_OPA_TRANSP);
    lv_obj_set_size(list_container, 166, DEVICE_LIST_HEIGHT);
    lv_obj_align(list_container, LV_ALIGN_BOTTOM_MID, 0, -14);
    lv_obj_set_flex_flow(list_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list_container, 5, LV_PART_MAIN);
    lv_obj_set_scroll_dir(list_container, LV_DIR_VER);
    lv_obj_add_flag(list_container, LV_OBJ_FLAG_SCROLLABLE);

    empty_label = create_label(list_container,
                               current_stage == LEA_UI_STAGE_SOURCE_SCAN ? "Scanning sources" : "Scanning sinks",
                               &lv_font_montserrat_12, LEA_COLOR_TEXT_DIM);
    lv_obj_set_width(empty_label, 150);
    lv_obj_set_style_text_align(empty_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_center(empty_label);
}

static void create_status_panel(const char *title, const char *subtitle, lv_color_t accent_color)
{
    lv_obj_t *panel = lv_obj_create(root_page);
    style_clean_container(panel, LEA_COLOR_PANEL, LV_OPA_COVER);
    lv_obj_set_size(panel, 166, 52);
    lv_obj_set_style_radius(panel, 8, LV_PART_MAIN);
    lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(panel, accent_color, LV_PART_MAIN);
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 102);

    lv_obj_t *title_label = create_label(panel, title, &lv_font_montserrat_14, lv_color_white());
    lv_obj_set_width(title_label, 142);
    lv_label_set_long_mode(title_label, LV_LABEL_LONG_DOT);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 8);

    lv_obj_t *subtitle_label = create_label(panel, subtitle, &lv_font_montserrat_10, LEA_COLOR_TEXT_DIM);
    lv_obj_set_width(subtitle_label, 142);
    lv_label_set_long_mode(subtitle_label, LV_LABEL_LONG_DOT);
    lv_obj_align(subtitle_label, LV_ALIGN_TOP_MID, 0, 29);
}

static void align_success_center(lv_obj_t *obj)
{
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 6);
}

static void success_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    success_tick++;

    for (int ring_index = 0; ring_index < SUCCESS_RING_COUNT; ring_index++) {
        if (success_rings[ring_index] == NULL) {
            continue;
        }

        uint8_t phase = (success_tick * 5 + ring_index * 32) % 100;
        int32_t size = SUCCESS_RING_MIN_SIZE + (phase * SUCCESS_RING_RANGE) / 100;
        lv_opa_t opacity = (lv_opa_t)(180 - (phase * 150) / 100);

        lv_obj_set_size(success_rings[ring_index], size, size);
        align_success_center(success_rings[ring_index]);
        lv_obj_set_style_border_opa(success_rings[ring_index], opacity, LV_PART_MAIN);
    }

    for (int bar_index = 0; bar_index < SUCCESS_BAR_COUNT; bar_index++) {
        if (success_bars[bar_index] == NULL) {
            continue;
        }

        uint8_t phase = (success_tick + bar_index * 2) % 10;
        if (phase > 5) {
            phase = 10 - phase;
        }

        lv_obj_set_height(success_bars[bar_index], 8 + phase * 4);
    }
}

static void create_success_scene(void)
{
    for (int ring_index = 0; ring_index < SUCCESS_RING_COUNT; ring_index++) {
        lv_obj_t *ring = lv_obj_create(root_page);
        style_clean_container(ring, lv_color_black(), LV_OPA_TRANSP);
        lv_obj_set_size(ring, SUCCESS_RING_MIN_SIZE + ring_index * 16, SUCCESS_RING_MIN_SIZE + ring_index * 16);
        lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_set_style_border_width(ring, 2, LV_PART_MAIN);
        lv_obj_set_style_border_color(ring, ring_index == 1 ? zsw_color_blue() : LEA_COLOR_MINT, LV_PART_MAIN);
        lv_obj_set_style_border_opa(ring, LV_OPA_60, LV_PART_MAIN);
        align_success_center(ring);
        success_rings[ring_index] = ring;
    }

    lv_obj_t *title = create_label(root_page, "Broadcast linked", &lv_font_montserrat_14, lv_color_white());
    lv_obj_set_width(title, 160);
    lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 62);

    lv_obj_t *core = lv_obj_create(root_page);
    style_clean_container(core, LEA_COLOR_MINT, LV_OPA_COVER);
    lv_obj_set_size(core, 58, 58);
    lv_obj_set_style_radius(core, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(core, 3, LV_PART_MAIN);
    lv_obj_set_style_border_color(core, zsw_color_blue(), LV_PART_MAIN);
    align_success_center(core);

    lv_obj_t *core_label = create_label(core, "LIVE", &lv_font_montserrat_16, lv_color_black());
    lv_obj_center(core_label);

    lv_obj_t *subtitle = create_label(root_page,
                                      active_name[0] != '\0' ? active_name : "Headphones tuned in",
                                      &lv_font_montserrat_12, LEA_COLOR_TEXT_DIM);
    lv_obj_set_width(subtitle, 160);
    lv_label_set_long_mode(subtitle, LV_LABEL_LONG_DOT);
    lv_obj_align(subtitle, LV_ALIGN_BOTTOM_MID, 0, -42);

    lv_obj_t *bars = lv_obj_create(root_page);
    style_clean_container(bars, lv_color_black(), LV_OPA_TRANSP);
    lv_obj_set_size(bars, 72, 28);
    lv_obj_align(bars, LV_ALIGN_BOTTOM_MID, 0, -14);
    lv_obj_set_flex_flow(bars, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bars, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
    lv_obj_set_style_pad_column(bars, 6, LV_PART_MAIN);

    for (int bar_index = 0; bar_index < SUCCESS_BAR_COUNT; bar_index++) {
        lv_obj_t *bar = lv_obj_create(bars);
        style_clean_container(bar, bar_index % 2 == 0 ? LEA_COLOR_MINT : zsw_color_blue(), LV_OPA_COVER);
        lv_obj_set_size(bar, 6, 10 + bar_index * 2);
        lv_obj_set_style_radius(bar, 3, LV_PART_MAIN);
        success_bars[bar_index] = bar;
    }

    success_tick = 0;
    success_timer = lv_timer_create(success_timer_cb, SUCCESS_TIMER_MS, NULL);
    success_timer_cb(success_timer);
}

static void timeout_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    timeout_timer = NULL;

    if (current_stage == LEA_UI_STAGE_SINK_SCAN && sink_list.num == 0) {
        set_empty_state_text("No sinks found");
        set_text(step_subtitle_labels[0], "Scan timed out");
    } else if (current_stage == LEA_UI_STAGE_SOURCE_SCAN && source_list.num == 0) {
        set_empty_state_text("No sources found");
        set_text(step_subtitle_labels[1], "Scan timed out");
    }
}

static void start_timeout_timer(void)
{
    stop_timeout_timer();
    timeout_timer = lv_timer_create(timeout_timer_cb, SCAN_TIMEOUT_MS, NULL);
    lv_timer_set_repeat_count(timeout_timer, 1);
}

static void build_page(lv_obj_t *root)
{
    clear_root_page();

    root_page = lv_obj_create(root);
    style_clean_container(root_page, lv_color_black(), LV_OPA_COVER);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_center(root_page);
    lv_obj_remove_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    create_header();
    create_progress_dots();

    if (current_stage != LEA_UI_STAGE_COMPLETE) {
        create_steps();
    }

    switch (current_stage) {
    case LEA_UI_STAGE_SINK_SCAN:
    case LEA_UI_STAGE_SOURCE_SCAN:
        create_device_list();
        start_timeout_timer();
        break;
    case LEA_UI_STAGE_CONNECTING:
        create_status_panel(active_name[0] != '\0' ? active_name : "Connecting", "Opening receiver link", zsw_color_blue());
        break;
    case LEA_UI_STAGE_SYNCING:
        create_status_panel(active_name[0] != '\0' ? active_name : "Broadcast source", "Sending broadcast info", LEA_COLOR_MINT);
        break;
    case LEA_UI_STAGE_COMPLETE:
        create_success_scene();
        break;
    case LEA_UI_STAGE_ERROR:
        create_status_panel(error_title[0] != '\0' ? error_title : "LE Audio error",
                            error_message[0] != '\0' ? error_message : "Try again", zsw_color_red());
        break;
    }
}

static void click_event_cb(lv_event_t *event)
{
    lea_assistant_device_t *selected = (lea_assistant_device_t *)lv_event_get_user_data(event);

    stop_timeout_timer();

    if (click_callback != NULL && selected != NULL) {
        click_callback(selected);
    }
}

static void copy_device_name(char *destination, size_t destination_size, const char *source)
{
    if (destination_size == 0) {
        return;
    }

    destination[0] = '\0';
    if (source != NULL) {
        strncpy(destination, source, destination_size - 1);
        destination[destination_size - 1] = '\0';
    }
}

static void lea_assistant_ui_add_list_entry(lea_assistant_device_t *device)
{
    if (list_container == NULL || device == NULL) {
        return;
    }

    if (empty_label != NULL) {
        lv_obj_del(empty_label);
        empty_label = NULL;
    }

    lv_obj_t *button = lv_obj_create(list_container);
    style_clean_container(button, LEA_COLOR_PANEL, LV_OPA_COVER);
    lv_obj_set_width(button, LV_PCT(100));
    lv_obj_set_height(button, DEVICE_ROW_HEIGHT);
    lv_obj_set_style_radius(button, 8, LV_PART_MAIN);
    lv_obj_set_style_border_width(button, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(button, current_stage == LEA_UI_STAGE_SOURCE_SCAN ? LEA_COLOR_MINT : zsw_color_blue(),
                                  LV_PART_MAIN);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_t *name = create_label(button, device->name, &lv_font_montserrat_12, lv_color_white());
    lv_obj_set_width(name, 112);
    lv_label_set_long_mode(name, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(name, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_align(name, LV_ALIGN_TOP_LEFT, 12, 7);

    char meta[18];
    snprintf(meta, sizeof(meta), "%02X:%02X", device->addr.a.val[1], device->addr.a.val[0]);
    lv_obj_t *details = create_label(button, meta, &lv_font_montserrat_10, LEA_COLOR_TEXT_DIM);
    lv_obj_set_width(details, 112);
    lv_obj_set_style_text_align(details, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
    lv_obj_align(details, LV_ALIGN_TOP_LEFT, 12, 24);

    lv_obj_t *chevron = create_label(button, ">", &lv_font_montserrat_14,
                                     current_stage == LEA_UI_STAGE_SOURCE_SCAN ? LEA_COLOR_MINT : zsw_color_blue());
    lv_obj_align(chevron, LV_ALIGN_RIGHT_MID, -12, 0);

    lv_obj_add_event_cb(button, click_event_cb, LV_EVENT_CLICKED, device);

    if (current_stage == LEA_UI_STAGE_SINK_SCAN) {
        set_text(step_subtitle_labels[0], "Tap receiver");
    } else if (current_stage == LEA_UI_STAGE_SOURCE_SCAN) {
        set_text(step_subtitle_labels[1], "Tap broadcast");
    }
}

void lea_assistant_ui_add_sink_list_entry(lea_assistant_device_t *device)
{
    for (size_t device_index = 0; device_index < sink_list.num; device_index++) {
        if (bt_addr_le_cmp(&device->addr, &sink_list.device[device_index].addr) == 0) {
            LOG_DBG("Device already added (%s)", device->name);
            return;
        }
    }

    if (sink_list.num >= MAX_DEVICES_NUM) {
        LOG_WRN("MAX devices reached");
        return;
    }

    bt_addr_le_copy(&sink_list.device[sink_list.num].addr, &device->addr);
    copy_device_name(sink_list.device[sink_list.num].name, sizeof(sink_list.device[sink_list.num].name), device->name);

    lea_assistant_ui_add_list_entry(&sink_list.device[sink_list.num]);
    sink_list.num++;
}

void lea_assistant_ui_add_source_list_entry(lea_assistant_device_t *device)
{
    for (size_t device_index = 0; device_index < source_list.num; device_index++) {
        if (bt_addr_le_cmp(&device->addr, &source_list.device[device_index].addr) == 0) {
            LOG_DBG("Device already added (%s)", device->name);
            return;
        }
    }

    if (source_list.num >= MAX_DEVICES_NUM) {
        LOG_WRN("MAX devices reached");
        return;
    }

    source_list.device[source_list.num].pa_interval = device->pa_interval;
    source_list.device[source_list.num].broadcast_id = device->broadcast_id;
    source_list.device[source_list.num].sid = device->sid;
    bt_addr_le_copy(&source_list.device[source_list.num].addr, &device->addr);
    copy_device_name(source_list.device[source_list.num].name, sizeof(source_list.device[source_list.num].name), device->name);

    lea_assistant_ui_add_list_entry(&source_list.device[source_list.num]);
    source_list.num++;
}

void lea_assistant_ui_show(lv_obj_t *root, on_button_press_cb_t on_button_click_cb, on_close_cb_t close_cb)
{
    sink_list.num = 0;
    source_list.num = 0;
    active_name[0] = '\0';
    error_title[0] = '\0';
    error_message[0] = '\0';
    click_callback = on_button_click_cb;
    close_callback = close_cb;
    current_stage = LEA_UI_STAGE_SINK_SCAN;

    build_page(root);
}

void lea_assistant_ui_show_connecting(lv_obj_t *root, const char *sink_name)
{
    copy_device_name(active_name, sizeof(active_name), sink_name);
    current_stage = LEA_UI_STAGE_CONNECTING;
    build_page(root);
}

void lea_assistant_ui_show_source(lv_obj_t *root, on_button_press_cb_t on_button_click_cb)
{
    source_list.num = 0;
    active_name[0] = '\0';
    click_callback = on_button_click_cb;
    current_stage = LEA_UI_STAGE_SOURCE_SCAN;

    build_page(root);
}

void lea_assistant_ui_show_syncing(lv_obj_t *root, const char *source_name)
{
    copy_device_name(active_name, sizeof(active_name), source_name);
    current_stage = LEA_UI_STAGE_SYNCING;
    build_page(root);
}

void lea_assistant_ui_show_complete(lv_obj_t *root, const char *message)
{
    copy_device_name(active_name, sizeof(active_name), message);
    current_stage = LEA_UI_STAGE_COMPLETE;
    build_page(root);
}

void lea_assistant_ui_show_error(lv_obj_t *root, const char *title, const char *message)
{
    copy_device_name(error_title, sizeof(error_title), title);
    copy_device_name(error_message, sizeof(error_message), message);
    current_stage = LEA_UI_STAGE_ERROR;
    build_page(root);
}

void lea_assistant_ui_remove(void)
{
    clear_root_page();
    click_callback = NULL;
    close_callback = NULL;
}