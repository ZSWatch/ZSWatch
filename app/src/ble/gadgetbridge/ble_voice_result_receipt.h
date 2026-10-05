/* SPDX-License-Identifier: GPL-3.0-only */
#pragma once

#include <stdint.h>
#include "events/zsw_voice_memo_event.h"

/* 1: new durable receipt; 0: identical accepted duplicate; negative: rejected.
 * Acceptance stores a result draft, never creates a calendar/alarm action.
 */
int ble_voice_result_accept(const char *recording_key, const char *result_id,
                           const char *result_sha256, uint32_t generation,
                           const struct zsw_voice_memo_result_event *event);
