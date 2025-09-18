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

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <events/periodic_event.h>

static struct k_work_delayable periodic_slow_work;
static struct k_work_delayable periodic_fast_work;
static struct k_work_delayable periodic_mid_work;

ZBUS_CHAN_DEFINE(periodic_event_10s_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_slow_work,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );

ZBUS_CHAN_DEFINE(periodic_event_100ms_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_fast_work,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );

ZBUS_CHAN_DEFINE(periodic_event_1s_chan,
                 struct periodic_event,
                 NULL,
                 &periodic_mid_work,
                 ZBUS_OBSERVERS_EMPTY,
                 ZBUS_MSG_INIT()
                );
