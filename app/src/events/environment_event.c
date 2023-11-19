#include <zephyr/zbus/zbus.h>

#include "environment_event.h"

ZBUS_CHAN_DEFINE(environment_data_chan,
                 struct environment_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );