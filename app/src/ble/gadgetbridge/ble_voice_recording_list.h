#pragma once

#include "managers/zsw_recording_manager_store.h"

/* The phone downloads and then deletes recordings, so one short page is enough.
 * "more" tells it to request the list again after deleting. */
#define BLE_VOICE_RECORDING_LIST_MAX_ENTRIES 10

/* Fits a full page with maximum filename and uint32 metadata lengths. */
#define BLE_VOICE_RECORDING_LIST_CAPACITY 1536

/** Encodes the oldest BLE_VOICE_RECORDING_LIST_MAX_ENTRIES entries; reorders entries by timestamp.
 *  Returns encoded length or a negative errno. */
int ble_voice_recording_list_encode(zsw_recording_entry_t *entries, size_t count,
                                    char *output, size_t capacity);
