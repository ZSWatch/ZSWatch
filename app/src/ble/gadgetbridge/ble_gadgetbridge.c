#include <zephyr/sys/base64.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "ui/zsw_ui.h"
#include "ble/ble_comm.h"
#include "ble/ble_transport.h"
#include "events/ble_event.h"
#include "events/music_event.h"
#include "ble_gadgetbridge.h"

LOG_MODULE_REGISTER(ble_gadgetbridge, CONFIG_ZSW_BLE_LOG_LEVEL);

typedef enum parse_state {
    WAIT_GB,
    WAIT_END,
    PARSE_STATE_DONE,
} parse_state_t;

static uint8_t num_parsed_brackets;
static parse_state_t parse_state = WAIT_GB;
static uint16_t parsed_data_index = 0;
static uint8_t receive_buf[MAX_GB_PACKET_LENGTH];

static void music_control_event_callback(const struct zbus_channel *chan);
static void parse_time_zone(char *offset);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(android_music_control_lis, music_control_event_callback);

static void send_ble_data_event(ble_comm_cb_data_t *data)
{
    struct ble_data_event evt;
    memcpy(&evt.data, data, sizeof(ble_comm_cb_data_t));

    zbus_chan_pub(&ble_comm_data_chan, &evt, K_MSEC(250));
}

static void music_control_event_callback(const struct zbus_channel *chan)
{
    const struct music_event *event = zbus_chan_const_msg(chan);

    uint8_t buf[50];
    int msg_len = 0;

    switch (event->control_type) {
        case MUSIC_CONTROL_UI_PLAY:
            msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"music\", \"n\": %s} \n", "play");
            break;
        case MUSIC_CONTROL_UI_PAUSE:
            msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"music\", \"n\": %s} \n", "pause");
            break;
        case MUSIC_CONTROL_UI_NEXT_TRACK:
            msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"music\", \"n\": %s} \n", "next");
            break;
        case MUSIC_CONTROL_UI_PREV_TRACK:
            msg_len = snprintf(buf, sizeof(buf), "{\"t\":\"music\", \"n\": %s} \n", "previous");
            break;
        case MUSIC_CONTROL_UI_CLOSE:
        default:
            // Nothing to do
            break;
    }
    if (msg_len > 0) {
        ble_comm_send(buf, msg_len);
    }
}

static void parse_time(char *start_time)
{
    char *end_time;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    end_time = strstr(start_time, ")");
    if (end_time) {
        errno = 0;
        cb.data.notify.id = strtol(start_time, &end_time, 10);
        if (start_time != end_time && errno == 0) {
            cb.type = BLE_COMM_DATA_TYPE_SET_TIME;
            send_ble_data_event(&cb);
        } else {
            LOG_WRN("Failed parsing time");
        }
    }

    // If setTime contains timezone, process here, i.e setTime(1700556601);E.setTimeZone(1.0);(...
    char *time_zone = strstr(start_time, ";E.setTimeZone(");
    if (time_zone) {
        time_zone += strlen(";E.setTimeZone(");
        parse_time_zone(time_zone);
    }
}

static void parse_time_zone(char *offset)
{
    char *end_timezone;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    end_timezone = strstr(offset, ")");
    if (end_timezone) {
        cb.data.time.tz_offset = strtof(offset, &end_timezone);

        if (offset != end_timezone) {
            cb.type = BLE_COMM_DATA_TYPE_SET_TIME;
            LOG_DBG("set time offset: %.1f", cb.data.time.tz_offset);
            send_ble_data_event(&cb);
        } else {
            LOG_WRN("Failed parsing time");
        }
    }
}

static char *extract_value_str(char *key, char *data, int *value_len)
{
    bool base64 = false;
    char *start;
    char *end;
    char *str = strstr(data, key);
    *value_len = 0;
    if (str == NULL) {
        return NULL;
    }
    str += strlen(key);

    if (strncmp(str, "atob(", strlen("atob(")) == 0) {
        str += strlen("atob(");
        base64 = true;
    }

    if (*str != '\"') {
        return NULL; // Seems to be an INT?
    }
    str++;
    if (*str == '\0') {
        return NULL; // Got end of data
    }
    end = strstr(str, "\"");
    if (end == NULL) {
        return NULL; // No end of value
    }

    start = str;
    if (base64) {
        // Since the size of the decoded result is 33% smaller, the decoded result can be stored in the original text buffer
        size_t msg_size = end - start;
        size_t decoded_len;
        base64_decode(str, msg_size, &decoded_len, str, msg_size);
        *value_len = decoded_len;
    } else {
        *value_len = end - start;
    }

    return start;
}

