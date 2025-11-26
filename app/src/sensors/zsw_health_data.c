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

#include <zephyr/random/random.h>

#include "zsw_health_data.h"

/* Heart rate simulation parameters */
#define HR_BASE_VALUE     70
#define HR_VARIATION      20

int16_t zsw_health_data_get_heart_rate(void)
{
#ifdef CONFIG_BT_HRS
    static int16_t current_heart_rate = HR_BASE_VALUE;
    int16_t variation;

    variation = (sys_rand32_get() % (HR_VARIATION * 2 + 1)) - HR_VARIATION;
    current_heart_rate = HR_BASE_VALUE + variation;
    return current_heart_rate;
#else
    return 0;
#endif
}
