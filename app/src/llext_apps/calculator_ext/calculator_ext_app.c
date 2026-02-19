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

/*
 * LLEXT version of the Calculator app.
 *
 * Combines calculator_app.c, calculator_ui.c, and smf_calculator_thread.c
 * into a single LLEXT module.
 *
 * K_THREAD_DEFINE → k_thread_create() at app start / k_thread_abort() at stop
 * K_MSGQ_DEFINE  → k_msgq_init() at app start
 * K_WORK_DEFINE  → k_work_init() at app start
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <zephyr/sys/printk.h>
#include <zephyr/smf.h>
#include <lvgl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

/* ---- Icon image compiled into .rodata (XIP flash) ---- */
#include "statistic_icon.c"

/* ---- Forward declarations ---- */
static void calculator_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void calculator_app_stop(void *user_data);

/* ---- App registration ---- */
static application_t app = {
    .name = "Calc",
    .icon = &statistic_icon,
    .start_func = calculator_app_start,
    .stop_func = calculator_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

/* ===========================================================================
 * SMF Calculator Thread (from smf_calculator_thread.c)
 * =========================================================================== */

#define SMF_THREAD_STACK_SIZE 1024
#define SMF_THREAD_PRIORITY   7
#define CALCULATOR_MAX_DIGITS 15
#define CALCULATOR_STRING_LENGTH (CALCULATOR_MAX_DIGITS + 2)

enum calculator_events {
    DIGIT_0,
    DIGIT_1_9,
    DECIMAL_POINT,
    OPERATOR,
    EQUALS,
    CANCEL_ENTRY,
    CANCEL_BUTTON,
};

struct calculator_event {
    enum calculator_events event_id;
    char operand;
};

/* Thread and msgq — runtime initialized */
static K_THREAD_STACK_DEFINE(smf_stack, SMF_THREAD_STACK_SIZE);
static struct k_thread smf_thread_data;
static k_tid_t smf_thread_id;

static char __aligned(4) msgq_buffer[sizeof(struct calculator_event) * 8];
static struct k_msgq event_msgq;

#define RESULT_STRING_LENGTH 64

enum display_mode {
    DISPLAY_OPERAND_1,
    DISPLAY_OPERAND_2,
    DISPLAY_RESULT,
    DISPLAY_ERROR,
};

struct operand {
    char string[CALCULATOR_STRING_LENGTH];
    int index;
};

struct s_object {
    struct smf_ctx ctx;
    struct calculator_event event;
    struct operand operand_1;
    struct operand operand_2;
    char operator_btn;
    struct operand result;
} s_obj;

static enum display_mode current_display_mode = DISPLAY_OPERAND_1;

/* ---- Calculator UI (from calculator_ui.c) ---- */

#define BUTTON_MIN_SIZE   42
#define BUTTON_GAP        4
#define CONTAINER_WIDTH   200
#define ROW_HEIGHT        38
#define SIDE_PADDING      20

static void display_update_work_handler(struct k_work *work);
static struct k_work display_update_work;

static char display_text_buffer[CALCULATOR_STRING_LENGTH];
static void calculator_event_handler(lv_event_t *e);

static lv_obj_t *ui_root_page = NULL;
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

static int post_calculator_event(struct calculator_event *event, k_timeout_t timeout)
{
    return k_msgq_put(&event_msgq, event, timeout);
}

static void calculator_ui_update_display(const char *text)
{
    if (!text) {
        return;
    }
    strncpy(display_text_buffer, text, CALCULATOR_STRING_LENGTH - 1);
    display_text_buffer[CALCULATOR_STRING_LENGTH - 1] = '\0';
    k_work_submit(&display_update_work);
}

static void update_display(const char *text)
{
    calculator_ui_update_display(text);
}

/* ---- SMF state machine ---- */

static void set_display_mode(enum display_mode mode)
{
    current_display_mode = mode;
}

static void setup_operand(struct operand *op)
{
    op->index = 1;
    op->string[0] = ' ';
    op->string[1] = '0';
    op->string[2] = 0x00;
}

static int insert(struct operand *op, char digit)
{
    if (op->index >= (CALCULATOR_STRING_LENGTH - 1)) {
        return -ENOBUFS;
    }
    op->string[op->index++] = digit;
    op->string[op->index] = 0x00;
    return 0;
}

static void negate(struct operand *op)
{
    if (op->string[0] == ' ') {
        op->string[0] = '-';
    } else {
        op->string[0] = ' ';
    }
}

static void copy_operand(struct operand *dest, struct operand *src)
{
    strncpy(dest->string, src->string, CALCULATOR_STRING_LENGTH);
    dest->index = src->index;
}

static int calculate_result(struct s_object *s)
{
    double operand_1 = strtod(s->operand_1.string, NULL);
    double operand_2 = strtod(s->operand_2.string, NULL);
    double result;
    char result_string[RESULT_STRING_LENGTH];

    switch (s->operator_btn) {
        case '+':
            result = operand_1 + operand_2;
            break;
        case '-':
            result = operand_1 - operand_2;
            break;
        case '*':
            result = operand_1 * operand_2;
            break;
        case '/':
            if (operand_2 != 0.0) {
                result = operand_1 / operand_2;
            } else {
                return -1;
            }
            break;
        default:
            return -1;
    }

    snprintf(result_string, RESULT_STRING_LENGTH, "% f", result);

    for (int i = strlen(result_string) - 1; i >= 0; i--) {
        if (result_string[i] != '0') {
            if (result_string[i] == '.') {
                result_string[i] = 0x00;
            } else {
                result_string[i + 1] = 0x00;
            }
            break;
        }
    }

    strncpy(s->result.string, result_string, CALCULATOR_STRING_LENGTH - 1);
    s->result.string[CALCULATOR_STRING_LENGTH - 1] = 0x00;
    s->result.index = strlen(s->result.string);
    return 0;
}

static void chain_calculations(struct s_object *s)
{
    copy_operand(&s->operand_1, &s->result);
    setup_operand(&s->operand_2);
}

enum demo_states {
    STATE_ON,
    STATE_READY,
    STATE_RESULT,
    STATE_BEGIN,
    STATE_NEGATED_1,
    STATE_OPERAND_1,
    STATE_ZERO_1,
    STATE_INT_1,
    STATE_FRAC_1,
    STATE_NEGATED_2,
    STATE_OPERAND_2,
    STATE_ZERO_2,
    STATE_INT_2,
    STATE_FRAC_2,
    STATE_OP_ENTERED,
    STATE_OP_CHAINED,
    STATE_OP_NORMAL,
    STATE_ERROR,
};

static const struct smf_state calculator_states[];

static void on_entry(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    setup_operand(&s->operand_1);
    setup_operand(&s->operand_2);
    setup_operand(&s->result);
    s->operator_btn = 0x00;
}

static enum smf_state_result on_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case CANCEL_BUTTON:
            smf_set_state(&s->ctx, &calculator_states[STATE_ON]);
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static enum smf_state_result ready_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case DECIMAL_POINT:
            insert(&s->operand_1, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_FRAC_1]);
            break;
        case DIGIT_1_9:
            insert(&s->operand_1, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_INT_1]);
            break;
        case DIGIT_0:
            smf_set_state(&s->ctx, &calculator_states[STATE_ZERO_1]);
            break;
        case OPERATOR:
            s->operator_btn = s->event.operand;
            smf_set_state(&s->ctx, &calculator_states[STATE_OP_CHAINED]);
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static void result_entry(void *obj) { set_display_mode(DISPLAY_RESULT); }
static enum smf_state_result result_run(void *obj) { return SMF_EVENT_PROPAGATE; }