static uint32_t extract_value_uint32(char *key, char *data)
{
    char *start;
    char *str = strstr(data, key);
    char *end;
    uint32_t id;

    if (str == NULL) {
        return 0;
    }
    str += strlen(key);
    if (!isdigit((int) * str)) {
        return 0; // No number found
    }
    start = str;
    id = strtol(str, &end, 10);
    // TODO error checking
    return id;
}

static int32_t extract_value_int32(char *key, char *data)
{
    char *start;
    char *str = strstr(data, key);
    char *end;
    int32_t id;

    if (str == NULL) {
        return 0;
    }
    str += strlen(key);
    if (!isdigit((int) * str)) {
        return 0; // No number found
    }
    start = str;
    id = strtol(str, &end, 10);
    // TODO error checking
    return id;
}

static void convert_to_encoded_text(char *data, int len, char *out_data, int out_buf_len)
{
    int i = 0, j = 0;
    // https://www.utf8-chartable.de/
    uint8_t basic_latin_utf16_to_utf8_table[0x80][3] = {
        // utf-16 => 2 byte utf-8
        {0x80, 0xc2, 0x80},
        {0x81, 0xc2, 0x81},
        {0x82, 0xc2, 0x82},
        {0x83, 0xc2, 0x83},
        {0x84, 0xc2, 0x84},
        {0x85, 0xc2, 0x85},
        {0x86, 0xc2, 0x86},
        {0x87, 0xc2, 0x87},
        {0x88, 0xc2, 0x88},
        {0x89, 0xc2, 0x89},
        {0x8A, 0xc2, 0x8a},
        {0x8B, 0xc2, 0x8b},
        {0x8C, 0xc2, 0x8c},
        {0x8D, 0xc2, 0x8d},
        {0x8E, 0xc2, 0x8e},
        {0x8F, 0xc2, 0x8f},
        {0x90, 0xc2, 0x90},
        {0x91, 0xc2, 0x91},
        {0x92, 0xc2, 0x92},
        {0x93, 0xc2, 0x93},
        {0x94, 0xc2, 0x94},
        {0x95, 0xc2, 0x95},
        {0x96, 0xc2, 0x96},
        {0x97, 0xc2, 0x97},
        {0x98, 0xc2, 0x98},
        {0x99, 0xc2, 0x99},
        {0x9A, 0xc2, 0x9a},
        {0x9B, 0xc2, 0x9b},
        {0x9C, 0xc2, 0x9c},
        {0x9D, 0xc2, 0x9d},
        {0x9E, 0xc2, 0x9e},
        {0x9F, 0xc2, 0x9f},
        {0xA0, 0xc2, 0xa0},
        {0xA1, 0xc2, 0xa1},
        {0xA2, 0xc2, 0xa2},
        {0xA3, 0xc2, 0xa3},
        {0xA4, 0xc2, 0xa4},
        {0xA5, 0xc2, 0xa5},
        {0xA6, 0xc2, 0xa6},
        {0xA7, 0xc2, 0xa7},
        {0xA8, 0xc2, 0xa8},
        {0xA9, 0xc2, 0xa9},
        {0xAA, 0xc2, 0xaa},
        {0xAB, 0xc2, 0xab},
        {0xAC, 0xc2, 0xac},
        {0xAD, 0xc2, 0xad},
        {0xAE, 0xc2, 0xae},
        {0xAF, 0xc2, 0xaf},
        {0xB0, 0xc2, 0xb0},
        {0xB1, 0xc2, 0xb1},
        {0xB2, 0xc2, 0xb2},
        {0xB3, 0xc2, 0xb3},
        {0xB4, 0xc2, 0xb4},
        {0xB5, 0xc2, 0xb5},
        {0xB6, 0xc2, 0xb6},
        {0xB7, 0xc2, 0xb7},
        {0xB8, 0xc2, 0xb8},
        {0xB9, 0xc2, 0xb9},
        {0xBA, 0xc2, 0xba},
        {0xBB, 0xc2, 0xbb},
        {0xBC, 0xc2, 0xbc},
        {0xBD, 0xc2, 0xbd},
        {0xBE, 0xc2, 0xbe},
        {0xBF, 0xc2, 0xbf},
        {0xC0, 0xc3, 0x80},
        {0xC1, 0xc3, 0x81},
        {0xC2, 0xc3, 0x82},
        {0xC3, 0xc3, 0x83},
        {0xC4, 0xc3, 0x84},
        {0xC5, 0xc3, 0x85},
        {0xC6, 0xc3, 0x86},
        {0xC7, 0xc3, 0x87},
        {0xC8, 0xc3, 0x88},
        {0xC9, 0xc3, 0x89},
        {0xCA, 0xc3, 0x8a},
        {0xCB, 0xc3, 0x8b},
        {0xCC, 0xc3, 0x8c},
        {0xCD, 0xc3, 0x8d},
        {0xCE, 0xc3, 0x8e},
        {0xCF, 0xc3, 0x8f},
        {0xD0, 0xc3, 0x90},
        {0xD1, 0xc3, 0x91},
        {0xD2, 0xc3, 0x92},
        {0xD3, 0xc3, 0x93},
        {0xD4, 0xc3, 0x94},
        {0xD5, 0xc3, 0x95},
        {0xD6, 0xc3, 0x96},
        {0xD7, 0xc3, 0x97},
        {0xD8, 0xc3, 0x98},
        {0xD9, 0xc3, 0x99},
        {0xDA, 0xc3, 0x9a},
        {0xDB, 0xc3, 0x9b},
        {0xDC, 0xc3, 0x9c},
        {0xDD, 0xc3, 0x9d},
        {0xDE, 0xc3, 0x9e},
        {0xDF, 0xc3, 0x9f},
        {0xE0, 0xc3, 0xa0},
        {0xE1, 0xc3, 0xa1},
        {0xE2, 0xc3, 0xa2},
        {0xE3, 0xc3, 0xa3},
        {0xE4, 0xc3, 0xa4},
        {0xE5, 0xc3, 0xa5},
        {0xE6, 0xc3, 0xa6},
        {0xE7, 0xc3, 0xa7},
        {0xE8, 0xc3, 0xa8},
        {0xE9, 0xc3, 0xa9},
        {0xEA, 0xc3, 0xaa},
        {0xEB, 0xc3, 0xab},
        {0xEC, 0xc3, 0xac},
        {0xED, 0xc3, 0xad},
        {0xEE, 0xc3, 0xae},
        {0xEF, 0xc3, 0xaf},
        {0xF0, 0xc3, 0xb0},
        {0xF1, 0xc3, 0xb1},
        {0xF2, 0xc3, 0xb2},
        {0xF3, 0xc3, 0xb3},
        {0xF4, 0xc3, 0xb4},
        {0xF5, 0xc3, 0xb5},
        {0xF6, 0xc3, 0xb6},
        {0xF7, 0xc3, 0xb7},
        {0xF8, 0xc3, 0xb8},
        {0xF9, 0xc3, 0xb9},
        {0xFA, 0xc3, 0xba},
        {0xFB, 0xc3, 0xbb},
        {0xFC, 0xc3, 0xbc},
        {0xFD, 0xc3, 0xbd},
        {0xFE, 0xc3, 0xbe},
        {0xFF, 0xc3, 0xbf},
    };

    // For none ascii characters Gadgetbridge encodes them in a strange utf-16 way.
    // For example Gadgetbridge sends ö as just 0xF6 byte, but then it sends ä as "\xe4" (4 bytes) string.
    // Which is very strange.
    // LVGL works with ascii, and properly formatted utf-8.
    while (data[i] != '\0' && i < len - 3 && j < out_buf_len - 3) {
        if (data[i] == '\\' && data[i + 1] == 'x') {
            // Parse string "\xe4" as 0xe4
            char hex[3] = {data[i + 2], data[i + 3], '\0'};
            int value = strtol(hex, NULL, 16);
            if (value < 0x80) {
                // Character encoded in a string, but it's an normal ascii, just copy it.
                out_data[j] = value;
            } else {
                // Character encoded as "\xe4" (4 bytes)
                out_data[j] = basic_latin_utf16_to_utf8_table[value - 0x80][1];
                j++;
                out_data[j] = basic_latin_utf16_to_utf8_table[value - 0x80][2];
                j++;
            }
            i += 4;
        } else if (data[i] >= 0x80) {
            // Character encoded as utf-16, but in a single byte and the value is not ascii.
            out_data[j] = basic_latin_utf16_to_utf8_table[data[i] - 0x80][1];
            j++;
            out_data[j] = basic_latin_utf16_to_utf8_table[data[i] - 0x80][2];
            j++;
            i++;
        } else {
            // Ascii character, just copy it.
            out_data[j] = data[i];
            i++;
            j++;
        }
    }
    // Copy the rest of the data
    for (; i < len; i++) {
        out_data[j] = data[i];
        j++;
    }
    out_data[j] = '\0';
}

