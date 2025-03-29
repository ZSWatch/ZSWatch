#pragma once

#include <time.h>
#include <sys/time.h>

#include "ble/ble_comm.h"
#include "ble/ble_transport.h"
#include "events/ble_event.h"
#include "events/music_event.h"

#define CHRONOSESP_VERSION_MAJOR 1
#define CHRONOSESP_VERSION_MINOR 6
#define CHRONOSESP_VERSION_PATCH 0

#define CH_NOTIF_SIZE 10
#define CH_WEATHER_SIZE 7
#define CH_ALARM_SIZE 8
#define CH_DATA_SIZE 512
#define CH_FORECAST_SIZE 24
#define CH_QR_SIZE 9
#define CH_ICON_SIZE 48
#define CH_ICON_DATA_SIZE (CH_ICON_SIZE * CH_ICON_SIZE) / 8
#define CH_CONTACTS_SIZE 255

typedef enum chronos_control {
    CH_CONTROL_MUSIC_PLAY = 0x9D00,
    CH_CONTROL_MUSIC_PAUSE = 0x9D01,
    CH_CONTROL_MUSIC_PREVIOUS = 0x9D02,
    CH_CONTROL_MUSIC_NEXT = 0x9D03,
    CH_CONTROL_MUSIC_TOGGLE = 0x9900,

    CH_CONTROL_VOLUME_UP = 0x99A1,
    CH_CONTROL_VOLUME_DOWN = 0x99A2,
    CH_CONTROL_VOLUME_MUTE = 0x99A3,
} chronos_control_t;

// struct to store assembled incoming data from chronos
typedef struct chronos_data {
    int length;
    uint8_t data[CH_DATA_SIZE];
} chronos_data_t;

typedef struct chronos_time {
    uint8_t hour;
    uint8_t minute;
} chronos_time_t;

typedef struct chronos_notification {
    bool available;
    int icon;
    chronos_time_t time;
    char *title;
    char *message;
} chronos_notification_t;

typedef struct chronos_weather {
    int icon;
    int day;
    int temp;
    int high;
    int low;
    int pressure;
    int uv;
} chronos_weather_t;

typedef struct chronos_weather_info {
    int size;
    chronos_time_t time;
    char *city;
} chronos_weather_info_t;

typedef struct chronos_hourly_forecast {
    int day;  // day of forecast
    int hour; // hour of the forecast
    int icon;
    int temp;     //
    int uv;       // uv index
    int humidity; // %
    int wind;     // wind speed km/h
} chronos_hourly_forecast_t;

typedef struct chronos_alarm {
    chronos_time_t time;
    uint8_t repeat;
    bool enabled;
} chronos_alarm_t;

typedef struct chronos_settings {
    chronos_time_t time;
    uint8_t repeat;
    bool enabled;
} chronos_settings_t;

typedef struct chronos_remote_touch {
    bool state;
    uint32_t x;
    uint32_t y;
} chronos_remote_touch_t;

typedef struct chronos_navigation {
    bool active;          // whether running or not
    bool is_navigation;    // navigation or general info
    bool has_icon;         // icon present in the navigation data
    char *distance;               // distance to destination
    char *duration;               // time to destination
    char *eta;                    // estimated time of arrival (time,date)
    char *title;                  // distance to next point or title
    char *directions;             // place info ie current street name/ instructions
    uint8_t icon[CH_ICON_DATA_SIZE]; // navigation icon 48x48 (1bpp)
    uint32_t icon_crc;             // to identify whether the icon has changed
} chronos_navigation_t;

typedef struct chronos_contact {
    char *name;
    char *number;
} chronos_contact_t;

typedef struct chronos_app_info {
    uint32_t code;
    char *version;
} chronos_app_info_t;

typedef struct chronos_phone_info {
    uint8_t state;
    uint8_t level;
} chronos_phone_info_t;

typedef enum chronos_config {
    CH_CONFIG_TIME = 0, // time -
    CH_CONFIG_RTW,      // raise to wake  -
    CH_CONFIG_HR24,     // 24 hour mode -
    CH_CONFIG_LANG,     // watch language -
    CH_CONFIG_RST,      // watch reset -
    CH_CONFIG_CLR,      // watch clear data
    CH_CONFIG_HOURLY,   // hour measurement -
    CH_CONFIG_FIND,     // find watch -
    CH_CONFIG_USER,     // user details (age)(height)(weight)(step length)(target)(units[])
    CH_CONFIG_ALARM,    // alarm (index)(hour) (minute) (enabled) (repeat) -
    CH_CONFIG_FONT,     // font settings (color[3])(b1+b2) -
    CH_CONFIG_SED,      // sedentary (hour)(minute)(hour)(minute)(interval)(enabled) -
    CH_CONFIG_SLEEP,    // sleep time (hour)(minute)(hour)(minute)(enabled) -
    CH_CONFIG_QUIET,    // quiet hours (hour)(minute)(hour)(minute)(enabled) -
    CH_CONFIG_WATER,    // water reminder (hour)(minute)(hour)(minute)(interval)(enabled)-
    CH_CONFIG_WEATHER,  // weather config (a Weekly) (b City Name) -
    CH_CONFIG_CAMERA,   // camera config, (ready state)
    CH_CONFIG_PBAT,     // phone battery ([a] isPhoneCharing, [b] phoneBatteryLevel)
    CH_CONFIG_APP,      // app version info
    CH_CONFIG_QR,       // qr codes received
    CH_CONFIG_NAV_DATA, // navigation data received
    CH_CONFIG_NAV_ICON, // navigation icon received
    CH_CONFIG_CONTACT,  // contacts data received
} chronos_config_t;

typedef void (*notification_callback_t)(const chronos_notification_t *notification);

typedef void (*ringer_callback_t)(bool state, char *caller);

typedef void (*configuration_callback_t)(chronos_config_t config, uint32_t a, uint32_t b);

typedef void (*touch_callback_t)(chronos_remote_touch_t *touch);

void ble_chronos_add_notification_cb(notification_callback_t callback);
void ble_chronos_add_ringer_cb(ringer_callback_t callback);
void ble_chronos_add_configuration_cb(configuration_callback_t callback);
void ble_chronos_add_touch_cb(touch_callback_t callback);

void ble_chronos_input(const uint8_t *const data, uint16_t len);
void ble_chronos_state(bool connect);
void ble_chronos_connection_update();

void ble_chronos_send_command(uint8_t *command, size_t length);

void ble_chronos_music_control(chronos_control_t command);

void ble_chronos_set_volume(uint8_t level);

void ble_chronos_capture_photo();

void ble_chronos_find_phone(bool state);

void ble_chronos_send_info();

void ble_chronos_send_battery(uint8_t level, bool charging);

void ble_chronos_set_notify_battery(bool state);

chronos_notification_t *ble_chronos_get_notification(int index);
int ble_chronos_get_notification_count();
void ble_chronos_clear_notifications();

chronos_weather_info_t *ble_chronos_get_weather_info();
chronos_weather_t *ble_chronos_get_weather(int index);
chronos_hourly_forecast_t *ble_chronos_get_forecast_hour(int hour);

chronos_navigation_t *ble_chronos_get_navigation();

chronos_app_info_t *ble_chronos_get_app_info();
chronos_phone_info_t *ble_chronos_get_phone_info();

struct tm ble_chronos_get_time_struct();
const char *ble_chronos_get_app_name(int id);

void ble_chronos_on_receive_data(const uint8_t *data, uint16_t len);
void ble_chronos_data_received();
