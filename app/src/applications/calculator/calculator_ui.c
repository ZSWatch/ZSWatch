#include "calculator_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include "smf_calculator_thread.h"

#include <string.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(calculator_ui, LOG_LEVEL_INF);

#define BUTTON_MIN_SIZE   42
#define BUTTON_GAP        4
#define CONTAINER_WIDTH   200
#define ROW_HEIGHT        38
#define SIDE_PADDING      20

// Work item for display updates
static void display_update_work_handler(struct k_work *work);
static K_WORK_DEFINE(display_update_work, display_update_work_handler);

// Buffer for display text used in work handler
static char display_text_buffer[CALCULATOR_STRING_LENGTH];

static void calculator_event_handler(lv_event_t *e);

static lv_obj_t *root_page = NULL;
static lv_obj_t *result_label = NULL;

static struct calculator_event event_ac = {CANCEL_BUTTON, 'C'};
static struct calculator_event event_backspace = {CANCEL_ENTRY, 'E'};
static struct calculator_event event_plus = {OPERATOR, '+'};
static struct calculator_event event_minus = {OPERATOR, '-'};
static struct calculator_event event_multiply = {OPERATOR, '*'};
static struct calculator_event event_divide = {OPERATOR, '/'};
static struct calculator_event event_equals = {EQUALS, '='};
static struct calculator_event event_dot = {DECIMAL_POINT, '.'};
static struct calculator_event events_numbers[10] = {
    {DIGIT_0, '0'}, {DIGIT_1_9, '1'}, {DIGIT_1_9, '2'}, {DIGIT_1_9, '3'}, {DIGIT_1_9, '4'},
    {DIGIT_1_9, '5'}, {DIGIT_1_9, '6'}, {DIGIT_1_9, '7'}, {DIGIT_1_9, '8'}, {DIGIT_1_9, '9'}
};

static lv_obj_t *create_button_row(lv_obj_t *parent, int padding)
{
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_set_size(row, LV_PCT(100), ROW_HEIGHT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(row, BUTTON_GAP, LV_PART_MAIN);

    if (padding > 0) {
        lv_obj_set_style_pad_left(row, padding, LV_PART_MAIN);
        lv_obj_set_style_pad_right(row, padding, LV_PART_MAIN);
    }

    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    return row;
}

static lv_obj_t *create_flex_button(lv_obj_t *parent, const char *text, lv_color_t bg_color,
                                    lv_color_t text_color, struct calculator_event *event, bool is_operator)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_flex_grow(btn, 1);
    lv_obj_set_height(btn, ROW_HEIGHT);
    lv_obj_set_style_radius(btn, (ROW_HEIGHT - 4) / 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, bg_color, LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    lv_obj_set_style_text_color(label, text_color, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, LV_PART_MAIN);

    lv_obj_set_user_data(btn, event);
    lv_obj_add_event_cb(btn, calculator_event_handler, LV_EVENT_CLICKED, NULL);

    return btn;
}

static void calculator_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        struct calculator_event *event = (struct calculator_event *)lv_obj_get_user_data(obj);

        if (event) {
            LOG_DBG("Posting event: %d, operand: %c", event->event_id, event->operand);
            int ret = post_calculator_event(event, K_FOREVER);
            if (ret != 0) {
                LOG_ERR("Failed to post event: %d", ret);
            }
        }
    }
}

static void display_update_work_handler(struct k_work *work)
{
    if (result_label) {
        const char *text = display_text_buffer;
        while (*text == ' ' && *(text + 1) != '\0') {
            text++;
        }
        if (*text == '\0') {
            text = "0";
        }
        lv_label_set_text(result_label, text);
        LOG_DBG("Display updated to: '%s'", text);
    }
}

