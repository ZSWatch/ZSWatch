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
#include "zsw_ble_chronos.h"

LOG_MODULE_REGISTER(ble_chronos, CONFIG_ZSW_BLE_LOG_LEVEL);

static int parse_data(char *data, int len);
static void parse_time(char *data);
static void parse_time_zone(char *offset);
static void music_control_event_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(android_music_control_lis_chronos, music_control_event_callback);

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
    ble_comm_cb_data_t cb;
    memset(&cb, 0, sizeof(cb));

    cb.type = BLE_COMM_DATA_TYPE_NOTIFY;

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
    return 0;
}

void zsw_ble_chronos_input(const uint8_t *const data, uint16_t len)
{
    LOG_HEXDUMP_DBG(data, len, "RX");
    parse_data((char *)data, len);
}


/* DATA TO CHRONOS APP FUNCTIONS */
// notify to TX (6e400003-b5a3-f393-e0a9-e50e24dcca9e) characteristic

// send command to app
void sendCommand(uint8_t *command, size_t length)
{
	// here implement notify to TX (6e400003-b5a3-f393-e0a9-e50e24dcca9e) characteristic
}

// music and volume control; See h file for commands
void musicControl(uint16_t command)
{
	uint8_t musicCmd[] = {0xAB, 0x00, 0x04, 0xFF, (uint8_t)(command >> 8), 0x80, (uint8_t)(command)};
	sendCommand(musicCmd, 7);
}
/*!
	@brief  send a command to set the volume level
	@param  level
			volume level (0 - 100)
*/
void setVolume(uint8_t level)
{
	uint8_t volumeCmd[] = {0xAB, 0x00, 0x05, 0xFF, 0x99, 0x80, 0xA0, level};
	sendCommand(volumeCmd, 8);
}

/*!
	@brief  send capture photo command to the app
*/
void capturePhoto()
{
	uint8_t captureCmd[] = {0xAB, 0x00, 0x04, 0xFF, 0x79, 0x80, 0x01};
	sendCommand(captureCmd, 7);
}

/*!
	@brief  send a command to find the phone
	@param  state
			enable or disable state
*/
void findPhone(bool state)
{
	uint8_t c = state ? 0x01 : 0x00;
	uint8_t findCmd[] = {0xAB, 0x00, 0x04, 0xFF, 0x7D, 0x80, c};
	sendCommand(findCmd, 7);
}

/*!
	@brief  send the info proprerties to the app
            should happen after every connect/reconnect
*/
void sendInfo()
{
	uint8_t infoCmd[] = {0xab, 0x00, 0x11, 0xff, 0x92, 0xc0, 0x01, 0x28, 0x00, 0xfb, 0x1e, 0x40, 0xc0, 0x0e, 0x32, 0x28, 0x00, 0xe2, 0x07, 0x80};
	sendCommand(infoCmd, 20);
}

/*!
	@brief  send watch battery level
            after reconnect and every time the level changes
*/
void sendBattery(uint8_t level, bool charging)
{
	uint8_t c = charging ? 0x01 : 0x00;
	uint8_t batCmd[] = {0xAB, 0x00, 0x05, 0xFF, 0x91, 0x80, c, level};
	sendCommand(batCmd, 8);
}

/*!
	@brief  enable/disable phone battery notifications

*/
void setNotifyBattery(bool state)
{
	uint8_t s = state ? 0x01 : 0x00;
	uint8_t batRq[] = {0xAB, 0x00, 0x04, 0xFE, 0x91, 0x80, s}; // custom command AB..FE
	sendCommand(batRq, 7);
}
/* END DATA TO CHRONOS APP FUNCTIONS */


/* DATA FROM CHRONOS APP FUNCTIONS */
// write on RX (6e400002-b5a3-f393-e0a9-e50e24dcca9e)

// Chronos received commands (data[0] is 0xAB or 0xEA or <= 0x19) on RX characteristic

// this function assembles data packets that are split over multiple transmissions
// when data on RX (6e400002-b5a3-f393-e0a9-e50e24dcca9e) is written
void onWrite(uint8_t *pData, int len)
{
    if (len > 0)
    {
        
        // Chronos app sends data starting with either AB or EA for the first packet and FE or FF at index 3
        if ((pData[0] == 0xAB || pData[0] == 0xEA) && (pData[3] == 0xFE || pData[3] == 0xFF))
        {
            // start of data, assign length from packet
            incomingData.length = pData[1] * 256 + pData[2] + 3;
            // copy data to incomingBuffer
            for (int i = 0; i < len; i++)
            {
                incomingData.data[i] = pData[i];
            }

            if (incomingData.length <= len)
            {
                // complete packet assembled
                dataReceived();
            }
            else
            {
                // data is still being assembled
                // printf("Incomplete\n");
            }
        }
        else
        {
            // subsequent packets start with 0 (max anticipated is 25 -> 0x19)
            int j = 20 + (pData[0] * 19); // data packet position
            // copy data to incomingBuffer
            for (int i = 0; i < len; i++)
            {
                incomingData.data[j + i] = pData[i + 1];
            }

            if (incomingData.length <= len + j - 1)
            {
                // complete packet assembled
                dataReceived();
            }
            else
            {
                // data is still being assembled
                // printf("Incomplete\n");
            }
        }
    }
}

