/**
 * @file trivia_app.c
 * @author Leonardo Bispo
 *
 * @brief Trivia game based using OpenTrivia DB
 *
 * @note Only works for Android
 *
 * @note Must be enabled first by clicking the gear icon next to the Bangle.js you're connected
 * to in Gadgetbridge, and then enabling Allow Internet Access
 *
 * @see https://opentdb.com/api_config.php
 */
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "trivia_ui.h"
#include "managers/zsw_app_manager.h"
#include "ble/ble_comm.h"
#include "events/ble_event.h"
#include "ui/utils/zsw_ui_utils.h"

/*Get 1x easy question with true/false type*/
#define GB_HTTP_REQUEST "{\"t\":\"http\", \"url\":\"https://opentdb.com/api.php?amount=1&difficulty=easy&type=boolean\"} \n"

LOG_MODULE_REGISTER(trivia_app, CONFIG_ZSW_TRIVIA_APP_LOG_LEVEL);

// Functions needed for all applications
static void trivia_app_start(lv_obj_t *root, lv_group_t *group);
static void trivia_app_stop(void);
static void on_button_click(trivia_button_t trivia_button);
static void request_new_question(void);
static void zbus_ble_comm_data_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_OBS_DECLARE(trivia_app_ble_comm_lis);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, trivia_app_ble_comm_lis, 1);
ZBUS_LISTENER_DEFINE(trivia_app_ble_comm_lis, zbus_ble_comm_data_callback);

LV_IMG_DECLARE(quiz);

typedef struct trivia_app_question {
    char question[MAX_HTTP_FIELD_LENGTH + 1];
    bool correct_answer;
} trivia_app_question_t;

static trivia_app_question_t trivia_app_question;

static application_t app = {
    .name = "Trivia",
    .icon = &quiz,
    .start_func = trivia_app_start,
    .stop_func = trivia_app_stop
};

/// @todo This will probably be on utils after PR #134
static char *extract_value_str(char *key, const char *data, int *data_len)
{
    char *start;
    char *end;
    char *str = strstr(data, key);

    if (str == NULL) {
        return NULL;
    }
    str += strlen(key);
    if ((*str != '\"') && (*str != '\\')) {
        return NULL; // Seems to be an INT?
    }
    str += sizeof("\\\":\\");
    if (*str == '\0') {
        return NULL; // Got end of data
    }
    end = strstr(str, "\\\"");
    if (end == NULL) {
        return NULL; // No end of value
    }

    start = str;
    *data_len = end - start;

    return start;
}

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);

    if (event->data.type == BLE_COMM_DATA_TYPE_HTTP) {
        int question_len = 0;
        memset(&trivia_app_question, 0, sizeof(trivia_app_question));

        // {"response_code":0,"results":[{"type":"boolean","difficulty":"easy","category":"Science: Computers","question":"The logo for Snapchat is a Bell.","correct_answer":"False","incorrect_answers":["True"]}]}
        char *temp_value = extract_value_str("question", event->data.data.http_response.response, &question_len);
        LOG_DBG("HTTP question extracted %s", temp_value);

        memcpy(trivia_app_question.question, temp_value, (question_len > MAX_HTTP_FIELD_LENGTH) ? MAX_HTTP_FIELD_LENGTH :
               question_len); /// @todo cast?
        trivia_ui_update_question(trivia_app_question.question);

        /*Get the correct answer*/
        temp_value = extract_value_str("correct_answer", event->data.data.http_response.response, &question_len);
        LOG_DBG("Correct answer %s", temp_value);

        trivia_app_question.correct_answer = (temp_value[0] == 'F') ? false : true;
    }
}

static void trivia_app_start(lv_obj_t *root, lv_group_t *group)
{
    LOG_DBG("Trivia app start");
    trivia_ui_show(root, on_button_click);
    request_new_question();
}

static void trivia_app_stop(void)
{
    trivia_ui_remove();
}

static int trivia_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

static void request_new_question()
{
    // Gadgetbridge does not like the null character.
    ble_comm_send(GB_HTTP_REQUEST, sizeof(GB_HTTP_REQUEST) - 1);
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
