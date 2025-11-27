#pragma once

#include <stdint.h>

#include "ble/ble_comm.h"
#include "ble/ble_transport.h"
#include "events/ble_event.h"
#include "events/music_event.h"
#include "managers/zsw_power_manager.h"
#include "sensors/zsw_imu.h"

#define MAX_GB_PACKET_LENGTH                    2000

void ble_gadgetbridge_input(const uint8_t *const data, uint16_t len);

void ble_gadgetbridge_send_version_info(void);

/**
 * @brief Send a notification action to Gadgetbridge.
 *
 * @param id Notification ID to act on
 * @param action Action to perform
 */
void ble_gadgetbridge_send_notification_action(uint32_t id, ble_comm_notify_action_t action);

/**
 * @brief Send activity data to Gadgetbridge.
 *
 * Maps step_activity and power_state to Gadgetbridge activities:
 *
 * @param heart_rate Heart rate in BPM (0 if not available)
 * @param steps Step count
 * @param step_activity IMU step activity
 * @param power_state Power manager state
 */
void ble_gadgetbridge_send_activity_data(uint16_t heart_rate, uint32_t steps,
                                         zsw_imu_data_step_activity_t step_activity,
                                         zsw_power_manager_state_t power_state);