static int parse_notify(char *data, int len)
{
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_NOTIFY;
    cb.data.notify.id = extract_value_uint32("\"id\":", data);
    cb.data.notify.src = extract_value_str("\"src\":", data, &cb.data.notify.src_len);
    cb.data.notify.sender = extract_value_str("\"sender\":", data, &cb.data.notify.sender_len);
    cb.data.notify.title = extract_value_str("\"title\":", data, &cb.data.notify.title_len);
    cb.data.notify.subject = extract_value_str("\"subject\":", data, &cb.data.notify.subject_len);
    cb.data.notify.body = extract_value_str("\"body\":", data, &cb.data.notify.body_len);

    // Little hack since we know it's JSON, we can terminate all values in the data
    // which saves us some hassle and we can just pass all values null terminated
    // to the callback. Make sure to do it after finish parsing!
    if (cb.data.notify.src) {
        cb.data.notify.src[cb.data.notify.src_len] = '\0';
    }
    if (cb.data.notify.sender) {
        cb.data.notify.sender[cb.data.notify.sender_len] = '\0';
    }
    if (cb.data.notify.title) {
        cb.data.notify.title[cb.data.notify.title_len] = '\0';
    }

    if (cb.data.notify.subject) {
        cb.data.notify.subject[cb.data.notify.subject_len] = '\0';
    }
    if (cb.data.notify.body) {
        cb.data.notify.body[cb.data.notify.body_len] = '\0';
    }

    send_ble_data_event(&cb);

    return 0;
}

