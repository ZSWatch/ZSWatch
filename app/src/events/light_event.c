#include <zephyr/zbus/zbus.h>

#include "light_event.h"

ZBUS_CHAN_DEFINE(light_data_chan,
                 struct light_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );