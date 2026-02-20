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

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

#ifdef CONFIG_ZSW_LLEXT_APPS
#include <zephyr/llext/symbol.h>
#include <zephyr/sys/printk.h>
#else
#include <zephyr/logging/log.h>
#endif

#include "trivia_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include <ble/ble_http.h>
#include "cJSON.h"

/*Get 1x easy question with true/false type*/
#define HTTP_REQUEST_URL "https://opentdb.com/api.php?amount=1&difficulty=easy&type=boolean"

#ifdef CONFIG_ZSW_LLEXT_APPS
#else
LOG_MODULE_REGISTER(trivia_app, CONFIG_ZSW_TRIVIA_APP_LOG_LEVEL);
ZSW_LV_IMG_DECLARE(quiz);
#endif

// Functions needed for all applications
static void trivia_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void trivia_app_stop(void *user_data);
static void on_button_click(trivia_button_t trivia_button);
static void request_new_question(void);

typedef struct trivia_app_question {
    char question[MAX_HTTP_FIELD_LENGTH + 1];
    bool correct_answer;
} trivia_app_question_t;

static trivia_app_question_t trivia_app_question;

static application_t app = {
    .name = "Trivia",
#ifdef CONFIG_ZSW_LLEXT_APPS
    /* icon set at runtime in app_entry() — PIC linker drops static relocation */
#else
    .icon = ZSW_LV_IMG_USE(quiz),
#endif
    .start_func = trivia_app_start,
    .stop_func = trivia_app_stop,
    .category = ZSW_APP_CATEGORY_GAMES
};

static void http_rsp_cb(ble_http_status_code_t status, char *response)
{
    if (status == BLE_HTTP_STATUS_OK && app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        cJSON *parsed_response = cJSON_Parse(response);
        if (parsed_response == NULL) {
            printk("trivia: Failed to parse JSON\n");
        } else {
            cJSON *results = cJSON_GetObjectItem(parsed_response, "results");
            if (cJSON_GetArraySize(results) == 1) {
                cJSON *result = cJSON_GetArrayItem(results, 0);
                cJSON *question = cJSON_GetObjectItem(result, "question");
                cJSON *correct_answer = cJSON_GetObjectItem(result, "correct_answer");
                if (question == NULL || correct_answer == NULL) {
                    printk("trivia: Failed to parse JSON data\n");
                    cJSON_Delete(parsed_response);
                    return;
                }
                memset(trivia_app_question.question, 0, sizeof(trivia_app_question.question));
                strncpy(trivia_app_question.question, question->valuestring, sizeof(trivia_app_question.question) - 1);
                trivia_app_question.correct_answer = (correct_answer->valuestring[0] == 'F') ? false : true;
                trivia_ui_update_question(trivia_app_question.question);
            } else {
                printk("trivia: Unexpected number of results\n");
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

static void trivia_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    trivia_ui_show(root, on_button_click);
    request_new_question();
}

static void trivia_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    trivia_ui_remove();
}

#ifdef CONFIG_ZSW_LLEXT_APPS
application_t *app_entry(void)
{
    printk("trivia: app_entry called\n");
    /* Set icon at runtime — static relocation is lost by the PIC linker */
    app.icon = "S:quiz.bin";
    zsw_app_manager_add_application(&app);
    return &app;
}
EXPORT_SYMBOL(app_entry);
#else
static int trivia_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}
SYS_INIT(trivia_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
#endif
