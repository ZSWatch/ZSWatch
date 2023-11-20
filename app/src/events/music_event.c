#include "music_event.h"
#include <zephyr/zbus/zbus.h>

ZBUS_CHAN_DEFINE(music_control_data_chan,
                 struct music_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(android_music_control_lis),
                 ZBUS_MSG_INIT()
                );
