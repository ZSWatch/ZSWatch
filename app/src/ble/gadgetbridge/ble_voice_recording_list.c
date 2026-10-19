#include "ble_voice_recording_list.h"
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static int append(char *output, size_t capacity, size_t *used, const char *format, ...)
{
    if (*used >= capacity) {
        return -ENOSPC;
    }
    va_list arguments;
    va_start(arguments, format);
    int length = vsnprintf(output + *used, capacity - *used, format, arguments);
    va_end(arguments);
    if (length < 0 || (size_t)length >= capacity - *used) {
        return -ENOSPC;
    }
    *used += length;
    return 0;
}

int ble_voice_recording_list_encode(zsw_recording_entry_t *entries, size_t count,
                                   char *output, size_t capacity)
{
    if (!output || !capacity || count > ZSW_RECORDING_MAX_FILES || (count && !entries)) {
        return -EINVAL;
    }
    // Oldest first, so the phone syncs recordings in the order they were made.
    for (size_t i = 1; i < count; i++) {
        zsw_recording_entry_t entry = entries[i];
        size_t j = i;
        for (; j > 0 && entries[j - 1].timestamp > entry.timestamp; j--) {
            entries[j] = entries[j - 1];
        }
        entries[j] = entry;
    }
    size_t page = count < BLE_VOICE_RECORDING_LIST_MAX_ENTRIES ? count : BLE_VOICE_RECORDING_LIST_MAX_ENTRIES;
    size_t used = 0;
    int rc = append(output, capacity, &used,
                    "{\"t\":\"voice_memo\",\"action\":\"list_result\",\"result_ack_version\":1,"
                    "\"total\":%u,\"more\":%s,\"recordings\":[",
                    (unsigned int)count, page < count ? "true" : "false");
    for (size_t i = 0; rc == 0 && i < page; i++) {
        const zsw_recording_entry_t *entry = &entries[i];
        if (!memchr(entry->filename, '\0', sizeof(entry->filename))) {
            return -EINVAL;
        }
        rc = append(output, capacity, &used, "%s{\"filename\":\"", i ? "," : "");
        for (const unsigned char *p = (const unsigned char *)entry->filename; rc == 0 && *p; p++) {
            if (*p == '"' || *p == '\\') {
                rc = append(output, capacity, &used, "\\%c", *p);
            } else if (*p < 0x20) {
                rc = append(output, capacity, &used, "\\u%04x", (unsigned)*p);
            } else {
                rc = append(output, capacity, &used, "%c", *p);
            }
        }
        if (rc == 0) {
            rc = append(output, capacity, &used,
                        "\",\"duration_ms\":%" PRIu32 ",\"size_bytes\":%" PRIu32 ",\"timestamp\":%" PRIu32 "}",
                        entry->duration_ms, entry->size_bytes, entry->timestamp);
        }
    }
    if (rc == 0) {
        rc = append(output, capacity, &used, "]}");
    }
    return rc == 0 ? (int)used : rc;
}