static int parse_notify_delete(char *data, int len)
{
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_NOTIFY_REMOVE;
    cb.data.notify.id = extract_value_uint32("\"id\":", data);

    send_ble_data_event(&cb);

    return 0;
}

static int parse_weather(char *data, int len)
{
    //{t:"weather",temp:268,hum:97,code:802,txt:"slightly cloudy",wind:2.0,wdir:14,loc:"MALMO"
    int temp_len;
    char *temp_value;
    float temperature;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_WEATHER;
    int32_t temperature_k = extract_value_uint32("\"temp\":", data);
    cb.data.weather.humidity = extract_value_uint32("\"hum\":", data);
    cb.data.weather.weather_code = extract_value_uint32("\"code\":", data);
    cb.data.weather.wind = extract_value_uint32("\"wind\":", data);
    cb.data.weather.wind_direction = extract_value_uint32("\"wdir\":", data);
    temp_value = extract_value_str("\"txt\":", data, &temp_len);

    strncpy(cb.data.weather.report_text, temp_value, MIN(temp_len, MAX_MUSIC_FIELD_LENGTH));

    // App sends temperature in Kelvin
    temperature = temperature_k - 273.15f;
    cb.data.weather.temperature_c = (int8_t)roundf(temperature);

    send_ble_data_event(&cb);

    return 0;
}

