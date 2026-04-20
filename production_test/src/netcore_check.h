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

#include "production_test_runner.h"

/**
 * @brief Check if the network core is alive and responding.
 *
 * Uses the Net Core Monitor (NCM) GPMEM heartbeat to verify that the
 * CPUNET is running and feeding its liveness counter. Requires the
 * net core image (ipc_radio) to have CONFIG_NET_CORE_MONITOR=y.
 *
 * @return TEST_RESULT_PASSED if net core is alive, TEST_RESULT_FAILED otherwise.
 */
test_result_t netcore_check_run(void);
