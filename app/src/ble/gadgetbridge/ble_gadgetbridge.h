#pragma once

#include "ble/ble_comm.h"
#include "ble/ble_transport.h"
#include "events/ble_event.h"
#include "events/music_event.h"

#define MAX_GB_PACKET_LENGTH                    1000

void ble_gadgetbridge_input(const uint8_t *const data, uint16_t len);