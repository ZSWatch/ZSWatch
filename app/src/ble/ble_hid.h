#pragma once

#include <zephyr/kernel.h>

void ble_hid_init(void);

int ble_hid_next(void);

int ble_hid_previous(void);
