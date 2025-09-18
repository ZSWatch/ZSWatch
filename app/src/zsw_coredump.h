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

#define ZSW_COREDUMP_MAX_FILENAME_LEN   32
#define ZSW_COREDUMP_DATETIME_LEN       32
#define ZSW_COREDUMP_MAX_STORED         1

typedef struct zsw_coredump_sumary_t {
    char datetime[ZSW_COREDUMP_DATETIME_LEN + 1];
    char file[ZSW_COREDUMP_MAX_FILENAME_LEN + 1];
    int line;
} zsw_coredump_sumary_t;

int zsw_coredump_init(void);

/*
* @brief: Dumps the coredump using the logging backend.
*/
int zsw_coredump_to_log(void);

void zsw_coredump_erase(int index);

int zsw_coredump_get_summary(zsw_coredump_sumary_t *summary, int max_dumps, int *num_dumps);