static void begin_entry(void *obj) { set_display_mode(DISPLAY_OPERAND_1); }
static enum smf_state_result begin_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case OPERATOR:
            if (s->event.operand == '-') {
                smf_set_state(&s->ctx, &calculator_states[STATE_NEGATED_1]);
            }
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static void negated_1_entry(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    negate(&s->operand_1);
}

static enum smf_state_result negated_1_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case DECIMAL_POINT:
            insert(&s->operand_1, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_FRAC_1]);
            break;
        case DIGIT_1_9:
            insert(&s->operand_1, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_INT_1]);
            break;
        case DIGIT_0:
            smf_set_state(&s->ctx, &calculator_states[STATE_ZERO_1]);
            break;
        case OPERATOR:
            if (s->event.operand == '-') {
                return SMF_EVENT_HANDLED;
            }
            break;
        case CANCEL_ENTRY:
            setup_operand(&s->operand_1);
            smf_set_state(&s->ctx, &calculator_states[STATE_BEGIN]);
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static void operand_1_entry(void *obj) { set_display_mode(DISPLAY_OPERAND_1); }
static enum smf_state_result operand_1_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case OPERATOR:
            s->operator_btn = s->event.operand;
            smf_set_state(&s->ctx, &calculator_states[STATE_OP_ENTERED]);
            break;
        case CANCEL_ENTRY:
            setup_operand(&s->operand_1);
            smf_set_state(&s->ctx, &calculator_states[STATE_READY]);
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static enum smf_state_result zero_1_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case DIGIT_0:
            return SMF_EVENT_HANDLED;
        case DIGIT_1_9:
            insert(&s->operand_1, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_INT_1]);
            break;
        case DECIMAL_POINT:
            insert(&s->operand_1, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_FRAC_1]);
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static enum smf_state_result int_1_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case DIGIT_0:
        case DIGIT_1_9:
            insert(&s->operand_1, s->event.operand);
            return SMF_EVENT_HANDLED;
        case DECIMAL_POINT:
            insert(&s->operand_1, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_FRAC_1]);
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static enum smf_state_result frac_1_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case DIGIT_0:
        case DIGIT_1_9:
            insert(&s->operand_1, s->event.operand);
            return SMF_EVENT_HANDLED;
        case DECIMAL_POINT:
            return SMF_EVENT_HANDLED;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static void negated_2_entry(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    negate(&s->operand_2);
}

