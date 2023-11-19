#include <zephyr/zbus/zbus.h>

#include "magnetometer_event.h"

ZBUS_CHAN_DEFINE(magnetometer_data_chan,
                 struct magnetometer_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );