/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

#include <zephyr/zbus/zbus.h>
#include "drivers/zsw_hr.h"

#ifdef CONFIG_ZSW_HR_MANAGER
/* Only heart rates with skin contact, at least CONFIG_ZSW_HR_MIN_CONFIDENCE (75 %) confidence
 * in CONFIG_ZSW_HR_GOOD_REPORTS (3) consecutive reports are published on this channel.
 */
ZBUS_CHAN_DEFINE(hr_sample_data_chan, struct zsw_hr_sample, NULL, NULL,
                 ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT());

#endif
