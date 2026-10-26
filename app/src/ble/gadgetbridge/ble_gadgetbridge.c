#include <zephyr/sys/base64.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/shell/shell.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <cJSON.h>

#include "ui/zsw_ui.h"
#include "ble/ble_comm.h"
#include "ble/ble_log_backend.h"
#include "ble/ble_transport.h"
#include "drivers/zsw_vibration_motor.h"
#include "events/ble_event.h"
#include "events/music_event.h"
#include "applications/watchface/watchface_app.h"
#include "managers/zsw_power_manager.h"
#include "managers/zsw_smp_manager.h"
#include "ble_gadgetbridge.h"
#include "ble_find_device.h"
#include "app_version.h"

#ifdef CONFIG_APPLICATIONS_USE_VOICE_MEMO
#include "managers/zsw_recording_manager.h"
#include "events/zsw_voice_memo_event.h"
#include "events/zsw_notification_event.h"
#include "ble_voice_recording_list.h"
#endif

LOG_MODULE_REGISTER(ble_gadgetbridge, CONFIG_ZSW_BLE_LOG_LEVEL);

typedef enum parse_state {
    WAIT_GB,
    WAIT_END,
    PARSE_STATE_DONE,
} parse_state_t;

static uint8_t num_parsed_brackets;
static parse_state_t parse_state = WAIT_GB;
static uint16_t parsed_data_index = 0;
static uint8_t receive_buf[MAX_GB_PACKET_LENGTH + 1];

static void music_control_event_callback(const struct zbus_channel *chan);
static void parse_time_zone(char *offset);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(android_music_control_lis, music_control_event_callback);

#ifdef CONFIG_APPLICATIONS_USE_VOICE_MEMO
static void on_ble_recording_event(const struct zbus_channel *chan);
ZBUS_CHAN_DECLARE(voice_memo_recording_chan);
ZBUS_LISTENER_DEFINE(ble_voice_memo_recording_lis, on_ble_recording_event);

static struct zsw_voice_memo_recording_event ble_recording_evt_copy;

static void ble_recording_notify_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    ble_gadgetbridge_send_voice_memo_new(
        ble_recording_evt_copy.filename,
        ble_recording_evt_copy.duration_ms,
        ble_recording_evt_copy.size_bytes,
        ble_recording_evt_copy.timestamp);
}

static K_WORK_DEFINE(ble_recording_notify_work, ble_recording_notify_work_fn);

static void on_ble_recording_event(const struct zbus_channel *chan)
{
    const struct zsw_voice_memo_recording_event *evt = zbus_chan_const_msg(chan);
    if (evt->state == ZSW_VOICE_MEMO_RECORDING_STOPPED) {
        memcpy(&ble_recording_evt_copy, evt, sizeof(ble_recording_evt_copy));
        k_work_submit(&ble_recording_notify_work);
    }
}

/* The result ACK echoes phone strings into JSON without escaping. */
static bool is_json_safe_string(const cJSON *item)
{
    if (!cJSON_IsString(item)) {
        return false;
    }
    for (const uint8_t *c = (const uint8_t *)item->valuestring; *c; c++) {
        if (*c == '"' || *c == '\\' || *c < 0x20) {
            return false;
        }
    }
    return true;
}
#endif /* CONFIG_APPLICATIONS_USE_VOICE_MEMO */

#ifdef CONFIG_ZSW_NOTIFICATION_REPLY
ZBUS_CHAN_DECLARE(zsw_notification_reply_chan);

static int publish_notification_reply(enum zsw_notification_reply_event_type type, uint32_t id, bool ok,
                                      const char *filename, const char *text)
{
    int rc = zbus_chan_claim(&zsw_notification_reply_chan, K_MSEC(100));
    if (rc != 0) {
        LOG_WRN("Reply event dropped: %d", rc);
        return rc;
    }

    struct zsw_notification_reply_event *evt = zbus_chan_msg(&zsw_notification_reply_chan);
    memset(evt, 0, sizeof(*evt));
    evt->type = type;
    evt->id = id;
    evt->ok = ok;
    if (filename) {
        strncpy(evt->filename, filename, sizeof(evt->filename) - 1);
    }
    if (text) {
        strncpy(evt->text, text, sizeof(evt->text) - 1);
    }
    zbus_chan_finish(&zsw_notification_reply_chan);

    return zbus_chan_notify(&zsw_notification_reply_chan, K_MSEC(100));
}
#endif