static enum smf_state_result negated_2_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case DECIMAL_POINT:
            insert(&s->operand_2, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_FRAC_2]);
            break;
        case DIGIT_1_9:
            insert(&s->operand_2, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_INT_2]);
            break;
        case DIGIT_0:
            smf_set_state(&s->ctx, &calculator_states[STATE_ZERO_2]);
            break;
        case OPERATOR:
            if (s->event.operand == '-') {
                return SMF_EVENT_HANDLED;
            }
            break;
        case CANCEL_ENTRY:
            setup_operand(&s->operand_2);
            smf_set_state(&s->ctx, &calculator_states[STATE_OP_ENTERED]);
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static void operand_2_entry(void *obj) { set_display_mode(DISPLAY_OPERAND_2); }
static enum smf_state_result operand_2_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case CANCEL_ENTRY:
            setup_operand(&s->operand_2);
            smf_set_state(&s->ctx, &calculator_states[STATE_OP_ENTERED]);
            break;
        case OPERATOR:
            if (calculate_result(s) == 0) {
                chain_calculations(s);
                s->operator_btn = s->event.operand;
                smf_set_state(&s->ctx, &calculator_states[STATE_OP_CHAINED]);
            } else {
                smf_set_state(&s->ctx, &calculator_states[STATE_ERROR]);
            }
            break;
        case EQUALS:
            if (calculate_result(s) == 0) {
                chain_calculations(s);
                smf_set_state(&s->ctx, &calculator_states[STATE_RESULT]);
            } else {
                smf_set_state(&s->ctx, &calculator_states[STATE_ERROR]);
            }
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static enum smf_state_result zero_2_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case DIGIT_0:
            return SMF_EVENT_HANDLED;
        case DIGIT_1_9:
            insert(&s->operand_2, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_INT_2]);
            break;
        case DECIMAL_POINT:
            insert(&s->operand_2, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_FRAC_2]);
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static enum smf_state_result int_2_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case DIGIT_0:
        case DIGIT_1_9:
            insert(&s->operand_2, s->event.operand);
            return SMF_EVENT_HANDLED;
        case DECIMAL_POINT:
            insert(&s->operand_2, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_FRAC_2]);
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static enum smf_state_result frac_2_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case DIGIT_0:
        case DIGIT_1_9:
            insert(&s->operand_2, s->event.operand);
            return SMF_EVENT_HANDLED;
        case DECIMAL_POINT:
            return SMF_EVENT_HANDLED;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static enum smf_state_result op_entered_run(void *obj)
{
    struct s_object *s = (struct s_object *)obj;
    switch (s->event.event_id) {
        case DIGIT_0:
            smf_set_state(&s->ctx, &calculator_states[STATE_ZERO_2]);
            break;
        case DIGIT_1_9:
            insert(&s->operand_2, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_INT_2]);
            break;
        case DECIMAL_POINT:
            insert(&s->operand_2, s->event.operand);
            smf_set_state(&s->ctx, &calculator_states[STATE_FRAC_2]);
            break;
        case OPERATOR:
            if (s->event.operand == '-') {
                smf_set_state(&s->ctx, &calculator_states[STATE_NEGATED_2]);
            }
            break;
        default:
            break;
    }
    return SMF_EVENT_PROPAGATE;
}

