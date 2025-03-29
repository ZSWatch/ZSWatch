#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "ui/zsw_ui.h"
#include "ble/ble_comm.h"
#include "ble/ble_transport.h"
#include "events/ble_event.h"
#include "events/music_event.h"
#include "ble_chronos.h"

LOG_MODULE_REGISTER(ble_chronos, CONFIG_ZSW_BLE_LOG_LEVEL);

// static void parse_time(char *data);
static void music_control_event_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(android_music_control_lis_chronos, music_control_event_callback);

static chronos_data_t incoming; // variable to store incoming data

static chronos_notification_t notifications[CH_NOTIF_SIZE];
static int notificationIndex = -1;

static chronos_weather_t weather[CH_WEATHER_SIZE];
static chronos_weather_info_t weather_info;
static chronos_hourly_forecast_t hourly_forecast[CH_FORECAST_SIZE];

static chronos_navigation_t navigation;

static chronos_remote_touch_t remote_touch;
static char *caller_name;

static chronos_phone_info_t phone_info;
static chronos_app_info_t app_info;

static notification_callback_t notification_callback = NULL;
static ringer_callback_t ringer_callback = NULL;
static configuration_callback_t configuration_callback = NULL;
static touch_callback_t touch_callback = NULL;

void ble_chronos_add_notification_cb(notification_callback_t callback)
{
    notification_callback = callback;
}

void ble_chronos_add_ringer_cb(ringer_callback_t callback)
{
    ringer_callback = callback;
}

void ble_chronos_add_configuration_cb(configuration_callback_t callback)
{
    configuration_callback = callback;
}

void ble_chronos_add_touch_cb(touch_callback_t callback)
{
    touch_callback = callback;
}

static void send_ble_data_event(struct ble_data_event *evt)
{
    zbus_chan_pub(&ble_comm_data_chan, evt, K_MSEC(250));
}

static void music_control_event_callback(const struct zbus_channel *chan)
{
    const struct music_event *event = zbus_chan_const_msg(chan);

    LOG_INF("Music control event %d", event->control_type);

    switch (event->control_type) {
        case MUSIC_CONTROL_UI_PLAY:
            ble_chronos_music_control(CH_CONTROL_MUSIC_PLAY);
            break;
        case MUSIC_CONTROL_UI_PAUSE:
            ble_chronos_music_control(CH_CONTROL_MUSIC_PAUSE);
            break;
        case MUSIC_CONTROL_UI_NEXT_TRACK:
            ble_chronos_music_control(CH_CONTROL_MUSIC_NEXT);
            break;
        case MUSIC_CONTROL_UI_PREV_TRACK:
            ble_chronos_music_control(CH_CONTROL_MUSIC_PREVIOUS);
            break;
        case MUSIC_CONTROL_UI_CLOSE:
        default:
            // Nothing to do
            break;
    }

}

static void parse_time(uint32_t epoch)
{
    char *end_time;
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    cb.data.data.time.seconds = epoch;
    cb.data.type = BLE_COMM_DATA_TYPE_SET_TIME;
    send_ble_data_event(&cb);

}

static int parse_notify(chronos_notification_t *notification)
{
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    struct tm tm_info = ble_chronos_get_time_struct();

    cb.data.type = BLE_COMM_DATA_TYPE_NOTIFY;
    cb.data.data.notify.id = tm_info.tm_sec + notification->time.hour * 10000 +  notification->time.minute * 100;
    cb.data.data.notify.src = strdup(ble_chronos_get_app_name(notification->icon));
    cb.data.data.notify.src_len = strlen(ble_chronos_get_app_name(notification->icon));
    cb.data.data.notify.sender = strdup(ble_chronos_get_app_name(notification->icon));
    cb.data.data.notify.sender_len = strlen(ble_chronos_get_app_name(notification->icon));
    cb.data.data.notify.title = strdup(notification->title);
    cb.data.data.notify.title_len = strlen(notification->title);
    cb.data.data.notify.subject = strdup(notification->title);
    cb.data.data.notify.subject_len = strlen(notification->title);
    cb.data.data.notify.body = strdup(notification->message);
    cb.data.data.notify.body_len = strlen(notification->message);

    send_ble_data_event(&cb);

    return 0;
}