static void send_ble_data_event(struct ble_data_event *evt)
{
    zbus_chan_pub(&ble_comm_data_chan, evt, K_MSEC(250));
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
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    end_time = strstr(start_time, ")");
    if (end_time) {
        errno = 0;
        cb.data.data.time.seconds = strtol(start_time, &end_time, 10);
        if (start_time != end_time && errno == 0) {
            cb.data.type = BLE_COMM_DATA_TYPE_SET_TIME;
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
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    end_timezone = strstr(offset, ")");
    if (end_timezone) {
        errno = 0;
        cb.data.data.time.tz_offset = strtof(offset, &end_timezone);

        if (offset != end_timezone && *end_timezone == ')' && errno == 0) {
            cb.data.data.time.tz_offset_valid = true;
            cb.data.type = BLE_COMM_DATA_TYPE_SET_TIME;
            LOG_DBG("set time offset: %.1f", cb.data.data.time.tz_offset);
            send_ble_data_event(&cb);
        } else {
            LOG_WRN("Failed parsing time");
        }
    }
}

static char *find_js_value(char *data, const char *key);
static int decode_js_literal(const char *src, char *dst, size_t cap, bool truncate);
static int extract_js_string(const char *key, char *data, char *dst, size_t cap, bool truncate);

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
    end = str;
    while (*end != '\0' && *end != '\"') {
        if (*end == '\\' && end[1] != '\0') {
            end++; // Skip the escaped character so \" does not end the value.
        }
        end++;
    }
    if (*end == '\0') {
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

// Return the length of a complete UTF-8 character, or zero for a raw Latin-1 byte.
static size_t utf8_character_length(const uint8_t *data, size_t len)
{
    uint8_t first = data[0];
    size_t count;
    if (first < 0x80) {
        return 1;
    } else if (first >= 0xC2 && first <= 0xDF) {
        count = 2;
    } else if (first >= 0xE0 && first <= 0xEF) {
        count = 3;
    } else if (first >= 0xF0 && first <= 0xF4) {
        count = 4;
    } else {
        return 0;
    }
    if (len < count) {
        return 0;
    }
    for (size_t i = 1; i < count; i++) {
        if ((data[i] & 0xC0) != 0x80) {
            return 0;
        }
    }
    if ((first == 0xE0 && data[1] < 0xA0) || (first == 0xED && data[1] >= 0xA0) ||
        (first == 0xF0 && data[1] < 0x90) || (first == 0xF4 && data[1] >= 0x90)) {
        return 0;
    }
    return count;
}

static bool is_valid_utf8(const char *data, int len)
{
    int i = 0;
    while (i < len) {
        if (data[i] == '\0') {
            return false;
        }
        // Check for Gadgetbridge's "\xNN" string escape pattern
        if (data[i] == '\\' && i + 1 < len && data[i + 1] == 'x') {
            return false; // Contains Gadgetbridge-style escape sequences
        }

        uint8_t byte = (uint8_t)data[i];
        if (byte <= 0x7F) {
            // ASCII - valid single byte
            i++;
        } else if ((byte & 0xE0) == 0xC0) {
            // 2-byte UTF-8 sequence (0xC0-0xDF)
            if (i + 1 >= len || ((uint8_t)data[i + 1] & 0xC0) != 0x80) {
                return false; // Invalid continuation byte
            }
            i += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            // 3-byte UTF-8 sequence (0xE0-0xEF)
            if (i + 2 >= len ||
                ((uint8_t)data[i + 1] & 0xC0) != 0x80 ||
                ((uint8_t)data[i + 2] & 0xC0) != 0x80) {
                return false;
            }
            i += 3;
        } else if ((byte & 0xF8) == 0xF0) {
            // 4-byte UTF-8 sequence (0xF0-0xF7)
            if (i + 3 >= len ||
                ((uint8_t)data[i + 1] & 0xC0) != 0x80 ||
                ((uint8_t)data[i + 2] & 0xC0) != 0x80 ||
                ((uint8_t)data[i + 3] & 0xC0) != 0x80) {
                return false;
            }
            i += 4;
        } else {
            // Invalid UTF-8 start byte (0x80-0xBF or 0xF8-0xFF)
            // This includes Gadgetbridge's raw high bytes like 0xF6 for 'ö'
            return false;
        }
    }
    return true;
}

static int convert_to_encoded_text(const char *data, size_t len, char *out_data, size_t out_buf_len)
{
    size_t i = 0, j = 0;
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

    if (out_buf_len == 0) {
        return -ENOSPC;
    }
    while (i < len) {
        uint8_t value = (uint8_t)data[i];
        size_t consumed = 1, written = 1;
        const uint8_t *bytes = (const uint8_t *)data + i;
        if (value == '\\' && len - i >= 2 && data[i + 1] == 'x') {
            if (len - i < 4 || !isxdigit((unsigned char)data[i + 2]) ||
                !isxdigit((unsigned char)data[i + 3])) {
                return -EINVAL;
            }
            char hex[3] = {data[i + 2], data[i + 3], '\0'};
            value = strtoul(hex, NULL, 16);
            consumed = 4;
            if (value >= 0x80) {
                bytes = &basic_latin_utf16_to_utf8_table[value - 0x80][1];
                written = 2;
            } else {
                bytes = &value;
            }
        } else if (value >= 0x80) {
            written = utf8_character_length(bytes, len - i);
            if (written != 0) {
                consumed = written;
            } else {
                bytes = &basic_latin_utf16_to_utf8_table[value - 0x80][1];
                written = 2;
            }
        }
        if (value == 0) {
            return -EINVAL;
        }
        if (written > out_buf_len - 1 - j) {
            return -ENOSPC;
        }
        memcpy(out_data + j, bytes, written);
        i += consumed;
        j += written;
    }
    out_data[j] = '\0';
    return j;
}

/* Decode a located string value in place; decoded text is never longer than the escaped text. */
static char *decode_js_value_in_place(char *value, int *len)
{
    *len = 0;
    if (value == NULL) {
        return NULL;
    }

    const char *end = value;
    if (strncmp(value, "atob(\"", strlen("atob(\"")) == 0) {
        end += strlen("atob(\"");
    } else if (*value == '"') {
        end++;
    } else {
        return NULL;
    }
    while (*end != '\0' && *end != '"') {
        if (*end == '\\' && end[1] != '\0') {
            end++;
        }
        end++;
    }
    if (*end == '\0') {
        return NULL;
    }

    int decoded_len = decode_js_literal(value, value, end - value + 1, false);
    if (decoded_len < 0) {
        return NULL;
    }
    *len = decoded_len;
    return value;
}

static int parse_notify(char *data, int len)
{
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    cb.data.type = BLE_COMM_DATA_TYPE_NOTIFY;
    cb.data.data.notify.id = extract_value_uint32("\"id\":", data);

    /* Locate every field before decoding, decoding rewrites data and could create false key matches. */
    char *src = find_js_value(data, "\"src\":");
    char *sender = find_js_value(data, "\"sender\":");
    char *title = find_js_value(data, "\"title\":");
    char *subject = find_js_value(data, "\"subject\":");
    char *body = find_js_value(data, "\"body\":");
    char *reply = find_js_value(data, "\"reply\":");

    // Only the ZSWatch companion sends this, Gadgetbridge has no dictated replies.
    cb.data.data.notify.can_reply = reply != NULL && strncmp(reply, "true", strlen("true")) == 0;

    cb.data.data.notify.src = decode_js_value_in_place(src, &cb.data.data.notify.src_len);
    cb.data.data.notify.sender = decode_js_value_in_place(sender, &cb.data.data.notify.sender_len);
    cb.data.data.notify.title = decode_js_value_in_place(title, &cb.data.data.notify.title_len);
    cb.data.data.notify.subject = decode_js_value_in_place(subject, &cb.data.data.notify.subject_len);
    cb.data.data.notify.body = decode_js_value_in_place(body, &cb.data.data.notify.body_len);

    send_ble_data_event(&cb);

    return 0;
}

static int parse_notify_delete(char *data, int len)
{
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    cb.data.type = BLE_COMM_DATA_TYPE_NOTIFY_REMOVE;
    cb.data.data.notify.id = extract_value_uint32("\"id\":", data);

    send_ble_data_event(&cb);

    return 0;
}

static int parse_weather(char *data, int len)
{
    //{t:"weather",temp:268,hum:97,code:802,txt:"slightly cloudy",wind:2.0,wdir:14,loc:"MALMO"
    float temperature;
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    cb.data.type = BLE_COMM_DATA_TYPE_WEATHER;
    int32_t temperature_k = extract_value_uint32("\"temp\":", data);
    cb.data.data.weather.humidity = extract_value_uint32("\"hum\":", data);
    cb.data.data.weather.weather_code = extract_value_uint32("\"code\":", data);
    cb.data.data.weather.wind = extract_value_uint32("\"wind\":", data);
    cb.data.data.weather.wind_direction = extract_value_uint32("\"wdir\":", data);
    extract_js_string("\"txt\":", data, cb.data.data.weather.report_text,
                      sizeof(cb.data.data.weather.report_text), true);

    // App sends temperature in Kelvin
    temperature = temperature_k - 273.15f;
    cb.data.data.weather.temperature_c = (int8_t)roundf(temperature);

    send_ble_data_event(&cb);

    return 0;
}

static int parse_musicinfo(char *data, int len)
{
    // {t:"musicinfo",artist:"Ava Max",album:"Heaven & Hell",track:"Sweet but Psycho",dur:187,c:-1,n:-1}
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    cb.data.type = BLE_COMM_DATA_TYPE_MUSIC_INFO;
    cb.data.data.music_info.duration = extract_value_int32("\"dur\":", data);
    cb.data.data.music_info.track_count = extract_value_int32("\"c\":", data);
    cb.data.data.music_info.track_num = extract_value_int32("\"n\":", data);
    extract_js_string("\"artist\":", data, cb.data.data.music_info.artist,
                      sizeof(cb.data.data.music_info.artist), true);
    extract_js_string("\"album\":", data, cb.data.data.music_info.album,
                      sizeof(cb.data.data.music_info.album), true);
    extract_js_string("\"track\":", data, cb.data.data.music_info.track_name,
                      sizeof(cb.data.data.music_info.track_name), true);

    send_ble_data_event(&cb);

    return 0;
}

static int parse_musicstate(char *data, int len)
{
    // {t:"musicinfo",artist:"Ava Max",album:"Heaven & Hell",track:"Sweet but Psycho",dur:187,c:-1,n:-1}
    char *temp_value;
    int temp_len;
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    cb.data.type = BLE_COMM_DATA_TYPE_MUSIC_STATE;
    cb.data.data.music_state.position = extract_value_int32("\"position\":", data);
    cb.data.data.music_state.shuffle = extract_value_int32("\"shuffle\":", data);
    cb.data.data.music_state.repeat = extract_value_int32("\"repeat\":", data);

    temp_value = extract_value_str("\"state\":", data, &temp_len);
    if (strncmp(temp_value, "play", temp_len) == 0) {
        cb.data.data.music_state.playing = true;
    } else {
        cb.data.data.music_state.playing = false;
    }

    send_ble_data_event(&cb);

    return 0;
}

static int utf8_put(uint32_t cp, char *dst, size_t *pos, size_t cap)
{
    uint8_t bytes[4];
    size_t n;
    if (cp < 0x80) {
        bytes[0] = cp;
        n = 1;
    } else if (cp < 0x800) {
        bytes[0] = 0xC0 | (cp >> 6);
        bytes[1] = 0x80 | (cp & 0x3F);
        n = 2;
    } else if (cp < 0x10000) {
        bytes[0] = 0xE0 | (cp >> 12);
        bytes[1] = 0x80 | ((cp >> 6) & 0x3F);
        bytes[2] = 0x80 | (cp & 0x3F);
        n = 3;
    } else {
        bytes[0] = 0xF0 | (cp >> 18);
        bytes[1] = 0x80 | ((cp >> 12) & 0x3F);
        bytes[2] = 0x80 | ((cp >> 6) & 0x3F);
        bytes[3] = 0x80 | (cp & 0x3F);
        n = 4;
    }
    if (*pos + n >= cap) {
        return -ENOSPC;
    }
    memcpy(&dst[*pos], bytes, n);
    *pos += n;
    return 0;
}

static int hex_value(const char *src, int digits, uint32_t *value)
{
    *value = 0;
    for (int i = 0; i < digits; i++) {
        char c = src[i];
        int v = (c >= '0' && c <= '9') ? c - '0' : (c >= 'a' && c <= 'f') ? c - 'a' + 10 :
                (c >= 'A' && c <= 'F') ? c - 'A' + 10 : -1;
        if (v < 0) {
            return -EINVAL;
        }
        *value = (*value << 4) | v;
    }
    return 0;
}

/* Return the value following key (leading spaces skipped), or NULL when key is absent. */
static char *find_js_value(char *data, const char *key)
{
    char *value = strstr(data, key);
    if (value == NULL) {
        return NULL;
    }
    value += strlen(key);
    while (*value == ' ') {
        value++;
    }
    return value;
}

/* Decode the JS string literal at src into dst as UTF-8. Gadgetbridge (Bangle.js) sends
 * JavaScript, not strict JSON: strings may use \xNN, \v and octal escapes, or be
 * replaced by atob("base64"). Strict JSON from the companion apps is a subset.
 * dst may be src, output is never longer than input. With truncate, an overlong result is
 * cut on a UTF-8 character boundary instead of failing with -ENOSPC. */
static int decode_js_literal(const char *src, char *dst, size_t cap, bool truncate)
{
    size_t pos = 0;
    size_t char_start = 0;
    if (cap == 0) {
        return -ENOSPC;
    }
    if (strncmp(src, "atob(\"", strlen("atob(\"")) == 0) {
        src += strlen("atob(\"");
        const char *end = strchr(src, '"');
        size_t decoded_len;
        if (!end || base64_decode((uint8_t *)dst, cap - 1, &decoded_len, (const uint8_t *)src, end - src) != 0) {
            return -EINVAL;
        }
        dst[decoded_len] = '\0';
        return decoded_len;
    }
    if (*src++ != '"') {
        return -EINVAL;
    }
    while (*src && *src != '"') {
        uint32_t cp = (uint8_t)src[0];
        src++;
        if (cp == '\\') {
            char esc = *src++;
            switch (esc) {
                case 'b':
                    cp = '\b';
                    break;
                case 'f':
                    cp = '\f';
                    break;
                case 'n':
                    cp = '\n';
                    break;
                case 'r':
                    cp = '\r';
                    break;
                case 't':
                    cp = '\t';
                    break;
                case 'v':
                    cp = '\v';
                    break;
                case 'x':
                    if (hex_value(src, 2, &cp)) {
                        return -EINVAL;
                    }
                    src += 2;
                    break;
                case 'u':
                    if (hex_value(src, 4, &cp)) {
                        return -EINVAL;
                    }
                    src += 4;
                    if (cp >= 0xD800 && cp <= 0xDBFF && src[0] == '\\' && src[1] == 'u') {
                        uint32_t low;
                        if (hex_value(src + 2, 4, &low) == 0 && low >= 0xDC00 && low <= 0xDFFF) {
                            cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                            src += 6;
                        }
                    }
                    break;
                case '\0':
                    return -EINVAL;
                default:
                    if (esc >= '0' && esc <= '7') {
                        cp = esc - '0';
                    } else {
                        cp = (uint8_t)esc; /* \" \\ \/ and unknown escapes map to the character. */
                    }
                    break;
            }
            if (utf8_put(cp, dst, &pos, cap)) {
                if (truncate) {
                    goto done;
                }
                return -ENOSPC;
            }
            char_start = pos;
        } else {
            /* Raw bytes are already UTF-8 (or converted before parsing). */
            if (pos + 1 >= cap) {
                if (!truncate) {
                    return -ENOSPC;
                }
                if ((cp & 0xC0) == 0x80) {
                    pos = char_start;
                }
                goto done;
            }
            if ((cp & 0xC0) != 0x80) {
                char_start = pos;
            }
            dst[pos++] = cp;
        }
    }
    if (*src != '"') {
        return -EINVAL;
    }
done:
    dst[pos] = '\0';
    return pos;
}

static int extract_js_string(const char *key, char *data, char *dst, size_t cap, bool truncate)
{
    char *value = find_js_value(data, key);
    if (value == NULL) {
        return -ENOENT;
    }
    return decode_js_literal(value, dst, cap, truncate);
}

static int parse_httpstate(char *data, int len)
{
    ARG_UNUSED(len);
    // {"t":"http","id":"5","resp":"{\"current\":...}"} or {"t":"http","id":"5","err":"..."}
    struct ble_data_event cb = {0};
    cb.data.type = BLE_COMM_DATA_TYPE_HTTP;

    /* The request sends the id as a string; accept a plain number too. */
    char *id = strstr(data, "\"id\":");
    if (!id) {
        return -EINVAL;
    }
    id += strlen("\"id\":");
    if (*id == '"') {
        id++;
    }
    char *end;
    errno = 0;
    long request_id = strtol(id, &end, 10);
    if (end == id || errno || request_id < 0 || request_id > UINT16_MAX) {
        return -EINVAL;
    }
    cb.data.data.http_response.id = request_id;

    if (strstr(data, "\"err\":") ||
        extract_js_string("\"resp\":", data, cb.data.data.http_response.response,
                          sizeof(cb.data.data.http_response.response), false) <= 0) {
        /* Even an error or malformed relay result must finish the matching request. */
        snprintf(cb.data.data.http_response.err, sizeof(cb.data.data.http_response.err), "HTTP relay error");
        cb.data.data.http_response.response[0] = '\0';
    }
    send_ble_data_event(&cb);
    return 0;
}

static int parse_gps_data(char *data, int len)
{
    //{"t":"gps","lat":55.6135542,"lon":12.9747185,"alt":41.900001525878906,"speed":0.1458607256412506,"time":1717002933835,"satellites":0,"hdop":16.215999603271484,"externalSource":true,"gpsSource":"network"}
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    cb.data.type = BLE_COMM_DATA_TYPE_GPS;

    cJSON *root = cJSON_Parse(data);
    if (root == NULL) {
        LOG_ERR("Failed to parse GPS data");
        return -EINVAL;
    }

    cJSON *lat = cJSON_GetObjectItem(root, "lat");
    cJSON *lon = cJSON_GetObjectItem(root, "lon");
    cJSON *alt = cJSON_GetObjectItem(root, "alt");
    cJSON *speed = cJSON_GetObjectItem(root, "speed");
    cJSON *time = cJSON_GetObjectItem(root, "time");
    cJSON *satellites = cJSON_GetObjectItem(root, "satellites");
    cJSON *hdop = cJSON_GetObjectItem(root, "hdop");

    cb.data.data.gps.lat = lat != NULL ? lat->valuedouble : -1;
    cb.data.data.gps.lon = lon != NULL ? lon->valuedouble : -1;
    cb.data.data.gps.alt = alt != NULL ? alt->valuedouble : -1;
    cb.data.data.gps.speed = speed != NULL ? speed->valuedouble : -1;
    cb.data.data.gps.time = time != NULL ? time->valuedouble : -1;
    cb.data.data.gps.satellites = satellites != NULL ? satellites->valueint : -1;
    cb.data.data.gps.hdop = hdop != NULL ? hdop->valuedouble : -1;

    cJSON_Delete(root);

    send_ble_data_event(&cb);

    return 0;
}

static int parse_log_command(char *data, int len)
{
    (void) len;

    cJSON *root = cJSON_Parse(data);
    if (root == NULL) {
        LOG_ERR("Failed to parse log command");
        return -EINVAL;
    }

    cJSON *status = cJSON_GetObjectItem(root, "status");
    if (!cJSON_IsBool(status)) {
        LOG_WRN("Log command missing status");
        cJSON_Delete(root);
        return -EINVAL;
    }

    bool enabled = cJSON_IsTrue(status);

    ble_log_backend_set_enabled(enabled);
    LOG_INF("BLE logging %s via command", enabled ? "enabled" : "disabled");

    cJSON_Delete(root);

    return 0;
}

static int parse_smp_command(char *data, int len)
{
    (void)len;

    cJSON *root = cJSON_Parse(data);
    if (root == NULL) {
        LOG_ERR("Failed to parse smp command");
        return -EINVAL;
    }

    cJSON *status = cJSON_GetObjectItem(root, "status");
    if (!cJSON_IsBool(status)) {
        LOG_WRN("smp command missing 'status'");
        cJSON_Delete(root);
        return -EINVAL;
    }

    bool enable = cJSON_IsTrue(status);
    cJSON_Delete(root);

#ifdef CONFIG_MCUMGR
    int rc;
    if (enable) {
        rc = zsw_smp_manager_enable(true);
    } else {
        rc = zsw_smp_manager_disable();
    }

    return rc;
#else
    ARG_UNUSED(enable);
    LOG_WRN("SMP not available");
    return -ENOTSUP;
#endif
}

// {"t":"reset"}
static int parse_reset_command(char *data, int len)
{
    ARG_UNUSED(data);
    ARG_UNUSED(len);
    LOG_INF("Reboot requested via companion app");
    /* Short delay to let the BLE response/ACK go out */
    k_sleep(K_MSEC(500));
    sys_reboot(SYS_REBOOT_COLD);
    /* unreachable */
    return 0;
}

static int parse_voice_memo_command(char *data, int len)
{
#ifndef CONFIG_APPLICATIONS_USE_VOICE_MEMO
    ARG_UNUSED(data);
    ARG_UNUSED(len);
    LOG_WRN("voice_memo: app not enabled");
    return -ENOTSUP;
#else
    cJSON *root = cJSON_Parse(data);
    if (root == NULL) {
        LOG_WRN("voice_memo: JSON parse failed");
        return -EINVAL;
    }

    cJSON *action_obj = cJSON_GetObjectItem(root, "action");
    if (!cJSON_IsString(action_obj)) {
        LOG_WRN("voice_memo: missing action");
        cJSON_Delete(root);
        return -EINVAL;
    }
    const char *action = action_obj->valuestring;

    if (strcmp(action, "list") == 0) {
        cJSON_Delete(root);

        /* Do not retain a cJSON node tree for every file on the libc heap.
         * Use one bounded transient buffer from the kernel heap instead. */
        zsw_recording_entry_t entries[ZSW_RECORDING_MAX_FILES];
        int count = zsw_recording_manager_list(entries, ARRAY_SIZE(entries));
        if (count < 0) {
            LOG_ERR("voice_memo: list failed: %d", count);
            return count;
        }

        char *response = k_malloc(BLE_VOICE_RECORDING_LIST_CAPACITY);
        if (!response) {
            return -ENOMEM;
        }
        int msg_len = ble_voice_recording_list_encode(entries, count, response, BLE_VOICE_RECORDING_LIST_CAPACITY);
        if (msg_len < 0) {
            LOG_ERR("voice_memo: list encoding failed: %d", msg_len);
            k_free(response);
            return msg_len;
        }

        LOG_INF("voice_memo: sending list_result (%d recordings, %d bytes)", count, msg_len);
        int sent = ble_comm_send(response, msg_len);
        k_free(response);
        return sent;
    }

    if (strcmp(action, "delete") == 0) {
        cJSON *fn_obj = cJSON_GetObjectItem(root, "filename");
        if (!cJSON_IsString(fn_obj) || fn_obj->valuestring[0] == '\0') {
            LOG_WRN("voice_memo delete: missing filename");
            cJSON_Delete(root);
            return -EINVAL;
        }

        int ret = zsw_recording_manager_delete(fn_obj->valuestring);
        if (ret < 0) {
            LOG_ERR("voice_memo: delete failed: %d", ret);
        } else {
            LOG_INF("voice_memo: deleted '%s'", fn_obj->valuestring);
        }
        cJSON_Delete(root);
        return ret;
    }

    if (strcmp(action, "result") == 0) {
        cJSON *title_obj = cJSON_GetObjectItem(root, "text");
        if (!cJSON_IsString(title_obj) || title_obj->valuestring[0] == '\0') {
            LOG_WRN("voice_memo result: missing text");
            cJSON_Delete(root);
            return -EINVAL;
        }

        cJSON *fn_obj = cJSON_GetObjectItem(root, "filename");
        cJSON *atype_obj = cJSON_GetObjectItem(root, "action_type");
        cJSON *dt_obj = cJSON_GetObjectItem(root, "datetime");

        LOG_DBG("voice_memo: result received, file='%s', type='%s', dt='%s'",
                cJSON_IsString(fn_obj) ? fn_obj->valuestring : "?",
                cJSON_IsString(atype_obj) ? atype_obj->valuestring : "?",
                cJSON_IsString(dt_obj) ? dt_obj->valuestring : "?");

        if (cJSON_IsString(atype_obj) && (strcmp(atype_obj->valuestring, "reply") == 0 ||
                                          strcmp(atype_obj->valuestring, "reply_error") == 0)) {
#ifdef CONFIG_ZSW_NOTIFICATION_REPLY
            publish_notification_reply(strcmp(atype_obj->valuestring, "reply") == 0 ?
                                       ZSW_NOTIFICATION_REPLY_DRAFT : ZSW_NOTIFICATION_REPLY_DRAFT_FAILED,
                                       0, false, cJSON_IsString(fn_obj) ? fn_obj->valuestring : NULL,
                                       title_obj->valuestring);
#endif
        } else if (cJSON_IsString(atype_obj) && strcmp(atype_obj->valuestring, "chat") == 0) {
#ifdef CONFIG_APPLICATIONS_USE_CHAT
            ZBUS_CHAN_DECLARE(chat_answer_chan);
            /* Too large for the BLE thread stack. */
            static struct zsw_chat_answer_event answer_evt;
            memset(&answer_evt, 0, sizeof(answer_evt));
            strncpy(answer_evt.text, title_obj->valuestring, sizeof(answer_evt.text) - 1);
            if (cJSON_IsString(fn_obj)) {
                strncpy(answer_evt.filename, fn_obj->valuestring, sizeof(answer_evt.filename) - 1);
            }
            zbus_chan_pub(&chat_answer_chan, &answer_evt, K_MSEC(100));
#endif
        } else {
            ZBUS_CHAN_DECLARE(voice_memo_result_chan);
            struct zsw_voice_memo_result_event result_evt = {0};
            strncpy(result_evt.title, title_obj->valuestring, sizeof(result_evt.title) - 1);
            if (cJSON_IsString(fn_obj)) {
                strncpy(result_evt.filename, fn_obj->valuestring, sizeof(result_evt.filename) - 1);
            }
            if (cJSON_IsString(atype_obj)) {
                strncpy(result_evt.action_type, atype_obj->valuestring, sizeof(result_evt.action_type) - 1);
            }
            if (cJSON_IsString(dt_obj)) {
                strncpy(result_evt.datetime, dt_obj->valuestring, sizeof(result_evt.datetime) - 1);
            }
            zbus_chan_pub(&voice_memo_result_chan, &result_evt, K_MSEC(100));
        }

        /* ACK so the phone stops resending. A resend after a lost ACK shows the popup again. */
        cJSON *generation = cJSON_GetObjectItem(root, "generation");
        cJSON *key = cJSON_GetObjectItem(root, "recording_key");
        cJSON *id = cJSON_GetObjectItem(root, "result_id");
        cJSON *sha = cJSON_GetObjectItem(root, "result_sha256");
        int rc = 0;
        if (cJSON_IsNumber(generation) && is_json_safe_string(key) && is_json_safe_string(id) &&
            is_json_safe_string(sha)) {
            char ack[360];
            int length = snprintf(ack, sizeof(ack),
                                  "{\"t\":\"voice_memo\",\"action\":\"result_ack\",\"version\":1,"
                                  "\"result_id\":\"%s\",\"recording_key\":\"%s\",\"result_sha256\":\"%s\","
                                  "\"generation\":%u,\"status\":\"accepted\"}",
                                  id->valuestring, key->valuestring, sha->valuestring, (uint32_t)generation->valuedouble);
            rc = length > 0 && length < sizeof(ack) ? ble_comm_send(ack, length) : -ENOSPC;
        }
        cJSON_Delete(root);
        return rc;
    }

    LOG_WRN("voice_memo: unknown action '%s'", action);
    cJSON_Delete(root);
    return -ENOTSUP;
#endif /* CONFIG_APPLICATIONS_USE_VOICE_MEMO */
}

static const char *watchface_bg_error_str(int rc)
{
    switch (rc) {
        case -ENOENT:
            return "staged_missing";
        case -EINVAL:
            return "invalid_background";
        default:
            return "operation_failed";
    }
}

static int send_watchface_bg_result(const char *action, int rc)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return -ENOMEM;
    }

    char action_result[24];
    int len = snprintf(action_result, sizeof(action_result), "%s_result", action);
    if (len <= 0 || len >= sizeof(action_result)) {
        cJSON_Delete(root);
        return -EINVAL;
    }

    cJSON_AddStringToObject(root, "t", "watchface_bg");
    cJSON_AddStringToObject(root, "action", action_result);
    cJSON_AddBoolToObject(root, "ok", rc == 0);
    if (rc != 0) {
        cJSON_AddNumberToObject(root, "rc", rc);
        cJSON_AddStringToObject(root, "error", watchface_bg_error_str(rc));
    }

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (json_str == NULL) {
        return -ENOMEM;
    }

    ble_comm_send(json_str, strlen(json_str));
    cJSON_free(json_str);
    return rc;
}

static int parse_watchface_bg_command(char *data, int len)
{
    ARG_UNUSED(len);

    cJSON *root = cJSON_Parse(data);
    if (root == NULL) {
        LOG_WRN("watchface_bg: JSON parse failed");
        return -EINVAL;
    }

    cJSON *action_obj = cJSON_GetObjectItem(root, "action");
    if (!cJSON_IsString(action_obj)) {
        LOG_WRN("watchface_bg: missing action");
        cJSON_Delete(root);
        return send_watchface_bg_result("unknown", -EINVAL);
    }

    int rc;
    const char *action = action_obj->valuestring;
    if (strcmp(action, "apply") == 0) {
        cJSON_Delete(root);
        rc = watchface_app_reload_bg();
        return send_watchface_bg_result("apply", rc);
    }

    if (strcmp(action, "reset") == 0) {
        cJSON_Delete(root);
        rc = watchface_app_reset_bg();
        return send_watchface_bg_result("reset", rc);
    }

    LOG_WRN("watchface_bg: unknown action '%s'", action);
    cJSON_Delete(root);
    return send_watchface_bg_result("unknown", -ENOTSUP);
}

/* {"t":"find","n":true/false} */
static int parse_find_command(char *data, int len)
{
    ARG_UNUSED(len);

    cJSON *root = cJSON_Parse(data);
    if (root == NULL) {
        LOG_ERR("Failed to parse find command");
        return -EINVAL;
    }

    cJSON *n = cJSON_GetObjectItem(root, "n");
    if (!cJSON_IsBool(n)) {
        LOG_WRN("Find command missing 'n'");
        cJSON_Delete(root);
        return -EINVAL;
    }

    bool find = cJSON_IsTrue(n);
    cJSON_Delete(root);

    int ret = ble_find_device_set(find);
    if (ret != 0) {
        LOG_WRN("Cannot start Find vibration: %d", ret);
        return ret;
    }

    LOG_INF("Find command received, find=%s", find ? "true" : "false");

    return 0;
}

// {"t":"notify_reply","id":1700974318,"ok":false,"err":"The message is gone from the phone"}
static int parse_notify_reply(char *data, int len)
{
#ifdef CONFIG_ZSW_NOTIFICATION_REPLY
    cJSON *root = cJSON_Parse(data);
    if (root == NULL) {
        return -EINVAL;
    }

    cJSON *id = cJSON_GetObjectItem(root, "id");
    cJSON *ok = cJSON_GetObjectItem(root, "ok");
    cJSON *err = cJSON_GetObjectItem(root, "err");
    int rc = -EINVAL;

    if (cJSON_IsNumber(id) && cJSON_IsBool(ok)) {
        rc = publish_notification_reply(ZSW_NOTIFICATION_REPLY_SENT, (uint32_t)id->valuedouble, cJSON_IsTrue(ok),
                                        NULL, cJSON_IsString(err) ? err->valuestring : NULL);
    }
    cJSON_Delete(root);
    return rc;
#else
    ARG_UNUSED(data);
    ARG_UNUSED(len);
    return -ENOTSUP;
#endif
}

static int parse_data_utf8(char *data, int len)
{
    int type_len;
    char *type;

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

    if (strlen("notify_reply") == type_len && strncmp(type, "notify_reply", type_len) == 0) {
        return parse_notify_reply(data, len);
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

    if (strlen("gps") == type_len && strncmp(type, "gps", type_len) == 0) {
        return parse_gps_data(data, len);
    }

    if (strlen("log") == type_len && strncmp(type, "log", type_len) == 0) {
        return parse_log_command(data, len);
    }

    if (strlen("ver") == type_len && strncmp(type, "ver", type_len) == 0) {
        ble_gadgetbridge_send_version_info();
        return 0;
    }

    if (strlen("voice_memo") == type_len && strncmp(type, "voice_memo", type_len) == 0) {
        return parse_voice_memo_command(data, len);
    }

    if (strlen("watchface_bg") == type_len && strncmp(type, "watchface_bg", type_len) == 0) {
        return parse_watchface_bg_command(data, len);
    }

    if (strlen("smp") == type_len && strncmp(type, "smp", type_len) == 0) {
        return parse_smp_command(data, len);
    }

    if (strlen("reset") == type_len && strncmp(type, "reset", type_len) == 0) {
        return parse_reset_command(data, len);
    }

    if (strlen("find") == type_len && strncmp(type, "find", type_len) == 0) {
        return parse_find_command(data, len);
    }

    return 0;
}

static int parse_data(char *data, int len)
{
    if (is_valid_utf8(data, len)) {
        return parse_data_utf8(data, len);
    }

    /* NUS input is parsed on a Bluetooth workqueue. Keep this large, rarely
     * used conversion buffer off its stack. */
    char *converted = k_calloc(MAX_GB_PACKET_LENGTH + 1, 1);
    if (converted == NULL) {
        LOG_ERR("Failed to allocate Gadgetbridge UTF-8 conversion buffer");
        return -ENOMEM;
    }
    int converted_len = convert_to_encoded_text(data, len, converted, MAX_GB_PACKET_LENGTH + 1);
    if (converted_len < 0) {
        LOG_WRN("Invalid or oversized Gadgetbridge text (%d)", converted_len);
        k_free(converted);
        return converted_len;
    }
    int rc = parse_data_utf8(converted, converted_len);
    k_free(converted);
    return rc;
}

void ble_gadgetbridge_input(const uint8_t *const data, uint16_t len)
{
    LOG_HEXDUMP_DBG(data, len, "RX");

    // NULL terminate the data to make string parsing easier and safer.
    char *safe_buf = k_malloc(len + 1);
    if (safe_buf == NULL) {
        LOG_ERR("Failed to allocate %u bytes for NUS parse buffer", len + 1);
        return;
    }
    memcpy(safe_buf, data, len);
    safe_buf[len] = '\0';

    char *gb_start = strstr(safe_buf, "GB(");
    if (gb_start && parse_state != WAIT_GB) {
        LOG_ERR("Parsing error, was waiting end, but got GB");
        parse_state = WAIT_GB;
    }

    char *time_start = strstr(safe_buf, "setTime(");
    if (time_start && parse_state == WAIT_GB) {
        time_start += strlen("setTime(");
        parse_time(time_start);
        goto done;
    }

    // ie. ;E.setTimeZone(1.0);
    char *offset = strstr(safe_buf, ";E.setTimeZone(");
    if (offset && parse_state == WAIT_GB) {
        offset += strlen(";E.setTimeZone(");
        parse_time_zone(offset);
        goto done;
    }

    size_t receive_buf_capacity = MAX_GB_PACKET_LENGTH;

    switch (parse_state) {
        case WAIT_GB: {
            if (gb_start) {
                gb_start += 3;
                parse_state = WAIT_END;
                num_parsed_brackets = 0;
                parsed_data_index = 0;
                memset(receive_buf, 0, sizeof(receive_buf));
                uint32_t index = gb_start - safe_buf;
                for (int i = index; i < len; i++) {
                    if (parsed_data_index >= receive_buf_capacity) {
                        LOG_ERR("Data from Gadgetbridge does not fit in receive_buf (%zu)", receive_buf_capacity);
                        parse_state = WAIT_GB;
                        break;
                    }
                    receive_buf[parsed_data_index] = safe_buf[i];
                    parsed_data_index++;
                    if (safe_buf[i] == '{') {
                        num_parsed_brackets++;
                    } else if (safe_buf[i] == '}') {
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
                if (parsed_data_index >= receive_buf_capacity) {
                    LOG_ERR("Data from Gadgetbridge does not fit in receive_buf (%zu)", receive_buf_capacity);
                    parse_state = WAIT_GB;
                    break;
                }
                receive_buf[parsed_data_index] = safe_buf[i];
                parsed_data_index++;
                if (safe_buf[i] == '{') {
                    num_parsed_brackets++;
                } else if (safe_buf[i] == '}') {
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
        receive_buf[parsed_data_index] = '\0';
        LOG_DBG("%s", receive_buf);
        parse_data((char *)receive_buf, parsed_data_index);
    }

done:
    k_free(safe_buf);
}

void ble_gadgetbridge_send_version_info(void)
{
    char version_msg[100];
    int len = snprintf(version_msg, sizeof(version_msg),
                       "{\"t\":\"ver\",\"fw\":\"%s\",\"hw\":\"%s\"} \n",
                       APP_VERSION_STRING, CONFIG_BOARD_TARGET);
    if (len > 0 && len < sizeof(version_msg)) {
        LOG_DBG("Sending version info: %s", version_msg);
        ble_comm_send(version_msg, len);
    }
}

void ble_gadgetbridge_send_notification_action(uint32_t id, ble_comm_notify_action_t action)
{
    const char *action_str;
    char buf[100];
    int len;

    switch (action) {
        case BLE_COMM_NOTIFY_ACTION_DISMISS:
            action_str = "DISMISS";
            break;
        case BLE_COMM_NOTIFY_ACTION_DISMISS_ALL:
            action_str = "DISMISS_ALL";
            break;
        case BLE_COMM_NOTIFY_ACTION_OPEN:
            action_str = "OPEN";
            break;
        case BLE_COMM_NOTIFY_ACTION_MUTE:
            action_str = "MUTE";
            break;
        case BLE_COMM_NOTIFY_ACTION_REPLY:
            action_str = "REPLY";
            // TODO: This can also contain a reply text, implement later.
            break;
        default:
            LOG_WRN("Unknown notification action (%d) for id %u", action, id);
            return;
    }

    len = snprintf(buf, sizeof(buf), "{\"t\":\"notify\",\"id\":%u,\"n\":\"%s\"} \n", id, action_str);

    if (len > 0 && len < sizeof(buf)) {
        LOG_DBG("Sending notification action: %s", buf);
        ble_comm_send(buf, len);
    } else {
        LOG_WRN("Failed to format notification action for id %u", id);
    }
}

int ble_gadgetbridge_send_notification_reply(uint32_t id, const char *text)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return -ENOMEM;
    }

    int rc = -ENOMEM;
    char *json = NULL;
    if (cJSON_AddStringToObject(root, "t", "notify") && cJSON_AddNumberToObject(root, "id", id) &&
        cJSON_AddStringToObject(root, "n", "REPLY") && cJSON_AddStringToObject(root, "msg", text)) {
        // Escapes quotes and control characters, the text is the user's own words.
        json = cJSON_PrintUnformatted(root);
    }
    cJSON_Delete(root);

    if (json != NULL) {
        size_t len = strlen(json);

        // The trailing newline ends the message like the other watch messages.
        char *msg = k_malloc(len + 3);
        if (msg != NULL) {
            memcpy(msg, json, len);
            memcpy(&msg[len], " \n", 3);
            rc = ble_comm_send((const uint8_t *)msg, len + 2);
            k_free(msg);
        }
        cJSON_free(json);
    }

    if (rc < 0) {
        LOG_WRN("Failed to send reply to %u: %d", id, rc);
    }
    return rc;
}

void ble_gadgetbridge_send_activity_state(zsw_power_manager_state_t activity_state)
{
    char activity_msg[100];
    const char *act_str;

    switch (activity_state) {
        case ZSW_ACTIVITY_STATE_ACTIVE:
            act_str = "ACTIVITY";
            break;
        case ZSW_ACTIVITY_STATE_INACTIVE:
            act_str = "ACTIVITY";
            break;
        case ZSW_ACTIVITY_STATE_NOT_WORN_STATIONARY:
            act_str = "NOT_WORN";
            break;
        default:
            act_str = "UNKNOWN";
            break;
    }

    int len = snprintf(activity_msg, sizeof(activity_msg),
                       "{\"t\":\"act\",\"act\":\"%s\",\"rt\":1} \n", act_str);
    if (len > 0 && len < sizeof(activity_msg)) {
        LOG_DBG("Sending activity state: %s", activity_msg);
        ble_comm_send(activity_msg, len);
    }
}

void ble_gadgetbridge_send_activity_data(uint16_t heart_rate, uint32_t steps,
                                         zsw_imu_data_step_activity_t step_activity,
                                         zsw_power_manager_state_t power_state)
{
    char activity_msg[150];
    const char *act_str;

    /* Map step_activity and power_state to Gadgetbridge activity strings:
     * UNKNOWN, NOT_WORN, DEEP_SLEEP, LIGHT_SLEEP, REM_SLEEP, ACTIVITY,
     * RUNNING, WALKING, SWIMMING, CYCLING, EXERCISE...
     */
    if (power_state == ZSW_ACTIVITY_STATE_NOT_WORN_STATIONARY) {
        act_str = "NOT_WORN";
    } else {
        switch (step_activity) {
            case ZSW_IMU_EVT_STEP_ACTIVITY_RUN:
                act_str = "RUNNING";
                break;
            case ZSW_IMU_EVT_STEP_ACTIVITY_WALK:
                act_str = "WALKING";
                break;
            case ZSW_IMU_EVT_STEP_ACTIVITY_STILL:
                act_str = "ACTIVITY";
                break;
            case ZSW_IMU_EVT_STEP_ACTIVITY_UNKNOWN:
            default:
                act_str = "ACTIVITY";
                break;
        }
    }

    int len;
    if (heart_rate == 0) {
        len = snprintf(activity_msg, sizeof(activity_msg),
                       "{\"t\":\"act\",\"stp\":%u,\"act\":\"%s\",\"rt\":%u} \n",
                       steps, act_str, 1);
    } else {
        len = snprintf(activity_msg, sizeof(activity_msg),
                       "{\"t\":\"act\",\"hrm\":%u,\"stp\":%u,\"act\":\"%s\",\"rt\":%u} \n",
                       heart_rate, steps, act_str, 1);
    }
    if (len > 0 && len < sizeof(activity_msg)) {
        ble_comm_send(activity_msg, len);
    }
}

void ble_gadgetbridge_send_voice_memo_new(const char *filename, uint32_t duration_ms,
                                          uint32_t size_bytes, uint32_t timestamp)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        LOG_ERR("voice_memo new: cJSON alloc failed");
        return;
    }

    cJSON_AddStringToObject(root, "t", "voice_memo");
    cJSON_AddStringToObject(root, "action", "new");
    cJSON_AddStringToObject(root, "filename", filename);
    cJSON_AddNumberToObject(root, "duration_ms", duration_ms);
    cJSON_AddNumberToObject(root, "size_bytes", size_bytes);
    cJSON_AddNumberToObject(root, "timestamp", timestamp);

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if (json_str == NULL) {
        LOG_ERR("voice_memo new: JSON print failed");
        return;
    }

    int msg_len = strlen(json_str);
    ble_comm_send(json_str, msg_len);
    cJSON_free(json_str);
    LOG_INF("voice_memo: sent new recording notification: %s", filename);
}

void ble_gadgetbridge_send_voice_memo_undo(const char *filename)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        LOG_ERR("voice_memo undo: cJSON alloc failed");
        return;
    }

    cJSON_AddStringToObject(root, "t", "voice_memo");
    cJSON_AddStringToObject(root, "action", "undo_last");
    cJSON_AddStringToObject(root, "filename", filename);

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if (json_str == NULL) {
        LOG_ERR("voice_memo undo: JSON print failed");
        return;
    }

    int msg_len = strlen(json_str);
    ble_comm_send(json_str, msg_len);
    cJSON_free(json_str);
    LOG_INF("voice_memo: sent undo command for: %s", filename);
}

#ifdef CONFIG_SHELL
static char *gb_shell_msg;
static int gb_shell_len;

static void gb_shell_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    ble_gadgetbridge_input((const uint8_t *)gb_shell_msg, gb_shell_len);
}

static K_WORK_DEFINE(gb_shell_work, gb_shell_work_fn);

/* For testing without a phone, races with real BLE input. Parsed on the system work queue,
 * shell threads have too little stack for the parser. */
static int cmd_gb(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    struct k_work_sync sync;
    size_t size = strlen(argv[1]) + sizeof("GB()\n");
    char *msg = k_malloc(size);

    if (msg == NULL) {
        return -ENOMEM;
    }
    gb_shell_len = snprintf(msg, size, "GB(%s)\n", argv[1]);
    gb_shell_msg = msg;
    k_work_submit(&gb_shell_work);
    k_work_flush(&gb_shell_work, &sync);
    k_free(msg);
    shell_print(sh, "Parsed %d bytes", gb_shell_len);
    return 0;
}

SHELL_CMD_ARG_REGISTER(gb, NULL, "Parse a message as if the phone sent it: gb '<json>'", cmd_gb, 2, 0);
#endif
