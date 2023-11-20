#include <zephyr/zbus/zbus.h>

#include "ble_event.h"

ZBUS_CHAN_DEFINE(ble_comm_data_chan,
                 struct ble_data_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(ble_data_subscriber, main_ble_comm_lis, music_app_ble_comm_lis, watchface_ble_comm_lis),
                 ZBUS_MSG_INIT()
                );

ZBUS_CHAN_DEFINE(ble_comm_connected_chan,
                 struct ble_connect_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(main_ble_comm_lis, music_app_ble_comm_lis, watchface_ble_comm_lis),
                 ZBUS_MSG_INIT()
                );

ZBUS_CHAN_DEFINE(ble_comm_disconnected_chan,
                 struct ble_disconnect_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(main_ble_comm_lis, music_app_ble_comm_lis, watchface_ble_comm_lis),
                 ZBUS_MSG_INIT()
                );