/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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

#pragma once

#include <zephyr/kernel.h>

#define MAX_MUSIC_FIELD_LENGTH          100
#define MAX_HTTP_FIELD_LENGTH           2000
#define MAX_WEATHER_REPORT_TEXT_LENGTH  25

typedef enum ble_comm_data_type {
    BLE_COMM_DATA_TYPE_NOTIFY,
    BLE_COMM_DATA_TYPE_NOTIFY_REMOVE,
    BLE_COMM_DATA_TYPE_SET_TIME,
    BLE_COMM_DATA_TYPE_WEATHER,
    BLE_COMM_DATA_TYPE_MUSIC_INFO,
    BLE_COMM_DATA_TYPE_MUSIC_STATE,
    BLE_COMM_DATA_TYPE_REMOTE_CONTROL,
    BLE_COMM_DATA_TYPE_HTTP,
    BLE_COMM_DATA_TYPE_GPS,
    BLE_COMM_DATA_TYPE_EMPTY
} ble_comm_data_type_t;

typedef struct ble_comm_notify {
    uint32_t id;
    char *body;
    int body_len;
    char *sender;
    int sender_len;
    char *title;
    int title_len;
    char *src;
    int src_len;
    char *subject;
    int subject_len;
} ble_comm_notify_t;

typedef struct ble_comm_notify_remove {
    uint32_t id;
} ble_comm_notify_remove_t;

typedef struct ble_comm_notify_time {
    uint32_t seconds;
    float tz_offset;
} ble_comm_notify_time_t;

typedef struct ble_comm_weather {
    int8_t temperature_c;
    uint16_t humidity;
    uint16_t wind;
    uint16_t wind_direction;
    uint16_t weather_code;
    char report_text[MAX_WEATHER_REPORT_TEXT_LENGTH];
} ble_comm_weather_t;

typedef struct ble_comm_music_info {
    char artist[MAX_MUSIC_FIELD_LENGTH + 1];
    char album[MAX_MUSIC_FIELD_LENGTH + 1];
    char track_name[MAX_MUSIC_FIELD_LENGTH + 1];
    int duration;
    int track_count;
    int track_num;
} ble_comm_music_info_t;

typedef struct ble_comm_music_state {
    bool playing;
    int position;
    int shuffle;
    int repeat;
} ble_comm_music_state_t;

typedef struct ble_comm_remote_control {
    int button;
} ble_comm_remote_control_t;

typedef struct ble_comm_http_response {
    char err[MAX_HTTP_FIELD_LENGTH + 1];
    char response[MAX_HTTP_FIELD_LENGTH + 1];
    int id;
} ble_comm_http_response_t;

typedef struct ble_comm_gps {
    double lat;
    double lon;
    double alt;
    double speed;
    uint64_t time;
    uint8_t satellites;
    float hdop;
    bool externalSource;
    char gpsSource[20];
} ble_comm_gps_t;

typedef struct ble_comm_cb_data {
    ble_comm_data_type_t type;
    union {
        ble_comm_notify_t notify;
        ble_comm_notify_remove_t notify_remove;
        ble_comm_notify_time_t time;
        ble_comm_weather_t weather;
        ble_comm_music_info_t music_info;
        ble_comm_music_state_t music_state;
        ble_comm_remote_control_t remote_control;
        ble_comm_http_response_t http_response;
        ble_comm_gps_t gps;
    } data;
} ble_comm_cb_data_t;

typedef void(*on_data_cb_t)(ble_comm_cb_data_t *data);

/** @brief
 *  @return 0 when successful
*/
int ble_comm_init(void);

/** @brief
 *  @param data
 *  @param len
 *  @return     0 when successful
*/
int ble_comm_send(uint8_t *data, uint16_t len);

/** @brief
 *  @param pairable
 *  @return         0 when successful
*/
void ble_comm_set_pairable(bool pairable);

/** @brief
 *  @return 0 when successful
*/
int ble_comm_short_connection_interval(void);

/** @brief
 *  @return 0 when successful
*/
int ble_comm_long_connection_interval(void);

/** @brief
 *  @return The MTU for current connection. 0 If no connection.
*/
int ble_comm_get_mtu(void);

int ble_comm_request_gps_status(bool enable);
