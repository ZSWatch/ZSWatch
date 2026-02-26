/*
 * Copyright (c) 2019 Linaro Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef H_ZEPHYR_BENCH_H__
#define H_ZEPHYR_BENCH_H__

#include <inttypes.h>
#include <stdint.h>
#include "zephyr.h"
#include "bootutil/bootutil_log.h"

/* TODO: Unclear if this can be here (redundantly). */
BOOT_LOG_MODULE_DECLARE(mcuboot);

typedef uint32_t bench_state_t;

#define plat_bench_start(_s) do { \
    BOOT_LOG_ERR("start benchmark"); \
    *(_s) = k_cycle_get_32(); \
} while (0)

#define plat_bench_stop(_s) do { \
    uint32_t _stop_time = k_cycle_get_32(); \
    BOOT_LOG_ERR("bench: %" PRId32 " cycles", _stop_time - *(_s)); \
} while (0)

#endif /* not H_ZEPHYR_BENCH_H__ */
