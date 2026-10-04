#pragma once
struct zbus_channel { const void *msg; };
static inline const void *zbus_chan_const_msg(const struct zbus_channel *chan) { return chan->msg; }
#define ZBUS_LISTENER_DEFINE(...)
#define ZBUS_CHAN_DECLARE(...)
#define ZBUS_CHAN_ADD_OBS(...)
