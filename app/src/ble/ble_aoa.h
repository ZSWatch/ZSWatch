/*
 * Copyright 2022 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/** Initiate BLE for angle of arrival advertisements
 * @return       Success or failure.
 */
bool bleAoaInit();

/** Start or stop angle of arrival advertisements
 * @param min_ms Minimum advertisement time in milliseconds.
 * @param max_ms Maximum advertisement time in milliseconds.
 * @param on     Start or stop.
 * @return       Success or failure.
 */
bool bleAoaAdvertise(uint16_t min_ms, uint16_t max_ms, bool on);