void dataReceived()
{
    int len = incomingData.length;

    if (incomingData.data[0] == 0xAB)
    {
        switch (incomingData.data[4])
        {
            case 0x23:
                // request to reset the watch
                break;
            case 0x53:
                // uint8_t hour = incomingData.data[7];
                // uint8_t minute = incomingData.data[8];
                // uint8_t hour2 = incomingData.data[9];
                // uint8_t minute2 = incomingData.data[10];
                // bool enabled = incomingData.data[6];
                // uint8_t interval = incomingData.data[11]; // interval in minutes
                break;
            case 0x71:
                // find watch
                // the watch should vibrate and/or ring for a few seconds
                break;
            case 0x72:
            {
                int icon = incomingData.data[6]; // See ALERT ICONS
                int state = incomingData.data[7];
                char message[512] = {0};

                for (int i = 8; i < len; i++)
                {
                    strncat(message, (char*)&incomingData.data[i], 1);
                }

                if (icon == 0x01)
                {
                    // caller command
                    // message contains the caller details (name or number)
                }
                else if (icon == 0x02)
                {
                    // cancel caller command
                    // 
                }
                else if (state == 0x02)
                {
                    // notification received (it only contains the icon id and message)
                    // see for icon ids
                    // no actions available
                }
            }
            break;
            case 0x73:
            {
                // alarms
                // uint8_t index = incomingData.data[6]; [0-7]
                // bool enabled = incomingData.data[7];
                // uint8_t hour = incomingData.data[8];
                // uint8_t minute = incomingData.data[9];
                // uint8_t repeat = incomingData.data[10]; //
                // repeat values 0x80->Once or 0x01-0x7F -> Specify days with bits. Order [null,Sun,Sat,Fri,Thu,Wed,Tue,Mon]
                // 0x80 [1000 0000] -> Once (one time alarm)
                // 0x7F [0111 1111] -> everyday (null,Sun,Sat,Fri,Thu,Wed,Tue,Mon)
                // 0x1F [0001 1111] -> Monday - Friday
                // 0x43 [0100 0011] -> Custom (Sun, Tue, Mon)
            }
            break;
            case 0x74:
                // user details and settings from the app
                // uint8_t stepLength = incomingData.data[6]; // cm
                // uint8_t age = incomingData.data[7]; //yrs
                // uint8_t height = incomingData.data[8]; // cm
                // uint8_t weight = incomingData.data[9]; // kg
                // uint8_t unit = incomingData.data[10]; 0->Imperial 1->Metric 
                // uint8_t targetSteps = incomingData.data[11] * 1000;
                // uint8_t tempUnit = incomingData.data[12]; 0->C 1->F
                
                break;
            case 0x75:
                // sedentary reminder
                // bool enabled = incomingData.data[6];
                // uint8_t hour = incomingData.data[7]; // start
                // uint8_t minute = incomingData.data[8];
                // uint8_t hour2 = incomingData.data[9]; // end
                // uint8_t minute2 = incomingData.data[10];
                // uint8_t interval = incomingData.data[11]; // interval in minutes
                break;
            case 0x76:
                // quiet hours settings
                // bool enabled = incomingData.data[6];
                // uint8_t hour = incomingData.data[7];
                // uint8_t minute = incomingData.data[8];
                // uint8_t hour2 = incomingData.data[9];
                // uint8_t minute2 = incomingData.data[10];
                break;
            case 0x77:
                // raise to wake settings
                // incomingData.data[6]; 1->ON 0->OFF
                
                break;
            case 0x78:
                // health hourly settings (used to trigger health measurements every hour)
                // incomingData.data[6]; 1->ON 0->OFF
                break;
            case 0x79:
                // remote camera function
                // this tells the watch that the camera is active on the app and ready to receive capture command
                // incomingData.data[6]; 1->ACTIVE 0->INACTIVE
                break;
            case 0x7B:
                // change watch language if supported
                // incomingData.data[6] is the language id; See LANGUAGE ID
                break;
            case 0x7C:
                // 24 hour clock mode
                // incomingData.data[6]; 1->ON 0->OFF
                break;
            case 0x7E:
                // weather data received
                // contains daily forecast 
                for (int k = 0; k < (len - 6) / 2; k++)
                {
                    int sign = (incomingData.data[(k * 2) + 6] & 1) ? -1 : 1;

                    int icon = incomingData.data[(k * 2) + 6] >> 4; // icon id; See WEATHER ICONS
                    int temp = ((int)incomingData.data[(k * 2) + 7]) * sign;

                }
                break;
            case 0x7F:
                // sleep settings
				// bool enabled = incomingData.data[6];
                // uint8_t hour = incomingData.data[7];
				// uint8_t minute = incomingData.data[8];
				// uint8_t hour2 = incomingData.data[9];
				// uint8_t minute2 = incomingData.data[10];
                break;
            case 0x88:
                // weather data received
                // contains high and low temperature forecast
                for (int k = 0; k < (len - 6) / 2; k++)
                {
                    int signH = (incomingData.data[(k * 2) + 6] >> 7 & 1) ? -1 : 1;
                    int tempH = ((int)incomingData.data[(k * 2) + 6] & 0x7F) * signH;

                    int signL = (incomingData.data[(k * 2) + 7] >> 7 & 1) ? -1 : 1;
                    int tempL = ((int)incomingData.data[(k * 2) + 7] & 0x7F) * signL;
                }
                break;
            case 0x91:
                if (incomingData.data[3] == 0xFE){
                    // custom app command
                    // status of the phone battery
                    // incomingData.data[6]; 1->Charging 0->Not Charging
                    // incomingData.data[7]; phone battery level %
                }
                
                
                break;
            case 0x93:
                // time received (update watch time immediately)
                // year; incomingData.data[7] * 256 + incomingData.data[8]
                // month; incomingData.data[9]
                // day; incomingData.data[10]
                // hour; incomingData.data[11]
                // minute; incomingData.data[12]
                // seconds; incomingData.data[13]
                break;
            case 0x9C:
                // watchface font style and color settings
                // uint32_t colorRGB = ((uint32_t)incomingData.data[5] << 16) | ((uint32_t)incomingData.data[6] << 8) | (uint32_t)incomingData.data[7]
                // style incomingData.data[8] [0-2]
                // position incomingData.data[9] 0->Top, 1->Center, 2->Bottom

                break;
            case 0xA8:
                if (incomingData.data[3] == 0xFE){
                    // end of qr data transmission (Chronos v3.7.0+)
                    // incomingData.data[5]; // number of links received
                }
                if (incomingData.data[3] == 0xFF){
                    // qr links with index
                    // incomingData.data[5]; // index of the current link
                    // link incomingData.data[6:len]
                }
                break;
            case 0xBF:
                if (incomingData.data[3] == 0xFE){
                    // remote touch data (Chronos v3.7.0+)
                    // touch.state = incomingData.data[5] == 1;
                    // touch.x = uint32_t(incomingData.data[6] << 8) | uint32_t(incomingData.data[7]);
                    // touch.y = uint32_t(incomingData.data[8] << 8) | uint32_t(incomingData.data[9]);
                }
                break;
            case 0xCA:
                if (incomingData.data[3] == 0xFE)
                {
                    // Chronos app version info
                    // appCode; (incomingData.data[6] * 256) + incomingData.data[7];
                    // appVersion; link incomingData.data[8:len]
                }
                break;
            default:
                break;
        }
    }
    else if (incomingData.data[0] == 0xEA)
    {
        if (incomingData.data[4] == 0x7E)
		{
            switch (incomingData.data[5])
			{
                case 0x01:
                    // weather city name
                    // incomingData.data[7:len]
                    break;
                case 0x02:
                    // hourly weather forecsat
                    int size = incomingData.data[6]; // data size
                    int hour = incomingData.data[7]; // current hour
                    for (int z = 0; z < size; z++)
                    {
                        
                        int sign = (incomingData.data[8 + (6 * z)] & 1) ? -1 : 1;

                        int icon = incomingData.data[8 + (6 * z)] >> 4; // See WEATHER ICONS
                        int temp = ((int)incomingData.data[9 + (6 * z)]) * sign;
                        // windSpeed km/h; (incomingData.data[10 + (6 * z)] * 256) + incomingData.data[11 + (6 * z)];
                        // humidity %; incomingData.data[12 + (6 * z)];
                        // uv index; incomingData.data[13 + (6 * z)];
                    }
                    break;
            }
        }
    }
}

/* END DATA FROM CHRONOS APP FUNCTIONS */
