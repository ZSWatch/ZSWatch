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
#include "ble_chronos.h"

LOG_MODULE_REGISTER(ble_chronos, CONFIG_ZSW_BLE_LOG_LEVEL);

static int parse_data(char *data, int len);
static void parse_time(char *data);
static void parse_time_zone(char *offset);
static void music_control_event_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(android_music_control_lis_chronos, music_control_event_callback);

static chronos_data_t incoming; // variable to store incoming data

static chronos_notification_t notifications[NOTIF_SIZE];
static int notificationIndex;

static chronos_weather_t weather[WEATHER_SIZE];
static int weatherSize;

static chronos_navigation_t navigation;


static notification_callback_t notification_callback = NULL;

void register_notification_callback(notification_callback_t callback)
{
    notification_callback = callback;
}

static void send_ble_data_event(struct ble_data_event *evt)
{
    zbus_chan_pub(&ble_comm_data_chan, evt, K_MSEC(250));
}

static void music_control_event_callback(const struct zbus_channel *chan)
{
    const struct music_event *event = zbus_chan_const_msg(chan);

    uint8_t buf[50];
    int msg_len = 0;

    if (msg_len > 0) {
        ble_comm_send(buf, msg_len);
    }
}

void parse_time(char *start_time)
{
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_SET_TIME;
    send_ble_data_event(&cb);
}

void parse_time_zone(char *offset)
{
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_SET_TIME;

    send_ble_data_event(&cb);
}

static int parse_notify(char *data, int len)
{
    struct ble_data_event cb;
    memset(&cb, 0, sizeof(cb));

    cb.data.type = BLE_COMM_DATA_TYPE_NOTIFY;

    send_ble_data_event(&cb);

    return 0;
}

static int parse_notify_delete(char *data, int len)
{
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_NOTIFY_REMOVE;

    send_ble_data_event(&cb);

    return 0;
}

static int parse_weather(char *data, int len)
{
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_WEATHER;

    send_ble_data_event(&cb);

    return 0;
}

static int parse_musicinfo(char *data, int len)
{
    char *temp_value;
    int temp_len;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_MUSIC_INFO;

    send_ble_data_event(&cb);

    return 0;
}

static int parse_musicstate(char *data, int len)
{
    char *temp_value;
    int temp_len;
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_MUSIC_STATE;

    send_ble_data_event(&cb);

    return 0;
}

int parse_data(char *data, int len)
{
    on_receive_data(data, len);

    return 0;
}

void ble_chronos_input(const uint8_t *const data, uint16_t len)
{
    // LOG_HEXDUMP_DBG(data, len, "RX");
    LOG_INF("Data received, length %d", len);
    parse_data((char *)data, len);
}

void send_command(uint8_t *command, size_t length)
{

    LOG_HEXDUMP_DBG(command, length, "Chronos TX");
    LOG_INF("Data sent, length %d", length);

    ble_comm_send(command, length);
}

void music_control(chronos_control_t command)
{
    uint8_t music_cmd[] = {0xAB, 0x00, 0x04, 0xFF, (uint8_t)(command >> 8), 0x80, (uint8_t)(command)};
    send_command(music_cmd, 7);
}

void set_volume(uint8_t level)
{
    uint8_t volume_cmd[] = {0xAB, 0x00, 0x05, 0xFF, 0x99, 0x80, 0xA0, level};
    send_command(volume_cmd, 8);
}

void capture_photo()
{
    uint8_t capture_cmd[] = {0xAB, 0x00, 0x04, 0xFF, 0x79, 0x80, 0x01};
    send_command(capture_cmd, 7);
}

void find_phone(bool state)
{
    uint8_t c = state ? 0x01 : 0x00;
    uint8_t find_cmd[] = {0xAB, 0x00, 0x04, 0xFF, 0x7D, 0x80, c};
    send_command(find_cmd, 7);
}

void send_info()
{
    uint8_t info_cmd[] = {0xab, 0x00, 0x11, 0xff, 0x92, 0xc0, CHRONOSESP_VERSION_MAJOR, (CHRONOSESP_VERSION_MINOR * 10 + CHRONOSESP_VERSION_PATCH), 0x00, 0xfb, 0x1e, 0x40, 0xc0, 0x0e, 0x32, 0x28, 0x00, 0xe2, 0x07, 0x80};
    send_command(info_cmd, 20);
}

void send_battery(uint8_t level, bool charging)
{
    uint8_t c = charging ? 0x01 : 0x00;
    uint8_t bat_cmd[] = {0xAB, 0x00, 0x05, 0xFF, 0x91, 0x80, c, level};
    send_command(bat_cmd, 8);
}

