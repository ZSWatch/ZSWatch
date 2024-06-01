#pragma once

#include "ble/ble_comm.h"
#include "ble/ble_transport.h"
#include "events/ble_event.h"
#include "events/music_event.h"



#define DATA_SIZE 512

#define MUSIC_PLAY 0x9D00
#define MUSIC_PAUSE 0x9D01
#define MUSIC_PREVIOUS 0x9D02
#define MUSIC_NEXT 0x9D03
#define MUSIC_TOGGLE 0x9900

#define VOLUME_UP 0x99A1
#define VOLUME_DOWN 0x99A2
#define VOLUME_MUTE 0x99A3


// struct to store assembled incoming data from chronos
struct ChronosData
{
    int length;
    uint8_t data[DATA_SIZE];
};
ChronosData incomingData; // variable to store incoming data

void zsw_ble_chronos_input(const uint8_t *const data, uint16_t len);

// Function prototypes
// TX
void sendCommand(uint8_t *command, size_t length);
void musicControl(uint16_t command);
void setVolume(uint8_t level);
void capturePhoto();
void findPhone(bool state);
void sendInfo();
void sendBattery(uint8_t level, bool charging);
void setNotifyBattery(bool state);

// RX
void onWrite(uint8_t *pData, int len);
void dataReceived();

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