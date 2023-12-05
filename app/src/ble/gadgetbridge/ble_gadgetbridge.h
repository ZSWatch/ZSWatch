#pragma once

#include "ble/ble_comm.h"
#include "ble/ble_transport.h"
#include "events/ble_event.h"
#include "events/music_event.h"

#define MAX_GB_PACKET_LENGTH                    1000

/** @brief
 *  @param key
 *  @param data
 *  @param value_lem
 *  @return
*/
char *extract_value_str(char *key, char *data, int *value_len);

/** @brief
 *  @param data
 *  @param len
 *  @return
*/
int parse_data(char *data, int len);

/** @brief
 *  @param data
 *  @return
*/
void parse_time(char *data);

/** @brief
 *  @param offset
 *  @return
*/
void parse_time_zone(char *offset);

/** @brief
 *  @param data
 *  @param len
*/
void parse_remote_control(char *data, int len);

/** @brief
 *  @param data
 *  @param len
*/
void ble_gadgetbridge_input(const uint8_t *const data, uint16_t len);