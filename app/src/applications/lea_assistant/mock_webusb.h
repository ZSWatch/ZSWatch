#pragma once

#include <zephyr/net/buf.h>

/**
 * @brief we don't use USB or the webversion of the code example, so mock some functions to keep it close to the source.
 */
int webusb_transmit(struct net_buf *tx_net_buf);
