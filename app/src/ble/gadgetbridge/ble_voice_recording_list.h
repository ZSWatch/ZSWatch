#pragma once

#include "managers/zsw_recording_manager_store.h"

/* Fits all 50 ordinary filenames and maximum uint32 metadata values. */
#define BLE_VOICE_RECORDING_LIST_CAPACITY 6144

/** Returns encoded length or a negative errno. Never sends a partial list. */
int ble_voice_recording_list_encode(const zsw_recording_entry_t *entries, size_t count,
                                   char *output, size_t capacity);
