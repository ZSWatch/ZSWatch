#pragma once

#include "ble/ble_comm.h"
#include "ble/ble_transport.h"
#include "events/ble_event.h"
#include "events/music_event.h"

#define CHRONOSESP_VERSION_MAJOR 1
#define CHRONOSESP_VERSION_MINOR 6
#define CHRONOSESP_VERSION_PATCH 0

#define NOTIF_SIZE 10
#define WEATHER_SIZE 7
#define ALARM_SIZE 8
#define DATA_SIZE 512
#define FORECAST_SIZE 24
#define QR_SIZE 9
#define ICON_SIZE 48
#define ICON_DATA_SIZE (ICON_SIZE * ICON_SIZE) / 8
#define CONTACTS_SIZE 255

typedef enum chronos_control {
    MUSIC_PLAY = 0x9D00,
    MUSIC_PAUSE = 0x9D01,
    MUSIC_PREVIOUS = 0x9D02,
    MUSIC_NEXT = 0x9D03,
    MUSIC_TOGGLE = 0x9900,

    VOLUME_UP = 0x99A1,
    VOLUME_DOWN = 0x99A2,
    VOLUME_MUTE = 0x99A3,
} chronos_control_t;

// struct to store assembled incoming data from chronos
typedef struct chronos_data {
    int length;
    uint8_t data[DATA_SIZE];
} chronos_data_t;

typedef struct chronos_notification {
    bool available;
    int icon;
    const char *app;
    char *time;
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
    uint8_t hour;
    uint8_t minute;
    uint8_t repeat;
    bool enabled;
} chronos_alarm_t;

typedef struct chronos_settings {
    uint8_t hour;
    uint8_t minute;
    uint8_t repeat;
    bool enabled;
} chronos_settings_t;

typedef struct chronos_remote_touch {
    bool state;
    uint32_t x;
    uint32_t y;
} chronos_remote_touch;

typedef struct chronos_navigation {
    bool active;          // whether running or not
    bool is_navigation;    // navigation or general info
    bool has_icon;         // icon present in the navigation data
    char *distance;               // distance to destination
    char *duration;               // time to destination
    char *eta;                    // estimated time of arrival (time,date)
    char *title;                  // distance to next point or title
    char *directions;             // place info ie current street name/ instructions
    uint8_t icon[ICON_DATA_SIZE]; // navigation icon 48x48 (1bpp)
    uint32_t icon_crc;             // to identify whether the icon has changed
} chronos_navigation_t;

typedef struct chronos_contact {
    char *name;
    char *number;
} chronos_contact_t;

typedef enum chronos_config {
    CF_TIME = 0, // time -
    CF_RTW,      // raise to wake  -
    CF_HR24,     // 24 hour mode -
    CF_LANG,     // watch language -
    CF_RST,      // watch reset -
    CF_CLR,      // watch clear data
    CF_HOURLY,   // hour measurement -
    CF_FIND,     // find watch -
    CF_USER,     // user details (age)(height)(weight)(step length)(target)(units[])
    CF_ALARM,    // alarm (index)(hour) (minute) (enabled) (repeat) -
    CF_FONT,     // font settings (color[3])(b1+b2) -
    CF_SED,      // sedentary (hour)(minute)(hour)(minute)(interval)(enabled) -
    CF_SLEEP,    // sleep time (hour)(minute)(hour)(minute)(enabled) -
    CF_QUIET,    // quiet hours (hour)(minute)(hour)(minute)(enabled) -
    CF_WATER,    // water reminder (hour)(minute)(hour)(minute)(interval)(enabled)-
    CF_WEATHER,  // weather config (a Weekly) (b City Name) -
    CF_CAMERA,   // camera config, (ready state)
    CF_PBAT,     // phone battery ([a] isPhoneCharing, [b] phoneBatteryLevel)
    CF_APP,      // app version info
    CF_QR,       // qr codes received
    CF_NAV_DATA, // navigation data received
    CF_NAV_ICON, // navigation icon received
    CF_CONTACT,  // contacts data received
} chronos_config_t;


typedef void (*notification_callback_t)(const chronos_notification_t *notification);


void register_notification_callback(notification_callback_t callback);

void ble_chronos_input(const uint8_t *const data, uint16_t len);

// Function prototypes
// TX
void send_command(uint8_t *command, size_t length);

void music_control(chronos_control_t command);

/*!
    @brief  send a command to set the volume level
    @param  level
            volume level (0 - 100)
*/
void set_volume(uint8_t level);

/*!
    @brief  send capture photo command to the app
*/
void capture_photo();

/*!
    @brief  send a command to find the phone
    @param  state
            enable or disable state
*/
void find_phone(bool state);
/*!
    @brief  send the info proprerties to the app
            should happen after every connect/reconnect
*/
void send_info();
/*!
    @brief  send watch battery level
            after reconnect and every time the level changes
*/
void send_battery(uint8_t level, bool charging);

/*!
    @brief  enable/disable phone battery notifications

*/
void set_notify_battery(bool state);

const char *app_name(int id);

chronos_notification_t *get_notification(int index);

// Chronos received commands (data[0] is 0xAB or 0xEA or <= 0x19) on RX characteristic

// this function assembles data packets that are split over multiple transmissions
// when data on RX (6e400002-b5a3-f393-e0a9-e50e24dcca9e) is written
void on_receive_data(uint8_t *pData, int len);
void data_received();

// Language ID
// 0 Chinese
// 1 English
// 2 Italian
// 3 Spanish
// 4 Portuguese
// 5 Russian
// 6 Japanese
// 7 Chinese
// 8 German
// 10 Thai

// WEATHER ICONS
// Weather Icon ID
// 0 - sun + cloud
// 1 - sun
// 2 - snow
// 3 - rain
// 4 - clouds
// 5 - tornado
// 6 - wind
// 7 - sun + haze

// ALERTS ICONS
// Call ID
// 1 0x01 Call
// 2 0x02 cancel call

// Notification Icon IDs start here
// Blank are not supported/known
// 3 0x03 Message
// 4 0x04 Mail
// 5 0x05 Calendar
// 6 0x06
// 7 0x07 QQ
// 8 0x08 Skype
// 9 0x09 Wechat
// 10 0x0A Whatsapp
// 11 0x0B Gmail
// 12 0x0C Hangouts
// 13 0x0D Downloads
// 14 0x0E Line
// 15 0x0F Twitter
// 16 0x10 Facebook
// 17 0x11 Messenger
// 18 0x12 Instagram
// 19 0x13 Weibo
// 20 0x14 KakaoTalk
// 21 0x15
// 22 0x16 Viber
// 23 0x17 VKontakte
// 24 0x18 Telegram
// 25 0x19
// 26 0x1A Snapchat
// 27 0x1B DingTalk
// 28 0x1C Alipay
// 29 0x1D Tiktok
// 30 0x1E
// 31 0x1F
// 32 0x20 Whatsapp Business
// 33 0x21
// 34 0x22 Wearfit Pro