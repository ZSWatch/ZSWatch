/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project, Leonardo Bispo.
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
 * LLEXT version of the Trivia app.
 * Combines trivia_app.c and trivia_ui.c into a single LLEXT module.
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <zephyr/sys/printk.h>
#include <lvgl.h>
#include <string.h>
#include <stdio.h>

#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "ble/ble_http.h"
#include "ble/ble_comm.h"
#include "cJSON.h"

#define HTTP_REQUEST_URL "https://opentdb.com/api.php?amount=1&difficulty=easy&type=boolean"
#define MAX_QUESTION_LEN (MAX_HTTP_FIELD_LENGTH + 1)

/* ---- Icon image compiled into .rodata (XIP flash) ---- */
#include "quiz.c"

/* ---- Forward declarations ---- */
static void trivia_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void trivia_app_stop(void *user_data);

/* ---- App registration ---- */
static application_t app = {
    .name = "Trivia",
    .icon = &quiz,
    .start_func = trivia_app_start,
    .stop_func = trivia_app_stop,
    .category = ZSW_APP_CATEGORY_GAMES,
};

/* ---- Trivia data ---- */
typedef enum trivia_button {
    TRUE_BUTTON = 0,
    FALSE_BUTTON,
    PLAY_MORE_BUTTON,
    CLOSE_BUTTON,
} trivia_button_t;

typedef struct trivia_app_question {
    char question[MAX_QUESTION_LEN];
    bool correct_answer;
} trivia_app_question_t;

static trivia_app_question_t trivia_app_question;

/* ---- UI state ---- */
static lv_obj_t *root_page;
static lv_obj_t *question_lb;
static lv_obj_t *mbox;
static lv_obj_t *more_btn;
static lv_obj_t *close_btn;

typedef void (*on_button_press_cb_t)(trivia_button_t trivia_button);
static on_button_press_cb_t click_callback;

#define CLOSE_TXT "Close"

/* ---- UI functions ---- */

static void click_event_cb(lv_event_t *e)
{
    trivia_button_t trivia_button = *(trivia_button_t *)lv_event_get_user_data(e);
    click_callback(trivia_button);
}

static void click_popup_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target_obj(e);
    trivia_button_t trivia_button;

    if (obj == close_btn) {
        trivia_button = CLOSE_BUTTON;
    } else if (obj == more_btn) {
        trivia_button = PLAY_MORE_BUTTON;
    } else {
        return;
    }
    click_callback(trivia_button);
}