void calculator_ui_show(lv_obj_t *root)
{
    root_page = lv_obj_create(root);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(root_page, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(root_page, 0, LV_PART_MAIN);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    // Main button container using flex layout
    lv_obj_t *button_container = lv_obj_create(root_page);
    lv_obj_set_size(button_container, CONTAINER_WIDTH, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(button_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(button_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(button_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(button_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(button_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(button_container, BUTTON_GAP, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(button_container, 50, LV_PART_MAIN);
    lv_obj_clear_flag(button_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(button_container, LV_ALIGN_CENTER, 0, 35);

    // Display area at top
    lv_obj_t *display_panel = lv_obj_create(root_page);
    lv_obj_set_size(display_panel, CONTAINER_WIDTH, 25);
    lv_obj_set_style_bg_opa(display_panel, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_bg_color(display_panel, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_border_width(display_panel, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(display_panel, 6, LV_PART_MAIN);
    lv_obj_set_style_pad_left(display_panel, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_right(display_panel, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_top(display_panel, 2, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(display_panel, 2, LV_PART_MAIN);
    lv_obj_clear_flag(display_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(display_panel, LV_ALIGN_TOP_MID, 0, 0);

    result_label = lv_label_create(display_panel);
    lv_obj_set_width(result_label, LV_PCT(100));
    lv_label_set_long_mode(result_label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(result_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(result_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(result_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_pad_top(result_label, 5, LV_PART_MAIN);
    lv_obj_align(result_label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(result_label, "0");

    lv_color_t number_color = lv_color_hex(0x505050);    // Dark gray for numbers
    lv_color_t operator_color = lv_color_hex(0xFF9500);  // Orange for operators
    lv_color_t function_color = lv_color_hex(0xA6A6A6);  // Light gray for functions
    lv_color_t white_text = lv_color_white();
    lv_color_t black_text = lv_color_black();

    // Row 1: AC, Backspace, Divide with padding to center the three buttons
    lv_obj_t *row1 = create_button_row(button_container, SIDE_PADDING);
    create_flex_button(row1, "AC", function_color, black_text, &event_ac, false);
    create_flex_button(row1, LV_SYMBOL_BACKSPACE, function_color, black_text, &event_backspace, false);
    create_flex_button(row1, "/", operator_color, white_text, &event_divide, true);

    // Row 2: 7, 8, 9, Multiply
    lv_obj_t *row2 = create_button_row(button_container, 0);
    create_flex_button(row2, "7", number_color, white_text, &events_numbers[7], false);
    create_flex_button(row2, "8", number_color, white_text, &events_numbers[8], false);
    create_flex_button(row2, "9", number_color, white_text, &events_numbers[9], false);
    create_flex_button(row2, "x", operator_color, white_text, &event_multiply, true);

    // Row 3: 4, 5, 6, Minus
    lv_obj_t *row3 = create_button_row(button_container, 0);
    create_flex_button(row3, "4", number_color, white_text, &events_numbers[4], false);
    create_flex_button(row3, "5", number_color, white_text, &events_numbers[5], false);
    create_flex_button(row3, "6", number_color, white_text, &events_numbers[6], false);
    create_flex_button(row3, "-", operator_color, white_text, &event_minus, true);

    // Row 4: 1, 2, 3, Plus
    lv_obj_t *row4 = create_button_row(button_container, 0);
    create_flex_button(row4, "1", number_color, white_text, &events_numbers[1], false);
    create_flex_button(row4, "2", number_color, white_text, &events_numbers[2], false);
    create_flex_button(row4, "3", number_color, white_text, &events_numbers[3], false);
    create_flex_button(row4, "+", operator_color, white_text, &event_plus, true);

    // Row 5: 0, ., Equals with padding to center the three buttons
    lv_obj_t *row5 = create_button_row(button_container, SIDE_PADDING + 10);
    create_flex_button(row5, "0", number_color, white_text, &events_numbers[0], false);
    create_flex_button(row5, ".", number_color, white_text, &event_dot, false);
    create_flex_button(row5, "=", operator_color, white_text, &event_equals, true);

    LOG_DBG("Calculator UI created successfully");
}

void calculator_ui_remove(void)
{
    k_work_cancel(&display_update_work);

    if (root_page) {
        lv_obj_del(root_page);
        root_page = NULL;
        result_label = NULL;
    }

    LOG_DBG("Calculator UI removed");
}

void calculator_ui_update_display(const char *text)
{
    if (!text) {
        LOG_ERR("calculator_ui_update_display: text is NULL");
        return;
    }

    LOG_DBG("calculator_ui_update_display called with: '%s'", text);

    strncpy(display_text_buffer, text, CALCULATOR_STRING_LENGTH - 1);
    display_text_buffer[CALCULATOR_STRING_LENGTH - 1] = '\0';

    // LVGL has to be called from system workqueue
    k_work_submit(&display_update_work);
}

