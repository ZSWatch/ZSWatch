#pragma once

#include <stdbool.h>

/* Existing Boolean Find protocol, bounded on the watch independently of phone timers. */
int ble_find_device_set(bool enabled);
void ble_find_device_disconnected(void);
