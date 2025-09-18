/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <inttypes.h>
#include <time.h>

struct retained_data {
    time_t current_time_seconds;
    uint64_t wakeup_time;
    uint64_t display_off_time;

    /* The uptime from the current session the last time the
     * retained data was updated.
     */
    uint64_t uptime_latest;

    /* Cumulative uptime from all previous sessions up through
     * uptime_latest of this session.
     */
    uint64_t uptime_sum;

    /* Number of times the application has started. */
    uint32_t boots;

    /* Number of times the application has gone into system off. */
    uint32_t off_count;

    char timezone[10];
};

/* For simplicity in the sample just allow anybody to see and
 * manipulate the retained state.
 */
extern struct retained_data retained;

/* Check whether the retained data is valid, and if not reset it.
 *
 * @return true if and only if the data was valid and reflects state
 * from previous sessions.
 */
bool retained_validate(void);

/* Update any generic retained state and recalculate its checksum so
 * subsequent boots can verify the retained state.
 */
void zsw_retained_ram_update(void);

void zsw_retained_ram_reset(void);
