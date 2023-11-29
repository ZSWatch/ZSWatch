#include "ble_data_event.h"
#include <zephyr/zbus/zbus.h>

ZBUS_CHAN_DEFINE(ble_comm_data_chan,
                 struct ble_data_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(main_ble_comm_lis, music_app_ble_comm_lis, watchface_ble_comm_lis, trivia_app_ble_comm_lis),
                 ZBUS_MSG_INIT()
                );