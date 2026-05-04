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

/**
 * @brief Send a voice memo "new recording" notification to the companion app.
 *
 * @param filename     Recording filename (without extension)
 * @param duration_ms  Recording duration in milliseconds
 * @param size_bytes   File size in bytes
 * @param timestamp    Unix epoch timestamp
 */
void ble_gadgetbridge_send_voice_memo_new(const char *filename, uint32_t duration_ms,
                                          uint32_t size_bytes, uint32_t timestamp);

/** Send an undo command for the last processed voice memo to the companion app. */
void ble_gadgetbridge_send_voice_memo_undo(const char *filename);

/**
 * @brief Notify companion app that a question audio file is ready for download.
 *
 * @param session_id   Session identifier
 * @param path         On-device path to the WAV file (e.g. "/lfs1/chat/question.wav")
 * @param duration_ms  Recording duration in milliseconds
 * @param size_bytes   File size in bytes (including WAV header)
 * @param sample_rate  Sample rate in Hz
 * @param codec        Codec identifier string (e.g. "opus_zsw", "pcm_wav")
 */
void ble_gadgetbridge_send_chat_question_ready(uint32_t session_id, const char *path,
                                               uint32_t duration_ms, uint32_t size_bytes,
                                               uint32_t sample_rate, const char *codec);

/** Cancel the current chat session. */
void ble_gadgetbridge_send_chat_cancel(uint32_t session_id);

/** Notify companion app that reply audio playback has finished. */
void ble_gadgetbridge_send_chat_playback_done(uint32_t session_id);