static void trivia_ui_show(lv_obj_t *root, on_button_press_cb_t on_button_click_cb)
{
    click_callback = on_button_click_cb;
    mbox = NULL;

    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    question_lb = lv_label_create(root_page);
    lv_obj_set_width(question_lb, LV_PCT(100));
    lv_label_set_long_mode(question_lb, LV_LABEL_LONG_WRAP);
    lv_obj_align(question_lb, LV_ALIGN_TOP_MID, 0, 35);
    lv_obj_set_style_text_align(question_lb, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(question_lb, &lv_font_montserrat_16, 0);
    lv_label_set_text(question_lb, "-");

    lv_obj_t *btn_true = lv_button_create(root_page);
    static trivia_button_t trivia_button_true = TRUE_BUTTON;
    lv_obj_add_event_cb(btn_true, click_event_cb, LV_EVENT_CLICKED, &trivia_button_true);
    lv_obj_align(btn_true, LV_ALIGN_CENTER, -45, 45);

    lv_obj_t *label_true = lv_label_create(btn_true);
    lv_label_set_text(label_true, "True");
    lv_obj_center(label_true);

    lv_obj_t *btn_false = lv_button_create(root_page);
    static trivia_button_t trivia_button_false = FALSE_BUTTON;
    lv_obj_add_event_cb(btn_false, click_event_cb, LV_EVENT_CLICKED, &trivia_button_false);
    lv_obj_align(btn_false, LV_ALIGN_CENTER, 45, 45);

    lv_obj_t *label_false = lv_label_create(btn_false);
    lv_label_set_text(label_false, "False");
    lv_obj_center(label_false);
}

static void trivia_ui_close_popup(void)
{
    if (mbox != NULL) {
        lv_msgbox_close(mbox);
        mbox = NULL;
    }
}

static void trivia_ui_remove(void)
{
    trivia_ui_close_popup();
    lv_obj_delete(root_page);
    root_page = NULL;
}

static void trivia_ui_guess_feedback(bool correct)
{
    char msg[sizeof("Your answer is correct!")];
    sprintf(msg, "Your answer is %s!", correct ? "Correct" : "Wrong");

    mbox = lv_msgbox_create(NULL);
    lv_msgbox_add_text(mbox, msg);
    more_btn = lv_msgbox_add_footer_button(mbox, "More");
    close_btn = lv_msgbox_add_footer_button(mbox, CLOSE_TXT);
    lv_obj_add_event_cb(more_btn, click_popup_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(close_btn, click_popup_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_center(mbox);
}

static void trivia_ui_not_supported(void)
{
    mbox = lv_msgbox_create(NULL);
    lv_msgbox_add_text(mbox, "Your phone does not support this app");
    close_btn = lv_msgbox_add_footer_button(mbox, CLOSE_TXT);
    lv_obj_add_event_cb(close_btn, click_popup_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_center(mbox);
}

static void trivia_ui_update_question(const char *buff)
{
    lv_label_set_text_fmt(question_lb, "%s", buff);
}

/* ---- App logic ---- */

static void request_new_question(void);

static void http_rsp_cb(ble_http_status_code_t status, char *response)
{
    if (status == BLE_HTTP_STATUS_OK && app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        cJSON *parsed_response = cJSON_Parse(response);
        if (parsed_response == NULL) {
            printk("trivia_ext: Failed to parse JSON\n");
        } else {
            cJSON *results = cJSON_GetObjectItem(parsed_response, "results");
            if (cJSON_GetArraySize(results) == 1) {
                cJSON *result = cJSON_GetArrayItem(results, 0);
                cJSON *question = cJSON_GetObjectItem(result, "question");
                cJSON *correct_answer = cJSON_GetObjectItem(result, "correct_answer");
                if (question == NULL || correct_answer == NULL) {
                    printk("trivia_ext: Failed to parse JSON data\n");
                    cJSON_Delete(parsed_response);
                    return;
                }
                memset(trivia_app_question.question, 0, sizeof(trivia_app_question.question));
                strncpy(trivia_app_question.question, question->valuestring, sizeof(trivia_app_question.question) - 1);
                trivia_app_question.correct_answer = (correct_answer->valuestring[0] == 'F') ? false : true;
                trivia_ui_update_question(trivia_app_question.question);
            } else {
                printk("trivia_ext: Unexpected number of results\n");
            }
            cJSON_Delete(parsed_response);
        }
    }
}

static void request_new_question(void)
{
    if (zsw_ble_http_get(HTTP_REQUEST_URL, http_rsp_cb) == -EINVAL) {
        trivia_ui_not_supported();
    }
}

static void on_button_click(trivia_button_t trivia_button)
{
    switch (trivia_button) {
        case TRUE_BUTTON:
            trivia_ui_guess_feedback(trivia_app_question.correct_answer == true);
            break;
        case FALSE_BUTTON:
            trivia_ui_guess_feedback(trivia_app_question.correct_answer == false);
            break;
        case PLAY_MORE_BUTTON:
            trivia_ui_close_popup();
            trivia_ui_update_question("-");
            request_new_question();
            break;
        case CLOSE_BUTTON:
            zsw_app_manager_exit_app();
            break;
        default:
            break;
    }
}

/* ---- App lifecycle ---- */

static void trivia_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    LV_UNUSED(group);
    trivia_ui_show(root, on_button_click);
    request_new_question();
}

static void trivia_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    trivia_ui_remove();
}

/* ---- Entry point ---- */
application_t *app_entry(void)
{
    printk("trivia_ext: app_entry called\n");
    return &app;
}
EXPORT_SYMBOL(app_entry);