static int parse_weather()
{
    //{t:"weather",temp:268,hum:97,code:802,txt:"slightly cloudy",wind:2.0,wdir:14,loc:"MALMO"
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    chronos_weather_info_t *info = ble_chronos_get_weather_info();
    chronos_weather_t *weather = ble_chronos_get_weather(0);
    chronos_hourly_forecast_t *forecast = ble_chronos_get_forecast_hour(ble_chronos_get_time_struct().tm_hour);

    cb.data.type = BLE_COMM_DATA_TYPE_WEATHER;
    cb.data.data.weather.humidity = forecast->humidity;
    cb.data.data.weather.weather_code = 0;
    cb.data.data.weather.wind = forecast->wind;
    cb.data.data.weather.wind_direction = 0;
    // cb.data.data.weather.report_text

    cb.data.data.weather.temperature_c = weather->temp;

    send_ble_data_event(&cb);

    return 0;
}

void ble_chronos_input(const uint8_t *const data, uint16_t len)
{
    // LOG_HEXDUMP_DBG(data, len, "RX");
    // LOG_INF("Data received, length %d", len);
    ble_chronos_on_receive_data(data, len);
}

void ble_chronos_state(bool connect)
{
    LOG_INF("Bluetooth  %s", connect ? "Connected" : "Disconnected");

    if (connect) {
        // connected
        ble_chronos_send_info(); // needed to detect watch type on Chronos app
        ble_chronos_set_notify_battery(true); // needed for navigation to work
    } else {
        // disconnected
        if (navigation.active) {
            navigation.active = false;
            if (configuration_callback != NULL) {
                configuration_callback(CH_CONFIG_NAV_DATA, navigation.active ? 1 : 0, 0);
            }
        }

        if (remote_touch.state) {
            remote_touch.state = false;
            if (touch_callback != NULL) {
                touch_callback(&remote_touch);
            }
        }
    }

}

void ble_chronos_connection_update()
{
    // connected
    ble_chronos_send_info(); // needed to detect watch type on Chronos app
    ble_chronos_set_notify_battery(true); // needed for navigation to work
}

void ble_chronos_send_command(uint8_t *command, size_t length)
{

    // LOG_HEXDUMP_DBG(command, length, "Chronos TX");
    // LOG_INF("Data sent, length %d", length);

    ble_comm_send(command, length);
}

void ble_chronos_music_control(chronos_control_t command)
{
    uint8_t music_cmd[] = {0xAB, 0x00, 0x04, 0xFF, (uint8_t)(command >> 8), 0x80, (uint8_t)(command)};
    ble_chronos_send_command(music_cmd, 7);
}

void ble_chronos_set_volume(uint8_t level)
{
    uint8_t volume_cmd[] = {0xAB, 0x00, 0x05, 0xFF, 0x99, 0x80, 0xA0, level};
    ble_chronos_send_command(volume_cmd, 8);
}

void ble_chronos_capture_photo()
{
    uint8_t capture_cmd[] = {0xAB, 0x00, 0x04, 0xFF, 0x79, 0x80, 0x01};
    ble_chronos_send_command(capture_cmd, 7);
}

void ble_chronos_find_phone(bool state)
{
    uint8_t c = state ? 0x01 : 0x00;
    uint8_t find_cmd[] = {0xAB, 0x00, 0x04, 0xFF, 0x7D, 0x80, c};
    ble_chronos_send_command(find_cmd, 7);
}

void ble_chronos_send_info()
{
    uint8_t info_cmd[] = {0xab, 0x00, 0x11, 0xff, 0x92, 0xc0, CHRONOSESP_VERSION_MAJOR, (CHRONOSESP_VERSION_MINOR * 10 + CHRONOSESP_VERSION_PATCH), 0x00, 0xfb, 0x1e, 0x40, 0xc0, 0x0e, 0x32, 0x28, 0x00, 0xe2, 0x07, 0x80};
    ble_chronos_send_command(info_cmd, 20);
}

void ble_chronos_send_battery(uint8_t level, bool charging)
{
    uint8_t c = charging ? 0x01 : 0x00;
    uint8_t bat_cmd[] = {0xAB, 0x00, 0x05, 0xFF, 0x91, 0x80, c, level};
    ble_chronos_send_command(bat_cmd, 8);
}

void ble_chronos_set_notify_battery(bool state)
{
    uint8_t s = state ? 0x01 : 0x00;
    uint8_t bat_cmd[] = {0xAB, 0x00, 0x04, 0xFE, 0x91, 0x80, s}; // custom command AB..FE
    ble_chronos_send_command(bat_cmd, 7);
}

chronos_notification_t *ble_chronos_get_notification(int index)
{
    return &notifications[index % CH_NOTIF_SIZE];
}

int ble_chronos_get_notification_count()
{
    if (notificationIndex + 1 >= CH_NOTIF_SIZE) {
        return CH_NOTIF_SIZE; // the buffer is full
    } else {
        return notificationIndex + 1; // the buffer is not full,
    }
}

void ble_chronos_clear_notifications()
{
    // here we just set the index to -1, existing data at the buffer will be overwritten
    // ble_chronos_get_notification_count() will return 0 but ble_chronos_get_notification(int index) will return previous existing data

    for (int i = 0; i < CH_NOTIF_SIZE; i++) {
        if (notifications[i].message) {
            free(notifications[i].message);
        }
        if (notifications[i].title) {
            free(notifications[i].title);
        }
        notifications[i].available = false;

    }
    notificationIndex = -1;
}

chronos_navigation_t *ble_chronos_get_navigation()
{
    return &navigation;
}

chronos_weather_info_t *ble_chronos_get_weather_info()
{
    return &weather_info;
}

chronos_weather_t *ble_chronos_get_weather(int index)
{
    return &weather[index % CH_WEATHER_SIZE];
}

chronos_hourly_forecast_t *ble_chronos_get_forecast_hour(int hour)
{
    return &hourly_forecast[hour % CH_FORECAST_SIZE];
}

chronos_app_info_t *ble_chronos_get_app_info()
{
    return &app_info;
}

chronos_phone_info_t *ble_chronos_get_phone_info()
{
    return &phone_info;
}

struct tm ble_chronos_get_time_struct()
{
    struct timeval tv;
    gettimeofday(&tv, NULL); // Get current time
    struct tm *tn = localtime(&tv.tv_sec);  // Convert to local time
    return *tn;
}

void ble_chronos_extract_notification(const char *input, char **title, char **message)
{
    *title = NULL;
    *message = NULL;

    if (!input) {
        return;
    }

    char *colon_pos = strchr(input, ':');
    if (!colon_pos) {
        // No ':' found, return original string as message
        *message = strdup(input);
        LOG_INF("No ':' found, return original string as message");
        return;
    }

    size_t title_length = colon_pos - input;
    char *newline_pos = strchr(input, '\n');

    // Check title conditions
    if (title_length >= 30 || (newline_pos && newline_pos < colon_pos)) {
        // Title is too long or contains a newline before ':'
        LOG_INF("Title is too long %d or contains a newline before ':' at %d - %d = %d", title_length, colon_pos, input,
                colon_pos - input);
        *message = strdup(input);
        return;
    }

    *title = (char *)malloc(title_length + 1);
    if (!*title) {
        LOG_INF("Memory allocation failed");
        return;    // Memory allocation failed
    }

    strncpy(*title, input, title_length);
    (*title)[title_length] = '\0'; // Null-terminate

    *message = strdup(colon_pos + 1); // Copy the message after ':'
}

const char *ble_chronos_get_app_name(int id)
{
    switch (id) {
        case 0x03:
            return "Message";
        case 0x04:
            return "Mail";
        case 0x07:
            return "Tencent";
        case 0x08:
            return "Skype";
        case 0x09:
            return "Wechat";
        case 0x0A:
            return "WhatsApp";
        case 0x0B:
            return "Gmail";
        case 0x0E:
            return "Line";
        case 0x0F:
            return "Twitter";
        case 0x10:
            return "Facebook";
        case 0x11:
            return "Messenger";
        case 0x12:
            return "Instagram";
        case 0x13:
            return "Weibo";
        case 0x14:
            return "KakaoTalk";
        case 0x16:
            return "Viber";
        case 0x17:
            return "Vkontakte";
        case 0x18:
            return "Telegram";
        case 0x1B:
            return "DingTalk";
        case 0x20:
            return "WhatsApp Business";
        case 0x22:
            return "WearFit Pro";
        case 0xC0:
            return "Chronos";
        default:
            return "Message";
    }
}

/* DATA FROM CHRONOS APP FUNCTIONS */

// Chronos received commands (data[0] is 0xAB or 0xEA or <= 0x19) on RX characteristic.
// this function assembles data packets that are split over multiple transmissions
void ble_chronos_on_receive_data(const uint8_t *data, uint16_t len)
{
    // LOG_HEXDUMP_DBG(data, len, "Chronos RX");
    if (len > 0) {

        // Chronos app sends data starting with either AB or EA for the first packet and FE or FF at index 3
        if ((data[0] == 0xAB || data[0] == 0xEA) && (data[3] == 0xFE || data[3] == 0xFF)) {
            // start of data, assign length from packet
            incoming.length = data[1] * 256 + data[2] + 3;
            // copy data to incomingBuffer
            for (int i = 0; i < len; i++) {
                incoming.data[i] = data[i];
            }

            if (incoming.length <= len) {
                // complete packet assembled
                ble_chronos_data_received();
            } else {
                // data is still being assembled
                // LOG_INF("Incomplete");
            }
        } else if (data[0] < 0x19) {
            // subsequent packets start with 0 (max anticipated is 25 -> 0x19)
            int j = 20 + (data[0] * 19); // data packet position
            // copy data to incomingBuffer
            for (int i = 0; i < len; i++) {
                incoming.data[j + i] = data[i + 1];
            }

            if (incoming.length <= len + j - 1) {
                // complete packet assembled
                ble_chronos_data_received();
            } else {
                // data is still being assembled
                // LOG_INF("Incomplete");
            }
        } else {
            LOG_INF("Not Chronos data");
        }
    }
}