void set_notify_battery(bool state)
{
    uint8_t s = state ? 0x01 : 0x00;
    uint8_t bat_cmd[] = {0xAB, 0x00, 0x04, 0xFE, 0x91, 0x80, s}; // custom command AB..FE
    send_command(bat_cmd, 7);
}

const char *app_name(int id)
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


chronos_notification_t *get_notification(int index)
{
    return &notifications[index % NOTIF_SIZE];
}

chronos_navigation_t *get_navigation()
{
    return &navigation;
}

/* DATA FROM CHRONOS APP FUNCTIONS */
// write on RX (6e400002-b5a3-f393-e0a9-e50e24dcca9e)

// Chronos received commands (data[0] is 0xAB or 0xEA or <= 0x19) on RX characteristic

// this function assembles data packets that are split over multiple transmissions
// when data on RX (6e400002-b5a3-f393-e0a9-e50e24dcca9e) is written
void on_receive_data(uint8_t *data, int len)
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
                data_received();
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
                data_received();
            } else {
                // data is still being assembled
                // LOG_INF("Incomplete");
            }
        } else {
            LOG_INF("Not Chronos data");
        }
    }
}

void data_received()
{
    int len = incoming.length;

    LOG_INF("Complete data length %d", len);
    LOG_HEXDUMP_DBG(incoming.data, len, "Chronos RX");

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

                for (int i = 0; i < NOTIF_SIZE; i++) {

                    LOG_INF("Notification index %d available %s", i,  notifications[i].available ? "true" : "false");

                    if (notifications[i].available) {
                        LOG_INF("Icon 0x%02X %s, Time %s: %s", notifications[i].icon, notifications[i].app, notifications[i].time,
                                notifications[i].message);
                    }
                }
                break;
            case 0x72: {
                int icon = incoming.data[6]; // See ALERT ICONS
                int state = incoming.data[7];
                char message[512] = {0};

                for (int i = 8; i < len; i++) {
                    strncat(message, (char *)&incoming.data[i], 1);
                }

                LOG_INF("Notification id: %02X, state: %d", icon, state);

                if (icon == 0x01) {
                    // caller command
                    // message contains the caller details (name or number)
                } else if (icon == 0x02) {
                    // cancel caller command
                    //
                } else if (state == 0x02) {


                    // Free old memory before assigning new values
                    notificationIndex++;
                    if (notifications[notificationIndex % NOTIF_SIZE].time) {
                        free(notifications[notificationIndex % NOTIF_SIZE].time);
                    }
                    if (notifications[notificationIndex % NOTIF_SIZE].message) {
                        free(notifications[notificationIndex % NOTIF_SIZE].message);
                    }
                    notifications[notificationIndex % NOTIF_SIZE].available = true;
                    notifications[notificationIndex % NOTIF_SIZE].icon = icon;
                    notifications[notificationIndex % NOTIF_SIZE].app = app_name(icon);
                    notifications[notificationIndex % NOTIF_SIZE].time = strdup("12:45");
                    notifications[notificationIndex % NOTIF_SIZE].message = strdup(message);

                    if (notification_callback != NULL) {
                        notification_callback(&notifications[notificationIndex % NOTIF_SIZE]);
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
                // incoming.data[6]; 1->ACTIVE 0->INACTIVE
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
                weatherSize = 0;
                for (int k = 0; k < (len - 6) / 2; k++) {
                    int sign = (incoming.data[(k * 2) + 6] & 1) ? -1 : 1;

                    int icon = incoming.data[(k * 2) + 6] >> 4; // icon id; See WEATHER ICONS
                    int temp = ((int)incoming.data[(k * 2) + 7]) * sign;

                    int dy = k; //int dy = this->getDayofWeek() + k;
                    weather[k].day = dy % 7;
                    weather[k].icon = icon;
                    weather[k].temp = temp;
                    weatherSize++;
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
                break;
            case 0x91:
                if (incoming.data[3] == 0xFE) {
                    // custom app command
                    // status of the phone battery
                    // incoming.data[6]; 1->Charging 0->Not Charging
                    // incoming.data[7]; phone battery level %
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
                LOG_INF("Set time");
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
                    // touch.state = incoming.data[5] == 1;
                    // touch.x = uint32_t(incoming.data[6] << 8) | uint32_t(incoming.data[7]);
                    // touch.y = uint32_t(incoming.data[8] << 8) | uint32_t(incoming.data[9]);
                }
                break;
            case 0xCA:
                if (incoming.data[3] == 0xFE) {
                    // Chronos app version info
                    // appCode; (incoming.data[6] * 256) + incoming.data[7];
                    // appVersion; link incoming.data[8:len]
                }
                break;
            // case 0xCC:
            //     if (incoming.data[3] == 0xFE) {
            //         setChunkedTransfer(incoming.data[5] != 0x00);
            //     }
            //     break;
            // case 0xEE:
            //     if (incoming.data[3] == 0xFE) {
            //         // navigation icon data received
            //         uint8_t pos = incoming.data[6];
            //         uint32_t crc = uint32_t(incoming.data[7] << 24) | uint32_t(incoming.data[8] << 16) | uint32_t(
            //                            incoming.data[9] << 8) | uint32_t(incoming.data[10]);
            //         for (int i = 0; i < 96; i++) {
            //             navigation.icon[i + (96 * pos)] = incoming.data[11 + i];
            //         }

            //         if (configurationReceivedCallback != nullptr) {
            //             configurationReceivedCallback(CF_NAV_ICON, pos, crc);
            //         }
            //     }
            //     break;
            // case 0xEF:
            //     if (incoming.data[3] == 0xFE) {
            //         // navigation data received
            //         if (incoming.data[5] == 0x00) {
            //             navigation.active = false;
            //         } else if (incoming.data[5] == 0xFF) {
            //             navigation.active = true;
            //             navigation.title = "Disabled";
            //             navigation.duration = "";
            //             navigation.distance = "";
            //             navigation.eta = "";
            //             navigation.directions = "Check app settings";
            //             navigation.has_icon = false;
            //             navigation.is_navigation = false;
            //         } else if (incoming.data[5] == 0x80) {
            //             navigation.active = true;
            //             navigation.has_icon = incoming.data[6] == 1;
            //             navigation.is_navigation = incoming.data[7] == 1;
            //             navigation.icon_crc = uint32_t(incoming.data[8] << 24) | uint32_t(incoming.data[9] << 16) | uint32_t(
            //                                       incoming.data[10] << 8) | uint32_t(incoming.data[11]);

            //             int i = 12;
            //             navigation.title = "";
            //             while (incoming.data[i] != 0 && i < len) {
            //                 navigation.title += char(incoming.data[i]);
            //                 i++;
            //             }
            //             i++;

            //             navigation.duration = "";
            //             while (incoming.data[i] != 0 && i < len) {
            //                 navigation.duration += char(incoming.data[i]);
            //                 i++;
            //             }
            //             i++;

            //             navigation.distance = "";
            //             while (incoming.data[i] != 0 && i < len) {
            //                 navigation.distance += char(incoming.data[i]);
            //                 i++;
            //             }
            //             i++;

            //             navigation.eta = "";
            //             while (incoming.data[i] != 0 && i < len) {
            //                 navigation.eta += char(incoming.data[i]);
            //                 i++;
            //             }
            //             i++;

            //             navigation.directions = "";
            //             while (incoming.data[i] != 0 && i < len) {
            //                 navigation.directions += char(incoming.data[i]);
            //                 i++;
            //             }
            //             i++;
            //         }
            //         // if (configurationReceivedCallback != nullptr) {
            //         //     configurationReceivedCallback(CF_NAV_DATA, navigation.active ? 1 : 0, 0);
            //         // }
            //     }
            //     break;
            default:
                break;
        }
    } else if (incoming.data[0] == 0xEA) {
        if (incoming.data[4] == 0x7E) {
            switch (incoming.data[5]) {
                case 0x01:
                    // weather city name
                    // incoming.data[7:len]
                    break;
                case 0x02:
                    // hourly weather forecsat
                    int size = incoming.data[6]; // data size
                    int hour = incoming.data[7]; // current hour
                    for (int z = 0; z < size; z++) {

                        int sign = (incoming.data[8 + (6 * z)] & 1) ? -1 : 1;

                        int icon = incoming.data[8 + (6 * z)] >> 4; // See WEATHER ICONS
                        int temp = ((int)incoming.data[9 + (6 * z)]) * sign;
                        // windSpeed km/h; (incoming.data[10 + (6 * z)] * 256) + incoming.data[11 + (6 * z)];
                        // humidity %; incoming.data[12 + (6 * z)];
                        // uv index; incoming.data[13 + (6 * z)];
                    }
                    break;
            }
        }
    }
}

/* END DATA FROM CHRONOS APP FUNCTIONS */