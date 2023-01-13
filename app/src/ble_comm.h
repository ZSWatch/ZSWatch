#ifndef __BLE_COMM_H
#define __BLE_COMM_H

#include <zephyr/kernel.h>

#define MAX_MUSIC_FIELD_LENGTH          25
#define MAX_WEATHER_REPORT_TEXT_LENGTH  25

typedef enum ble_comm_data_type {
    BLE_COMM_DATA_TYPE_NOTIFY,
    BLE_COMM_DATA_TYPE_NOTIFY_REMOVE,
    BLE_COMM_DATA_TYPE_SET_TIME,
    BLE_COMM_DATA_TYPE_WEATHER,
    BLE_COMM_DATA_TYPE_MUSTIC_INFO,
    BLE_COMM_DATA_TYPE_MUSTIC_STATE
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
} ble_comm_notify_t;

typedef struct ble_comm_notify_remove {
    uint32_t id;
} ble_comm_notify_remove_t;

typedef struct ble_comm_notify_time {
    uint32_t seconds;
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

typedef struct ble_comm_cb_data {
    ble_comm_data_type_t type;
    union {
        ble_comm_notify_t notify;
        ble_comm_notify_remove_t notify_remove;
        ble_comm_notify_time_t time;
        ble_comm_weather_t weather;
        ble_comm_music_info_t music_info;
        ble_comm_music_state_t music_state;
    } data;
} ble_comm_cb_data_t;

typedef void(*on_data_cb_t)(ble_comm_cb_data_t *data);

int ble_comm_init(on_data_cb_t data_cb);
int ble_comm_send(uint8_t *data, uint16_t len);
#endif