void ble_chronos_data_received()
{
    int len = incoming.length;

    // LOG_INF("Complete data length %d", len);
    // LOG_HEXDUMP_DBG(incoming.data, len, "Chronos RX");

    if (incoming.data[0] == 0xAB) {
        switch (incoming.data[4]) {
            case 0x23:
                // request to reset the watch
                LOG_INF("Reset watch");
                break;
            case 0x53:
                // uint8_t hour = incoming.data[7];
                // uint8_t minute = incoming.data[8];
                // uint8_t hour2 = incoming.data[9];
                // uint8_t minute2 = incoming.data[10];
                // bool enabled = incoming.data[6];
                // uint8_t interval = incoming.data[11]; // interval in minutes
                break;
            case 0x71:
                // find watch
                // the watch should vibrate and/or ring for a few seconds
                LOG_INF("find watch");
                break;
            case 0x72: {
                int icon = incoming.data[6]; // See ALERT ICONS
                int state = incoming.data[7];
                char message[512] = {0};

                for (int i = 8; i < len; i++) {
                    strncat(message, (char *)&incoming.data[i], 1);
                }

                // LOG_INF("Notification id: %02X, state: %d", icon, state);

                if (icon == 0x01) {
                    // caller command
                    // message contains the caller details (name or number)
                    if (caller_name) {
                        free(caller_name);
                    }
                    caller_name = strdup(message);
                    if (ringer_callback != NULL) {
                        ringer_callback(true, caller_name);
                    }
                } else if (icon == 0x02) {
                    // cancel caller command
                    if (ringer_callback != NULL) {
                        ringer_callback(false, caller_name);
                    }
                } else if (state == 0x02) {

                    struct tm tm_info = ble_chronos_get_time_struct();

                    chronos_time_t time = {
                        .hour = tm_info.tm_hour,
                        .minute = tm_info.tm_min
                    };

                    // Free old memory before assigning new values
                    notificationIndex++;

                    if (notifications[notificationIndex % CH_NOTIF_SIZE].message) {
                        free(notifications[notificationIndex % CH_NOTIF_SIZE].message);
                    }
                    if (notifications[notificationIndex % CH_NOTIF_SIZE].title) {
                        free(notifications[notificationIndex % CH_NOTIF_SIZE].title);
                    }

                    ble_chronos_extract_notification(message,
                                                     &notifications[notificationIndex % CH_NOTIF_SIZE].title,
                                                     &notifications[notificationIndex % CH_NOTIF_SIZE].message
                                                    );

                    if (!notifications[notificationIndex % CH_NOTIF_SIZE].title) {
                        notifications[notificationIndex % CH_NOTIF_SIZE].title = strdup(ble_chronos_get_app_name(icon));
                    }

                    notifications[notificationIndex % CH_NOTIF_SIZE].available = true;
                    notifications[notificationIndex % CH_NOTIF_SIZE].icon = icon;
                    notifications[notificationIndex % CH_NOTIF_SIZE].time = time;

                    parse_notify(&notifications[notificationIndex % CH_NOTIF_SIZE]);

                    if (notification_callback != NULL) {
                        notification_callback(&notifications[notificationIndex % CH_NOTIF_SIZE]);
                    }
                }
            }
            break;
            case 0x73: {
                // alarms
                // uint8_t index = incoming.data[6]; [0-7]
                // bool enabled = incoming.data[7];
                // uint8_t hour = incoming.data[8];
                // uint8_t minute = incoming.data[9];
                // uint8_t repeat = incoming.data[10]; //
                // repeat values 0x80->Once or 0x01-0x7F -> Specify days with bits. Order [null,Sun,Sat,Fri,Thu,Wed,Tue,Mon]
                // 0x80 [1000 0000] -> Once (one time alarm)
                // 0x7F [0111 1111] -> everyday (null,Sun,Sat,Fri,Thu,Wed,Tue,Mon)
                // 0x1F [0001 1111] -> Monday - Friday
                // 0x43 [0100 0011] -> Custom (Sun, Tue, Mon)
            }
            break;
            case 0x74:
                // user details and settings from the app
                // uint8_t stepLength = incoming.data[6]; // cm
                // uint8_t age = incoming.data[7]; //yrs
                // uint8_t height = incoming.data[8]; // cm
                // uint8_t weight = incoming.data[9]; // kg
                // uint8_t unit = incoming.data[10]; 0->Imperial 1->Metric
                // uint8_t targetSteps = incoming.data[11] * 1000;
                // uint8_t tempUnit = incoming.data[12]; 0->C 1->F

                break;
            case 0x75:
                // sedentary reminder
                // bool enabled = incoming.data[6];
                // uint8_t hour = incoming.data[7]; // start
                // uint8_t minute = incoming.data[8];
                // uint8_t hour2 = incoming.data[9]; // end
                // uint8_t minute2 = incoming.data[10];
                // uint8_t interval = incoming.data[11]; // interval in minutes
                break;
            case 0x76:
                // quiet hours settings
                // bool enabled = incoming.data[6];
                // uint8_t hour = incoming.data[7];
                // uint8_t minute = incoming.data[8];
                // uint8_t hour2 = incoming.data[9];
                // uint8_t minute2 = incoming.data[10];
                break;
            case 0x77:
                // raise to wake settings
                // incoming.data[6]; 1->ON 0->OFF

                break;
            case 0x78:
                // health hourly settings (used to trigger health measurements every hour)
                // incoming.data[6]; 1->ON 0->OFF
                break;
            case 0x79:
                // remote camera function
                // this tells the watch that the camera is active on the app and ready to receive capture command
                // _cameraReady = ((uint8_t)incoming.data[6] == 1);
                if (configuration_callback != NULL) {
                    configuration_callback(CH_CONFIG_CAMERA, 0, (uint32_t)incoming.data[6]);
                }
                break;
            case 0x7B:
                // change watch language if supported
                // incoming.data[6] is the language id; See LANGUAGE ID
                break;
            case 0x7C:
                // 24 hour clock mode
                // incoming.data[6]; 1->ON 0->OFF
                break;
            case 0x7E:
                // weather data received
                // contains daily forecast
                struct tm tm_info = ble_chronos_get_time_struct();
                chronos_time_t time = {
                    .hour = tm_info.tm_hour,
                    .minute = tm_info.tm_min
                };
                weather_info.time = time;
                weather_info.size = 0;
                for (int k = 0; k < (len - 6) / 2; k++) {
                    int sign = (incoming.data[(k * 2) + 6] & 1) ? -1 : 1;

                    int icon = incoming.data[(k * 2) + 6] >> 4; // icon id; See WEATHER ICONS
                    int temp = ((int)incoming.data[(k * 2) + 7]) * sign;

                    int dy = tm_info.tm_wday + k;
                    weather[k].day = dy % 7;
                    weather[k].icon = icon;
                    weather[k].temp = temp;
                    weather_info.size++;
                }
                if (configuration_callback != NULL) {
                    configuration_callback(CH_CONFIG_WEATHER, 1, 0);
                }
                break;
            case 0x7F:
                // sleep settings
                // bool enabled = incoming.data[6];
                // uint8_t hour = incoming.data[7];
                // uint8_t minute = incoming.data[8];
                // uint8_t hour2 = incoming.data[9];
                // uint8_t minute2 = incoming.data[10];
                break;
            case 0x88:
                // weather data received
                // contains high and low temperature forecast
                for (int k = 0; k < (len - 6) / 2; k++) {
                    int signH = (incoming.data[(k * 2) + 6] >> 7 & 1) ? -1 : 1;
                    int tempH = ((int)incoming.data[(k * 2) + 6] & 0x7F) * signH;

                    int signL = (incoming.data[(k * 2) + 7] >> 7 & 1) ? -1 : 1;
                    int tempL = ((int)incoming.data[(k * 2) + 7] & 0x7F) * signL;

                    weather[k].high = tempH;
                    weather[k].low = tempL;
                }
                parse_weather();
                if (configuration_callback != NULL) {
                    configuration_callback(CH_CONFIG_WEATHER, 2, 0);
                }
                break;
            case 0x91:
                if (incoming.data[3] == 0xFE) {
                    // custom app command
                    // status of the phone battery
                    phone_info.state = incoming.data[6]; // 1->Charging 0->Not Charging
                    phone_info.level = incoming.data[7]; // phone battery level %
                    if (configuration_callback != NULL) {
                        configuration_callback(CH_CONFIG_PBAT, phone_info.state, phone_info.level);
                    }
                }

                break;
            case 0x93:
                // time received (update watch time immediately)
                // year; incoming.data[7] * 256 + incoming.data[8]
                // month; incoming.data[9]
                // day; incoming.data[10]
                // hour; incoming.data[11]
                // minute; incoming.data[12]
                // seconds; incoming.data[13]
                struct tm t = {0, 0, 0, 0, 0, 0, 0, 0, 0};      // Initalize to all 0's
                t.tm_year = (incoming.data[7] * 256) + incoming.data[8] - 1900;    // This is year-1900, so 121 = 2021
                t.tm_mon = incoming.data[9] - 1;
                t.tm_mday = incoming.data[10];
                t.tm_hour = incoming.data[11];
                t.tm_min = incoming.data[12];
                t.tm_sec = incoming.data[13];
                time_t epoch = mktime(&t);
                parse_time((uint32_t)epoch);

                break;
            case 0x9C:
                // watchface font style and color settings
                // uint32_t colorRGB = ((uint32_t)incoming.data[5] << 16) | ((uint32_t)incoming.data[6] << 8) | (uint32_t)incoming.data[7]
                // style incoming.data[8] [0-2]
                // position incoming.data[9] 0->Top, 1->Center, 2->Bottom

                break;
            case 0xA8:
                if (incoming.data[3] == 0xFE) {
                    // end of qr data transmission (Chronos v3.7.0+)
                    // incoming.data[5]; // number of links received
                }
                if (incoming.data[3] == 0xFF) {
                    // qr links with index
                    // incoming.data[5]; // index of the current link
                    // link incoming.data[6:len]
                }
                break;
            case 0xBF:
                if (incoming.data[3] == 0xFE) {
                    // remote touch data (Chronos v3.7.0+)
                    remote_touch.state = incoming.data[5] == 1;
                    remote_touch.x = (uint32_t)(incoming.data[6] << 8) | (uint32_t)(incoming.data[7]);
                    remote_touch.y = (uint32_t)(incoming.data[8] << 8) | (uint32_t)(incoming.data[9]);

                    if (touch_callback != NULL) {
                        touch_callback(&remote_touch);
                    }
                }
                break;
            case 0xCA:
                if (incoming.data[3] == 0xFE) {
                    app_info.code = (incoming.data[6] * 256) + incoming.data[7];

                    char version[50] = {0};
                    for (int i = 8; i < len; i++) {
                        strncat(version, (char *)&incoming.data[i], 1);
                    }
                    if (app_info.version) {
                        free(app_info.version);
                    }
                    app_info.version = strdup(version);

                    if (configuration_callback != NULL) {
                        configuration_callback(CH_CONFIG_APP, app_info.code, 0);
                    }
                }
                break;
            // case 0xCC:
            //     if (incoming.data[3] == 0xFE) {
            //         setChunkedTransfer(incoming.data[5] != 0x00);
            //     }
            //     break;
            case 0xEE:
                if (incoming.data[3] == 0xFE) {
                    // navigation icon data received
                    uint8_t pos = incoming.data[6];
                    uint32_t crc = (uint32_t)(incoming.data[7] << 24) | (uint32_t)(incoming.data[8] << 16) | (uint32_t)(
                                       incoming.data[9] << 8) | (uint32_t)(incoming.data[10]);
                    for (int i = 0; i < 96; i++) {
                        navigation.icon[i + (96 * pos)] = incoming.data[11 + i];
                    }

                    if (configuration_callback != NULL) {
                        configuration_callback(CH_CONFIG_NAV_ICON, pos, crc);
                    }
                }
                break;
            case 0xEF:
                if (incoming.data[3] == 0xFE) {
                    // navigation data received
                    char **fields[] = {
                        &navigation.title,
                        &navigation.duration,
                        &navigation.distance,
                        &navigation.eta,
                        &navigation.directions
                    };

                    for (int i = 0; i < 5; i++) {
                        free(*fields[i]);
                        *fields[i] = NULL;
                    }

                    if (incoming.data[5] == 0x00) {
                        navigation.active = false;
                        navigation.eta = strdup("Navigation");
                        navigation.duration = strdup("Inactive");
                        navigation.distance = strdup("");
                        navigation.title = strdup("Chronos");
                        navigation.directions = strdup("Start navigation on Google maps");
                    } else if (incoming.data[5] == 0xFF) {
                        navigation.active = true;
                        navigation.eta = strdup("Navigation");
                        navigation.duration = strdup("Disabled");
                        navigation.distance = strdup("");
                        navigation.title = strdup("Chronos");
                        navigation.directions = strdup("Check Chronos app settings");
                        navigation.has_icon = false;
                        navigation.is_navigation = false;
                    } else if (incoming.data[5] == 0x80) {
                        navigation.active = true;
                        navigation.has_icon = incoming.data[6] == 1;
                        navigation.is_navigation = incoming.data[7] == 1;
                        navigation.icon_crc = (uint32_t)(incoming.data[8] << 24) | (uint32_t)(incoming.data[9] << 16) | (uint32_t)(
                                                  incoming.data[10] << 8) | (uint32_t)(incoming.data[11]);

                        const uint8_t *ptr = incoming.data + 12;
                        const uint8_t *end = incoming.data + len;

                        for (int i = 0; i < 5 && ptr < end; i++) {
                            size_t field_len = 0;
                            const uint8_t *sep = memchr(ptr, '\0', end - ptr);

                            field_len = sep ? (size_t)(sep - ptr) : (size_t)(end - ptr);
                            *fields[i] = strndup((const char *)ptr, field_len);

                            ptr += field_len + (sep ? 1 : 0);  // Move past separator
                        }
                    }

                    if (configuration_callback != NULL) {
                        configuration_callback(CH_CONFIG_NAV_DATA, navigation.active ? 1 : 0, 0);
                    }
                }
                break;
            default:
                break;
        }
    } else if (incoming.data[0] == 0xEA) {
        if (incoming.data[4] == 0x7E) {
            switch (incoming.data[5]) {
                case 0x01:
                    // weather city name
                    // incoming.data[7:len]
                    char city[512] = {0};

                    for (int i = 7; i < len; i++) {
                        strncat(city, (char *)&incoming.data[i], 1);
                    }

                    if (weather_info.city) {
                        free(weather_info.city);
                    }

                    weather_info.city = strdup(city);

                    if (configuration_callback != NULL) {
                        configuration_callback(CH_CONFIG_WEATHER, 0, 1);
                    }

                    break;
                case 0x02:
                    // hourly weather forecsat
                    int size = incoming.data[6]; // data size
                    int hour = incoming.data[7]; // current hour
                    struct tm tm_info = ble_chronos_get_time_struct();
                    for (int z = 0; z < size; z++) {

                        int sign = (incoming.data[8 + (6 * z)] & 1) ? -1 : 1;

                        int icon = incoming.data[8 + (6 * z)] >> 4; // See WEATHER ICONS
                        int temp = ((int)incoming.data[9 + (6 * z)]) * sign;

                        hourly_forecast[hour + z].day = tm_info.tm_yday;
                        hourly_forecast[hour + z].hour = hour + z;
                        hourly_forecast[hour + z].wind = (incoming.data[10 + (6 * z)] * 256) + incoming.data[11 + (6 * z)];
                        hourly_forecast[hour + z].humidity = incoming.data[12 + (6 * z)];
                        hourly_forecast[hour + z].uv = incoming.data[13 + (6 * z)];
                        hourly_forecast[hour + z].icon = icon;
                        hourly_forecast[hour + z].temp = temp;
                    }
                    break;
            }
        }
    }
}

/* END DATA FROM CHRONOS APP FUNCTIONS */