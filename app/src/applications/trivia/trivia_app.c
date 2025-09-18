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
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "trivia_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include <ble/ble_http.h>
#include "cJSON.h"

/*Get 1x easy question with true/false type*/
#define HTTP_REQUEST_URL "https://opentdb.com/api.php?amount=1&difficulty=easy&type=boolean"

LOG_MODULE_REGISTER(trivia_app, CONFIG_ZSW_TRIVIA_APP_LOG_LEVEL);

// Functions needed for all applications
static void trivia_app_start(lv_obj_t *root, lv_group_t *group);
static void trivia_app_stop(void);
static void on_button_click(trivia_button_t trivia_button);
static void request_new_question(void);

ZSW_LV_IMG_DECLARE(quiz);

typedef struct trivia_app_question {
    char question[MAX_HTTP_FIELD_LENGTH + 1];
    bool correct_answer;
} trivia_app_question_t;

static trivia_app_question_t trivia_app_question;
static bool active;

static application_t app = {
    .name = "Trivia",
    .icon = ZSW_LV_IMG_USE(quiz),
    .start_func = trivia_app_start,
    .stop_func = trivia_app_stop,
    .category = ZSW_APP_CATEGORY_GAMES
};

static void trivia_app_start(lv_obj_t *root, lv_group_t *group)
{
    LOG_DBG("Trivia app start");
    active = true;
    trivia_ui_show(root, on_button_click);
    request_new_question();
}

static void trivia_app_stop(void)
{
    active = false;
    trivia_ui_remove();
}

static int trivia_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

static void http_rsp_cb(ble_http_status_code_t status, char *response)
{
    if (status == BLE_HTTP_STATUS_OK && active) {
        cJSON *parsed_response = cJSON_Parse(response);
        if (parsed_response == NULL) {
            LOG_ERR("Failed to parse JSON rsp data from HTTP request");
        } else {
            cJSON *results = cJSON_GetObjectItem(parsed_response, "results");
            if (cJSON_GetArraySize(results) == 1) {
                cJSON *result = cJSON_GetArrayItem(results, 0);
                cJSON *question = cJSON_GetObjectItem(result, "question");
                cJSON *correct_answer = cJSON_GetObjectItem(result, "correct_answer");
                if (question == NULL || correct_answer == NULL) {
                    LOG_ERR("Failed to parse JSON data");
                    return;
                }
                memset(trivia_app_question.question, 0, sizeof(trivia_app_question.question));
                strncpy(trivia_app_question.question, question->valuestring, sizeof(trivia_app_question.question) - 1);
                trivia_app_question.correct_answer = (correct_answer->valuestring[0] == 'F') ? false : true;
                trivia_ui_update_question(trivia_app_question.question);
            } else {
                LOG_ERR("Unexpected number of results: %d, expected 1", cJSON_GetArraySize(results));
            }
        }
        cJSON_Delete(parsed_response);
    }
}

static void request_new_question()
{
    /// @todo create a more generic error code that would cover when GadgetBridge doesn't allow HTTP over BLE, potentially a Systemwide pop-up
    if (zsw_ble_http_get(HTTP_REQUEST_URL, http_rsp_cb) == -EINVAL) {
        trivia_ui_not_supported();
    }
}

static void on_button_click(trivia_button_t trivia_button)
{
    switch (trivia_button) {
        case TRUE_BUTTON:
            LOG_DBG("True button pressed");
            trivia_ui_guess_feedback(trivia_app_question.correct_answer == true);
            break;

        case FALSE_BUTTON:
            LOG_DBG("False button pressed");
            trivia_ui_guess_feedback(trivia_app_question.correct_answer == false);
            break;

        case PLAY_MORE_BUTTON:
            LOG_DBG("More button pressed");
            trivia_ui_close_popup();
            trivia_ui_update_question("-");
            request_new_question();
            break;

        case CLOSE_BUTTON:
            LOG_DBG("Close button pressed");
            zsw_app_manager_exit_app();
            break;

        default:
            LOG_DBG("Button event not handled %d", trivia_button);
            break;
    }
}

SYS_INIT(trivia_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
