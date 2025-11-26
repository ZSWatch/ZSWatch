#pragma once

#include "ble/ble_comm.h"
#include "ble/ble_transport.h"
#include "events/ble_event.h"
#include "events/music_event.h"
#include "managers/zsw_power_manager.h"

#define MAX_GB_PACKET_LENGTH                    2000

void ble_gadgetbridge_input(const uint8_t *const data, uint16_t len);

void ble_gadgetbridge_send_version_info(void);

void ble_gadgetbridge_send_activity_state(zsw_power_manager_state_t activity_state);