static int parse_musicinfo(char *data, int len)
{
    // {t:"musicinfo",artist:"Ava Max",album:"Heaven & Hell",track:"Sweet but Psycho",dur:187,c:-1,n:-1}
    char *temp_value;
    int temp_len;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_MUSIC_INFO;
    cb.data.music_info.duration = extract_value_int32("\"dur\":", data);
    cb.data.music_info.track_count = extract_value_int32("\"c\":", data);
    cb.data.music_info.track_num = extract_value_int32("\"n\":", data);
    temp_value = extract_value_str("\"artist\":", data, &temp_len);
    strncpy(cb.data.music_info.artist, temp_value, MIN(temp_len, MAX_MUSIC_FIELD_LENGTH));
    temp_value = extract_value_str("\"album\":", data, &temp_len);
    strncpy(cb.data.music_info.album, temp_value, MIN(temp_len, MAX_MUSIC_FIELD_LENGTH));
    temp_value = extract_value_str("\"track\":", data, &temp_len);
    strncpy(cb.data.music_info.track_name, temp_value, MIN(temp_len, MAX_MUSIC_FIELD_LENGTH));

    send_ble_data_event(&cb);

    return 0;
}

static int parse_musicstate(char *data, int len)
{
    // {t:"musicinfo",artist:"Ava Max",album:"Heaven & Hell",track:"Sweet but Psycho",dur:187,c:-1,n:-1}
    char *temp_value;
    int temp_len;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_MUSIC_STATE;
    cb.data.music_state.position = extract_value_int32("\"position\":", data);
    cb.data.music_state.shuffle = extract_value_int32("\"shuffle\":", data);
    cb.data.music_state.repeat = extract_value_int32("\"repeat\":", data);

    temp_value = extract_value_str("\"state\":", data, &temp_len);
    if (strncmp(temp_value, "play", temp_len) == 0) {
        cb.data.music_state.playing = true;
    } else {
        cb.data.music_state.playing = false;
    }

    send_ble_data_event(&cb);

    return 0;
}

static int parse_httpstate(char *data, int len)
{
    // {"t":"http","resp":"{\"response_code\":0,\"results\":[{\"type\":\"boolean\",\"difficulty\":\"easy\",\"category\":\"Geography\",\"question\":\"Hungary is the only country in the world beginning with H.\",\"correct_answer\":\"False\",\"incorrect_answers\":[\"True\"]}]}"}
    char *temp_value;
    int temp_len;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_HTTP;

    temp_value = extract_value_str("\"id\":", data, &temp_len);
    if (temp_value) {
        errno = 0;
        char *end_data;
        cb.data.http_response.id  = strtol(temp_value, &end_data, 10);
        if (temp_value == end_data || errno != 0) {
            LOG_WRN("Failed parsing http request id");
            cb.data.http_response.id = -1;
        }
    } else {
        cb.data.http_response.id = -1;
    }

    // {"t":"http","err":"Internet access not enabled in this Gadgetbridge build"}
    temp_value = extract_value_str("\"err\":", data, &temp_len);

    if (temp_value != NULL) {
        LOG_ERR("HTTP err: %s", temp_value);
        memcpy(cb.data.http_response.err, temp_value, temp_len);
        send_ble_data_event(&cb);
    } else {
        temp_value = extract_value_str("\"resp\":", data, &temp_len);
        if (temp_value) {
            LOG_DBG("HTTP response: %s", temp_value);
            memcpy(cb.data.http_response.response, temp_value,
                   (strlen(temp_value) > MAX_HTTP_FIELD_LENGTH) ? MAX_HTTP_FIELD_LENGTH : strlen(temp_value)); /// @todo cast?
            send_ble_data_event(&cb);
        }
    }

    return 0;
}

