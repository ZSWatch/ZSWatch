
#pragma once

#include <zephyr/zbus/zbus.h>

#include "events/periodic_event.h"

int zsw_periodic_chan_add_obs(const struct zbus_channel *chan, const struct zbus_observer *obs);
int zsw_periodic_chan_rm_obs(const struct zbus_channel *chan, const struct zbus_observer *obs);