static void op_chained_entry(void *obj) { set_display_mode(DISPLAY_OPERAND_1); }
static void op_normal_entry(void *obj) { set_display_mode(DISPLAY_OPERAND_2); }
static void error_entry(void *obj) { set_display_mode(DISPLAY_ERROR); }

static const struct smf_state calculator_states[] = {
    [STATE_ON] =         SMF_CREATE_STATE(on_entry, on_run, NULL,
                                          NULL, &calculator_states[STATE_READY]),
    [STATE_READY] =      SMF_CREATE_STATE(NULL, ready_run, NULL,
                                          &calculator_states[STATE_ON],
                                          &calculator_states[STATE_BEGIN]),
    [STATE_RESULT] =     SMF_CREATE_STATE(result_entry, result_run,
                                          NULL, &calculator_states[STATE_READY], NULL),
    [STATE_BEGIN] =      SMF_CREATE_STATE(begin_entry, begin_run, NULL,
                                          &calculator_states[STATE_READY], NULL),
    [STATE_NEGATED_1] =  SMF_CREATE_STATE(negated_1_entry, negated_1_run, NULL,
                                          &calculator_states[STATE_ON], NULL),
    [STATE_OPERAND_1] =  SMF_CREATE_STATE(operand_1_entry, operand_1_run, NULL,
                                          &calculator_states[STATE_ON], NULL),
    [STATE_ZERO_1] =     SMF_CREATE_STATE(NULL, zero_1_run, NULL,
                                          &calculator_states[STATE_OPERAND_1], NULL),
    [STATE_INT_1] =      SMF_CREATE_STATE(NULL, int_1_run, NULL,
                                          &calculator_states[STATE_OPERAND_1], NULL),
    [STATE_FRAC_1] =     SMF_CREATE_STATE(NULL, frac_1_run, NULL,
                                          &calculator_states[STATE_OPERAND_1], NULL),
    [STATE_NEGATED_2] =  SMF_CREATE_STATE(negated_2_entry, negated_2_run, NULL,
                                          &calculator_states[STATE_ON], NULL),
    [STATE_OPERAND_2] =  SMF_CREATE_STATE(operand_2_entry, operand_2_run, NULL,
                                          &calculator_states[STATE_ON], NULL),
    [STATE_ZERO_2] =     SMF_CREATE_STATE(NULL, zero_2_run, NULL,
                                          &calculator_states[STATE_OPERAND_2], NULL),
    [STATE_INT_2] =      SMF_CREATE_STATE(NULL, int_2_run, NULL,
                                          &calculator_states[STATE_OPERAND_2], NULL),
    [STATE_FRAC_2] =     SMF_CREATE_STATE(NULL, frac_2_run, NULL,
                                          &calculator_states[STATE_OPERAND_2], NULL),
    [STATE_OP_ENTERED] = SMF_CREATE_STATE(NULL, op_entered_run, NULL,
                                          &calculator_states[STATE_ON],
                                          &calculator_states[STATE_OP_NORMAL]),
    [STATE_OP_CHAINED] = SMF_CREATE_STATE(op_chained_entry, NULL, NULL,
                                          &calculator_states[STATE_OP_ENTERED], NULL),
    [STATE_OP_NORMAL] =  SMF_CREATE_STATE(op_normal_entry, NULL, NULL,
                                          &calculator_states[STATE_OP_ENTERED], NULL),
    [STATE_ERROR] =      SMF_CREATE_STATE(error_entry, NULL, NULL,
                                          &calculator_states[STATE_ON], NULL),
};

static void output_display(void)
{
    char *output;
    switch (current_display_mode) {
        case DISPLAY_OPERAND_1:
            output = s_obj.operand_1.string;
            break;
        case DISPLAY_OPERAND_2:
            output = s_obj.operand_2.string;
            break;
        case DISPLAY_RESULT:
            output = s_obj.result.string;
            break;
        case DISPLAY_ERROR:
            output = "ERROR";
            break;
        default:
            output = "";
    }
    update_display(output);
}

static volatile bool smf_thread_running;

static void smf_calculator_thread_fn(void *arg1, void *arg2, void *arg3)
{
    smf_set_initial(SMF_CTX(&s_obj), &calculator_states[STATE_ON]);
    while (smf_thread_running) {
        int rc = k_msgq_get(&event_msgq, &s_obj.event, K_MSEC(100));
        if (rc != 0) {
            continue;
        }
        int ret = smf_run_state(SMF_CTX(&s_obj));
        if (ret) {
            break;
        }
        output_display();
    }
}

/* ---- Calculator UI ---- */

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
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    return row;
}

static lv_obj_t *create_flex_button(lv_obj_t *parent, const char *text, lv_color_t bg_color,
                                    lv_color_t text_color, struct calculator_event *event, bool is_operator)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_flex_grow(btn, 1);
    lv_obj_set_height(btn, ROW_HEIGHT);
    lv_obj_set_style_radius(btn, (ROW_HEIGHT - 4) / 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, bg_color, LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_remove_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

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
            post_calculator_event(event, K_FOREVER);
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
    }
}

static void calculator_ui_show(lv_obj_t *root)
{
    ui_root_page = lv_obj_create(root);
    lv_obj_set_size(ui_root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(ui_root_page, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_root_page, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ui_root_page, 0, LV_PART_MAIN);
    lv_obj_remove_flag(ui_root_page, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *button_container = lv_obj_create(ui_root_page);
    lv_obj_set_size(button_container, CONTAINER_WIDTH, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(button_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(button_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(button_container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(button_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(button_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(button_container, BUTTON_GAP, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(button_container, 50, LV_PART_MAIN);
    lv_obj_remove_flag(button_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(button_container, LV_ALIGN_CENTER, 0, 35);

    lv_obj_t *display_panel = lv_obj_create(ui_root_page);
    lv_obj_set_size(display_panel, CONTAINER_WIDTH, 25);
    lv_obj_set_style_bg_opa(display_panel, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_bg_color(display_panel, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_border_width(display_panel, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(display_panel, 6, LV_PART_MAIN);
    lv_obj_set_style_pad_left(display_panel, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_right(display_panel, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_top(display_panel, 2, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(display_panel, 2, LV_PART_MAIN);
    lv_obj_remove_flag(display_panel, LV_OBJ_FLAG_SCROLLABLE);
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

    lv_color_t number_color = lv_color_hex(0x505050);
    lv_color_t operator_color = lv_color_hex(0xFF9500);
    lv_color_t function_color = lv_color_hex(0xA6A6A6);
    lv_color_t white_text = lv_color_white();
    lv_color_t black_text = lv_color_black();

    lv_obj_t *row1 = create_button_row(button_container, SIDE_PADDING);
    create_flex_button(row1, "AC", function_color, black_text, &event_ac, false);
    create_flex_button(row1, LV_SYMBOL_BACKSPACE, function_color, black_text, &event_backspace, false);
    create_flex_button(row1, "/", operator_color, white_text, &event_divide, true);

    lv_obj_t *row2 = create_button_row(button_container, 0);
    create_flex_button(row2, "7", number_color, white_text, &events_numbers[7], false);
    create_flex_button(row2, "8", number_color, white_text, &events_numbers[8], false);
    create_flex_button(row2, "9", number_color, white_text, &events_numbers[9], false);
    create_flex_button(row2, "x", operator_color, white_text, &event_multiply, true);

    lv_obj_t *row3 = create_button_row(button_container, 0);
    create_flex_button(row3, "4", number_color, white_text, &events_numbers[4], false);
    create_flex_button(row3, "5", number_color, white_text, &events_numbers[5], false);
    create_flex_button(row3, "6", number_color, white_text, &events_numbers[6], false);
    create_flex_button(row3, "-", operator_color, white_text, &event_minus, true);

    lv_obj_t *row4 = create_button_row(button_container, 0);
    create_flex_button(row4, "1", number_color, white_text, &events_numbers[1], false);
    create_flex_button(row4, "2", number_color, white_text, &events_numbers[2], false);
    create_flex_button(row4, "3", number_color, white_text, &events_numbers[3], false);
    create_flex_button(row4, "+", operator_color, white_text, &event_plus, true);

    lv_obj_t *row5 = create_button_row(button_container, SIDE_PADDING + 10);
    create_flex_button(row5, "0", number_color, white_text, &events_numbers[0], false);
    create_flex_button(row5, ".", number_color, white_text, &event_dot, false);
    create_flex_button(row5, "=", operator_color, white_text, &event_equals, true);
}

static void calculator_ui_remove(void)
{
    k_work_cancel(&display_update_work);
    if (ui_root_page) {
        lv_obj_delete(ui_root_page);
        ui_root_page = NULL;
        result_label = NULL;
    }
}

/* ---- App lifecycle ---- */

static void calculator_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    LV_UNUSED(group);

    calculator_ui_show(root);

    /* Start the SMF thread */
    smf_thread_running = true;
    smf_thread_id = k_thread_create(&smf_thread_data, smf_stack,
                                     K_THREAD_STACK_SIZEOF(smf_stack),
                                     smf_calculator_thread_fn,
                                     NULL, NULL, NULL,
                                     SMF_THREAD_PRIORITY, 0, K_NO_WAIT);
}

static void calculator_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);

    /* Stop the SMF thread */
    smf_thread_running = false;
    if (smf_thread_id) {
        k_thread_join(&smf_thread_data, K_MSEC(200));
        smf_thread_id = NULL;
    }

    calculator_ui_remove();
}

/* ---- Entry point ---- */
application_t *app_entry(void)
{
    printk("calculator_ext: app_entry called\n");

    /* Initialize msgq and work at load time */
    k_msgq_init(&event_msgq, msgq_buffer, sizeof(struct calculator_event), 8);
    k_work_init(&display_update_work, display_update_work_handler);

    return &app;
}
EXPORT_SYMBOL(app_entry);