static int parse_data(char *data, int len)
{
    int type_len;
    char *type;
    uint8_t input_data_utf8[MAX_GB_PACKET_LENGTH];

    memset(input_data_utf8, 0, sizeof(input_data_utf8));
    // Convert data from Gadgetbridge into properly encoded text.
    convert_to_encoded_text(data, len, input_data_utf8, sizeof(input_data_utf8));
    data = input_data_utf8;

    type = extract_value_str("\"t\":", data, &type_len);
    if (type == NULL) {
        return -1;
    }

    if (strlen("notify") == type_len && strncmp(type, "notify", type_len) == 0) {
        return parse_notify(data, len);
    }

    if (strlen("notify-") == type_len && strncmp(type, "notify-", type_len) == 0) {
        return parse_notify_delete(data, len);
    }

    if (strlen("weather") == type_len && strncmp(type, "weather", type_len) == 0) {
        return parse_weather(data, len);
    }

    if (strlen("musicinfo") == type_len && strncmp(type, "musicinfo", type_len) == 0) {
        return parse_musicinfo(data, len);
    }

    if (strlen("musicstate") == type_len && strncmp(type, "musicstate", type_len) == 0) {
        return parse_musicstate(data, len);
    }

    if (strlen("http") == type_len && strncmp(type, "http", type_len) == 0) {
        return parse_httpstate(data, len);
    }

    return 0;
}

static void parse_remote_control(char *data, int len)
{
    int button;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    button = atoi(data);
    LOG_DBG("Pressed: %d, len: %d", button, len);
    if (button < 0) {
        return;
    }

    cb.type = BLE_COMM_DATA_TYPE_REMOTE_CONTROL;
    cb.data.remote_control.button = button;

    send_ble_data_event(&cb);
}

void ble_gadgetbridge_input(const uint8_t *const data, uint16_t len)
{
    LOG_HEXDUMP_DBG(data, len, "RX");

    if (strncmp("Control:", data, MIN(strlen("Control:"), len)) == 0) {
        char *time_start = strstr(data, "Control:");
        return parse_remote_control(time_start + strlen("Control:"), len - strlen("Control:"));
    }

    char *gb_start = strstr(data, "GB(");
    if (gb_start && parse_state != WAIT_GB) {
        LOG_ERR("Parsing error, was waiting end, but got GB");
        parse_state = WAIT_GB;
    }

    char *time_start = strstr(data, "setTime(");
    if (time_start && parse_state == WAIT_GB) {
        time_start += strlen("setTime(");
        parse_time(time_start);
        return;
    }

    // ie. ;E.setTimeZone(1.0);
    char *offset = strstr(data, ";E.setTimeZone(");
    if (offset && parse_state == WAIT_GB) {
        offset += strlen(";E.setTimeZone(");
        parse_time_zone(offset);
        return;
    }

    switch (parse_state) {
        case WAIT_GB: {
            if (gb_start) {
                gb_start += 3;
                parse_state = WAIT_END;
                num_parsed_brackets = 0;
                parsed_data_index = 0;
                memset(receive_buf, 0, sizeof(receive_buf));
                uint32_t index = gb_start - (char *)data;
                for (int i = index; i < len; i++) {
                    receive_buf[parsed_data_index] = data[i];
                    parsed_data_index++;
                    if (data[i] == '{') {
                        num_parsed_brackets++;
                    } else if (data[i] == '}') {
                        num_parsed_brackets--;
                        if (num_parsed_brackets == 0) {
                            parse_state = PARSE_STATE_DONE;
                            break;
                        }
                    }
                }
            }
            break;
        }
        case WAIT_END: {
            for (int i = 0; i < len; i++) {
                receive_buf[parsed_data_index] = data[i];
                parsed_data_index++;
                if (parsed_data_index >= MAX_GB_PACKET_LENGTH) {
                    LOG_ERR("Data from Gadgetbridge does not fit in MAX_GB_PACKET_LENGTH (%d)", MAX_GB_PACKET_LENGTH);
                    parse_state = WAIT_GB;
                    break;
                }
                if (data[i] == '{') {
                    num_parsed_brackets++;
                } else if (data[i] == '}') {
                    num_parsed_brackets--;
                    if (num_parsed_brackets == 0) {
                        parse_state = PARSE_STATE_DONE;
                        break;
                    }
                }
            }
            break;
        }
        case PARSE_STATE_DONE: {
            LOG_WRN("Busy parsing, ingnoring...");
            break;
        }
        default:
            LOG_WRN("Unhandled state");
            break;
    }
    if (parse_state == PARSE_STATE_DONE) {
        parse_state = WAIT_GB;
        LOG_DBG("%s", receive_buf);
        parse_data(receive_buf, parsed_data_index);
    }
}